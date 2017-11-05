/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "format/zip/RCZipOut.h"
#include "format/zip/RCZipHeader.h"
#include "filesystem/RCStreamUtils.h"
#include "filesystem/RCOffsetOutStream.h"
#include "common/RCStringUtil.h"

/////////////////////////////////////////////////////////////////
//RCZipOut class implementation

BEGIN_NAMESPACE_RCZIP

void RCZipOut::Create(IOutStream* outStream)
{
    if (!m_outBuffer.Create(1 << 16))
    {
        _ThrowCode(RC_E_OUTOFMEMORY) ;
    }

    m_stream = outStream;
    m_outBuffer.SetStream(outStream);
    m_outBuffer.Init();
    m_basePosition = 0;

    /** 检查是否为分卷输出
    */
    HResult hr = outStream->QueryInterface(IMultiVolStream::IID, (void**)m_multiVolStream.GetAddress());
    if (!IsSuccess(hr))
    {
        //不分卷
        m_isSplit = false;
    }
    else
    {
        IZipMultiVolStreamPtr zipMultiVol;
        hr = outStream->QueryInterface(IZipMultiVolStream::IID, (void**)zipMultiVol.GetAddress());
        if (!IsSuccess(hr))
        {
            _ThrowCode(RC_E_MultiVolumeArchiveAreNotSupported) ;
        }
        hr = zipMultiVol->SetToZipMultiVolFormat(true);
        if (!IsSuccess(hr))
        {
            _ThrowCode(RC_E_MultiVolumeArchiveAreNotSupported) ;
        }

        //分卷操作
        m_isSplit = true;
        WriteUInt32(RCZipSignature::kDataDescriptor);
        m_outBuffer.FlushWithCheck();
    }
}

RCZipOut::~RCZipOut()
{
    if (m_isSplit)
    {
        WriteSignleVolumeSignature();
    }
}

void RCZipOut::WriteSignleVolumeSignature()
{
    if (m_isSplit && m_multiVolStream)
    {
        uint32_t volCount = 0;
        HResult hr = m_multiVolStream->GetVolumeCount(volCount);
        if (!IsSuccess(hr))
        {
            return;
        }
        if (volCount == 1)
        {
            SeekTo(0);
            WriteUInt32(RCZipSignature::kZipSingleVolumeSignature);
            m_outBuffer.FlushWithCheck();
        }
    }
}

uint64_t RCZipOut::GetCurrentPosition() const 
{ 
    return m_basePosition; 
}

void RCZipOut::MoveBasePosition(uint64_t distanceToMove)
{
    m_basePosition += distanceToMove; // test overflow
}

void RCZipOut::PrepareWriteCompressedDataZip64(uint16_t fileNameLength, 
                                               bool isZip64,
                                               bool aesEncryption)
{
    m_isZip64 = isZip64;
    m_extraSize = isZip64 ? (4 + 8 + 8) : 0;
    if (aesEncryption)
    {
        m_extraSize += 4 + 7;
    }

    m_localFileHeaderSize = 4 + RCZipHeader::kLocalBlockSize + fileNameLength + m_extraSize;
}

void RCZipOut::PrepareWriteCompressedData(uint16_t fileNameLength,
                                          uint64_t unPackSize,
                                          bool aesEncryption)
{
    // We test it to 0xF8000000 to support case when compressed size
    // can be larger than uncompressed size.
    PrepareWriteCompressedDataZip64(fileNameLength, unPackSize >= 0xF8000000, aesEncryption);

    if (m_isSplit)
    {   //To Do 确保local header没有跨卷存储
        uint64_t freeByte = 0;
        HResult hr = m_multiVolStream->GetCurVolFreeByte(freeByte);
        if (!IsSuccess(hr))
        {
            _ThrowCode(RC_E_MultiVolumeArchiveAreNotSupported) ;
        }
        if (freeByte < m_localFileHeaderSize)
        {
            hr = m_multiVolStream->ForceStartNextVol();
            if (!IsSuccess(hr))
            {
                _ThrowCode(RC_E_MultiVolumeArchiveAreNotSupported) ;
            }
        }
    }
}

