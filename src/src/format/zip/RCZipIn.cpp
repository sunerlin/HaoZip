/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "format/zip/RCZipIn.h"
#include "algorithm/CpuArch.h"
#include "common/RCDynamicBuffer.h"
#include "filesystem/RCStreamUtils.h"
#include "filesystem/RCLimitedSequentialInStream.h"
#include <memory>

#define Get16(p) GetUi16(p)
#define Get32(p) GetUi32(p)
#define Get64(p) GetUi64(p)

/////////////////////////////////////////////////////////////////
//RCZipIn class implementation

BEGIN_NAMESPACE_RCZIP

RCZipIn::RCZipIn():
    m_signature(0),
    m_streamStartPosition(0),
    m_position(0),
    m_inBufMode(false),
    m_isZip64(false),
    m_isOkHeaders(false)
{   
}

HResult RCZipIn::Open(IInStream* stream, const uint64_t* searchHeaderSizeLimit)
{
    m_inBufMode = false ;
    Close();    
    HResult hr = stream->Seek(0, RC_STREAM_SEEK_CUR, &m_streamStartPosition);
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

    hr = stream->Seek(m_position, RC_STREAM_SEEK_SET, NULL);
    if (hr != RC_S_OK)
    {
        return hr;
    }

    m_stream = stream;

    return RC_S_OK;
}

void RCZipIn::Close()
{
    m_inBuffer.ReleaseStream() ;
    m_stream.Release();
}

HResult RCZipIn::Seek(uint64_t offset)
{
    return m_stream->Seek(offset, RC_STREAM_SEEK_SET, NULL);
}

//////////////////////////////////////
// Markers

static inline bool TestMarkerCandidate(const byte_t* p, uint32_t& value)
{
    value = Get32(p);

    return (value == RCZipSignature::kLocalFileHeader) ||
           (value == RCZipSignature::kEndOfCentralDir);
}

static const uint32_t g_numMarkerAddtionalBytes = 2;

static inline bool TestMarkerCandidate2(const byte_t* p, uint32_t& value)
{
    value = Get32(p);
    if (value == RCZipSignature::kEndOfCentralDir)
    {
        return (Get16(p + 4) == 0);
    }

    return (value == RCZipSignature::kLocalFileHeader && p[4] < 128);
}

HResult RCZipIn::FindAndReadMarker(IInStream* stream, const uint64_t *searchHeaderSizeLimit)
{
    m_archiveInfo.Clear();

    m_position = m_streamStartPosition;

    byte_t marker[RCZipSignature::kMarkerSize];

    HResult hr = RCStreamUtils::ReadStream_FALSE(stream, marker, RCZipSignature::kMarkerSize);
    if (hr != RC_S_OK)
    {
        return hr;
    }

    m_position += RCZipSignature::kMarkerSize;

    if (TestMarkerCandidate(marker, m_signature))
    {
        return RC_S_OK;
    }

    RCDynamicByteBuffer dynamicBuffer;
    const uint32_t kSearchMarkerBufferSize = 0x10000;
    dynamicBuffer.EnsureCapacity(kSearchMarkerBufferSize);

    byte_t *buffer = dynamicBuffer.data();
    uint32_t numBytesPrev = RCZipSignature::kMarkerSize - 1;
    memcpy(buffer, marker + 1, numBytesPrev);

    uint64_t curTestPos = m_streamStartPosition + 1;

    for (;;)
    {
        if (searchHeaderSizeLimit != NULL)
        {
            if (curTestPos - m_streamStartPosition > *searchHeaderSizeLimit)
            {
                break;
            }
        }

        size_t numReadBytes = kSearchMarkerBufferSize - numBytesPrev;

        HResult hr = RCStreamUtils::ReadStream(stream, buffer + numBytesPrev, &numReadBytes);
        if (hr != RC_S_OK)
        {
            return hr;
        }

        m_position += numReadBytes;

        uint32_t numBytesInBuffer   = numBytesPrev + (uint32_t)numReadBytes;
        const uint32_t kMarker2Size = RCZipSignature::kMarkerSize + g_numMarkerAddtionalBytes;
        if (numBytesInBuffer < kMarker2Size)
        {
            break;
        }

        uint32_t numTests = numBytesInBuffer - kMarker2Size + 1;
        for (uint32_t pos = 0; pos < numTests; pos++)
        {
            if (buffer[pos] != 0x50)
            {
                continue;
            }

            if (TestMarkerCandidate2(buffer + pos, m_signature))
            {
                curTestPos += pos;
                m_archiveInfo.m_startPosition = curTestPos;
                m_position = curTestPos + RCZipSignature::kMarkerSize;

                return RC_S_OK;
            }
        }

        curTestPos += numTests;
        numBytesPrev = numBytesInBuffer - numTests;
        memmove(buffer, buffer + numTests, numBytesPrev);
    }

    return RC_S_FALSE;
}

HResult RCZipIn::ReadBytes(void* data, uint32_t size, uint32_t* processedSize)
{
    size_t realProcessedSize = size ;
    HResult result = RC_S_OK ;
    if (m_inBufMode)
    {
        try
        {
            realProcessedSize = m_inBuffer.ReadBytes((byte_t *)data, size) ; 
        }
        catch (HResult errorCode)
        {
            return errorCode ;
        }
    }
    else
    {
        result = RCStreamUtils::ReadStream(m_stream.Get(), data, &realProcessedSize) ;
    }
    if (processedSize)
    {
        *processedSize = (uint32_t)realProcessedSize ;
    }
    m_position += realProcessedSize ;
    return result;
}

