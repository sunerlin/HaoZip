/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "format/7z/RC7zOutArchive.h"
#include "filesystem/RCBufInStream.h"
#include "common/RCStringConvert.h"
#include "algorithm/7zCrc.h"

/////////////////////////////////////////////////////////////////
//RC7zOutArchive class implementation

BEGIN_NAMESPACE_RCZIP

RC7zOutArchive::RC7zOutArchive():
    m_prefixHeaderPos(0),
    m_countMode(false),
    m_writeToStream(false),
    m_countSize(0),
    m_crc(0)    
{
    m_outByte.Create(1 << 16) ;
#ifdef _7Z_VOL
    m_endMarker = false ;
#endif
}

RC7zOutArchive::~RC7zOutArchive()
{
}

#ifdef _7Z_VOL
HResult RC7zOutArchive::WriteFinishSignature()
{
    HResult hr = WriteDirect(RC7zHeader::s_k7zFinishSignature, RC7zHeader::s_k7zSignatureSize) ;
    if(!IsSuccess(hr))
    {
        return hr ;
    }
    RC7zArchiveVersion av;
    av.Major = RC7zHeader::s_k7zMajorVersion;
    av.Minor = 2;
    hr = WriteDirectByte(av.Major) ;
    if(!IsSuccess(hr))
    {
        return hr ;
    }
    return WriteDirectByte(av.Minor);
}

HResult RC7zOutArchive::WriteFinishHeader(const RC7zFinishHeader& h)
{
    CCRC crc;
    crc.UpdateUInt64(h.NextHeaderOffset);
    crc.UpdateUInt64(h.NextHeaderSize);
    crc.UpdateUInt32(h.NextHeaderCRC);
    crc.UpdateUInt64(h.ArchiveStartOffset);
    crc.UpdateUInt64(h.push_backitionalStartBlockSize);
    HResult hr = WriteDirectUInt32(crc.GetDigest()) ;
    if(!IsSuccess(hr))
    {
        return hr ;
    }
    hr = WriteDirectUInt64(h.NextHeaderOffset) ;
    if(!IsSuccess(hr))
    {
        return hr ;
    }
    hr = WriteDirectUInt64(h.NextHeaderSize) ;
    if(!IsSuccess(hr))
    {
        return hr ;
    }
    hr = WriteDirectUInt32(h.NextHeaderCRC) ;
    if(!IsSuccess(hr))
    {
        return hr ;
    }
    hr = WriteDirectUInt64(h.ArchiveStartOffset) ;
    if(!IsSuccess(hr))
    {
        return hr ;
    }
    return WriteDirectUInt64(h.push_backitionalStartBlockSize);
}

uint32_t RC7zOutArchive::GetVolHeadersSize(uint64_t dataSize, int32_t nameLength, bool props)
{
    uint32_t result = GetBigNumberSize(dataSize) * 2 + 41;
    if (nameLength != 0)
    {
        nameLength = (nameLength + 1) * 2;
        result += nameLength + GetBigNumberSize(nameLength) + 2;
    }
    if (props)
    {
        result += 20;
    }
    if (result >= 128)
    {
        result++;
    }
    result += s_k7zSignatureSize + 2 + s_k7zFinishHeaderSize;
    return result;
}

uint64_t RC7zOutArchive::GetVolPureSize(uint64_t volSize, int32_t nameLength, bool props)
{
    uint32_t headersSizeBase = RC7zOutArchive::GetVolHeadersSize(1, nameLength, props);
    int32_t testSize;
    if (volSize > headersSizeBase)
    {
        testSize = volSize - headersSizeBase;
    }
    else
    {
        testSize = 1;
    }
    uint32_t headersSize = RC7zOutArchive::GetVolHeadersSize(testSize, nameLength, props);
    uint64_t pureSize = 1;
    if (volSize > headersSize)
    {
        pureSize = volSize - headersSize;
    }
    return pureSize;
}

#endif

