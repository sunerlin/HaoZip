/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "compress/bzip2/RCBZip2CRC.h"
#include "compress/bzip2/RCBZip2CRCTableInit.h"

/////////////////////////////////////////////////////////////////
//RCBZip2CRC class implementation

BEGIN_NAMESPACE_RCZIP

uint32_t RCBZip2CRC::s_table[256] ;
static const uint32_t s_kBZip2CRCPoly = 0x04c11db7;  /* AUTODIN II, Ethernet, & FDDI */

RCBZip2CRC::RCBZip2CRC():
    m_value(0xFFFFFFFF)
{
    (void)RCBZip2CRCInit::Instance() ;
}

RCBZip2CRC::~RCBZip2CRC()
{
}

void RCBZip2CRC::InitTable()
{
    for (uint32_t i = 0; i < 256; i++)
    {
        uint32_t r = (i << 24);
        for (int32_t j = 8; j > 0; j--)
        {
            r = (r & 0x80000000) ? ((r << 1) ^ s_kBZip2CRCPoly) : (r << 1);
        }
        s_table[i] = r;
    }
}

void RCBZip2CRC::Init()
{
    m_value = 0xFFFFFFFF ;
}

void RCBZip2CRC::UpdateByte(byte_t b)
{
    m_value = s_table[(m_value >> 24) ^ b] ^ (m_value << 8);
}

void RCBZip2CRC::UpdateByte(uint32_t b)
{
    m_value = s_table[(m_value >> 24) ^ b] ^ (m_value << 8);
}

uint32_t RCBZip2CRC::GetDigest() const
{
    return m_value ^ 0xFFFFFFFF ;
}

END_NAMESPACE_RCZIP
