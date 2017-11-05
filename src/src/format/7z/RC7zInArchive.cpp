/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "format/7z/RC7zInArchive.h"
#include "filesystem/RCStreamUtils.h"
#include "format/7z/RC7zHeader.h"
#include "common/RCBuffer.h"
#include "format/7z/RC7zDecoder.h"
#include "filesystem/RCSequentialOutStreamImp2.h"
#include "common/RCStringConvert.h"
#include "algorithm/7zCrc.h"
#include "algorithm/CpuArch.h"

#define Get16(p) GetUi16(p)
#define Get32(p) GetUi32(p)
#define Get64(p) GetUi64(p)

#ifndef RC_STATIC_SFX
    #define FORMAT_7Z_RECOVERY
#endif

/////////////////////////////////////////////////////////////////
//RC7zInArchive class implementation

BEGIN_NAMESPACE_RCZIP

static void BoolVector_Fill_False(RCBoolVector &v, int32_t size)
{
    v.clear();
    v.reserve(size);
    for (int i = 0; i < size; i++)
    {
        v.push_back(false);
    }
}

static bool BoolVector_GetAndSet(RCBoolVector &v, uint32_t index)
{
    if (index >= (uint32_t)v.size())
    {
        return true;
    }
    bool res = v[index];
    v[index] = true;
    return res;
}

bool RC7zFolder::CheckStructure() const
{
    const int32_t kNumCodersMax = sizeof(uint32_t) * 8; // don't change it
    const int32_t kMaskSize = sizeof(uint32_t) * 8; // it must be >= kNumCodersMax
    const int32_t kNumBindsMax = 32;

    if (m_coders.size() > kNumCodersMax || m_bindPairs.size() > kNumBindsMax)
    {
        return false;
    }

    {
        RCBoolVector v;
        BoolVector_Fill_False(v, (int32_t)(m_bindPairs.size() + m_packStreams.size()));

        int32_t i;
        for (i = 0; i < (int32_t)m_bindPairs.size(); i++)
        {
            if (BoolVector_GetAndSet(v, m_bindPairs[i].m_inIndex))
            {
                return false;
            }
        }

        for (i = 0; i < (int32_t)m_packStreams.size(); i++)
        {
            if (BoolVector_GetAndSet(v, m_packStreams[i]))
            {
                return false;
            }
        }

        BoolVector_Fill_False(v, (int32_t)m_unpackSizes.size());
        for (i = 0; i < (int32_t)m_bindPairs.size(); i++)
        {
            if (BoolVector_GetAndSet(v, m_bindPairs[i].m_outIndex))
            {
                return false;
            }
        }
    }

    uint32_t mask[kMaskSize];
    int32_t i;
    for (i = 0; i < kMaskSize; i++)
    {
        mask[i] = 0;
    }

    {
        RCIntVector inStreamToCoder, outStreamToCoder;
        for (i = 0; i < (int32_t)m_coders.size(); i++)
        {
            RC7zNum j;
            const RC7zCoderInfo &coder = m_coders[i];
            for (j = 0; j < coder.m_numInStreams; j++)
            {
                inStreamToCoder.push_back(i);
            }
            for (j = 0; j < coder.m_numOutStreams; j++)
            {
                outStreamToCoder.push_back(i);
            }
        }

        for (i = 0; i < (int32_t)m_bindPairs.size(); i++)
        {
            const RC7zBindPair &bp = m_bindPairs[i];
            mask[inStreamToCoder[bp.m_inIndex]] |= (1 << outStreamToCoder[bp.m_outIndex]);
        }
    }

    for (i = 0; i < kMaskSize; i++)
    {
        for (int j = 0; j < kMaskSize; j++)
        {
            if (((1 << j) & mask[i]) != 0)
            {
                mask[i] |= mask[j];
            }
        }
    }

    for (i = 0; i < kMaskSize; i++)
    {
        if (((1 << i) & mask[i]) != 0)
        {
            return false;
        }
    }

    return true;
}


class RC7zInArchive::RC7zStreamSwitch
{
public:
    RC7zStreamSwitch(): 
        _needRemove(false)
    {
    }
    ~RC7zStreamSwitch()
    {
        Remove();
    }
    
    void Remove();
    void Set(RC7zInArchive* archive, const byte_t *data, size_t size);
    void Set(RC7zInArchive* archive, const RCByteBuffer& byteBuffer);
    void Set(RC7zInArchive* archive, const RCVector<RCByteBuffer>* dataVector);
private:
    RC7zInArchive *_archive;
    bool _needRemove;
};

void RC7zInArchive::RC7zStreamSwitch::Remove()
{
    if (_needRemove)
    {
        _archive->DeleteByteStream();
        _needRemove = false;
    }
}

void RC7zInArchive::RC7zStreamSwitch::Set(RC7zInArchive* archive, const byte_t *data, size_t size)
{
    Remove();
    _archive = archive;
    _archive->AddByteStream(data, size);
    _needRemove = true;
}

void RC7zInArchive::RC7zStreamSwitch::Set(RC7zInArchive* archive, const RCByteBuffer& byteBuffer)
{
    Set(archive, byteBuffer.data(), byteBuffer.GetCapacity());
}

void RC7zInArchive::RC7zStreamSwitch::Set(RC7zInArchive* archive, const RCVector<RCByteBuffer>* dataVector)
{
    Remove();
    byte_t external = archive->ReadByte();
    if (external != 0)
    {
        int32_t dataIndex = (int32_t)archive->ReadNum();
        if (dataIndex < 0 || dataIndex >= (int32_t)dataVector->size())
        {
            _ThrowCode(RC_E_DataError) ;
        }
        Set(archive, (*dataVector)[dataIndex]);
    }
}