void RCZipIn::Skip(uint64_t num)
{
    for (uint64_t i = 0; i < num; ++i)
    {
        ReadByte() ;
    }
}

void RCZipIn::IncreaseRealPosition(uint64_t addValue)
{
    if (m_stream->Seek(addValue, RC_STREAM_SEEK_CUR, &m_position) != RC_S_OK)
    {
        _ThrowCode(RC_E_SeekStreamError) ;
    }
}

bool RCZipIn::ReadBytesAndTestSize(void* data, uint32_t size)
{
    uint32_t realProcessedSize;

    if (ReadBytes(data, size, &realProcessedSize) != RC_S_OK)
    {
        _ThrowCode(RC_E_ReadStreamError) ;
    }

    return (realProcessedSize == size);
}

void RCZipIn::SafeReadBytes(void* data, uint32_t size)
{
    if (!ReadBytesAndTestSize(data, size))
    {
        _ThrowCode(RC_E_UnexpectedEndOfArchive) ;
    }
}

void RCZipIn::ReadBuffer(RCByteBuffer& buffer, uint32_t size)
{
    buffer.SetCapacity(size);
    if (size > 0)
    {
        SafeReadBytes(buffer.data(), size);
    }
}

byte_t RCZipIn::ReadByte()
{
    byte_t b;
    SafeReadBytes(&b, 1);
    return b;
}

uint16_t RCZipIn::ReadUInt16()
{
    byte_t buf[2];
    SafeReadBytes(buf, 2);
    return Get16(buf);
}

uint32_t RCZipIn::ReadUInt32()
{
    byte_t buf[4];
    SafeReadBytes(buf, 4);
    return Get32(buf) ;
}

uint64_t RCZipIn::ReadUInt64()
{
    byte_t buf[8];
    SafeReadBytes(buf, 8) ;
    return Get64(buf) ;
}

bool RCZipIn::ReadUInt32(uint32_t& value)
{
    byte_t buf[4];
    if (!ReadBytesAndTestSize(buf, 4))
    {
        return false;
    }
    value = Get32(buf) ;
    return true;
}

void RCZipIn::ReadFileName(uint32_t nameSize, RCStringA& dest)
{
    if (nameSize == 0)
    {
        dest.clear() ;
    }
    else
    {
        char* p = new char[nameSize + 1] ;
        std::auto_ptr<char> spBuf(p) ;
        SafeReadBytes(p, nameSize) ;
        p[nameSize] = 0;
        dest = p ;
    }
}

void RCZipIn::GetArchiveInfo(RCZipInArchiveInfo& archiveInfo) const
{
    archiveInfo = m_archiveInfo;
}

void RCZipIn::ReadExtra(uint32_t extraSize,
                        RCZipExtraBlock& extraBlock,
                        uint64_t& unpackSize,
                        uint64_t& packSize,
                        uint64_t& localHeaderOffset,
                        uint32_t& diskStartNumber)
{
    extraBlock.Clear();

    uint32_t remain = extraSize;

    while (remain >= 4)
    {
        RCZipExtraSubBlock subBlock;
        subBlock.m_id = ReadUInt16();
        uint32_t dataSize = ReadUInt16();
        remain -= 4;

        if (dataSize > remain) // it's bug
        {
            dataSize = remain;
        }

        if (subBlock.m_id == RCZipHeader::NExtraID::kZip64)
        {
            if (unpackSize == 0xFFFFFFFF)
            {
                if (dataSize < 8)
                {
                    break;
                }
                unpackSize = ReadUInt64();
                remain -= 8;
                dataSize -= 8;
            }

            if (packSize == 0xFFFFFFFF)
            {
                if (dataSize < 8)
                {
                    break;
                }
                packSize = ReadUInt64();
                remain -= 8;
                dataSize -= 8;
            }

            if (localHeaderOffset == 0xFFFFFFFF)
            {
                if (dataSize < 8)
                {
                    break;
                }
                localHeaderOffset = ReadUInt64();
                remain -= 8;
                dataSize -= 8;
            }

            if (diskStartNumber == 0xFFFF)
            {
                if (dataSize < 4)
                {
                    break;
                }
                diskStartNumber = ReadUInt32();
                remain -= 4;
                dataSize -= 4;
            }

            for (uint32_t i = 0; i < dataSize; i++)
            {
                ReadByte();
            }
        }
        else
        {
            ReadBuffer(subBlock.m_data, dataSize);
            extraBlock.m_subBlocks.push_back(subBlock);
        }

        remain -= dataSize;
    }
    Skip(remain) ;
}

HResult RCZipIn::ReadLocalItem(RCZipItemEx& item)
{
    const int32_t kBufSize = 26 ;
    byte_t p[kBufSize] ;
    SafeReadBytes(p, kBufSize) ;

    item.m_extractVersion.m_version     = p[0];
    item.m_extractVersion.m_hostOS      = p[1];
    item.m_flags                      = Get16(p + 2);
    item.m_compressionMethod          = Get16(p + 4);
    item.m_time                       = Get32(p + 6);
    item.m_fileCRC                    = Get32(p + 10);
    item.m_packSize                   = Get32(p + 14);
    item.m_unPackSize                 = Get32(p + 18);
    uint32_t fileNameSize             = Get16(p + 22);
    item.m_localExtraSize             = Get16(p + 24);
    
    ReadFileName(fileNameSize, item.m_name);
    item.m_fileHeaderWithNameSize = 4 + RCZipHeader::kLocalBlockSize + fileNameSize ;
    if (item.m_localExtraSize > 0)
    {
        uint64_t localHeaderOffset = 0;
        uint32_t diskStartNumber   = 0;
        ReadExtra(item.m_localExtraSize, 
                  item.m_localExtra,
                  item.m_unPackSize, 
                  item.m_packSize,
                  localHeaderOffset, 
                  diskStartNumber) ;
    }
    /*
    if (item.IsDir())
        item.UnPackSize = 0;    // check It
    */
    return RC_S_OK ;
}