HResult RC7zOutArchive::WriteBytes(ISequentialOutStream* stream, const void* data, size_t size)
{
    while (size > 0)
    {
        uint32_t curSize = (uint32_t)MyMin(size, (size_t)0xFFFFFFFF);
        uint32_t processedSize;
        HResult hr = stream->Write(data, curSize, &processedSize) ;
        if(!IsSuccess(hr))
        {
            return hr ;
        }
        if (processedSize == 0)
        {
            return RC_E_FAIL;
        }
        data = (const void *)((const byte_t *)data + processedSize);
        size -= processedSize;
    }
    return RC_S_OK;
}

uint32_t RC7zOutArchive::GetBigNumberSize(uint64_t value)
{
    int32_t i = 0 ;
    for (i = 1; i < 9; i++)
    if (value < (((uint64_t)1 << (i * 7))))
    {
        break;
    }
    return i ;
}

void RC7zOutArchive::SetUInt32(byte_t *p, uint32_t d)
{
    for (int32_t i = 0; i < 4; i++, d >>= 8)
    {
        p[i] = (byte_t)d;
    }
}

void RC7zOutArchive::SetUInt64(byte_t *p, uint64_t d)
{
    for (int32_t i = 0; i < 8; i++, d >>= 8)
    {
        p[i] = (byte_t)d;
    }
}


HResult RC7zOutArchive::WriteDirect(const void* data, uint32_t size)
{
    return WriteBytes(m_spSeqOutStream.Get(), data, size);
}

uint64_t RC7zOutArchive::GetPos() const
{
    if (m_countMode)
    {
        return m_countSize;
    }
    if (m_writeToStream)
    {
        return m_outByte.GetProcessedSize();
    }
    return m_outByte2.GetPos();
}

void RC7zOutArchive::WriteBytes(const void *data, size_t size)
{
    if (m_countMode)
    {
        m_countSize += size;
    }
    else if (m_writeToStream)
    {
        m_outByte.WriteBytes(data, size);
        m_crc = CrcUpdate(m_crc, data, size);
    }
    else
    {
        m_outByte2.WriteBytes(data, size);
    }
}

void RC7zOutArchive::WriteBytes(const RCByteBuffer& data)
{
    WriteBytes(data.data(), data.GetCapacity());
}

void RC7zOutArchive::WriteByte(byte_t b)
{
    if (m_countMode)
    {
        m_countSize++;
    }
    else if (m_writeToStream)
    {
        m_outByte.WriteByte(b);
        m_crc = CRC_UPDATE_BYTE(m_crc, b);
    }
    else
    {
        m_outByte2.WriteByte(b);
    }
}

void RC7zOutArchive::WriteUInt32(uint32_t value)
{
    for (int32_t i = 0; i < 4; i++)
    {
        WriteByte((byte_t)value);
        value >>= 8;
    }
}

void RC7zOutArchive::WriteUInt64(uint64_t value)
{
    for (int32_t i = 0; i < 8; i++)
    {
        WriteByte((byte_t)value);
        value >>= 8;
    }
}

void RC7zOutArchive::WriteNumber(uint64_t value)
{
    byte_t firstByte = 0;
    byte_t mask = 0x80;
    int32_t i;
    for (i = 0; i < 8; i++)
    {
        if (value < ((uint64_t(1) << ( 7  * (i + 1)))))
        {
            firstByte |= byte_t(value >> (8 * i));
            break;
        }
        firstByte |= mask;
        mask >>= 1;
    }
    WriteByte(firstByte);
    for (;i > 0; i--)
    {
        WriteByte((byte_t)value);
        value >>= 8;
    }
}

void RC7zOutArchive::WriteID(uint64_t value)
{
    WriteNumber(value);
}

