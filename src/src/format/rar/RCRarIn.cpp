/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "format/rar/RCRarIn.h"
#include "format/common/RCFindSignature.h"
#include "filesystem/RCStreamUtils.h"
#include "filesystem/RCLimitedSequentialInStream.h"
#include "common/RCStringConvert.h"
#include "common/RCCreateCoder.h"
#include "interface/RCMethodDefs.h"
#include "algorithm/7zCrc.h"

/////////////////////////////////////////////////////////////////
//RCRarIn class implementation

BEGIN_NAMESPACE_RCZIP

HResult RCRarIn::Open(IInStream *inStream, const uint64_t *searchHeaderSizeLimit, IArchiveOpenCallback* openArchiveCallback /* = 0 */)
{
    Close();
    HResult hr = Open2(inStream, searchHeaderSizeLimit, openArchiveCallback);
    if (hr == RC_S_OK)
    {
        return hr;
    }
    Close();
    return hr;
}

void RCRarIn::Close()
{
    m_stream.Release();
}

static inline bool TestMarkerCandidate(const void *aTestBytes)
{
    for (uint32_t i = 0; i < RCRarHeader::kMarkerSize; i++)
    {
        if (((const byte_t *)aTestBytes)[i] != RCRarHeader::kMarker[i])
        {
            return false;
        }
    }
    return true;
}

HResult RCRarIn::FindAndReadMarker(IInStream* stream, const uint64_t* searchHeaderSizeLimit)
{
    HResult hr  = RCFindSignature::FindSignatureInStream(stream,
                                                         RCRarHeader::kMarker,
                                                         RCRarHeader::kMarkerSize,
                                                         searchHeaderSizeLimit,
                                                         m_archiveStartPosition);
    if (hr!= RC_S_OK)
    {
        return hr;
    }

    m_stream = stream;
    m_position = m_archiveStartPosition + RCRarHeader::kMarkerSize;

    return m_stream->Seek(m_position, RC_STREAM_SEEK_SET, NULL);
}

bool RCRarIn::ReadBytesAndTestSize(void* data, uint32_t size)
{
    if (m_cryptoMode)
    {
        const byte_t *bufData = (const byte_t *)m_decryptedData.data();
        uint32_t bufSize = m_decryptedDataSize;
        uint32_t i;
        for (i = 0; i < size && m_cryptoPos < bufSize; i++)
        {
            ((byte_t *)data)[i] = bufData[m_cryptoPos++];
        }

        return (i == size);
    }

    return (RCStreamUtils::ReadStream_FALSE(m_stream.Get(), data, size) == RC_S_OK);
}

void RCRarIn::ReadBytesAndTestResult(void* data, uint32_t size)
{
    if(!ReadBytesAndTestSize(data, size))
    {
        _ThrowCode(RC_E_UnexpectedEndOfArchive) ;
    }
}

HResult RCRarIn::ReadBytes(void* data, uint32_t size, uint32_t *processedSize)
{
    size_t realProcessedSize = size;
    HResult hr = RCStreamUtils::ReadStream(m_stream.Get(), data, &realProcessedSize);
    if (processedSize != NULL)
    {
        *processedSize = (uint32_t)realProcessedSize;
    }
    AddToSeekValue(realProcessedSize);
    return hr;
}

static uint32_t CrcUpdateUInt16(uint32_t crc, uint16_t v)
{
    crc = CRC_UPDATE_BYTE(crc, (byte_t)(v & 0xFF));
    crc = CRC_UPDATE_BYTE(crc, (byte_t)((v >> 8) & 0xFF));
    return crc;
}

static uint32_t CrcUpdateUInt32(uint32_t crc, uint32_t v)
{
    crc = CRC_UPDATE_BYTE(crc, (byte_t)(v & 0xFF));
    crc = CRC_UPDATE_BYTE(crc, (byte_t)((v >> 8) & 0xFF));
    crc = CRC_UPDATE_BYTE(crc, (byte_t)((v >> 16) & 0xFF));
    crc = CRC_UPDATE_BYTE(crc, (byte_t)((v >> 24) & 0xFF));
    return crc;
}