static bool FlagsAreSame(RCZipItem& i1, RCZipItem& i2)
{
    if (i1.m_compressionMethod != i2.m_compressionMethod)
    {
        return false;
    }

    if (i1.m_flags == i2.m_flags)
    {
        return true;
    }
    uint32_t mask = 0xFFFF ;
    switch(i1.m_compressionMethod)
    {
    case RCZipHeader::NCompressionMethod::kDeflated :
        mask = 0x7FF9 ;
        break;
    default:
        if (i1.m_compressionMethod <= RCZipHeader::NCompressionMethod::kImploded)
        {
            mask = 0x7FFF ;
        }
        break ;
    }
    return ((i1.m_flags & mask) == (i2.m_flags & mask)) ;
}

HResult RCZipIn::ReadLocalItemAfterCdItem(RCZipItemEx& item)
{
    if (item.m_fromLocal)
    {
        return RC_S_OK;
    }
 
    try
    {
        HResult hr = Seek(m_archiveInfo.m_base + item.m_localHeaderPosition);
        if (hr != RC_S_OK)
        {
            return hr;
        }
      
        if (ReadUInt32() != RCZipSignature::kLocalFileHeader)
        {
            return RC_S_FALSE;
        }
        RCZipItemEx localItem ;
        hr = ReadLocalItem(localItem);
        if (hr != RC_S_OK)
        {
            return hr;
        }

        if (!FlagsAreSame(item, localItem))
        {
            return RC_S_FALSE ;
        }

        if ( (!localItem.HasDescriptor() &&
               ( item.m_fileCRC != localItem.m_fileCRC ||
                 item.m_packSize != localItem.m_packSize ||
                 item.m_unPackSize != localItem.m_unPackSize
               )
              ) ||
              item.m_name.size() != localItem.m_name.size())
      {
          return RC_S_FALSE;
      }

      item.m_fileHeaderWithNameSize = localItem.m_fileHeaderWithNameSize;
      item.m_localExtraSize = localItem.m_localExtraSize;
      item.m_localExtra     = localItem.m_localExtra;
      item.m_fromLocal      = true;
  }
  catch(...) 
  {
      return RC_S_FALSE; 
  }

  return RC_S_OK;
}

HResult RCZipIn::ReadLocalItemDescriptor(RCZipItemExPtr& item)
{
    if (item->HasDescriptor())
    {
        const int32_t kBufferSize = (1 << 12);
        byte_t buffer[kBufferSize];

        uint32_t numBytesInBuffer = 0;
        uint32_t packedSize = 0;

        bool descriptorWasFound = false;

        for (;;)
        {
            uint32_t processedSize;
            HResult hr = ReadBytes(buffer + numBytesInBuffer,
                                   kBufferSize - numBytesInBuffer,
                                   &processedSize);
            if (hr != RC_S_OK)
            {
                return hr;
            }

            numBytesInBuffer += processedSize;
            if (numBytesInBuffer < RCZipHeader::kDataDescriptorSize)
            {
                return RC_S_FALSE;
            }

            uint32_t i;
            for (i = 0; i <= numBytesInBuffer - RCZipHeader::kDataDescriptorSize; i++)
            {
                // descriptorSignature field is Info-ZIP's extension
                // to Zip specification.
                uint32_t descriptorSignature = Get32(buffer + i);

                // !!!! It must be fixed for Zip64 archives
                uint32_t descriptorPackSize = Get32(buffer + i + 8);

                if (descriptorSignature == RCZipSignature::kDataDescriptor && 
                    descriptorPackSize  == packedSize + i)
                {
                    descriptorWasFound = true;
                    item->m_fileCRC = Get32(buffer + i + 4);
                    item->m_packSize = descriptorPackSize;
                    item->m_unPackSize = Get32(buffer + i + 12);

                    IncreaseRealPosition(int64_t(int32_t(0 - (numBytesInBuffer - i - RCZipHeader::kDataDescriptorSize))));
                    
                    break;
                }
            }

            if (descriptorWasFound)
            {
                break;
            }

            packedSize += i;
            int32_t j;
            for (j = 0; i < numBytesInBuffer; i++, j++)
            {
                buffer[j] = buffer[i];
            }

            numBytesInBuffer = j;
        }
    }
    else
    {
        IncreaseRealPosition(item->m_packSize);
    }

    return RC_S_OK;
}

HResult RCZipIn::ReadLocalItemAfterCdItemFull(RCZipItemEx& item)
{
    if (item.m_fromLocal)
    {
        return RC_S_OK;
    }

    try
    {
        HResult hr = ReadLocalItemAfterCdItem(item);
        if (hr != RC_S_OK)
        {
            return hr;
        }

        if (item.HasDescriptor())
        {
            hr = Seek(m_archiveInfo.m_base + item.GetDataPosition() + item.m_packSize);
            if (hr != RC_S_OK)
            {
                return hr;
            }

            if (ReadUInt32() != RCZipSignature::kDataDescriptor)
            {
                return RC_S_FALSE;
            }

            uint32_t crc = ReadUInt32();
            uint64_t packSize = 0 ;
            uint64_t unpackSize = 0 ;

            /*
            if (IsZip64)
            {
            packSize = ReadUInt64();
            unpackSize = ReadUInt64();
            }
            else
            */
            {
                packSize   = ReadUInt32();
                unpackSize = ReadUInt32();
            }

            if (crc != item.m_fileCRC || item.m_packSize != packSize || item.m_unPackSize != unpackSize)
            {
                return RC_S_FALSE;
            }
        }
    }
    catch(...) 
    { 
        return RC_S_FALSE; 
    }

    return RC_S_OK;
}