void RC7zOutArchive::WriteFolder(const RC7zFolder& folder)
{
    WriteNumber(folder.m_coders.size());
    int32_t i;
    for (i = 0; i < (int32_t)folder.m_coders.size(); i++)
    {
        const RC7zCoderInfo& coder = folder.m_coders[i];
        {
            size_t propertiesSize = coder.m_properties.GetCapacity();

            uint64_t id = coder.m_methodID;
            int32_t idSize;
            for (idSize = 1; idSize < sizeof(id); idSize++)
            {
                if ((id >> (8 * idSize)) == 0)
                {
                    break;
                }
            }
            byte_t longID[15];
            for (int32_t t = idSize - 1; t >= 0 ; t--, id >>= 8)
            {
                longID[t] = (byte_t)(id&  0xFF);
            }
            byte_t b;
            b = (byte_t)(idSize&  0xF);
            bool isComplex = !coder.IsSimpleCoder();
            b |= (isComplex ? 0x10 : 0);
            b |= ((propertiesSize != 0) ? 0x20 : 0 );
            WriteByte(b);
            WriteBytes(longID, idSize);
            if (isComplex)
            {
                WriteNumber(coder.m_numInStreams);
                WriteNumber(coder.m_numOutStreams);
            }
            if (propertiesSize == 0)
            {
                continue;
            }
            WriteNumber(propertiesSize);
            WriteBytes(coder.m_properties.data(), propertiesSize);
        }
    }
    for (i = 0; i < (int32_t)folder.m_bindPairs.size(); i++)
    {
        const RC7zBindPair& bindPair = folder.m_bindPairs[i];
        WriteNumber(bindPair.m_inIndex);
        WriteNumber(bindPair.m_outIndex);
    }
    if (folder.m_packStreams.size() > 1)
    {
        for (i = 0; i < (int32_t)folder.m_packStreams.size(); i++)
        {
            WriteNumber(folder.m_packStreams[i]);
        }
    }
}

void RC7zOutArchive::WriteBoolVector(const RCBoolVector& boolVector)
{
    byte_t b = 0;
    byte_t mask = 0x80;
    for (int32_t i = 0; i < (int32_t)boolVector.size(); i++)
    {
        if (boolVector[i])
        {
            b |= mask;
        }
        mask >>= 1;
        if (mask == 0)
        {
            WriteByte(b);
            mask = 0x80;
            b = 0;
        }
    }
    if (mask != 0x80)
    {
        WriteByte(b);
    }
}

void RC7zOutArchive::WriteHashDigests(const RCBoolVector& digestsDefined,
                                      const RCVector<uint32_t>& digests)
{
    int32_t numDefined = 0;
    int32_t i = 0 ;
    for (i = 0; i < (int32_t)digestsDefined.size(); i++)
    {
        if (digestsDefined[i])
        {
            numDefined++;
        }
    }
    if (numDefined == 0)
    {
        return;
    }

    WriteByte(RC7zID::kCRC);
    if (numDefined == (int32_t)digestsDefined.size())
    {
        WriteByte(1);
    }
    else
    {
        WriteByte(0);
        WriteBoolVector(digestsDefined);
    }
    for (i = 0; i < (int32_t)digests.size(); i++)
    {
        if (digestsDefined[i])
        {
            WriteUInt32(digests[i]);
        }
    }
}

void RC7zOutArchive::WritePackInfo(uint64_t dataOffset,
                                   const RCVector<uint64_t>& packSizes,
                                   const RCBoolVector& packCRCsDefined,
                                   const RCVector<uint32_t>& packCRCs)
{
    if (packSizes.empty())
    {
        return;
    }
    WriteByte(RC7zID::kPackInfo);
    WriteNumber(dataOffset);
    WriteNumber(packSizes.size());
    WriteByte(RC7zID::kSize);
    for (int32_t i = 0; i < (int32_t)packSizes.size(); i++)
    {
        WriteNumber(packSizes[i]);
    }

    WriteHashDigests(packCRCsDefined, packCRCs);
    WriteByte(RC7zID::kEnd);
}