RC7zInArchive::RC7zInArchive()
{
}

RC7zInArchive::~RC7zInArchive()
{
}

void RC7zInArchive::AddByteStream(const byte_t *buffer, size_t size)
{
    m_inByteBack = RC7zInBytePtr(new RC7zInByte) ;
    m_inByteVector.push_back(m_inByteBack);
    m_inByteBack->Init(buffer, size);
}

void RC7zInArchive::DeleteByteStream()
{
    m_inByteBack.reset();
    m_inByteVector.pop_back();
    if (!m_inByteVector.empty())
    {
        m_inByteBack = m_inByteVector.back() ;
    }
}

bool RC7zInArchive::TestSignatureCandidate(const byte_t *p)
{
    for (int32_t i = 0; i < RC7zHeader::s_k7zSignatureSize; i++)
    {
        if (p[i] != RC7zHeader::s_k7zSignature[i])
        {
            return false;
        }
    }
    return (p[0x1A] == 0 && p[0x1B] == 0) ;
}

HResult RC7zInArchive::FindAndReadSignature(IInStream *stream, const uint64_t *searchHeaderSizeLimit)
{
    HResult hr = RCStreamUtils::ReadStream_FALSE(stream, m_header, RC7zDefs::s_headerSize) ;
    if(!IsSuccess(hr))
    {
        return hr ;
    }

    if (TestSignatureCandidate(m_header))
    {
        return RC_S_OK;
    }

    RCByteBuffer byteBuffer;
    const uint32_t kBufferSize = (1 << 16);
    byteBuffer.SetCapacity(kBufferSize);
    byte_t *buffer = byteBuffer.data();
    uint32_t numPrevBytes = RC7zDefs::s_headerSize - 1;
    memcpy(buffer, m_header + 1, numPrevBytes);
    uint64_t curTestPos = m_arhiveBeginStreamPosition + 1;
    for (;;)
    {
        if (searchHeaderSizeLimit != NULL)
        {
            if (curTestPos - m_arhiveBeginStreamPosition > *searchHeaderSizeLimit)
            {
                break;
            }
        }
        do
        {
            uint32_t numReadBytes = kBufferSize - numPrevBytes;
            uint32_t processedSize;
            hr = stream->Read(buffer + numPrevBytes, numReadBytes,& processedSize) ;
            if(!IsSuccess(hr))
            {
                return hr ;
            }
            numPrevBytes += processedSize;
            if (processedSize == 0)
            {
                return RC_S_FALSE;
            }
        } while (numPrevBytes < RC7zDefs::s_headerSize);
        uint32_t numTests = numPrevBytes - RC7zDefs::s_headerSize + 1;
        for (uint32_t pos = 0; pos < numTests; pos++)
        {
            for (; buffer[pos] != _T('7') && pos < numTests; pos++);
            if (pos == numTests)
            {
                break;
            }
            if (TestSignatureCandidate(buffer + pos))
            {
                memcpy(m_header, buffer + pos, RC7zDefs::s_headerSize);
                curTestPos += pos;
                m_arhiveBeginStreamPosition = curTestPos;
                return stream->Seek(curTestPos + RC7zDefs::s_headerSize, RC_STREAM_SEEK_SET, NULL);
            }
        }
        curTestPos += numTests;
        numPrevBytes -= numTests;
        memmove(buffer, buffer + numTests, numPrevBytes);
    }
    return RC_S_FALSE;
}

void RC7zInArchive::ReadBytes(byte_t *data, size_t size)
{
    m_inByteBack->ReadBytes(data, size);
}

byte_t RC7zInArchive::ReadByte()
{
    return m_inByteBack->ReadByte();
}

uint64_t RC7zInArchive::ReadNumber()
{
    return m_inByteBack->ReadNumber();
}

RC7zNum RC7zInArchive::ReadNum()
{
    return m_inByteBack->ReadNum();
}

uint64_t RC7zInArchive::ReadID()
{
    return m_inByteBack->ReadNumber();
}

uint32_t RC7zInArchive::ReadUInt32()
{
    return m_inByteBack->ReadUInt32();
}

uint64_t RC7zInArchive::ReadUInt64()
{
    return m_inByteBack->ReadUInt64();
}

void RC7zInArchive::SkipData(uint64_t size)
{
    m_inByteBack->SkipData(size);
}

void RC7zInArchive::SkipData()
{
    m_inByteBack->SkipData();
}

void RC7zInArchive::WaitAttribute(uint64_t attribute)
{
    for (;;)
    {
        uint64_t type = ReadID();
        if (type == attribute)
        {
            return;
        }
        if (type == RC7zID::kEnd)
        {
            _ThrowCode(RC_E_ReadStreamError) ;
        }
        SkipData();
    }
}

void RC7zInArchive::ReadArchiveProperties(RC7zInArchiveInfo&  /* archiveInfo */)
{
    for (;;)
    {
        if (ReadID() == RC7zID::kEnd)
        {
            break;
        }
        SkipData();
    }
}