HResult RCRarIn::Open2(IInStream* stream, const uint64_t *searchHeaderSizeLimit, IArchiveOpenCallback* openArchiveCallback)
{
    m_cryptoMode = false;
    HResult hr  = stream->Seek(0, RC_STREAM_SEEK_SET, &m_streamStartPosition);
    if (hr != RC_S_OK)
    {
        return hr;
    }

    m_position = m_streamStartPosition;

    hr = FindAndReadMarker(stream, searchHeaderSizeLimit);
    if (hr != RC_S_OK)
    {
        return hr;
    }

    byte_t buf[RCRarHeader::NArchive::kArchiveHeaderSize];
    uint32_t processedSize;
    ReadBytes(buf, sizeof(buf), &processedSize);
    if (processedSize != sizeof(buf))
    {
        return RC_S_FALSE;
    }

    m_curData = buf;
    m_curPos = 0;
    m_posLimit = sizeof(buf);

    m_archiveHeader.m_crc = ReadUInt16();
    m_archiveHeader.m_type = ReadByte();
    m_archiveHeader.m_flags = ReadUInt16();
    m_archiveHeader.m_size = ReadUInt16();
    m_archiveHeader.m_reserved1 = ReadUInt16();
    m_archiveHeader.m_reserved2 = ReadUInt32();
    m_archiveHeader.m_encryptVersion = 0;

    uint32_t crc = CRC_INIT_VAL;
    crc = CRC_UPDATE_BYTE(crc, m_archiveHeader.m_type);
    crc = CrcUpdateUInt16(crc, m_archiveHeader.m_flags);
    crc = CrcUpdateUInt16(crc, m_archiveHeader.m_size);
    crc = CrcUpdateUInt16(crc, m_archiveHeader.m_reserved1);
    crc = CrcUpdateUInt32(crc, m_archiveHeader.m_reserved2);

    if (m_archiveHeader.IsThereEncryptVer() && m_archiveHeader.m_size > RCRarHeader::NArchive::kArchiveHeaderSize)
    {
        ReadBytes(&m_archiveHeader.m_encryptVersion, 1, &processedSize);
        if (processedSize != 1)
        {
            return RC_S_FALSE;
        }
        crc = CRC_UPDATE_BYTE(crc, m_archiveHeader.m_encryptVersion);
    }

    if (m_archiveHeader.m_crc != (CRC_GET_DIGEST(crc) & 0xFFFF))
    {
        if (openArchiveCallback && (openArchiveCallback->SetError(RC_E_ArchiveHeaderCRCError) != RC_S_OK))
        {
            _ThrowCode(RC_E_ArchiveHeaderCRCError) ;
        }
    }

    if (m_archiveHeader.m_type != RCRarHeader::NBlockType::kArchiveHeader)
    {
        return RC_S_FALSE;
    }

    m_archiveCommentPosition = m_position;
    m_seekOnArchiveComment = true;

    return RC_S_OK;
}

void RCRarIn::SkipArchiveComment()
{
    if (!m_seekOnArchiveComment)
    {
        return;
    }
    AddToSeekValue(m_archiveHeader.m_size - m_archiveHeader.GetBaseSize());
    m_seekOnArchiveComment = false;
}

void RCRarIn::GetArchiveInfo(RCRarInArchiveInfo& archiveInfo) const
{
    archiveInfo.m_startPosition = m_archiveStartPosition;
    archiveInfo.m_flags = m_archiveHeader.m_flags;
    archiveInfo.m_commentPosition = m_archiveCommentPosition;
    archiveInfo.m_commentSize = (uint16_t)(m_archiveHeader.m_size - RCRarHeader::NArchive::kArchiveHeaderSize);
}

static void DecodeUnicodeFileName(const char *name,
                                  const byte_t* encName,
                                  int32_t encSize,
                                  char_t* unicodeName,
                                  int32_t maxDecSize)
{
    int32_t encPos = 0;
    int32_t decPos = 0;
    int32_t flagBits = 0;
    byte_t flags = 0;
    byte_t highByte = encName[encPos++];
    while (encPos < encSize && decPos < maxDecSize)
    {
        if (flagBits == 0)
        {
            flags = encName[encPos++];
            flagBits = 8;
        }

        switch(flags >> 6)
        {
        case 0:
            {
                unicodeName[decPos++] = encName[encPos++];
            }
            break;

        case 1:
            {
                unicodeName[decPos++] = (wchar_t)(encName[encPos++] + (highByte << 8));
            }
            break;

        case 2:
            {
                unicodeName[decPos++] = (wchar_t)(encName[encPos] + (encName[encPos + 1] << 8));
                encPos += 2;
            }
            break;

        case 3:
            {
                int32_t length = encName[encPos++];
                if (length & 0x80)
                {
                    byte_t correction = encName[encPos++];
                    for (length = (length & 0x7f) + 2;
                        length > 0 && decPos < maxDecSize; length--, decPos++)
                    {
                        unicodeName[decPos] = (wchar_t)(((name[decPos] + correction) & 0xff) + (highByte << 8));
                    }
                }
                else
                {
                    for (length += 2; length > 0 && decPos < maxDecSize; length--, decPos++)
                    {
                        unicodeName[decPos] = name[decPos];
                    }
                }
            }
            break;
        }
        flags <<= 2;
        flagBits -= 2;
    }
    unicodeName[decPos < maxDecSize ? decPos : maxDecSize - 1] = 0;
}