void RC7zOutArchive::WriteUnpackInfo(const RCVector<RC7zFolder>& folders)
{
    if (folders.empty())
    {
        return;
    }

    WriteByte(RC7zID::kUnpackInfo);
    WriteByte(RC7zID::kFolder);
    WriteNumber(folders.size());
    {
        WriteByte(0);
        for (int32_t i = 0; i < (int32_t)folders.size(); i++)
        {
            WriteFolder(folders[i]);
        }
    }

    WriteByte(RC7zID::kCodersUnpackSize);
    int32_t i = 0 ;
    for (i = 0; i < (int32_t)folders.size(); i++)
    {
        const RC7zFolder& folder = folders[i];
        for (int32_t j = 0; j < (int32_t)folder.m_unpackSizes.size(); j++)
        {
            WriteNumber(folder.m_unpackSizes[j]);
        }
    }

    RCBoolVector unpackCRCsDefined;
    RCVector<uint32_t> unpackCRCs;
    for (i = 0; i < (int32_t)folders.size(); i++)
    {
        const RC7zFolder& folder = folders[i];
        unpackCRCsDefined.push_back(folder.m_unpackCRCDefined);
        unpackCRCs.push_back(folder.m_unpackCRC);
    }
    WriteHashDigests(unpackCRCsDefined, unpackCRCs);
    WriteByte(RC7zID::kEnd);
}

void RC7zOutArchive::WriteSubStreamsInfo(const RCVector<RC7zFolder>& folders,
                                         const RCVector<RC7zNum>& numUnpackStreamsInFolders,
                                         const RCVector<uint64_t>& unpackSizes,
                                         const RCBoolVector& digestsDefined,
                                         const RCVector<uint32_t>& digests)
{
    WriteByte(RC7zID::kSubStreamsInfo);
    int32_t i = 0 ;
    for (i = 0; i < (int32_t)numUnpackStreamsInFolders.size(); i++)
    {
        if (numUnpackStreamsInFolders[i] != 1)
        {
            WriteByte(RC7zID::kNumUnpackStream);
            for (i = 0; i < (int32_t)numUnpackStreamsInFolders.size(); i++)
            {
                WriteNumber(numUnpackStreamsInFolders[i]);
            }
            break;
        }
    }


    bool needFlag = true;
    RC7zNum index = 0;
    for (i = 0; i < (int32_t)numUnpackStreamsInFolders.size(); i++)
    {
        for (RC7zNum j = 0; j < numUnpackStreamsInFolders[i]; j++)
        {
            if (j + 1 != numUnpackStreamsInFolders[i])
            {
                if (needFlag)
                {
                    WriteByte(RC7zID::kSize);
                }
                needFlag = false;
                WriteNumber(unpackSizes[index]);
            }
            index++;
        }
    }

    RCBoolVector digestsDefined2;
    RCVector<uint32_t> digests2;
    int32_t digestIndex = 0;
    for (i = 0; i < (int32_t)folders.size(); i++)
    {
        int32_t numSubStreams = (int32_t)numUnpackStreamsInFolders[i];
        if (numSubStreams == 1 && folders[i].m_unpackCRCDefined)
        {
            digestIndex++;
        }
        else
        {
            for (int32_t j = 0; j < numSubStreams; j++, digestIndex++)
            {
                digestsDefined2.push_back(digestsDefined[digestIndex]);
                digests2.push_back(digests[digestIndex]);
            }
        }
    }
    WriteHashDigests(digestsDefined2, digests2);
    WriteByte(RC7zID::kEnd);
}

void RC7zOutArchive::SkipAlign(uint32_t /* pos */, uint32_t /* alignSize */)
{
    return ;
}

void RC7zOutArchive::WriteAlignedBoolHeader(const RCBoolVector& v, int32_t numDefined, byte_t type, uint32_t itemSize)
{
    const uint64_t bvSize = (numDefined == (int32_t)v.size()) ? 0 : (v.size() + 7) / 8;
    const uint64_t dataSize = (uint64_t)numDefined * itemSize + bvSize + 2;
    SkipAlign(3 + (uint32_t)bvSize + (uint32_t)GetBigNumberSize(dataSize), itemSize);

    WriteByte(type);
    WriteNumber(dataSize);
    if (numDefined == (int32_t)v.size())
    {
        WriteByte(1);
    }
    else
    {
        WriteByte(0);
        WriteBoolVector(v);
    }
    WriteByte(0);
}