void RC7zInArchive::GetNextFolderItem(RC7zFolder& folder)
{
    RC7zNum numCoders = ReadNum();

    folder.m_coders.clear();
    folder.m_coders.reserve((int32_t)numCoders);
    RC7zNum numInStreams = 0;
    RC7zNum numOutStreams = 0;
    RC7zNum i;
    for (i = 0; i < numCoders; i++)
    {
        folder.m_coders.push_back(RC7zCoderInfo());
        RC7zCoderInfo& coder = folder.m_coders.back();

        {
            byte_t mainByte = ReadByte();
            int32_t idSize = (mainByte&  0xF);
            byte_t longID[15];
            ReadBytes(longID, idSize);
            if (idSize > 8)
            {
                _ThrowCode(RC_E_ReadStreamError) ;
            }
            uint64_t id = 0;
            for (int32_t j = 0; j < idSize; j++)
            {
                id |= (uint64_t)longID[idSize - 1 - j] << (8 * j);
            }
            coder.m_methodID = id;

            if ((mainByte&  0x10) != 0)
            {
                coder.m_numInStreams = ReadNum();
                coder.m_numOutStreams = ReadNum();
            }
            else
            {
                coder.m_numInStreams = 1;
                coder.m_numOutStreams = 1;
            }
            if ((mainByte&  0x20) != 0)
            {
                RC7zNum propertiesSize = ReadNum();
                coder.m_properties.SetCapacity((size_t)propertiesSize);
                ReadBytes((byte_t *)coder.m_properties.data(), (size_t)propertiesSize);
            }
            if ((mainByte&  0x80) != 0)
            {
                _ThrowCode(RC_E_ReadStreamError) ;
            }
        }
        numInStreams += coder.m_numInStreams;
        numOutStreams += coder.m_numOutStreams;
    }

    RC7zNum numBindPairs = numOutStreams - 1;
    folder.m_bindPairs.clear();
    folder.m_bindPairs.reserve(numBindPairs);
    for (i = 0; i < numBindPairs; i++)
    {
        RC7zBindPair bindPair;
        bindPair.m_inIndex = ReadNum();
        bindPair.m_outIndex = ReadNum();
        folder.m_bindPairs.push_back(bindPair);
    }

    if (numInStreams < numBindPairs)
    {
        _ThrowCode(RC_E_ReadStreamError) ;
    }

    RC7zNum numPackedStreams = numInStreams - numBindPairs;
    folder.m_packStreams.reserve(numPackedStreams);
    if (numPackedStreams == 1)
    {
        for (i = 0; i < numInStreams; i++)
        {
            if (folder.FindBindPairForInStream(i) < 0)
            {
                folder.m_packStreams.push_back(i);
                break;
            }
        }

        if (folder.m_packStreams.size() != 1)
        {
            _ThrowCode(RC_E_ReadStreamError) ;
        }
    }
    else
    {
        for (i = 0; i < numPackedStreams; i++)
        {
            folder.m_packStreams.push_back(ReadNum());
        }
    }
}

void RC7zInArchive::ReadHashDigests(int32_t numItems,
                                    RCBoolVector& digestsDefined,
                                    RCVector<uint32_t>& digests)
{
    ReadBoolVector2(numItems, digestsDefined);
    digests.clear();
    digests.reserve(numItems);
    for (int32_t i = 0; i < numItems; i++)
    {
        uint32_t crc = 0;
        if (digestsDefined[i])
        {
            crc = ReadUInt32();
        }
        digests.push_back(crc);
    }
}

void RC7zInArchive::ReadPackInfo( uint64_t& dataOffset,
                                  RCVector<uint64_t>& packSizes,
                                  RCBoolVector& packCRCsDefined,
                                  RCVector<uint32_t>& packCRCs)
{
    dataOffset = ReadNumber();
    RC7zNum numPackStreams = ReadNum();

    WaitAttribute(RC7zID::kSize);
    packSizes.clear();
    packSizes.reserve(numPackStreams);
    for (RC7zNum i = 0; i < numPackStreams; i++)
    {
        packSizes.push_back(ReadNumber());
    }

    uint64_t type = 0 ;
    for (;;)
    {
        type = ReadID();
        if (type == RC7zID::kEnd)
        {
            break;
        }
        if (type == RC7zID::kCRC)
        {
            ReadHashDigests(numPackStreams, packCRCsDefined, packCRCs);
            continue;
        }
        SkipData();
    }
    if (packCRCsDefined.empty())
    {
        BoolVector_Fill_False(packCRCsDefined, numPackStreams);
        packCRCs.reserve(numPackStreams);
        packCRCs.clear();
        for (RC7zNum i = 0; i < numPackStreams; i++)
        {
            packCRCs.push_back(0);
        }
    }
}

void RC7zInArchive::ReadUnpackInfo( const RCVector<RCByteBuffer>* dataVector,
                                    RCVector<RC7zFolder>& folders)
{
    WaitAttribute(RC7zID::kFolder);
    RC7zNum numFolders = ReadNum();
    {
        RC7zStreamSwitch streamSwitch ;
        streamSwitch.Set(this, dataVector);
        folders.clear();
        folders.reserve(numFolders);
        for (RC7zNum i = 0; i < numFolders; i++)
        {
            folders.push_back(RC7zFolder());
            GetNextFolderItem(folders.back());
        }
    }

    WaitAttribute(RC7zID::kCodersUnpackSize);

    RC7zNum i = 0 ;
    for (i = 0; i < numFolders; i++)
    {
        RC7zFolder& folder = folders[i];
        RC7zNum numOutStreams = folder.GetNumOutStreams();
        folder.m_unpackSizes.reserve(numOutStreams);
        for (RC7zNum j = 0; j < numOutStreams; j++)
        {
            folder.m_unpackSizes.push_back(ReadNumber());
        }
    }

    for (;;)
    {
        uint64_t type = ReadID();
        if (type == RC7zID::kEnd)
        {
            return;
        }
        if (type == RC7zID::kCRC)
        {
            RCBoolVector crcsDefined;
            RCVector<uint32_t> crcs;
            ReadHashDigests(numFolders, crcsDefined, crcs);
            for (i = 0; i < numFolders; i++)
            {
                RC7zFolder& folder = folders[i];
                folder.m_unpackCRCDefined = crcsDefined[i];
                folder.m_unpackCRC = crcs[i];
            }
            continue;
        }
        SkipData();
    }
}