void RCZipOut::PrepareWriteCompressedData2(uint16_t fileNameLength,
                                           uint64_t unPackSize,
                                           uint64_t packSize,
                                           bool aesEncryption)
{
    bool isUnPack64 = unPackSize >= 0xFFFFFFFF;
    bool isPack64   = packSize >= 0xFFFFFFFF;
    bool isZip64    = isPack64 || isUnPack64;

    PrepareWriteCompressedDataZip64(fileNameLength, isZip64, aesEncryption);
}

void RCZipOut::WriteBytes(const void *buffer, uint32_t size)
{
    m_outBuffer.WriteBytes(buffer, size);
    m_basePosition += size;
}

void RCZipOut::WriteByte(byte_t b)
{
    WriteBytes(&b, 1);
}

void RCZipOut::WriteUInt16(uint16_t value)
{
    for (int32_t i = 0; i < 2; i++)
    {
        WriteByte((byte_t)value);
        value >>= 8;
    }
}

void RCZipOut::WriteUInt32(uint32_t value)
{
    for (int32_t i = 0; i < 4; i++)
    {
        WriteByte((byte_t)value);
        value >>= 8;
    }
}

void RCZipOut::WriteUInt64(uint64_t value)
{
    for (int32_t i = 0; i < 8; i++)
    {
        WriteByte((byte_t)value);
        value >>= 8;
    }
}

void RCZipOut::WriteExtra(const RCZipExtraBlock& extra)
{
    if (extra.m_subBlocks.size() != 0)
    {
        for (int32_t i = 0; i < (int32_t)extra.m_subBlocks.size(); i++)
        {
            const RCZipExtraSubBlock &subBlock = extra.m_subBlocks[i];

            WriteUInt16(subBlock.m_id);

            WriteUInt16((uint16_t)subBlock.m_data.GetCapacity());

            WriteBytes(subBlock.m_data.data(), (uint32_t)subBlock.m_data.GetCapacity());
        }
    }
}

void RCZipOut::SeekTo(uint64_t offset)
{
    HResult res = m_stream->Seek(offset, RC_STREAM_SEEK_SET, NULL);
    if (res != RC_S_OK)
    {
        _ThrowCode(res) ;
    }
}

void RCZipOut::WriteLocalHeader(RCZipItem& item)
{
    bool withDataDescriptor = false;
    uint64_t begin_pos = 0;

    begin_pos = m_basePosition;
    SeekTo(m_basePosition);

    if (m_isSplit)
    {   //如果是分卷, 重新填写分卷号和偏移量
        uint32_t cur_index;
        uint64_t cur_pos;
        HResult hr = m_multiVolStream->GetCurVolIndex(cur_index);
        if (IsSuccess(hr))
        {
            item.m_diskNumberStart = static_cast<uint16_t>(cur_index & 0xFFFF);
        }
        hr = m_multiVolStream->GetCurVolPosition(cur_pos);
        if (IsSuccess(hr))
        {
            item.m_localHeaderPosition = cur_pos;
        }
        uint64_t itemSize = m_localFileHeaderSize + item.m_packSize;
        bool acrossVol = false;
        hr = m_multiVolStream->CheckDataAcrossVol(begin_pos, itemSize, acrossVol);
        if (!IsSuccess(hr))
        {
            _ThrowCode(RC_E_DataError) ;
        }
        if (acrossVol)
        {
            withDataDescriptor = true;
            item.m_flags |= (uint16_t)(RCZipHeader::NFlags::kDescriptorUsedMask);
        }
    }

    bool isZip64 = m_isZip64 || item.m_packSize >= 0xFFFFFFFF || item.m_unPackSize >= 0xFFFFFFFF;

    WriteUInt32(RCZipSignature::kLocalFileHeader);

    WriteByte(item.m_extractVersion.m_version);

    WriteByte(item.m_extractVersion.m_hostOS);

    WriteUInt16(item.m_flags);

    WriteUInt16(item.m_compressionMethod);

    WriteUInt32(item.m_time);

    WriteUInt32(item.m_fileCRC);

    WriteUInt32(isZip64 ? 0xFFFFFFFF: (uint32_t)item.m_packSize);

    WriteUInt32(isZip64 ? 0xFFFFFFFF: (uint32_t)item.m_unPackSize);

    WriteUInt16((uint16_t)item.m_name.length());

    {
        uint16_t localExtraSize = (uint16_t)((isZip64 ? (4 + 16): 0) + item.m_localExtra.GetSize());
        if (localExtraSize > m_extraSize)
        {
            _ThrowCode(RC_E_WriteStreamError) ;
        }
    }

    WriteUInt16((uint16_t)m_extraSize); // test it;

    WriteBytes(item.m_name.c_str(), (uint32_t)item.m_name.length());

    uint32_t extraPos = 0;
    if (isZip64)
    {
        extraPos += 4 + 16;

        WriteUInt16(RCZipHeader::NExtraID::kZip64);

        WriteUInt16(16);

        WriteUInt64(item.m_unPackSize);

        WriteUInt64(item.m_packSize);
    }

    WriteExtra(item.m_localExtra);

    extraPos += (uint32_t)item.m_localExtra.GetSize();

    for (; extraPos < m_extraSize; extraPos++)
    {
        WriteByte(0);
    }

    m_outBuffer.FlushWithCheck();

    MoveBasePosition(item.m_packSize);

    SeekTo(m_basePosition);

    if (m_isSplit && withDataDescriptor)
    {   //生成分卷zip文件,如果单个文件跨卷存储
        //写入Data Descriptor 结构
        WriteUInt32(RCZipSignature::kDataDescriptor);
        WriteUInt32(item.m_fileCRC);
        WriteUInt32(isZip64 ? 0xFFFFFFFF: (uint32_t)item.m_packSize);
        WriteUInt32(isZip64 ? 0xFFFFFFFF: (uint32_t)item.m_unPackSize);
        m_outBuffer.FlushWithCheck();
    }
}

