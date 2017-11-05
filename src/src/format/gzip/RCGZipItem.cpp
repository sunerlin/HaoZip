/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "format/gzip/RCGZipItem.h"
#include "format/gzip/RCGZipDefs.h"
#include "filesystem/RCStreamUtils.h"
#include "algorithm/CpuArch.h"

/////////////////////////////////////////////////////////////////
//RCGZipItem class implementation

BEGIN_NAMESPACE_RCZIP

#define Get32(p) GetUi32(p)

bool RCGZipItem::TestFlag(byte_t flag)const
{
    return ((m_flags & flag) != 0) ;
}

bool RCGZipItem::HeaderCrcIsPresent() const
{
    return TestFlag(RCGZipHeader::NFlags::kCrc);
}

bool RCGZipItem::ExtraFieldIsPresent() const
{
    return TestFlag(RCGZipHeader::NFlags::kExtra);
}

bool RCGZipItem::NameIsPresent() const
{
    return TestFlag(RCGZipHeader::NFlags::kName);
}

bool RCGZipItem::CommentIsPresent() const
{
    return TestFlag(RCGZipHeader::NFlags::kComment);
}

void RCGZipItem::Clear()
{
    m_name.clear() ;
    m_comment.clear() ;
}

HResult RCGZipItem::ReadBytes(IGZipResumeDecoder* stream, byte_t* data, uint32_t size)
{
    for (uint32_t i = 0; i < size; i++)
    {
        data[i] = stream->ReadByte() ;
    }
    return stream->InputEofError() ? RC_S_FALSE : RC_S_OK ;
}

HResult RCGZipItem::SkipBytes(IGZipResumeDecoder* stream, uint32_t size)
{
    for (uint32_t i = 0; i < size; i++)
    {
        stream->ReadByte();
    }
    return stream->InputEofError() ? RC_S_FALSE : RC_S_OK ;
}

HResult RCGZipItem::ReadUInt16(IGZipResumeDecoder* stream, uint16_t& value)
{
    value = 0;
    for (int32_t i = 0; i < 2; i++)
    {
        byte_t b = stream->ReadByte();
        if (stream->InputEofError())
        {
            return RC_S_FALSE ;
        }
        // crc = CRC_UPDATE_BYTE(crc, b);
        value |= (uint16_t(b) << (8 * i)) ;
    }
    return RC_S_OK ;
}

HResult RCGZipItem::ReadString(IGZipResumeDecoder* stream, RCStringA& s, uint32_t limit)
{
    s.clear();
    for (uint32_t i = 0; i < limit; i++)
    {
        byte_t b = stream->ReadByte();
        if (stream->InputEofError())
        {
            return RC_S_FALSE;
        }
        // crc = CRC_UPDATE_BYTE(crc, b);
        if (b == 0)
        {
            return RC_S_OK;
        }
        s += (char)b ;
    }
    return RC_S_FALSE ;
}

HResult RCGZipItem::ReadHeader(IGZipResumeDecoder* stream)
{
    Clear() ;

    // Header-CRC field had another meaning in old version of gzip!
    // uint32_t crc = CRC_INIT_VAL;
    byte_t buf[10] ;

    HResult hr = ReadBytes(stream, buf, 10) ;
    if(!IsSuccess(hr))
    {
        return hr ;
    }
  
    if (GetUi16(buf) != RCGZipSignature::kSignature)
    {
        return RC_S_FALSE;
    }

    m_method = buf[2] ;

    if (m_method != RCGZipHeader::NCompressionMethod::kDeflate)
    {
        return RC_S_FALSE;
    }

    m_flags = buf[3];
    m_time = Get32(buf + 4);
    m_extraFlags = buf[8];
    m_hostOS = buf[9];

    // crc = CrcUpdate(crc, buf, 10);  
    if (ExtraFieldIsPresent())
    {
        uint16_t extraSize;
        hr = ReadUInt16(stream, extraSize /* , crc */) ;
        if(!IsSuccess(hr))
        {
            return hr ;
        }
        hr = SkipBytes(stream, extraSize) ;
        if(!IsSuccess(hr))
        {
            return hr ;
        }
        // Extra.SetCapacity(extraSize);
        // RINOK(ReadStream_FALSE(stream, Extra, extraSize));
        // crc = CrcUpdate(crc, Extra, extraSize);
    }
    if (NameIsPresent())
    {
        hr = ReadString(stream, m_name, (1 << 10) /* , crc */) ;
        if(!IsSuccess(hr))
        {
            return hr ;
        }
    }
    if (CommentIsPresent())
    {
        hr = ReadString(stream, m_comment, (1 << 16) /* , crc */) ;
        if(!IsSuccess(hr))
        {
            return hr ;
        }
    }

    if (HeaderCrcIsPresent())
    {
        uint16_t headerCRC;
        // uint32_t dummy = 0;
        hr = ReadUInt16(stream, headerCRC /* , dummy */) ;
        if(!IsSuccess(hr))
        {
            return hr ;
        }
        /*
        if ((uint16_t)CRC_GET_DIGEST(crc) != headerCRC)
        {
            return RC_S_FALSE;
        }
        */
    }
    return stream->InputEofError() ? RC_S_FALSE : RC_S_OK ;
}

HResult RCGZipItem::ReadFooter1(IGZipResumeDecoder* stream)
{
    byte_t buf[8] ;
    HResult hr = ReadBytes(stream, buf, 8) ;
    if(!IsSuccess(hr))
    {
        return hr ;
    }
    m_crc = Get32(buf);
    m_size32 = Get32(buf + 4);
    return stream->InputEofError() ? RC_S_FALSE : RC_S_OK;
}

HResult RCGZipItem::ReadFooter2(ISequentialInStream *stream)
{
    byte_t buf[8];
    HResult hr = RCStreamUtils::ReadStream_FALSE(stream, buf, 8) ;
    if(!IsSuccess(hr))
    {
        return hr ;
    }
    m_crc = Get32(buf);
    m_size32 = Get32(buf + 4);
    return RC_S_OK ;
}

HResult RCGZipItem::WriteHeader(ISequentialOutStream *stream)
{
    byte_t buf[10];
    SetUi16(buf, RCGZipSignature::kSignature);
    buf[2] = m_method;
    buf[3] = m_flags & RCGZipHeader::NFlags::kName;
    // buf[3] |= RCGZipHeader::NFlags::kCrc;
    SetUi32(buf + 4, m_time);
    buf[8] = m_extraFlags;
    buf[9] = m_hostOS;
    HResult hr = RCStreamUtils::WriteStream(stream, buf, 10) ;
    if(!IsSuccess(hr))
    {
        return hr ;
    }
    // crc = CrcUpdate(CRC_INIT_VAL, buf, 10);
    if (NameIsPresent())
    {
        // crc = CrcUpdate(crc, (const char *)m_name, m_name.Length() + 1);
        hr = RCStreamUtils::WriteStream(stream, (const char *)m_name.c_str(), m_name.size() + 1) ;
        if(!IsSuccess(hr))
        {
            return hr ;
        }
    }
    // SetUi16(buf, (uint16_t)CRC_GET_DIGEST(crc));
    // RINOK(WriteStream(stream, buf, 2));
    return RC_S_OK;
}

HResult RCGZipItem::WriteFooter(ISequentialOutStream *stream)
{
    byte_t buf[8];
    SetUi32(buf, m_crc);
    SetUi32(buf + 4, m_size32);
    return RCStreamUtils::WriteStream(stream, buf, 8);
}

END_NAMESPACE_RCZIP