HResult RCZipIn::ReadCdItem(RCZipItemEx& item)
{
    item.m_fromCentral = true;
    const int32_t kBufSize = 42;
    byte_t p[kBufSize];

    SafeReadBytes(p, kBufSize);

    item.m_madeByVersion.m_version  = p[0];
    item.m_madeByVersion.m_hostOS   = p[1];
    item.m_extractVersion.m_version = p[2];
    item.m_extractVersion.m_hostOS  = p[3];

    item.m_flags = Get16(p + 4);
    item.m_compressionMethod = Get16(p + 6);
    item.m_time       = Get32(p + 8);
    item.m_fileCRC    = Get32(p + 12);
    item.m_packSize   = Get32(p + 16);
    item.m_unPackSize = Get32(p + 20);

    uint16_t headerNameSize        = Get16(p + 24);
    uint16_t headerExtraSize       = Get16(p + 26);
    uint16_t headerCommentSize     = Get16(p + 28);
    uint32_t headerDiskNumberStart = Get16(p + 30);

    item.m_internalAttributes  = Get16(p + 32);
    item.m_externalAttributes  = Get32(p + 34);
    item.m_localHeaderPosition = Get32(p + 38);

    ReadFileName(headerNameSize, item.m_name);

    if (headerExtraSize > 0)
    {
        ReadExtra(headerExtraSize,
                  item.m_centralExtra,
                  item.m_unPackSize,
                  item.m_packSize,
                  item.m_localHeaderPosition,
                  headerDiskNumberStart);
    }

    if (headerDiskNumberStart != 0)
    {
        if (m_stream)
        {   //如果是分卷,取得分卷大小
            IMultiStreamPtr spMultiStream;
            uint64_t volumeSize;
            HResult hr = m_stream->QueryInterface(IMultiStream::IID, (void**)spMultiStream.GetAddress());
            if (!IsSuccess(hr))
            {
                _ThrowCode(RC_E_MultiVolumeArchiveAreNotSupported) ;
            }
            for (uint32_t i = 0; i < headerDiskNumberStart; i++) 
            {
                volumeSize = 0;
                hr = spMultiStream->GetVolumeSize(i, volumeSize);
                if (!IsSuccess(hr))
                {
                    _ThrowCode(RC_E_MultiVolumeArchiveAreNotSupported) ;
                }
                item.m_localHeaderPosition += volumeSize;
            }
        }
        else
        {
            _ThrowCode(RC_E_MultiVolumeArchiveAreNotSupported) ;
        }
    }

    // May be these strings must be deleted
    /*
    if (item->IsDir())
    item->UnPackSize = 0;
    */

    ReadBuffer(item.m_comment, headerCommentSize);

    return RC_S_OK;
}

HResult RCZipIn::TryEcd64(uint64_t offset, RCZipCdInfo& cdInfo)
{
    HResult hr = Seek(offset);
    if (hr != RC_S_OK)
    {
        return hr;
    }

    const uint32_t kEcd64Size = 56;
    byte_t buf[kEcd64Size];
    if (!ReadBytesAndTestSize(buf, kEcd64Size))
    {
        return RC_S_FALSE;
    }

    if (Get32(buf) != RCZipSignature::kZip64EndOfCentralDir)
    {
        return RC_S_FALSE;
    }

    // cdInfo.NumEntries = GetUInt64(buf + 24);
    uint32_t startDisk = Get32(buf + 20);
    cdInfo.m_size   = Get64(buf + 40);
    cdInfo.m_offset = Get64(buf + 48);

    if (startDisk > 0)
    {
        IMultiStreamPtr spMultiStream;
        uint64_t volumeTotalSize = 0;
        HResult hr = m_stream->QueryInterface(IMultiStream::IID, (void**)spMultiStream.GetAddress());
        if (!IsSuccess(hr))
        {
            return RC_S_FALSE;
        }
        hr = spMultiStream->GetVolumeTotalSize(startDisk, volumeTotalSize);
        if (!IsSuccess(hr))
        {
            return RC_S_FALSE;
        }
        cdInfo.m_offset += volumeTotalSize;
    }

    return RC_S_OK;
}

