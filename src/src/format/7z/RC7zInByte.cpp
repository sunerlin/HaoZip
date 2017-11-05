/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "format/7z/RC7zInByte.h"
#include "common/RCStringBuffer.h"
#include "algorithm/CpuArch.h"
/////////////////////////////////////////////////////////////////
//RC7zInByte class implementation

#define Get16(p) GetUi16(p)
#define Get32(p) GetUi32(p)
#define Get64(p) GetUi64(p)

BEGIN_NAMESPACE_RCZIP

RC7zInByte::RC7zInByte():
    m_pos(0),
    m_buffer(NULL),
    m_size(0)
{
}

RC7zInByte::~RC7zInByte()
{
}

void RC7zInByte::Init(const byte_t* buffer, size_t size)
{
    m_buffer = buffer;
    m_size = size;
    m_pos = 0;
}

byte_t RC7zInByte::ReadByte()
{
    if (m_pos >= m_size)
    {
        _ThrowCode(RC_E_ReadStreamError) ;
    }
    return m_buffer[m_pos++] ;
}

void RC7zInByte::ReadBytes(byte_t *data, size_t size)
{
    if (size > m_size - m_pos)
    {
        _ThrowCode(RC_E_ReadStreamError) ;
    }
    for (size_t i = 0; i < size; i++)
    {
        data[i] = m_buffer[m_pos++];
    }
}

void RC7zInByte::SkipData(uint64_t size)
{
    if (size > m_size - m_pos)
    {
        _ThrowCode(RC_E_SeekStreamError) ;
    }
    m_pos += (size_t)size ;
}

void RC7zInByte::SkipData()
{
    SkipData(ReadNumber());
}

uint64_t RC7zInByte::ReadNumber()
{
    if (m_pos >= m_size)
    {
        _ThrowCode(RC_E_ReadStreamError) ;
    }
    byte_t firstByte = m_buffer[m_pos++];
    byte_t mask = 0x80;
    uint64_t value = 0;
    for (int32_t i = 0; i < 8; i++)
    {
        if ((firstByte&  mask) == 0)
        {
            uint64_t highPart = firstByte&  (mask - 1);
            value += (highPart << (i * 8));
            return value;
        }
        if (m_pos >= m_size)
        {
            _ThrowCode(RC_E_ReadStreamError) ;
        }
        value |= ((uint64_t)m_buffer[m_pos++] << (8 * i));
        mask >>= 1;
    }
    return value;
}

RC7zNum RC7zInByte::ReadNum()
{
    uint64_t value = ReadNumber();
    if (value > RC7zDefs::s_7zNumMax)
    {
        _ThrowCode(RC_E_ReadStreamError) ;
    }
    return (RC7zNum)value;
}

uint32_t RC7zInByte::ReadUInt32()
{
    if (m_pos + 4 > m_size)
    {
        _ThrowCode(RC_E_ReadStreamError) ;
    }
    uint32_t res = Get32(m_buffer + m_pos);
    m_pos += 4 ;
    return res ;
}

uint64_t RC7zInByte::ReadUInt64()
{
    if (m_pos + 8 > m_size)
    {
        _ThrowCode(RC_E_ReadStreamError) ;
    }
    uint64_t res = Get64(m_buffer + m_pos);
    m_pos += 8;
    return res;
}

void RC7zInByte::ReadString(RCString& s)
{
    const byte_t* buf = m_buffer + m_pos;
    size_t rem = (m_size - m_pos) / 2 * 2;
    {
        size_t i = 0 ;
        for (i = 0; i < rem; i += 2)
        {
            if (buf[i] == 0 && buf[i + 1] == 0)
            {
                break;
            }
        }
        if (i == rem)
        {
            _ThrowCode(RC_E_ReadStreamError) ;
        }
        rem = i;
    }
    int32_t len = (int32_t)(rem / 2);
    if (len < 0 || (size_t)len * 2 != rem)
    {
        _ThrowCode(RC_E_ReadStreamError) ;
    }
    RCStringBuffer sBuf(&s) ;
    RCString::value_type* p = sBuf.GetBuffer(len);
    int32_t i = 0 ;
    for (i = 0; i < len; i++, buf += 2)
    {
        p[i] = (RCString::value_type)Get16(buf);
    }
    sBuf.ReleaseBuffer(len) ;
    m_pos += rem + 2 ;
}

size_t RC7zInByte::GetPos() const
{
    return m_pos ;
}

END_NAMESPACE_RCZIP