void RC7zInArchive::ReadSubStreamsInfo(const RCVector<RC7zFolder>& folders,
                                       RCVector<RC7zNum>& numUnpackStreamsInFolders,
                                       RCVector<uint64_t>& unpackSizes,
                                       RCBoolVector& digestsDefined,
                                       RCVector<uint32_t>& digests)
{
    numUnpackStreamsInFolders.clear();
    numUnpackStreamsInFolders.reserve(folders.size());
    uint64_t type = 0;
    for (;;)
    {
        type = ReadID();
        if (type == RC7zID::kNumUnpackStream)
        {
            for (int32_t i = 0; i < (int32_t)folders.size(); i++)
            {
                numUnpackStreamsInFolders.push_back(ReadNum());
            }
            continue;
        }
        if (type == RC7zID::kCRC || type == RC7zID::kSize)
        {
            break;
        }
        if (type == RC7zID::kEnd)
        {
            break;
        }
        SkipData();
    }

    if (numUnpackStreamsInFolders.empty())
    {
        for (int32_t i = 0; i < (int32_t)folders.size(); i++)
        {
            numUnpackStreamsInFolders.push_back(1);
        }
    }

    int32_t i = 0 ;
    for (i = 0; i < (int32_t)numUnpackStreamsInFolders.size(); i++)
    {
        // v3.13 incorrectly worked with empty folders
        // v4.07: we check that folder is empty
        RC7zNum numSubstreams = numUnpackStreamsInFolders[i];
        if (numSubstreams == 0)
        {
            continue;
        }
        uint64_t sum = 0;
        for (RC7zNum j = 1; j < numSubstreams; j++)
        {
            if (type == RC7zID::kSize)
            {
                uint64_t size = ReadNumber();
                unpackSizes.push_back(size);
                sum += size;
            }
        }
        unpackSizes.push_back(folders[i].GetUnpackSize() - sum);
    }
    if (type == RC7zID::kSize)
    {
        type = ReadID();
    }

    int32_t numDigests = 0;
    int32_t numDigestsTotal = 0;
    for (i = 0; i < (int32_t)folders.size(); i++)
    {
        RC7zNum numSubstreams = numUnpackStreamsInFolders[i];
        if (numSubstreams != 1 || !folders[i].m_unpackCRCDefined)
        {
            numDigests += numSubstreams;
        }
        numDigestsTotal += numSubstreams;
    }

    for (;;)
    {
        if (type == RC7zID::kCRC)
        {
            RCBoolVector digestsDefined2;
            RCVector<uint32_t> digests2;
            ReadHashDigests(numDigests, digestsDefined2, digests2);
            int32_t digestIndex = 0;
            for (i = 0; i < (int32_t)folders.size(); i++)
            {
                RC7zNum numSubstreams = numUnpackStreamsInFolders[i];
                const RC7zFolder& folder = folders[i];
                if (numSubstreams == 1 && folder.m_unpackCRCDefined)
                {
                    digestsDefined.push_back(true);
                    digests.push_back(folder.m_unpackCRC);
                }
                else
                {
                    for (RC7zNum j = 0; j < numSubstreams; j++, digestIndex++)
                    {
                        digestsDefined.push_back(digestsDefined2[digestIndex]);
                        digests.push_back(digests2[digestIndex]);
                    }
                }
            }
        }
        else if (type == RC7zID::kEnd)
        {
            if (digestsDefined.empty())
            {
                BoolVector_Fill_False(digestsDefined, numDigestsTotal);
                digests.clear();
                for (int32_t i = 0; i < numDigestsTotal; i++)
                {
                    digests.push_back(0);
                }
            }
            return;
        }
        else
        {
            SkipData();
        }
        type = ReadID();
    }
}

void RC7zInArchive::ReadStreamsInfo(const RCVector<RCByteBuffer>* dataVector,
                                    uint64_t& dataOffset,
                                    RCVector<uint64_t>& packSizes,
                                    RCBoolVector& packCRCsDefined,
                                    RCVector<uint32_t>& packCRCs,
                                    RCVector<RC7zFolder>& folders,
                                    RCVector<RC7zNum>& numUnpackStreamsInFolders,
                                    RCVector<uint64_t>& unpackSizes,
                                    RCBoolVector& digestsDefined,
                                    RCVector<uint32_t>& digests)
{
    for (;;)
    {
        uint64_t type = ReadID();
        if (type > ((uint32_t)1 << 30))
        {
            _ThrowCode(RC_E_ReadStreamError) ;
        }
        switch((uint32_t)type)
        {
        case RC7zID::kEnd:
            return;
        case RC7zID::kPackInfo:
            {
                ReadPackInfo(dataOffset, packSizes, packCRCsDefined, packCRCs);
                break;
            }
        case RC7zID::kUnpackInfo:
            {
                ReadUnpackInfo(dataVector, folders);
                break;
            }
        case RC7zID::kSubStreamsInfo:
            {
                ReadSubStreamsInfo(folders, numUnpackStreamsInFolders,
                                   unpackSizes, digestsDefined, digests);
                break;
            }
        default:
            {
                _ThrowCode(RC_E_ReadStreamError) ;
                break ;
            }
        }
    }
}