uint64_t RCZipOut::GetCentralHeaderSize(const RCZipItemPtr& item)
{
    uint64_t ret = 0;

    bool isUnPack64   = item->m_unPackSize >= 0xFFFFFFFF;
    bool isPack64     = item->m_packSize >= 0xFFFFFFFF;
    bool isPosition64 = item->m_localHeaderPosition >= 0xFFFFFFFF;
    bool isZip64      = isPack64 || isUnPack64 || isPosition64;

    ret = 46;
    ret += item->m_name.length();
    if (isZip64)
    {
        ret += 4;
        if (isUnPack64)
        {
            ret += 8;
        }
        if (isPack64)
        {
            ret += 8;
        }
        if (isPosition64)
        {
            ret += 8;
        }
    }
    if (item->m_ntfsTimeIsDefined)
    {
        ret += 36;
    }
    if (item->m_centralExtra.m_subBlocks.size() != 0)
    {
        for (int32_t i = 0; i < (int32_t)item->m_centralExtra.m_subBlocks.size(); i++)
        {
            const RCZipExtraSubBlock& subBlock = item->m_centralExtra.m_subBlocks[i];
            ret += 4;
            ret += subBlock.m_data.GetCapacity();
        }
    }
    ret += item->m_comment.GetCapacity();
    return ret;
}