HResult RCZipIn::FindCd(RCZipCdInfo& cdInfo)
{
    uint64_t endPosition;
    HResult hr = m_stream->Seek(0, RC_STREAM_SEEK_END, &endPosition);
    if (hr != RC_S_OK)
    {
        return hr;
    }

    const uint32_t kBufSizeMax = (1 << 16) + g_ecdSize + g_zip64EcdLocatorSize;
    
    RCByteBuffer byteBuffer;
    byteBuffer.SetCapacity(kBufSizeMax);
    byte_t* buf = byteBuffer.data() ;
  
    uint32_t bufSize = (endPosition < kBufSizeMax) ? (uint32_t)endPosition : kBufSizeMax;
    if (bufSize < g_ecdSize)
    {
        return RC_S_FALSE;
    }

    uint64_t startPosition = endPosition - bufSize;
    hr = m_stream->Seek(startPosition, RC_STREAM_SEEK_SET, &m_position);
    if (hr != RC_S_OK)
    {
        return hr;
    }

    if (m_position != startPosition)
    {
        return RC_S_FALSE;
    }

    if (!ReadBytesAndTestSize(buf, bufSize))
    {
        return RC_S_FALSE;
    }

    for (int32_t i = (int32_t)(bufSize - g_ecdSize); i >= 0; i--)
    {
        if (Get32(buf + i) == RCZipSignature::kEndOfCentralDir)
        {
            if (i >= g_zip64EcdLocatorSize)
            {
                const byte_t *locator = buf + i - g_zip64EcdLocatorSize;
                if (Get32(locator) == RCZipSignature::kZip64EndOfCentralDirLocator)
                {
                    uint32_t ecd64Disk = Get32(locator + 4);
                    uint64_t ecd64Offset = Get64(locator + 8);
                    if (TryEcd64(ecd64Offset, cdInfo) == RC_S_OK)
                    {
                        return RC_S_OK;
                    }

                    if (TryEcd64(m_archiveInfo.m_startPosition + ecd64Offset, cdInfo) == RC_S_OK)
                    {
                        m_archiveInfo.m_base = m_archiveInfo.m_startPosition;
                        return RC_S_OK;
                    }
                    if (ecd64Disk > 0)
                    {   //开始磁盘编号 > 0, 为分卷zip64
                        IMultiStreamPtr spMultiStream;
                        HResult hr = m_stream->QueryInterface(IMultiStream::IID, (void**)spMultiStream.GetAddress());
                        if (!IsSuccess(hr))
                        {
                            return RC_S_FALSE;
                        }
                        uint64_t preSize = 0;
                        hr = spMultiStream->GetVolumeTotalSize(ecd64Disk, preSize);
                        if (!IsSuccess(hr))
                        {
                            return RC_S_FALSE;
                        }
                        if (TryEcd64(ecd64Offset + preSize, cdInfo) == RC_S_OK)
                        {
                            return RC_S_OK;
                        }
                    }
                }
            }

            if (Get32(buf + i + 4) == 0)
            {
                // cdInfo.NumEntries = GetUInt16(buf + i + 10);
                cdInfo.m_size     = Get32(buf + i + 12);
                cdInfo.m_offset   = Get32(buf + i + 16);
                uint64_t curPos = endPosition - bufSize + i;
                uint64_t cdEnd  = cdInfo.m_size + cdInfo.m_offset;

                if (curPos > cdEnd)
                {
                    m_archiveInfo.m_base = curPos - cdEnd;
                }

                return RC_S_OK;
            }
        }
    }

    return RC_S_FALSE;
}

HResult RCZipIn::TryReadCd(RCVector <RCZipItemExPtr>& items,
                           uint64_t cdOffset,
                           uint64_t cdSize,
                           RCZipProgressVirt *progress)
{
    items.clear();

    HResult hr = m_stream->Seek(cdOffset, RC_STREAM_SEEK_SET, &m_position);
    if (hr != RC_S_OK)
    {
        return hr;
    }

    if (m_position != cdOffset)
    {
        return RC_S_FALSE;
    }
    if (!m_inBuffer.Create(1 << 15))
    {
        return RC_E_OUTOFMEMORY ;
    }
    m_inBuffer.SetStream(m_stream.Get());
    m_inBuffer.Init();
    m_inBufMode = true;

    while(m_position - cdOffset < cdSize)
    {
        if (ReadUInt32() != RCZipSignature::kCentralFileHeader)
        {
            return RC_S_FALSE;
        }

        RCZipItemExPtr cdItem(new RCZipItemEx);
        if (!cdItem)
        {
            return RC_S_FALSE;
        }

        hr = ReadCdItem(*cdItem);
        if (hr != RC_S_OK)
        {
            return hr;
        }

        items.push_back(cdItem);
        
        if (progress && items.size() % 1000 == 0)
        {
            hr = progress->SetCompleted(items.size());
            if (hr != RC_S_OK)
            {
                return hr;
            }
        }
    }

    return (m_position - cdOffset == cdSize) ? RC_S_OK : RC_S_FALSE;
}

HResult RCZipIn::ReadCd(RCVector<RCZipItemExPtr>& items,
                        uint64_t& cdOffset,
                        uint64_t& cdSize,
                        RCZipProgressVirt* progress)
{
    m_archiveInfo.m_base = 0;
    RCZipCdInfo cdInfo;

    HResult hr = FindCd(cdInfo);
    if (hr != RC_S_OK)
    {
        return hr;
    }

    hr = RC_S_FALSE;

    cdSize   = cdInfo.m_size;
    cdOffset = cdInfo.m_offset;

    hr = TryReadCd(items, m_archiveInfo.m_base + cdOffset, cdSize, progress);
    if (hr == RC_S_FALSE && m_archiveInfo.m_base == 0)
    {
        hr = TryReadCd(items, cdInfo.m_offset + m_archiveInfo.m_startPosition, cdSize, progress);
        if (hr == RC_S_OK)
        {
            m_archiveInfo.m_base = m_archiveInfo.m_startPosition;
        }
    }

    if (!ReadUInt32(m_signature))
    {
        return RC_S_FALSE;
    }

    return hr;
}