void RC7zInArchive::ReadBoolVector(int32_t numItems, RCBoolVector& v)
{
    v.clear();
    v.reserve(numItems);
    byte_t b = 0;
    byte_t mask = 0;
    for (int32_t i = 0; i < numItems; i++)
    {
        if (mask == 0)
        {
            b = ReadByte();
            mask = 0x80;
        }
        v.push_back((b&  mask) != 0);
        mask >>= 1;
    }
}

void RC7zInArchive::ReadBoolVector2(int32_t numItems, RCBoolVector& v)
{
    byte_t allAreDefined = ReadByte();
    if (allAreDefined == 0)
    {
        ReadBoolVector(numItems, v);
        return;
    }
    v.clear();
    v.reserve(numItems);
    for (int32_t i = 0; i < numItems; i++)
    {
        v.push_back(true);
    }
}

void RC7zInArchive::ReadUInt64DefVector(const RCVector<RCByteBuffer>& dataVector,
                                        RC7zUInt64DefVector& v,
                                        int32_t numFiles)
{
    ReadBoolVector2(numFiles, v.m_defined);

    RC7zStreamSwitch streamSwitch;
    streamSwitch.Set(this,& dataVector);
    v.m_values.reserve(numFiles);

    for (int32_t i = 0; i < numFiles; i++)
    {
        uint64_t t = 0;
        if (v.m_defined[i])
        {
            t = ReadUInt64();
        }
        v.m_values.push_back(t);
    }
}

HResult RC7zInArchive::ReadAndDecodePackedStreams(ICompressCodecsInfo* codecsInfo,
                                                  uint64_t baseOffset,
                                                  uint64_t& dataOffset, 
                                                  RCVector<RCByteBuffer>& dataVector , 
                                                  ICryptoGetTextPassword* getTextPassword, 
                                                  bool& passwordIsDefined )
{
    RCVector<uint64_t> packSizes;
    RCBoolVector packCRCsDefined;
    RCVector<uint32_t> packCRCs;
    RCVector<RC7zFolder> folders;

    RCVector<RC7zNum> numUnpackStreamsInFolders;
    RCVector<uint64_t> unpackSizes;
    RCBoolVector digestsDefined;
    RCVector<uint32_t> digests;

    ReadStreamsInfo(NULL,
                    dataOffset,
                    packSizes,
                    packCRCsDefined,
                    packCRCs,
                    folders,
                    numUnpackStreamsInFolders,
                    unpackSizes,
                    digestsDefined,
                    digests);

    RC7zNum packIndex = 0;
    RC7zDecoder decoder ;
    uint64_t dataStartPos = baseOffset + dataOffset;
    for (int32_t i = 0; i < (int32_t)folders.size(); i++)
    {
        const RC7zFolder& folder = folders[i];
        dataVector.push_back(RCByteBuffer());
        RCByteBuffer& data = dataVector.back();
        uint64_t unpackSize64 = folder.GetUnpackSize();
        size_t unpackSize = (size_t)unpackSize64;
        if (unpackSize != unpackSize64)
        {
           _ThrowCode(RC_E_ReadStreamError) ;
        }
        data.SetCapacity(unpackSize);

        RCSequentialOutStreamImp2 *outStreamSpec = new RCSequentialOutStreamImp2;
        ISequentialOutStreamPtr outStream = outStreamSpec;
        outStreamSpec->Init(data.data(), unpackSize);

        HResult result = decoder.Decode(codecsInfo,
                                        m_stream.Get(), 
                                        dataStartPos,
                                        &packSizes[packIndex], 
                                        folder, 
                                        outStream.Get(), 
                                        NULL, 
                                        getTextPassword, 
                                        passwordIsDefined,
                                        1 );        

        if (folder.m_unpackCRCDefined)
        {
            if (CrcCalc(data.data(), unpackSize) != folder.m_unpackCRC)
            {
                if(passwordIsDefined)
                {
                    return RC_E_Password ;
                }
            }
        }
        if(!IsSuccess(result))
        {
            return result ;
        }
        for (int32_t j = 0; j < (int32_t)folder.m_packStreams.size(); j++)
        {
            uint64_t packSize = packSizes[packIndex++];
            dataStartPos += packSize;
            m_headersSize += packSize;
        }
    }
    return RC_S_OK;
}