void RC7zOutArchive::WriteUInt64DefVector(const RC7zUInt64DefVector& v, byte_t type)
{
    int32_t numDefined = 0;
    int32_t i = 0 ;
    for (i = 0; i < (int32_t)v.m_defined.size(); i++)
    {
        if (v.m_defined[i])
        {
            numDefined++;
        }
    }
    if (numDefined == 0)
    {
        return;
    }
    WriteAlignedBoolHeader(v.m_defined, numDefined, type, 8);

    for (i = 0; i < (int32_t)v.m_defined.size(); i++)
    {
        if (v.m_defined[i])
        {
            WriteUInt64(v.m_values[i]);
        }
    }
}

HResult RC7zOutArchive::EncodeStream( ICompressCodecsInfo* codecsInfo,
                                      RC7zEncoder& encoder, 
                                      const RCByteBuffer& data,
                                      RCVector<uint64_t>& packSizes, 
                                      RCVector<RC7zFolder>& folders)
{
    RCBufInStream* streamSpec = new RCBufInStream;
    ISequentialInStreamPtr stream = streamSpec ;    
    streamSpec->Init((const byte_t*)data.data(), data.GetCapacity()) ;
    RC7zFolder folderItem ;
    folderItem.m_unpackCRCDefined = true;
    folderItem.m_unpackCRC = CrcCalc((const byte_t*)data.data(), data.GetCapacity());
    uint64_t dataSize64 = data.GetCapacity() ;
    HResult hr = encoder.Encode(codecsInfo,    
                                stream.Get(), 
                                NULL, 
                                &dataSize64, 
                                folderItem, 
                                m_spSeqOutStream.Get(), 
                                packSizes, 
                                NULL) ;
    if(!IsSuccess(hr))
    {
        return hr ;
    }
    folders.push_back(folderItem) ;
    return RC_S_OK;
}