void RCZipOut::WriteCentralHeader(const RCZipItemPtr& item)
{
    bool isUnPack64   = item->m_unPackSize >= 0xFFFFFFFF;
    bool isPack64     = item->m_packSize >= 0xFFFFFFFF;
    bool isPosition64 = item->m_localHeaderPosition >= 0xFFFFFFFF;
    bool isZip64      = isPack64 || isUnPack64 || isPosition64;

    if (m_isSplit)
    {    
        //To: 确保central header 结构不会被跨卷存储
        uint64_t headSize = GetCentralHeaderSize(item);
        uint64_t freeByte = 0;
        HResult hr = m_multiVolStream->GetCurVolFreeByte(freeByte);
        if (!IsSuccess(hr))
        {
            _ThrowCode(RC_E_MultiVolumeArchiveAreNotSupported) ;
        }
        if (freeByte < headSize)
        {
            hr = m_multiVolStream->ForceStartNextVol();
            if (!IsSuccess(hr))
            {
                _ThrowCode(RC_E_MultiVolumeArchiveAreNotSupported) ;
            }
        }
    }

    WriteUInt32(RCZipSignature::kCentralFileHeader);

    WriteByte(item->m_madeByVersion.m_version);

    WriteByte(item->m_madeByVersion.m_hostOS);

    WriteByte(item->m_extractVersion.m_version);

    WriteByte(item->m_extractVersion.m_hostOS);

    WriteUInt16(item->m_flags);

    WriteUInt16(item->m_compressionMethod);

    WriteUInt32(item->m_time);

    WriteUInt32(item->m_fileCRC);

    WriteUInt32(isPack64 ? 0xFFFFFFFF: (uint32_t)item->m_packSize);

    WriteUInt32(isUnPack64 ? 0xFFFFFFFF: (uint32_t)item->m_unPackSize);

    WriteUInt16((uint16_t)item->m_name.length());

    uint16_t zip64ExtraSize = (uint16_t)((isUnPack64 ? 8: 0) +  (isPack64 ? 8: 0) + (isPosition64 ? 8: 0));

    const uint16_t kNtfsExtraSize = 4 + 2 + 2 + (3 * 8);

    uint16_t centralExtraSize = (uint16_t)(isZip64 ? (4 + zip64ExtraSize) : 0) + (item->m_ntfsTimeIsDefined ? (4 + kNtfsExtraSize) : 0);

    centralExtraSize = (uint16_t)(centralExtraSize + item->m_centralExtra.GetSize());

    WriteUInt16(centralExtraSize); // test it;

    WriteUInt16((uint16_t)item->m_comment.GetCapacity());

    if (m_isSplit)
    {
        WriteUInt16(item->m_diskNumberStart);
    }
    else
    {
        WriteUInt16(0); // DiskNumberStart;
    }

    WriteUInt16(item->m_internalAttributes);

    WriteUInt32(item->m_externalAttributes);

    WriteUInt32(isPosition64 ? 0xFFFFFFFF: (uint32_t)item->m_localHeaderPosition);

    WriteBytes(item->m_name.c_str(), (uint32_t)item->m_name.length());

    if (isZip64)
    {
        WriteUInt16(RCZipHeader::NExtraID::kZip64);

        WriteUInt16(zip64ExtraSize);

        if(isUnPack64)
        {
            WriteUInt64(item->m_unPackSize);
        }

        if(isPack64)
        {
            WriteUInt64(item->m_packSize);
        }

        if(isPosition64)
        {
            WriteUInt64(item->m_localHeaderPosition);
        }
    }
    if (item->m_ntfsTimeIsDefined)
    {
        WriteUInt16(RCZipHeader::NExtraID::kNTFS);

        WriteUInt16(kNtfsExtraSize);

        WriteUInt32(0); // reserved

        WriteUInt16(RCZipHeader::NNtfsExtra::kTagTime);

        WriteUInt16(8 * 3);

        WriteUInt32(item->m_ntfsMTime.u32LowDateTime);

        WriteUInt32(item->m_ntfsMTime.u32HighDateTime);

        WriteUInt32(item->m_ntfsATime.u32LowDateTime);

        WriteUInt32(item->m_ntfsATime.u32HighDateTime);

        WriteUInt32(item->m_ntfsCTime.u32LowDateTime);

        WriteUInt32(item->m_ntfsCTime.u32HighDateTime);
    }

    WriteExtra(item->m_centralExtra);

    if (item->m_comment.GetCapacity() > 0)
    {
        WriteBytes(item->m_comment.data(), (uint32_t)item->m_comment.GetCapacity());
    }
}

