/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "format/lzh/RCLzhIn.h"
#include "filesystem/RCStreamUtils.h"
#include "algorithm/CpuArch.h"

/////////////////////////////////////////////////////////////////
//RCLzhIn class implementation

BEGIN_NAMESPACE_RCZIP

#define Get16(p) GetUi16(p)
#define Get32(p) GetUi32(p)

HResult RCLzhIn::ReadBytes(void* data, uint32_t size, uint32_t& processedSize)
{
    size_t realProcessedSize = size;
    HResult hr = RCStreamUtils::ReadStream(m_stream.Get(), data, &realProcessedSize);
    if (hr != RC_S_OK)
    {
        return hr;
    }

    processedSize = (uint32_t)realProcessedSize;
    m_position += processedSize;

    return RC_S_OK;
}

HResult RCLzhIn::CheckReadBytes(void* data, uint32_t size)
{
    uint32_t processedSize;
    HResult hr = ReadBytes(data, size, processedSize);
    if (hr != RC_S_OK)
    {
        return hr;
    }

    return (processedSize == size) ? RC_S_OK: RC_E_FAIL;
}

HResult RCLzhIn::Open(IInStream* inStream)
{
    HResult hr = inStream->Seek(0, RC_STREAM_SEEK_CUR, &m_position);
    if (hr != RC_S_OK)
    {
        return hr;
    }

    m_stream = inStream;
    return RC_S_OK;
}

static const byte_t* ReadUInt16(const byte_t* p, uint16_t &v)
{
    v = Get16(p) ;
    return p + 2 ;
}

static const byte_t *ReadString(const byte_t* p, size_t size, RCStringA& s)
{
    s.empty();
    for (size_t i = 0; i < size; i++)
    {
        char c = p[i];
        if (c == 0)
        {
            break;
        }
        s += c;
    }
    return p + size;
}

static byte_t CalcSum(const byte_t *data, size_t size)
{
    byte_t sum = 0;
    for (size_t i = 0; i < size; i++)
    {
        sum = (byte_t)(sum + data[i]);
    }
    return sum;
}

HResult RCLzhIn::GetNextItem(bool& filled, RCLzhItemEx& item)
{
    filled = false ;
    uint32_t processedSize = 0 ;
    byte_t startHeader[2] ;
    HResult hr = ReadBytes(startHeader, 2, processedSize);
    if (hr != RC_S_OK)
    {
        return hr;
    }

    if (processedSize == 0)
    {
        return RC_S_OK;
    }

    if (processedSize == 1)
    {
        return (startHeader[0] == 0) ? RC_S_OK: RC_E_FAIL;
    }

    if (startHeader[0] == 0 && startHeader[1] == 0)
    {
        return RC_S_OK;
    }

    byte_t header[256];
    const uint32_t kBasicPartSize = 22;
    hr = ReadBytes(header, kBasicPartSize, processedSize);
    if (hr != RC_S_OK)
    {
        return hr;
    }

    if (processedSize != kBasicPartSize)
    {
        if( (processedSize == 1) && 
            (startHeader[1] == 0) &&
            (header[0] == 0) )
        {
            //IZArc、PowerArchiver软件生成的LZH软件都是这种格式，特殊处理            
            return RC_E_UnexpectedEndOfArchive ;
        }
        return (startHeader[0] == 0) ? RC_S_OK: RC_E_FAIL;
    }

    const byte_t* p = header;
    memmove(item.m_method, p, RCLzhHeader::s_methodIdSize);
    if (!item.IsValidMethod())
    {
        return RC_S_OK;
    }

    p += RCLzhHeader::s_methodIdSize;
    item.m_packSize = Get32(p) ;
    item.m_size = Get32(p + 4) ;
    item.m_modifiedTime = Get32(p + 8) ;
    item.m_attributes = p[12] ;
    item.m_level = p[13] ;
    p += 14 ;
    if (item.m_level > 2)
    {
        return RC_E_FAIL ;
    }

    uint32_t headerSize;
    if (item.m_level < 2)
    {
        headerSize = startHeader[0];
        if (headerSize < kBasicPartSize)
        {
            return RC_E_FAIL;
        }

        uint32_t remain = headerSize - kBasicPartSize;
        hr = CheckReadBytes(header + kBasicPartSize, remain);
        if (hr != RC_S_OK)
        {
            return hr;
        }

        if (startHeader[1] != CalcSum(header, headerSize))
        {
            return RC_E_FAIL;
        }

        size_t nameLength = *p++;
        if ((p - header) + nameLength + 2 > headerSize)
        {
            return RC_E_FAIL;
        }

        p = ReadString(p, nameLength, item.m_name);
    }
    else
    {
        headerSize = startHeader[0] | ((uint32_t)startHeader[1] << 8);
    }

    p = ReadUInt16(p, item.m_crc);
    if (item.m_level != 0)
    {
        if (item.m_level == 2)
        {
            hr = CheckReadBytes(header + kBasicPartSize, 2);
            if (hr != RC_S_OK)
            {
                return hr;
            }
        }

        if ((size_t)(p - header) + 3 > headerSize)
        {
            return RC_E_FAIL;
        }

        item.m_osId = *p++;
        uint16_t nextSize;
        p = ReadUInt16(p, nextSize);
        while (nextSize != 0)
        {
            if (nextSize < 3)
            {
                return RC_E_FAIL;
            }

            if (item.m_level == 1)
            {
                if (item.m_packSize < nextSize)
                {
                    return RC_E_FAIL;
                }
                item.m_packSize -= nextSize;
            }

            RCLzhExtension ext;
            hr = CheckReadBytes(&ext.m_type, 1);
            if (hr != RC_S_OK)
            {
                return hr;
            }

            nextSize -= 3;
            ext.m_data.SetCapacity(nextSize);
            hr = CheckReadBytes((byte_t *)ext.m_data.data(), nextSize);
            if (hr != RC_S_OK)
            {
                return hr;
            }

            item.m_extensions.push_back(ext);
            byte_t hdr2[2];
            hr = CheckReadBytes(hdr2, 2);
            if (hr != RC_S_OK)
            {
                return hr;
            }

            ReadUInt16(hdr2, nextSize);
        }
    }
    item.m_dataPosition = m_position;
    filled = true;

    return RC_S_OK;
}

HResult RCLzhIn::Skip(uint64_t numBytes)
{
    uint64_t newPostion;
    HResult hr = m_stream->Seek(numBytes, RC_STREAM_SEEK_CUR, &newPostion);
    if (hr != RC_S_OK)
    {
        return hr;
    }

    m_position += numBytes;
    if (m_position != newPostion)
    {
        return RC_E_FAIL;
    }

    return RC_S_OK;
}

END_NAMESPACE_RCZIP