void RC7zOutArchive::WriteHeader(const RC7zArchiveDatabase& db,
                                 const RC7zHeaderOptions& headerOptions,
                                 uint64_t& headerOffset)
{
    int32_t i = 0 ;
    uint64_t packedSize = 0;
    for (i = 0; i < (int32_t)db.m_packSizes.size(); i++)
    {
        packedSize += db.m_packSizes[i];
    }

    headerOffset = packedSize;
    WriteByte(RC7zID::kHeader);

    // Archive Properties
    if(!db.m_folders.empty())
    {
        WriteByte(RC7zID::kMainStreamsInfo);
        WritePackInfo(0,
                      db.m_packSizes,
                      db.m_packCRCsDefined,
                      db.m_packCRCs);
        WriteUnpackInfo(db.m_folders);
        RCVector<uint64_t> unpackSizes;
        RCBoolVector digestsDefined;
        RCVector<uint32_t> digests;
        for (i = 0; i < (int32_t)db.m_files.size(); i++)
        {
            const RC7zFileItemPtr& file = db.m_files[i];
            if (!file->m_hasStream)
            {
                continue;
            }
            unpackSizes.push_back(file->m_size);
            digestsDefined.push_back(file->m_crcDefined);
            digests.push_back(file->m_crc);
        }

        WriteSubStreamsInfo(db.m_folders,
                            db.m_numUnpackStreamsVector,
                            unpackSizes,
                            digestsDefined,
                            digests) ;
        WriteByte(RC7zID::kEnd);
    }

    if (db.m_files.empty())
    {
        WriteByte(RC7zID::kEnd) ;
        return;
    }

    WriteByte(RC7zID::kFilesInfo);
    WriteNumber(db.m_files.size());

    {
        /* ---------- Empty Streams ---------- */
        RCBoolVector emptyStreamVector;
        emptyStreamVector.reserve(db.m_files.size());
        int32_t numEmptyStreams = 0;
        for (i = 0; i < (int32_t)db.m_files.size(); i++)
        {
            if (db.m_files[i]->m_hasStream)
            {
                emptyStreamVector.push_back(false);
            }
            else
            {
                emptyStreamVector.push_back(true);
                numEmptyStreams++;
            }
        }
        if (numEmptyStreams > 0)
        {
            WriteByte(RC7zID::kEmptyStream);
            WriteNumber((emptyStreamVector.size() + 7) / 8);
            WriteBoolVector(emptyStreamVector);

            RCBoolVector emptyFileVector, antiVector;
            emptyFileVector.reserve(numEmptyStreams);
            antiVector.reserve(numEmptyStreams);
            RC7zNum numEmptyFiles = 0, numAntiItems = 0;
            for (i = 0; i < (int32_t)db.m_files.size(); i++)
            {
                const RC7zFileItemPtr& file = db.m_files[i];
                if (!file->m_hasStream)
                {
                    emptyFileVector.push_back(!file->m_isDir);
                    if (!file->m_isDir)
                    {
                        numEmptyFiles++;
                    }
                    bool isAnti = db.IsItemAnti(i);
                    antiVector.push_back(isAnti);
                    if (isAnti)
                    {
                        numAntiItems++;
                    }
                }
            }

            if (numEmptyFiles > 0)
            {
                WriteByte(RC7zID::kEmptyFile);
                WriteNumber((emptyFileVector.size() + 7) / 8);
                WriteBoolVector(emptyFileVector);
            }

            if (numAntiItems > 0)
            {
                WriteByte(RC7zID::kAnti);
                WriteNumber((antiVector.size() + 7) / 8);
                WriteBoolVector(antiVector);
            }
        }            
    }


    {
        /* ---------- Names ---------- */

        int32_t numDefined = 0;
        size_t namesDataSize = 0;
        for (int32_t i = 0; i < (int32_t)db.m_files.size(); i++)
        {
            const RCString& name = db.m_files[i]->m_name;
            if (!name.empty())
            {
                numDefined++;
            }
            namesDataSize += (name.size() + 1) * 2;
        }

        if (numDefined > 0)
        {
            namesDataSize++;
            SkipAlign(2 + GetBigNumberSize(namesDataSize), 2);

            WriteByte(RC7zID::kName);
            WriteNumber(namesDataSize);
            WriteByte(0);
            for (int32_t i = 0; i < (int32_t)db.m_files.size(); i++)
            {
                const RCString& name = db.m_files[i]->m_name;
                for (int32_t t = 0; t < (int32_t)name.size(); t++)
                {
                    RCString::value_type c = name[t];
                    WriteByte((byte_t)c);
                    WriteByte((byte_t)(c >> 8));
                }
                RCString::value_type c = 0 ;
                WriteByte((byte_t)c);
                WriteByte((byte_t)(c >> 8));
            }
        }
    }

    if (headerOptions.m_writeCTime)
    {
        WriteUInt64DefVector(db.m_cTime, RC7zID::kCTime);
    }
    if (headerOptions.m_writeATime)
    {
        WriteUInt64DefVector(db.m_aTime, RC7zID::kATime);
    }
    if (headerOptions.m_writeMTime)
    {
        WriteUInt64DefVector(db.m_mTime, RC7zID::kMTime);
    }
    WriteUInt64DefVector(db.m_startPos, RC7zID::kStartPos);

    {
        /* ---------- Write Attrib ---------- */
        RCBoolVector boolVector;
        boolVector.reserve(db.m_files.size());
        int32_t numDefined = 0;
        for (i = 0; i < (int32_t)db.m_files.size(); i++)
        {
            bool defined = db.m_files[i]->m_attribDefined;
            boolVector.push_back(defined);
            if (defined)
            {
                numDefined++;
            }
        }
        if (numDefined > 0)
        {
            WriteAlignedBoolHeader(boolVector, numDefined, RC7zID::kWinAttributes, 4);
            for (i = 0; i < (int32_t)db.m_files.size(); i++)
            {
                const RC7zFileItemPtr& file = db.m_files[i];
                if (file->m_attribDefined)
                {
                    WriteUInt32(file->m_attrib);
                }
            }
        }
    }

    WriteByte(RC7zID::kEnd); // for files
    WriteByte(RC7zID::kEnd); // for headers
}