HResult RCZipIn::ReadLocalsAndCd(RCVector<RCZipItemExPtr>& items,
                                 RCZipProgressVirt* progress,
                                 uint64_t& cdOffset,
                                 int32_t& numCdItems)
{
    items.clear();
    numCdItems = 0 ;
    
    while (m_signature == RCZipSignature::kLocalFileHeader)
    {
        // FSeek points to next byte after signature
        // RCZipHeader::CLocalBlock localHeader;
        RCZipItemExPtr item(new RCZipItemEx);
        if (!item)
        {
            return RC_S_FALSE;
        }

        item->m_localHeaderPosition = m_position - m_streamStartPosition - 4; // points to signature;

        HResult hr = ReadLocalItem(*item);
        if (hr != RC_S_OK)
        {
            return hr;
        }

        item->m_fromLocal = true;
        ReadLocalItemDescriptor(item);
        items.push_back(item);

        if (progress && items.size() % 100 == 0)
        {
            hr = progress->SetCompleted(items.size());
            if (hr != RC_S_OK)
            {
                return hr;
            }
        }

        if (!ReadUInt32(m_signature))
        {
            break;
        }
    }

    cdOffset = m_position - 4;
    int32_t i = 0 ;
    for (i = 0; i < (int32_t)items.size(); ++i, ++numCdItems)
    {
        if (progress && i % 1000 == 0)
        {
            HResult hr = progress->SetCompleted(items.size());
            if (hr != RC_S_OK)
            {
                return hr;
            }
        }
        if (m_signature == RCZipSignature::kEndOfCentralDir)
        {
            break ;
        }

        if (m_signature != RCZipSignature::kCentralFileHeader)
        {
            return RC_S_FALSE;
        }

        RCZipItemEx cdItem ;
        HResult hr = ReadCdItem(cdItem);
        if (hr != RC_S_OK)
        {
            return hr;
        }

        if (i == 0)
        {
            int32_t j = 0 ;
            for (j = 0; j < (int32_t)items.size(); ++j)
            {
                RCZipItemEx& item = *(items[j]) ;
                if (item.m_name == cdItem.m_name)
                {
                    m_archiveInfo.m_base = item.m_localHeaderPosition - cdItem.m_localHeaderPosition ;
                    break ;
                }
            }
            if (j == (int32_t)items.size())
            {
                return RC_S_FALSE ;
            }
        }

        int32_t index = 0 ;
        int32_t left  = 0;
        int32_t right = (int32_t)items.size();

        for (;;)
        {
            if (left >= right)
            {
                return RC_S_FALSE;
            }

            index = (left + right) / 2;
            uint64_t position = items[index]->m_localHeaderPosition - m_archiveInfo.m_base;

            if (cdItem.m_localHeaderPosition == position)
            {
                break;
            }

            if (cdItem.m_localHeaderPosition < position)
            {
                right = index;
            }
            else
            {
                left = index + 1;
            }
        }

        RCZipItemExPtr& item = items[index];
        //item->LocalHeaderPosition = cdItem->LocalHeaderPosition;
        item->m_madeByVersion = cdItem.m_madeByVersion;
        item->m_centralExtra  = cdItem.m_centralExtra;

        if ( // item->ExtractVersion != cdItem->ExtractVersion ||
            !FlagsAreSame(*item, cdItem) ||
            item->m_fileCRC != cdItem.m_fileCRC)
        {
            return RC_S_FALSE;
        }

        if (item->m_name.length() != cdItem.m_name.length() ||
            item->m_packSize      != cdItem.m_packSize ||
            item->m_unPackSize    != cdItem.m_unPackSize)
        {
            return RC_S_FALSE;
        }

        item->m_name               = cdItem.m_name;
        item->m_internalAttributes = cdItem.m_internalAttributes;
        item->m_externalAttributes = cdItem.m_externalAttributes;
        item->m_comment            = cdItem.m_comment;
        item->m_fromCentral        = cdItem.m_fromCentral;

        if (!ReadUInt32(m_signature))
        {
            return RC_S_FALSE;
        }
    }
    for (i = 0; i < (int32_t)items.size(); ++i)
    {
        items[i]->m_localHeaderPosition -= m_archiveInfo.m_base ;
    }
    return RC_S_OK;
}

struct RCZipInEcd
{
    /** 当前磁盘号
    */
    uint16_t m_thisDiskNumber ;

    /** 起始磁盘号
    */
    uint16_t m_startCDDiskNumber ;

    /** 当前磁盘有多少个入口
    */
    uint16_t m_numEntriesInCDOnThisDisk ;

    /** 磁盘有多少个入口
    */
    uint16_t m_numEntriesInCD ;

    /** cd大小
    */
    uint32_t m_cdSize ;

    /** cd起始偏移
    */
    uint32_t m_cdStartOffset ;

    /** 注释大小
    */
    uint16_t m_commentSize ;

    /** 解析
    @param [in] p 内存
    */
    void Parse(const byte_t* p) ;

    /** 默认构造函数
    */
    RCZipInEcd():
        m_thisDiskNumber(0),
        m_startCDDiskNumber(0),
        m_numEntriesInCDOnThisDisk(0),
        m_numEntriesInCD(0),
        m_cdSize(0),
        m_cdStartOffset(0),
        m_commentSize(0)
    {
    }
};

void RCZipInEcd::Parse(const byte_t* p)
{
    m_thisDiskNumber           = Get16(p);
    m_startCDDiskNumber        = Get16(p + 2);
    m_numEntriesInCDOnThisDisk = Get16(p + 4);

    m_numEntriesInCD = Get16(p + 6);
    m_cdSize         = Get32(p + 8);
    m_cdStartOffset  = Get32(p + 12);
    m_commentSize    = Get16(p + 16);
}