HResult RC7zInArchive::ReadHeader(ICompressCodecsInfo* codecsInfo,
                                  RC7zArchiveDatabaseEx& db,
                                  ICryptoGetTextPassword* getTextPassword, 
                                  bool& passwordIsDefined )
{
    uint64_t type = ReadID();

    if (type == RC7zID::kArchiveProperties)
    {
        ReadArchiveProperties(db.m_archiveInfo);
        type = ReadID();
    }

    RCVector<RCByteBuffer> dataVector;

    if (type == RC7zID::kAdditionalStreamsInfo)
    {
        HResult result = ReadAndDecodePackedStreams(codecsInfo,
                                                    db.m_archiveInfo.m_startPositionAfterHeader,
                                                    db.m_archiveInfo.m_dataStartPosition2,
                                                    dataVector, 
                                                    getTextPassword, 
                                                    passwordIsDefined );
        if(!IsSuccess(result))
        {
            return result ;
        }
        db.m_archiveInfo.m_dataStartPosition2 += db.m_archiveInfo.m_startPositionAfterHeader;
        type = ReadID();
    }

    RCVector<uint64_t> unpackSizes;
    RCBoolVector digestsDefined;
    RCVector<uint32_t> digests;

    if (type == RC7zID::kMainStreamsInfo)
    {
        ReadStreamsInfo(&dataVector,
                        db.m_archiveInfo.m_dataStartPosition,
                        db.m_packSizes,
                        db.m_packCRCsDefined,
                        db.m_packCRCs,
                        db.m_folders,
                        db.m_numUnpackStreamsVector,
                        unpackSizes,
                        digestsDefined,
                        digests);
        db.m_archiveInfo.m_dataStartPosition += db.m_archiveInfo.m_startPositionAfterHeader;
        type = ReadID();
    }
    else
    {
        for (int32_t i = 0; i < (int32_t)db.m_folders.size(); i++)
        {
            db.m_numUnpackStreamsVector.push_back(1);
            RC7zFolder& folder = db.m_folders[i];
            unpackSizes.push_back(folder.GetUnpackSize());
            digestsDefined.push_back(folder.m_unpackCRCDefined);
            digests.push_back(folder.m_unpackCRC);
        }
    }

    db.m_files.clear();

    if (type == RC7zID::kEnd)
    {
        return RC_S_OK;
    }
    if (type != RC7zID::kFilesInfo)
    {
        _ThrowCode(RC_E_ReadStreamError) ;
    }

    RC7zNum numFiles = ReadNum();
    db.m_files.reserve(numFiles);
    RC7zNum i = 0 ;
    for (i = 0; i < numFiles; i++)
    {
        RC7zFileItemPtr item(new RC7zFileItem);
        if (!item)
        {
            return RC_S_FALSE;
        }
        db.m_files.push_back(item);
    }

    db.m_archiveInfo.m_fileInfoPopIDs.push_back(RC7zID::kSize);
    if (!db.m_packSizes.empty())
    {
        db.m_archiveInfo.m_fileInfoPopIDs.push_back(RC7zID::kPackInfo);
    }
    if (numFiles > 0  && !digests.empty())
    {
        db.m_archiveInfo.m_fileInfoPopIDs.push_back(RC7zID::kCRC);
    }

    RCBoolVector emptyStreamVector;
    BoolVector_Fill_False(emptyStreamVector, (int32_t)numFiles);

    RCBoolVector emptyFileVector;
    RCBoolVector antiFileVector;
    RC7zNum numEmptyStreams = 0;

    for (;;)
    {
        uint64_t type = ReadID();
        if (type == RC7zID::kEnd)
        {
            break;
        }
        uint64_t size = ReadNumber();
        size_t ppp = m_inByteBack->GetPos() ;
        bool addPropIdToList = true;
        bool isKnownType = true;
        if (type > ((uint32_t)1 << 30))
        {
            isKnownType = false;
        }
        else 
        {
            switch((uint32_t)type)
            {
                case RC7zID::kName:
                {
                    RC7zStreamSwitch streamSwitch;
                    streamSwitch.Set(this,& dataVector);
                    for (int32_t i = 0; i < (int32_t)db.m_files.size(); i++)
                    {
                        m_inByteBack->ReadString(db.m_files[i]->m_name);
                    }
                    break;
                }
                case RC7zID::kWinAttributes:
                {
                    RCBoolVector boolVector;
                    ReadBoolVector2((int32_t)db.m_files.size(), boolVector);
                    RC7zStreamSwitch streamSwitch;
                    streamSwitch.Set(this,& dataVector);
                    for (i = 0; i < numFiles; i++)
                    {
                        RC7zFileItemPtr& file = db.m_files[i];
                        file->m_attribDefined = boolVector[i];
                        if (file->m_attribDefined)
                        {
                            file->m_attrib = ReadUInt32();
                        }
                    }
                    break;
                }
                case RC7zID::kEmptyStream:
                {
                    ReadBoolVector(numFiles, emptyStreamVector);
                    for (i = 0; i < (RC7zNum)emptyStreamVector.size(); i++)
                    {
                        if (emptyStreamVector[i])
                        {
                            numEmptyStreams++;
                        }
                    }

                    BoolVector_Fill_False(emptyFileVector, numEmptyStreams);
                    BoolVector_Fill_False(antiFileVector, numEmptyStreams);
                    break;
                }
                case RC7zID::kEmptyFile:  
                    ReadBoolVector(numEmptyStreams, emptyFileVector); 
                    break;
                case RC7zID::kAnti:  
                    ReadBoolVector(numEmptyStreams, antiFileVector);
                    break;
                case RC7zID::kStartPos:
                    ReadUInt64DefVector(dataVector, db.m_startPos, (int32_t)numFiles);
                     break;
                case RC7zID::kCTime:  
                    ReadUInt64DefVector(dataVector, db.m_cTime, (int32_t)numFiles); 
                    break;
                case RC7zID::kATime:  
                    ReadUInt64DefVector(dataVector, db.m_aTime, (int32_t)numFiles); 
                    break;
                case RC7zID::kMTime:  
                    ReadUInt64DefVector(dataVector, db.m_mTime, (int32_t)numFiles); 
                    break;
                case RC7zID::kDummy:
                {
                    for (uint64_t j = 0; j < size; j++)
                    {
                        if (ReadByte() != 0)
                        {
                            _ThrowCode(RC_E_ReadStreamError) ;
                        }
                    }
                    addPropIdToList = false;
                    break;
                }
                default:
                    addPropIdToList = false ;
                    isKnownType = false ;
                    break ;
            }
        }
        if (isKnownType)
        {
            if(addPropIdToList)
            {
                db.m_archiveInfo.m_fileInfoPopIDs.push_back(type);
            }
        }
        else
        {
            SkipData(size);
        }
        bool checkRecordsSize = (db.m_archiveInfo.m_version.m_major > 0 ||
                                db.m_archiveInfo.m_version.m_minor > 2) ;
        if (checkRecordsSize && m_inByteBack->GetPos() - ppp != size)
        {
            _ThrowCode(RC_E_ReadStreamError) ;
        }
    }

    RC7zNum emptyFileIndex = 0;
    RC7zNum sizeIndex = 0;

    RC7zNum numAntiItems = 0;
    for (i = 0; i < numEmptyStreams; i++)
    {
        if (antiFileVector[i])
        {
            numAntiItems++;
        }
    }

    for (i = 0; i < numFiles; i++)
    {
        RC7zFileItemPtr& file = db.m_files[i];
        bool isAnti = false ;
        file->m_hasStream = !emptyStreamVector[i];
        if (file->m_hasStream)
        {
            file->m_isDir = false;
            isAnti = false;
            file->m_size = unpackSizes[sizeIndex];
            file->m_crc = digests[sizeIndex];
            file->m_crcDefined = digestsDefined[sizeIndex];
            sizeIndex++;
        }
        else
        {
            file->m_isDir = !emptyFileVector[emptyFileIndex];
            isAnti = antiFileVector[emptyFileIndex];
            emptyFileIndex++;
            file->m_size = 0;
            file->m_crcDefined = false;
        }
        if (numAntiItems != 0)
        {
            db.m_isAnti.push_back(isAnti);
        }
    }
    return RC_S_OK;
}