HResult RC7zOutArchive::WriteSignature()
{
    const int32_t kHeaderBufSize = RC7zHeader::s_k7zSignatureSize + 2 ;
    byte_t buf[kHeaderBufSize];
    memcpy(buf, RC7zHeader::s_k7zSignature, RC7zHeader::s_k7zSignatureSize);
    buf[RC7zHeader::s_k7zSignatureSize] = RC7zHeader::s_k7zMajorVersion;
    buf[RC7zHeader::s_k7zSignatureSize + 1] = 3;
    return WriteDirect(buf, kHeaderBufSize);
}

HResult RC7zOutArchive::WriteStartHeader(const RC7zStartHeader& h)
{
    byte_t buf[24];
    SetUInt64(buf + 4, h.m_nextHeaderOffset);
    SetUInt64(buf + 12, h.m_nextHeaderSize);
    SetUInt32(buf + 20, h.m_nextHeaderCRC);
    SetUInt32(buf, CrcCalc(buf + 4, 20));
    return WriteDirect(buf, 24);
}

HResult RC7zOutArchive::Create(ISequentialOutStream *stream, bool endMarker)
{
    Close();
#ifdef _7Z_VOL
    m_endMarker = endMarker;
#endif
    m_spSeqOutStream = stream;
    if (!endMarker)
    {
        m_spSeqOutStream.QueryInterface(IID_IOutStream,m_spOutStream.GetAddress());
        if (!m_spOutStream)
        {
            return RC_E_NOTIMPL ;
        }
    }
#ifdef _7Z_VOL
    if (endMarker)
    {
        /*
        RC7zStartHeader sh;
        sh.NextHeaderOffset = (uint32_t)(int32_t)-1;
        sh.NextHeaderSize = (uint32_t)(int32_t)-1;
        sh.NextHeaderCRC = 0;
        WriteStartHeader(sh);
        */
    }
    else
#endif
    {
        if (!m_spOutStream)
        {
            return RC_E_FAIL;
        }
        HResult hr = WriteSignature() ;
        if(!IsSuccess(hr))
        {
            return hr ;
        }
        hr = m_spOutStream->Seek(0, RC_STREAM_SEEK_CUR,&m_prefixHeaderPos) ;
        if(!IsSuccess(hr))
        {
            return hr ;
        }
    }
    return RC_S_OK;
}

void RC7zOutArchive::Close()
{
    m_spSeqOutStream.Release();
    m_spOutStream.Release();
}

HResult RC7zOutArchive::SkipPrefixArchiveHeader()
{
#ifdef _7Z_VOL
    if (m_endMarker)
    {
        return RC_S_OK;
    }
#endif
    return m_spOutStream->Seek(24, RC_STREAM_SEEK_CUR, NULL);
}