struct RCZipInEcd64
{
    /** 制作版本
    */
    uint16_t m_versionMade ;

    /** 解压版本
    */
    uint16_t m_versionNeedExtract ;

    /** 当前磁盘号
    */
    uint32_t m_thisDiskNumber ;

    /** 起始磁盘号
    */
    uint32_t m_startCDDiskNumber ;

    /** 当前磁盘有多少个入口
    */
    uint64_t m_numEntriesInCDOnThisDisk ;

    /** 磁盘有多少个入口
    */
    uint64_t m_numEntriesInCD ;

    /** cd大小
    */
    uint64_t m_cdSize ;

    /** cd起始偏移
    */
    uint64_t m_cdStartOffset;

    /** 解析
    @param [in] p 内存
    */
    void Parse(const byte_t *p) ;

    /** 默认构造函数
    */
    RCZipInEcd64()
    {
        memset(this, 0, sizeof(*this));
    }
};

void RCZipInEcd64::Parse(const byte_t* p)
{
    m_versionMade        = Get16(p);
    m_versionNeedExtract = Get16(p + 2);
    m_thisDiskNumber     = Get32(p + 4);
    m_startCDDiskNumber  = Get32(p + 8);

    m_numEntriesInCDOnThisDisk = Get64(p + 12);

    m_numEntriesInCD = Get64(p + 20);
    m_cdSize         = Get64(p + 28);
    m_cdStartOffset  = Get64(p + 36);
}

#define COPY_ECD_ITEM_16(n) if (!isZip64 || ecd. n != 0xFFFF)     ecd64. n = ecd. n;
#define COPY_ECD_ITEM_32(n) if (!isZip64 || ecd. n != 0xFFFFFFFF) ecd64. n = ecd. n;