void RCZipOut::WriteCentralDir(const RCVector<RCZipItemPtr>& items, 
                               const RCByteBuffer& comment)
{
    SeekTo(m_basePosition);

    uint32_t start_index = 0;
    uint32_t cur_index = 0;
    uint64_t cur_pos = GetCurrentPosition();
    uint64_t end_pos = 0;
    if (m_isSplit)
    {
        m_outBuffer.FlushWithCheck();
        HResult hr = m_multiVolStream->GetCurVolIndex(start_index);
        hr = m_multiVolStream->GetCurVolPosition(cur_pos);
    }

    uint64_t cdOffset = GetCurrentPosition();
    for(int32_t i = 0; i < (int32_t)items.size(); i++)
    {
        WriteCentralHeader(items[i]);
        if (m_isSplit)
        {
            m_outBuffer.FlushWithCheck();
        }
    }

    if (m_isSplit)
    {
        m_outBuffer.FlushWithCheck();
        uint64_t freeByte = 0;
        uint64_t needSize = 56 + 20 + 22 + comment.GetCapacity();
        HResult hr = m_multiVolStream->GetCurVolFreeByte(freeByte);
        if (IsSuccess(hr) && freeByte < needSize)
        {
            hr = m_multiVolStream->ForceStartNextVol();
        }
        hr = m_multiVolStream->GetCurVolIndex(cur_index);
        hr = m_multiVolStream->GetCurVolPosition(end_pos);
    }

    uint64_t cd64EndOffset = GetCurrentPosition();
    uint64_t cdSize        = cd64EndOffset - cdOffset;

    bool cdOffset64 = cdOffset >= 0xFFFFFFFF;
    bool cdSize64   = cdSize >= 0xFFFFFFFF;
    bool items64    = items.size() >= 0xFFFF;
    bool isZip64    = (cdOffset64 || cdSize64 || items64);

    if (isZip64)
    {
        WriteUInt32(RCZipSignature::kZip64EndOfCentralDir);

        WriteUInt64(g_zip64EcdSize); // ThisDiskNumber = 0;

        WriteUInt16(45); // version

        WriteUInt16(45); // version

        if (m_isSplit)
        {   //写入分卷号
            WriteUInt32(cur_index);

            WriteUInt32(start_index);
        }
        else
        {
            WriteUInt32(0); // ThisDiskNumber = 0;

            WriteUInt32(0); // StartCentralDirectoryDiskNumber;;
        }

        WriteUInt64((uint64_t)items.size());

        WriteUInt64((uint64_t)items.size());

        WriteUInt64((uint64_t)cdSize);

        if (m_isSplit)
        {
            WriteUInt64((uint64_t)cur_pos);
        }
        else
        {
            WriteUInt64((uint64_t)cdOffset);
        }

        WriteUInt32(RCZipSignature::kZip64EndOfCentralDirLocator);

        if (m_isSplit)
        {
            WriteUInt32(cur_index);
            WriteUInt64(end_pos);
            WriteUInt32(cur_index + 1);
        }
        else
        {
            WriteUInt32(0); // number of the disk with the start of the zip64 end of central directory
            WriteUInt64(cd64EndOffset);
            WriteUInt32(1); // total number of disks
        }
    }

    WriteUInt32(RCZipSignature::kEndOfCentralDir);

    if (m_isSplit)
    {
        uint16_t diskNum = static_cast<uint16_t>(cur_index & 0xFFFF);
        WriteUInt16(diskNum);
        diskNum = static_cast<uint16_t>(start_index & 0xFFFF);
        WriteUInt16(diskNum);
    }
    else
    {
        WriteUInt16(0); // ThisDiskNumber = 0;

        WriteUInt16(0); // StartCentralDirectoryDiskNumber;
    }

    WriteUInt16((uint16_t)(items64 ? 0xFFFF: items.size()));

    WriteUInt16((uint16_t)(items64 ? 0xFFFF: items.size()));

    WriteUInt32(cdSize64 ? 0xFFFFFFFF: (uint32_t)cdSize);

    if (m_isSplit)
    {
        WriteUInt32(cdOffset64 ? 0xFFFFFFFF: (uint32_t)cur_pos);
    }
    else
    {
        WriteUInt32(cdOffset64 ? 0xFFFFFFFF: (uint32_t)cdOffset);
    }

    uint16_t commentSize = (uint16_t)comment.GetCapacity();

    WriteUInt16(commentSize);

    if (commentSize > 0)
    {
        WriteBytes(comment.data(), commentSize);
    }

    m_outBuffer.FlushWithCheck();
}

void RCZipOut::CreateStreamForCompressing(IOutStream** outStream)
{
    RCOffsetOutStream *streamSpec = new RCOffsetOutStream;
    IOutStreamPtr tempStream(streamSpec);
    streamSpec->Init(m_stream.Get(), m_basePosition + m_localFileHeaderSize);
    *outStream = tempStream.Detach();
}

void RCZipOut::SeekToPackedDataPosition()
{
    SeekTo(m_basePosition + m_localFileHeaderSize);
}

void RCZipOut::CreateStreamForCopying(ISequentialOutStream** outStream)
{
    ISequentialOutStreamPtr tempStream(m_stream.Get());
    *outStream = tempStream.Detach();
}

END_NAMESPACE_RCZIP