HResult RC7zOutArchive::WriteDatabase(ICompressCodecsInfo* codecsInfo,
                                      const RC7zArchiveDatabase& db,
                                      const RC7zCompressionMethodMode *options,
                                      const RC7zHeaderOptions& headerOptions)
{
    if (!db.CheckNumFiles())
    {
        return RC_E_FAIL;
    }

    uint64_t headerOffset = 0 ;
    uint32_t headerCRC = 0 ;
    uint64_t headerSize = 0 ;
    if (db.IsEmpty())
    {
        headerSize = 0 ;
        headerOffset = 0 ;
        headerCRC = CrcCalc(0, 0);
    }
    else
    {
        bool encodeHeaders = false;
        if (options != 0)
        {
            if (options->IsEmpty())
            {
                options = 0;
            }
        }
        if (options != 0)
        {
            if (options->m_passwordIsDefined || headerOptions.m_compressMainHeader)
            {
                encodeHeaders = true;
            }
        }

        m_outByte.SetStream(m_spSeqOutStream.Get());
        m_outByte.Init();
        m_crc = CRC_INIT_VAL;
        m_countMode = encodeHeaders;
        m_writeToStream = true;
        m_countSize = 0;
        WriteHeader(db, headerOptions, headerOffset);

        if (encodeHeaders)
        {
            RCByteBuffer buf;
            buf.SetCapacity(m_countSize);
            m_outByte2.Init((byte_t *)buf.data(), m_countSize);

            m_countMode = false;
            m_writeToStream = false;
            WriteHeader(db, headerOptions, headerOffset);

            if (m_countSize != m_outByte2.GetPos())
            {
                return RC_E_FAIL;
            }

            RC7zCompressionMethodMode encryptOptions;
            encryptOptions.m_passwordIsDefined = options->m_passwordIsDefined;
            encryptOptions.m_password = options->m_password;
            RC7zEncoder encoder(headerOptions.m_compressMainHeader ? *options : encryptOptions);
            RCVector<uint64_t> packSizes;
            RCVector<RC7zFolder> folders;
            HResult hr = EncodeStream(codecsInfo,
                                      encoder, 
                                      buf,
                                      packSizes, 
                                      folders) ;
            if(!IsSuccess(hr))
            {
                return hr ;
            }
            m_writeToStream = true;

            if (folders.empty())
            {
                _ThrowCode(RC_E_WriteStreamError) ;
            }

            WriteID(RC7zID::kEncodedHeader);
            WritePackInfo(headerOffset, packSizes, RCBoolVector(), RCVector<uint32_t>());
            WriteUnpackInfo(folders);
            WriteByte(RC7zID::kEnd);
            for (int32_t i = 0; i < (int32_t)packSizes.size(); i++)
            {
                headerOffset += packSizes[i];
            }
        }
        HResult hr = m_outByte.Flush() ;
        if(!IsSuccess(hr))
        {
            return hr ;
        }
        headerCRC = CRC_GET_DIGEST(m_crc);
        headerSize = m_outByte.GetProcessedSize();
    }
#ifdef _7Z_VOL
    if (m_endMarker)
    {
        RC7zFinishHeader h;
        h.NextHeaderSize = headerSize;
        h.NextHeaderCRC = headerCRC;
        h.NextHeaderOffset = uint64_t(0) - (headerSize + 4 + RC7zHeader::s_k7zFinishHeaderSize);
        h.ArchiveStartOffset = h.NextHeaderOffset - headerOffset;
        h.push_backitionalStartBlockSize = 0;
        HResult hr = WriteFinishHeader(h) ;
        if(!IsSuccess(hr))
        {
            return hr ;
        }
        return WriteFinishSignature();
    }
    else
#endif
    {
        RC7zStartHeader h;
        h.m_nextHeaderSize = headerSize;
        h.m_nextHeaderCRC = headerCRC;
        h.m_nextHeaderOffset = headerOffset;
        HResult hr = m_spOutStream->Seek(m_prefixHeaderPos, RC_STREAM_SEEK_SET, NULL) ;
        if(!IsSuccess(hr))
        {
            return hr ;
        }
        hr = WriteStartHeader(h);

        //写入7z格式注释
        if (db.m_isCommented)
        {
            hr = m_spOutStream->Seek(0, RC_STREAM_SEEK_END, NULL);
            RCStringA comment = RCStringConvert::UnicodeStringToMultiByte(db.m_comment);
            uint16_t comment_size = (uint16_t)comment.size();
            hr = m_spOutStream->Write(&comment_size, sizeof(uint16_t), NULL);
            if (!IsSuccess(hr))
            {
                return hr;
            }
            hr = m_spOutStream->Write(comment.c_str(), (uint32_t)comment.size(), NULL);
        }

        return hr;
        
    }
}

ISequentialOutStreamPtr RC7zOutArchive::GetSeqOutStream()
{
    return m_spSeqOutStream ;
}

END_NAMESPACE_RCZIP