HResult RC7zInArchive::ReadDatabase2(ICompressCodecsInfo* codecsInfo,
                                     RC7zArchiveDatabaseEx& db,
                                     ICryptoGetTextPassword* getTextPassword, 
                                     bool& passwordIsDefined)
{
    db.Clear();
    db.m_archiveInfo.m_startPosition = m_arhiveBeginStreamPosition;

    db.m_archiveInfo.m_version.m_major = m_header[6];
    db.m_archiveInfo.m_version.m_minor = m_header[7];

    if (db.m_archiveInfo.m_version.m_major != RC7zHeader::s_k7zMajorVersion)
    {
        //版本不支持
        _ThrowCode(RC_E_VersionNotSupported) ;
    }

    uint32_t crcFromArchive = Get32(m_header + 8);
    uint64_t nextHeaderOffset = Get64(m_header + 0xC);
    uint64_t nextHeaderSize = Get64(m_header + 0x14);
    uint32_t nextHeaderCRC = Get32(m_header + 0x1C);
    uint32_t crc = CrcCalc(m_header + 0xC, 20);

#ifdef FORMAT_7Z_RECOVERY
    if (crcFromArchive == 0 && nextHeaderOffset == 0 && nextHeaderSize == 0 && nextHeaderCRC == 0)
    {
        uint64_t cur = 0 ;
        uint64_t cur2 = 0 ;
        HResult hr = m_stream->Seek(0, RC_STREAM_SEEK_CUR, &cur) ;
        if(!IsSuccess(hr))
        {
            return hr ;
        }
        const int32_t kCheckSize = 500;
        byte_t buf[kCheckSize];
        hr = m_stream->Seek(0, RC_STREAM_SEEK_END, &cur2) ;
        if(!IsSuccess(hr))
        {
            return hr ;
        }
        int32_t checkSize = kCheckSize;
        if (cur2 - cur < kCheckSize)
        {
            checkSize = (int32_t)(cur2 - cur);
        }
        hr = m_stream->Seek(-checkSize, RC_STREAM_SEEK_END,&cur2) ;
        if(!IsSuccess(hr))
        {
            return hr ;
        }
        hr = RCStreamUtils::ReadStream_FALSE(m_stream.Get(), buf, (size_t)checkSize) ;
        if(!IsSuccess(hr))
        {
            return hr ;
        }
        int32_t i;
        for (i = (int32_t)checkSize - 2; i >= 0; i--)
        {
            if (buf[i] == 0x17 && buf[i + 1] == 0x6 || buf[i] == 0x01 && buf[i + 1] == 0x04)
            {
                break;
            }
        }
        if (i < 0)
        {
            return RC_S_FALSE;
        }
        nextHeaderSize = checkSize - i;
        nextHeaderOffset = cur2 - cur + i;
        nextHeaderCRC = CrcCalc(buf + i, (size_t)nextHeaderSize);
        hr = m_stream->Seek(cur, RC_STREAM_SEEK_SET, NULL) ;
        if(!IsSuccess(hr))
        {
            return hr ;
        } 
    }
#endif

#ifdef FORMAT_7Z_RECOVERY
    crcFromArchive = crc;
#endif

    db.m_archiveInfo.m_startPositionAfterHeader = m_arhiveBeginStreamPosition + RC7zDefs::s_headerSize;

    if (crc != crcFromArchive)
    {
        _ThrowCode(RC_E_FileHeaderCRCError) ;
    }

    if (nextHeaderSize == 0)
    {
        //空的7z压缩文件，检查和读取7z压缩包注释
        uint64_t comment_pos = db.m_archiveInfo.m_startPositionAfterHeader ;
        db.m_commentStartPos = comment_pos ;
        uint16_t comment_size = 0;
        HResult hr = m_stream->Seek(comment_pos, RC_STREAM_SEEK_SET, NULL);
        if(!IsSuccess(hr))
        {
            return RC_S_OK ;
        }
        hr = RCStreamUtils::ReadStream_FALSE(m_stream.Get(), &comment_size, sizeof(comment_size));        
        if (IsSuccess(hr) && (comment_size > 0))
        {
            RCByteBuffer comment_buffer;
            uint32_t cs = MyMin((uint32_t)(1 << 16) , (uint32_t)comment_size);
            comment_buffer.SetCapacity(cs + 2);
            memset(comment_buffer.data(), 0, cs + 2);
            hr = RCStreamUtils::ReadStream_FALSE(m_stream.Get(), comment_buffer.data(), (size_t)cs);
            if (IsSuccess(hr))
            {
                db.m_isCommented = true;
                RCStringA comment = (char*)comment_buffer.data();
                db.m_comment = RCStringConvert::MultiByteToUnicodeString(comment, RC_CP_ACP);
            }
        }
        return RC_S_OK;
    }

    if (nextHeaderSize > (uint64_t)0xFFFFFFFF)
    {
        return RC_S_FALSE;
    }

    HResult hr = m_stream->Seek(nextHeaderOffset, RC_STREAM_SEEK_CUR, NULL) ;
    if(!IsSuccess(hr))
    {
        return hr ;
    }

    RCByteBuffer buffer2;
    buffer2.SetCapacity((size_t)nextHeaderSize);

    hr = RCStreamUtils::ReadStream_FALSE(m_stream.Get(), buffer2.data(), (size_t)nextHeaderSize) ;
    if(!IsSuccess(hr))
    {
        return hr ;
    }
    m_headersSize += RC7zDefs::s_headerSize + nextHeaderSize;
    db.m_phySize = RC7zDefs::s_headerSize + nextHeaderOffset + nextHeaderSize;

    if (CrcCalc(buffer2.data(), (uint32_t)nextHeaderSize) != nextHeaderCRC)
    {
        _ThrowCode(RC_E_FileHeaderCRCError) ;
    }

    //检查和读取7z压缩包注释
    uint64_t comment_pos = m_arhiveBeginStreamPosition + db.m_phySize;
    uint16_t comment_size = 0;
    hr = m_stream->Seek(comment_pos, RC_STREAM_SEEK_SET, NULL);
    if(IsSuccess(hr))
    {
        hr = RCStreamUtils::ReadStream_FALSE(m_stream.Get(), &comment_size, sizeof(comment_size));
    }
    db.m_commentStartPos = comment_pos;
    if (IsSuccess(hr) && (comment_size > 0))
    {
        RCByteBuffer comment_buffer;
        uint32_t cs = MyMin((uint32_t)(1 << 16) , (uint32_t)comment_size);
        comment_buffer.SetCapacity(cs + 2);
        memset(comment_buffer.data(), 0, cs + 2);
        hr = RCStreamUtils::ReadStream_FALSE(m_stream.Get(), comment_buffer.data(), (size_t)cs);
        if (IsSuccess(hr))
        {
            db.m_isCommented = true;
            RCStringA comment = (char*)comment_buffer.data();
            db.m_comment = RCStringConvert::MultiByteToUnicodeString(comment, RC_CP_ACP);
        }
    }
    //


    RC7zStreamSwitch streamSwitch;
    streamSwitch.Set(this, buffer2);

    RCVector<RCByteBuffer> dataVector;

    uint64_t type = ReadID();
    if (type != RC7zID::kHeader)
    {
        if (type != RC7zID::kEncodedHeader)
        {
            _ThrowCode(RC_E_ReadStreamError) ;
        }
        HResult result = ReadAndDecodePackedStreams(codecsInfo,
                                                    db.m_archiveInfo.m_startPositionAfterHeader,
                                                    db.m_archiveInfo.m_dataStartPosition2,
                                                    dataVector,
                                                    getTextPassword, 
                                                    passwordIsDefined );
        if(!IsSuccess(result))
        {
            return result ;
        }
        if (dataVector.empty())
        {
            return RC_S_OK; 
        }
        if (dataVector.size() > 1)
        {
            _ThrowCode(RC_E_ReadStreamError) ;
        }
        streamSwitch.Remove();
        streamSwitch.Set(this, dataVector.front());
        if (ReadID() != RC7zID::kHeader)
        {
            _ThrowCode(RC_E_ReadStreamError) ;
        }
    }

    db.m_headersSize = m_headersSize;
    return ReadHeader(codecsInfo,db,getTextPassword, passwordIsDefined);
}

HResult RC7zInArchive::Open(IInStream *stream, const uint64_t *searchHeaderSizeLimit)
{
    m_headersSize = 0;
    Close();
    HResult hr = stream->Seek(0, RC_STREAM_SEEK_CUR,& m_arhiveBeginStreamPosition) ;
    if(!IsSuccess(hr))
    {
        return hr ;
    }
    hr = FindAndReadSignature(stream, searchHeaderSizeLimit) ;
    if(!IsSuccess(hr))
    {
        return hr ;
    }
    m_stream = stream;
    return RC_S_OK;
}

void RC7zInArchive::Close()
{
    m_stream.Release();
}

HResult RC7zInArchive::ReadDatabase( ICompressCodecsInfo* codecsInfo,
                                     RC7zArchiveDatabaseEx& db,
                                     ICryptoGetTextPassword* getTextPassword, 
                                     bool& passwordIsDefined )
{
    try
    {
        return ReadDatabase2(codecsInfo,
                             db,
                             getTextPassword, 
                             passwordIsDefined) ;
    }
    catch(HResult ) 
    {
        return RC_S_FALSE;
    }
}

END_NAMESPACE_RCZIP