void RCRarIn::ReadName(RCRarItemExPtr& item, int32_t nameSize)
{
    item->m_unicodeName.clear();
    if (nameSize > 0)
    {
        m_nameBuffer.SetCapacity(nameSize + 1);
        char* buffer = m_nameBuffer.data();

        for (int32_t i = 0; i < nameSize; i++)
        {
            buffer[i] = ReadByte();
        }

        int32_t mainLen;
        for (mainLen = 0; mainLen < nameSize; mainLen++)
        {
            if (buffer[mainLen] == '\0')
            {
                break;
            }
        }

        buffer[mainLen] = '\0';
        item->m_name = buffer;

        if(item->HasUnicodeName())
        {
            if(mainLen < nameSize)
            {
                int32_t unicodeNameSizeMax = MyMin(nameSize, (0x400));
                m_unicodeNameBuffer.SetCapacity(unicodeNameSizeMax + 1);
                DecodeUnicodeFileName(buffer,
                                      (const byte_t *)buffer + mainLen + 1,
                                      nameSize - (mainLen + 1),
                                      m_unicodeNameBuffer.data(),
                                      unicodeNameSizeMax);
                item->m_unicodeName = m_unicodeNameBuffer.data();
            }
            else if (!RCStringConvert::ConvertUTF8ToUnicode(item->m_name.c_str(), item->m_name.size(), item->m_unicodeName))
            {
                item->m_unicodeName.clear();
            }
        }
    }
    else
    {
        item->m_name.clear();
    }
}

byte_t RCRarIn::ReadByte()
{
    if (m_curPos >= m_posLimit)
    {
        _ThrowCode(RC_E_IncorrectArchive) ;
    }
    return m_curData[m_curPos++];
}

uint16_t RCRarIn::ReadUInt16()
{
    uint16_t value = 0;
    for (int32_t i = 0; i < 2; i++)
    {
        byte_t b = ReadByte();
        value |= (uint16_t(b) << (8 * i));
    }
    return value;
}

uint32_t RCRarIn::ReadUInt32()
{
    uint32_t value = 0;
    for (int32_t i = 0; i < 4; i++)
    {
        byte_t b = ReadByte();
        value |= (uint32_t(b) << (8 * i));
    }
    return value;
}

void RCRarIn::ReadTime(byte_t mask, RCRarTime& rarTime)
{
    rarTime.m_lowSecond  = (byte_t)(((mask & 4) != 0) ? 1 : 0);
    int32_t numDigits  = (mask & 3);
    rarTime.m_subTime[0] = rarTime.m_subTime[1] = rarTime.m_subTime[2] = 0;

    for (int32_t i = 0; i < numDigits; i++)
    {
        rarTime.m_subTime[3 - numDigits + i] = ReadByte();
    }
}

void RCRarIn::ReadHeaderReal(RCRarItemExPtr& item)
{
    item->m_flags    = m_blockHeader.m_flags;
    item->m_packSize = ReadUInt32();
    item->m_size     = ReadUInt32();
    item->m_hostOS   = ReadByte();
    item->m_fileCRC  = ReadUInt32();
    item->m_mTime.m_dosTime = ReadUInt32();
    item->m_unPackVersion = ReadByte();
    item->m_method        = ReadByte();

    int32_t nameSize   = ReadUInt16();
    item->m_attrib        = ReadUInt32();

    item->m_mTime.m_lowSecond  = 0;
    item->m_mTime.m_subTime[0] = 0;
    item->m_mTime.m_subTime[1] = 0;
    item->m_mTime.m_subTime[2] = 0;

    if((item->m_flags & RCRarHeader::NFile::kSize64Bits) != 0)
    {
        item->m_packSize |= ((uint64_t)ReadUInt32() << 32);
        item->m_size |= ((uint64_t)ReadUInt32() << 32);
    }

    ReadName(item, nameSize);

    if (item->HasSalt())
    {
        for (int32_t i = 0; i < sizeof(item->m_salt); i++)
        {
            item->m_salt[i] = ReadByte();
        }
    }

    // some rar archives have HasExtTime flag without field.
    if (m_curPos < m_posLimit && item->HasExtTime())
    {
        byte_t accessMask = (byte_t)(ReadByte() >> 4);
        byte_t b = ReadByte();
        byte_t modifMask = (byte_t)(b >> 4);
        byte_t createMask = (byte_t)(b & 0xF);

        if ((modifMask & 8) != 0)
        {
            ReadTime(modifMask, item->m_mTime);
        }

        item->m_cTimeDefined = ((createMask & 8) != 0);

        if (item->m_cTimeDefined)
        {
            item->m_cTime.m_dosTime = ReadUInt32();
            ReadTime(createMask, item->m_cTime);
        }

        item->m_aTimeDefined = ((accessMask & 8) != 0);

        if (item->m_aTimeDefined)
        {
            item->m_aTime.m_dosTime = ReadUInt32();
            ReadTime(accessMask, item->m_aTime);
        }
    }

    uint16_t fileHeaderWithNameSize = (uint16_t)m_curPos;

    item->m_position = m_position;
    item->m_mainPartSize = fileHeaderWithNameSize;
    item->m_commentSize = (uint16_t)(m_blockHeader.m_headSize - fileHeaderWithNameSize);

    if (m_cryptoMode)
    {
        item->m_alignSize = (uint16_t)((16 - ((m_blockHeader.m_headSize) & 0xF)) & 0xF);
    }
    else
    {
        item->m_alignSize = 0;
    }

    AddToSeekValue(m_blockHeader.m_headSize);
}

