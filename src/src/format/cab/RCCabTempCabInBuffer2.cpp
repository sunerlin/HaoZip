/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#include "format/cab/RCCabTempCabInBuffer2.h"

BEGIN_NAMESPACE_RCZIP

byte_t RCCabTempCabInBuffer2::ReadByte()
{
    return m_buffer[m_pos++];
}

uint32_t RCCabTempCabInBuffer2::ReadUInt32()
{
    uint32_t value = 0;
    for (int32_t i = 0; i < 4; i++)
    {
        value |= (((uint32_t)ReadByte()) << (8 * i));
    }
    return value;
}

uint16_t RCCabTempCabInBuffer2::ReadUInt16()
{
    uint16_t value = 0;
    for (int32_t i = 0; i < 2; i++)
    {
        value |= (((uint16_t)ReadByte()) << (8 * i));
    }
    return value;
}

END_NAMESPACE_RCZIP