HResult RCZipIn::ReadHeaders(RCVector<RCZipItemExPtr>& items, RCZipProgressVirt* progress)
{
    // m_Signature must be kLocalFileHeaderSignature or
    // kEndOfCentralDirSignature
    // m_Position points to next byte after signature

    m_isZip64 = false;
    items.clear();

    uint64_t cdSize = 0 ;
    uint64_t cdStartOffset = 0 ;
    
    HResult hr = RC_S_OK ;
    try
    {
        hr = ReadCd(items, cdStartOffset, cdSize, progress);
    }
    catch(HResult)
    {
        hr = RC_S_FALSE ;
    }
    
    if (hr != RC_S_FALSE && hr != RC_S_OK)
    {
        return hr;
    }

    m_archiveInfo.m_commentSizePosition = cdStartOffset + cdSize;

    /*
    if (hr != RC_S_OK)
    {
        return hr;
    }
    hr = RC_S_FALSE;
    */

    int32_t numCdItems = (int32_t)items.size() ;
    if (hr == RC_S_FALSE)
    {
        m_inBufMode = false ;
        m_archiveInfo.m_base = 0;
        hr = m_stream->Seek(m_archiveInfo.m_startPosition, RC_STREAM_SEEK_SET, &m_position);
        if (hr != RC_S_OK)
        {
            return hr;
        }

        if (m_position != m_archiveInfo.m_startPosition)
        {
            return RC_S_FALSE;
        }

        if (!ReadUInt32(m_signature))
        {
            return RC_S_FALSE;
        }

        hr = ReadLocalsAndCd(items, progress, cdStartOffset, numCdItems);
        if (hr != RC_S_OK)
        {
            return hr;
        }

        cdSize = (m_position - 4) - cdStartOffset;
        cdStartOffset -= m_archiveInfo.m_base;
    }

    RCZipInEcd64 ecd64;
    bool isZip64 = false;
    uint64_t zip64EcdStartOffset = m_position - 4 - m_archiveInfo.m_base;

    if (m_signature == RCZipSignature::kZip64EndOfCentralDir)
    {
        m_isZip64 = true;
        isZip64 = true;
        uint64_t recordSize = ReadUInt64();

        const int32_t kBufSize = g_zip64EcdSize;
        byte_t buf[kBufSize];
        SafeReadBytes(buf, kBufSize);
        ecd64.Parse(buf);

        Skip(recordSize - g_zip64EcdSize);
        if (!ReadUInt32(m_signature))
        {
            return RC_S_FALSE;
        }

        if (ecd64.m_thisDiskNumber != 0 || ecd64.m_startCDDiskNumber != 0)
        {
            if (m_stream)
            {
                IMultiStreamPtr spMultiStream;
                uint64_t volTotalSize = 0;
                HResult hr = m_stream->QueryInterface(IMultiStream::IID, (void**)spMultiStream.GetAddress());
                if (!IsSuccess(hr))
                {
                    return RC_S_FALSE;
                }
                hr = spMultiStream->GetVolumeTotalSize(ecd64.m_startCDDiskNumber, volTotalSize);
                if (!IsSuccess(hr))
                {
                    return RC_S_FALSE;
                }
                ecd64.m_cdStartOffset += volTotalSize;
            }
            else
            {
                _ThrowCode(RC_E_MultiVolumeArchiveAreNotSupported) ;
            }
        }

        if (ecd64.m_numEntriesInCDOnThisDisk != numCdItems)
        {
            ecd64.m_numEntriesInCDOnThisDisk = numCdItems ;
        }
        if(ecd64.m_numEntriesInCD != numCdItems ||
           ecd64.m_cdSize != cdSize ||
           (ecd64.m_cdStartOffset != cdStartOffset &&
           (!items.empty())))
        {
            return RC_S_FALSE;
        }
    }

    if (m_signature == RCZipSignature::kZip64EndOfCentralDirLocator)
    {
        /* uint32_t startEndCDDiskNumber = */
        uint64_t startDisk = ReadUInt32();
        uint64_t endCDStartOffset = ReadUInt64();
        /* uint32_t numberOfDisks = */
        ReadUInt32();

        if (startDisk > 0)
        {
            IMultiStreamPtr spMultiStream;
            uint64_t volumeSize;
            HResult hr = m_stream->QueryInterface(IMultiStream::IID, (void**)spMultiStream.GetAddress());
            if (!IsSuccess(hr))
            {
                return RC_S_FALSE;
            }
            hr = spMultiStream->GetVolumeTotalSize(static_cast<uint32_t>(startDisk), volumeSize);
            if (!IsSuccess(hr))
            {
                return RC_S_FALSE;
            }
            endCDStartOffset += volumeSize;
        }

        if (zip64EcdStartOffset != endCDStartOffset)
        {
            return RC_S_FALSE;
        }

        if (!ReadUInt32(m_signature))
        {
            return RC_S_FALSE;
        }
    }

    if (m_signature == RCZipSignature::kZip64EndOfCentralDirLocator)
    {
        m_archiveInfo.m_commentSizePosition += 76;
    }

    if (m_signature != RCZipSignature::kEndOfCentralDir)
    {
        return RC_S_FALSE;
    }

    m_archiveInfo.m_commentSizePosition += 20;

    const int32_t kBufSize = g_ecdSize - 4;
    byte_t buf[kBufSize];
    SafeReadBytes(buf, kBufSize);

    RCZipInEcd ecd;
    ecd.Parse(buf);

    if (!isZip64)
    {
        COPY_ECD_ITEM_16(m_thisDiskNumber);
        COPY_ECD_ITEM_16(m_startCDDiskNumber);
        COPY_ECD_ITEM_16(m_numEntriesInCDOnThisDisk);
        COPY_ECD_ITEM_16(m_numEntriesInCD);
        COPY_ECD_ITEM_32(m_cdSize);
        COPY_ECD_ITEM_32(m_cdStartOffset);
    }

    ReadBuffer(m_archiveInfo.m_comment, ecd.m_commentSize);

    if (ecd64.m_thisDiskNumber != 0 || ecd64.m_startCDDiskNumber != 0)
    {
        if (m_stream)
        {
            IMultiStreamPtr spMultiStream;
            uint64_t volumeSize;
            HResult hr = m_stream->QueryInterface(IMultiStream::IID, (void**)spMultiStream.GetAddress());
            if (!IsSuccess(hr))
            {
                _ThrowCode(RC_E_MultiVolumeArchiveAreNotSupported) ;
            }
            /*
            //for (uint32_t i = 0; i < ecd.thisDiskNumber; i++)
            for (uint32_t i = 0; i < ecd.startCDDiskNumber; i++)
            {
                volumeSize = 0;
                hr = spMultiStream->GetVolumeSize(i, volumeSize);
                if (!IsSuccess(hr))
                {
                    _ThrowCode(RC_E_MultiVolumeArchiveAreNotSupported) ;
                }
                ecd64.cdStartOffset += volumeSize;
            }
            */
            hr = spMultiStream->GetVolumeTotalSize(ecd.m_startCDDiskNumber, volumeSize);
            if (!IsSuccess(hr))
            {
                _ThrowCode(RC_E_MultiVolumeArchiveAreNotSupported) ;
            }
            if (ecd64.m_cdStartOffset != cdStartOffset)
            {
                ecd64.m_cdStartOffset += volumeSize;
            }
        }
        else
        {
            _ThrowCode(RC_E_MultiVolumeArchiveAreNotSupported) ;
        }
    }

    if((uint16_t)ecd64.m_numEntriesInCDOnThisDisk != ((uint16_t)numCdItems))
    {
        ecd64.m_numEntriesInCDOnThisDisk = (uint16_t)numCdItems ;
    }

    if ((uint16_t)ecd64.m_numEntriesInCD         != (uint16_t)numCdItems ||
        (uint32_t)ecd64.m_cdSize                 != (uint32_t)cdSize ||
        ((uint32_t)ecd64.m_cdStartOffset         != (uint32_t)cdStartOffset &&
        (!items.empty())))
    {
        return RC_S_FALSE;
    }
    
    m_inBufMode = false;
    m_inBuffer.Free();
    m_isOkHeaders = (numCdItems == (int32_t)items.size()) ;
    return RC_S_OK;
}

ISequentialInStream* RCZipIn::CreateLimitedStream(uint64_t position, uint64_t size)
{
    RCLimitedSequentialInStream *streamSpec = new RCLimitedSequentialInStream;
    ISequentialInStreamPtr stream(streamSpec);
    SeekInArchive(m_archiveInfo.m_base + position);
    streamSpec->SetStream(m_stream.Get());
    streamSpec->Init(size);
    return stream.Detach();
}

IInStream* RCZipIn::CreateStream()
{
    IInStreamPtr stream = m_stream;
    return stream.Detach();
}

bool RCZipIn::SeekInArchive(uint64_t position)
{
    uint64_t newPosition;
    if (m_stream->Seek(position, RC_STREAM_SEEK_SET, &newPosition) != RC_S_OK)
    {
        return false;
    }
    return (newPosition == position);
}

END_NAMESPACE_RCZIP