void RCRarIn::AddToSeekValue(uint64_t addValue)
{
    m_position += addValue;
}

HResult RCRarIn::GetNextItem(ICompressCodecsInfo* codecsInfo,
                             RCRarItemExPtr& item,
                             ICryptoGetTextPassword* getTextPassword,
                             IArchiveOpenCallback* openArchiveCallback,
                             bool& decryptionError)
{
    decryptionError = false ;
    if (m_seekOnArchiveComment)
    {
        SkipArchiveComment();
    }

    for (;;)
    {
        if(!SeekInArchive(m_position))
        {
            return RC_S_FALSE;
        }

        if (!m_cryptoMode && (m_archiveHeader.m_flags &
            RCRarHeader::NArchive::kBlockHeadersAreEncrypted) != 0)
        {
            m_cryptoMode = false;
            if (getTextPassword == 0)
            {
                return RC_S_FALSE;
            }

            if(!SeekInArchive(m_position))
            {
                return RC_S_FALSE;
            }

            if (!m_rarAES)
            {
                ICompressFilterPtr filter;
                HResult hr = RCCreateCoder::CreateFilter(codecsInfo,
                                                         RCMethod::ID_CRYPTO_RAR_29,
                                                         filter,
                                                         false);
                if (hr != RC_S_OK)
                {
                    return hr;
                }

                m_rarAESSpec = dynamic_cast<RCRarAESDecoder*>(filter.Get());
                m_rarAES     = filter;
            }

            //SetRar350Mode
            RCPropertyIDPairArray proparray;
            ICompressSetCoderPropertiesPtr propset;

            HResult hr = m_rarAESSpec->QueryInterface(IID_ICompressSetCoderProperties, (void**)propset.GetAddress());
            if (hr != RC_S_OK)
            {
                return hr;
            }

            proparray.push_back(RCPropertyIDPairArray::value_type(RCMethodProperty::ID_SET_RAR_350_MODE, m_archiveHeader.IsEncryptOld()));
          
            hr = propset->SetCoderProperties(proparray);
            if (hr != RC_S_OK)
            {
                return hr;
            }

            // Salt
            const uint32_t kSaltSize = 8;
            byte_t salt[kSaltSize];
            if(!ReadBytesAndTestSize(salt, kSaltSize))
            {
                return RC_S_FALSE;
            }

            m_position += kSaltSize;
            hr = m_rarAESSpec->SetDecoderProperties2(salt, kSaltSize);
            if (hr != RC_S_OK)
            {
                return hr;
            }

            // Password
            RCString password;
            hr = getTextPassword->CryptoGetTextPassword(password);
            if (hr != RC_S_OK)
            {
                return hr;
            }

            RCString unicodePassword(password);
            RCByteBuffer buffer;
            const uint32_t sizeInBytes = (uint32_t)unicodePassword.length() * 2;
            buffer.SetCapacity(sizeInBytes);
            for (int32_t i = 0; i < (int32_t)unicodePassword.length(); i++)
            {
                char_t c = unicodePassword[i];
                (buffer.data())[i * 2] = (byte_t)c;
                (buffer.data())[i * 2 + 1] = (byte_t)(c >> 8);
            }

            hr = m_rarAESSpec->CryptoSetPassword(buffer.data(), sizeInBytes);
            if (hr != RC_S_OK)
            {
                return hr;
            }

            const uint32_t kDecryptedBufferSize = (1 << 12);
            if (m_decryptedData.GetCapacity() == 0)
            {
                m_decryptedData.SetCapacity(kDecryptedBufferSize);
            }

            hr = m_rarAES->Init();
            if (hr != RC_S_OK)
            {
                return hr;
            }

            size_t decryptedDataSizeT = kDecryptedBufferSize;
            hr = RCStreamUtils::ReadStream(m_stream.Get(), m_decryptedData.data(), &decryptedDataSizeT);
            if (hr != RC_S_OK)
            {
                return hr;
            }

            m_decryptedDataSize = (uint32_t)decryptedDataSizeT;
            m_decryptedDataSize = m_rarAES->Filter(m_decryptedData.data(), m_decryptedDataSize);

            m_cryptoMode = true;
            m_cryptoPos  = 0;
        }

        m_fileHeaderData.SetCapacity(7);
        if (!ReadBytesAndTestSize(m_fileHeaderData.data(), 7))
        {
            return RC_S_FALSE;
        }

        m_curData  = m_fileHeaderData.data();
        m_curPos   = 0;
        m_posLimit = 7;
        m_blockHeader.m_crc      = ReadUInt16();
        m_blockHeader.m_type     = ReadByte();
        m_blockHeader.m_flags    = ReadUInt16();
        m_blockHeader.m_headSize = ReadUInt16();

        if (m_blockHeader.m_headSize < 7)
        {
            if (openArchiveCallback->SetError(RC_E_IncorrectArchive) != RC_S_OK)
            {
                _ThrowCode(RC_E_IncorrectArchive) ;
            }
            return RC_S_FALSE;
        }

        if (m_blockHeader.m_type == RCRarHeader::NBlockType::kEndOfArchive)
        {
            return RC_S_FALSE;
        }

        if (m_blockHeader.m_type == RCRarHeader::NBlockType::kFileHeader)
        {
            m_fileHeaderData.SetCapacity(m_blockHeader.m_headSize);
            m_curData  = m_fileHeaderData.data();
            m_posLimit = m_blockHeader.m_headSize;

            ReadBytesAndTestResult(m_curData + m_curPos, m_blockHeader.m_headSize - 7);
            ReadHeaderReal(item);

            if ((CrcCalc(m_curData + 2,
                m_blockHeader.m_headSize - item->m_commentSize - 2) & 0xFFFF) != m_blockHeader.m_crc)
            {  
                if (openArchiveCallback->SetError(RC_E_FileHeaderCRCError) != RC_S_OK)
                {
                    _ThrowCode(RC_E_FileHeaderCRCError) ;
                }
            }

            FinishCryptoBlock();
            m_cryptoMode = false;
            SeekInArchive(m_position); // Move Position to compressed Data;
            AddToSeekValue(item->m_packSize);  // m_position points to next header;

            return RC_S_OK;
        }

        if (m_cryptoMode && m_blockHeader.m_headSize > (1 << 10))
        {
            decryptionError = true ;
            return RC_E_Password; // it's for bad passwords
        }

        if ((m_blockHeader.m_flags & RCRarHeader::NBlock::kLongBlock) != 0)
        {
            m_fileHeaderData.SetCapacity(7 + 4);
            m_curData = m_fileHeaderData.data();
            ReadBytesAndTestResult(m_curData + m_curPos, 4); // test it
            m_posLimit = 7 + 4;
            uint32_t dataSize = ReadUInt32();
            AddToSeekValue(dataSize);

            if (m_cryptoMode && dataSize > (1 << 27))
            {
                decryptionError = true ;
                return RC_E_Password; // it's for bad passwords
            }
            m_cryptoPos = m_blockHeader.m_headSize;
        }
        else
        {
            m_cryptoPos = 0;
        }

        AddToSeekValue(m_blockHeader.m_headSize);
        FinishCryptoBlock();
        m_cryptoMode = false;
    }
}

bool RCRarIn::SeekInArchive(uint64_t position)
{
    uint64_t newPosition;
    m_stream->Seek(position, RC_STREAM_SEEK_SET, &newPosition);
    return newPosition == position;
}

ISequentialInStream* RCRarIn::CreateLimitedStream(uint64_t position, uint64_t size)
{
    RCLimitedSequentialInStream* streamSpec = new RCLimitedSequentialInStream;
    ISequentialInStreamPtr inStream(streamSpec);

    SeekInArchive(position);

    streamSpec->SetStream(m_stream.Get());
    streamSpec->Init(size);

    return inStream.Detach();
}

END_NAMESPACE_RCZIP
