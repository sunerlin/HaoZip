/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#include "format/udf/RCUdfCrc16.h"

#define CRC16_INIT_VAL 0
#define CRC16_UPDATE_BYTE(crc, b) (s_crc16Table[(((crc) >> 8) ^ (b)) & 0xFF] ^ ((crc) << 8))
#define kCrc16Poly 0x1021

BEGIN_NAMESPACE_RCZIP

uint16_t RCUdfCrc16::s_crc16Table[256];

void MY_FAST_CALL RCUdfCrc16::Crc16GenerateTable(void)
{
    uint32_t i;
    for (i = 0; i < 256; i++)
    {
        uint32_t r = (i << 8);
        for (int32_t j = 8; j > 0; j--)
        {
            r = ((r & 0x8000) ? ((r << 1) ^ kCrc16Poly) : (r << 1)) & 0xFFFF;
        }
        s_crc16Table[i] = (uint16_t)r;
    }
}

uint16_t MY_FAST_CALL RCUdfCrc16::Crc16_Update(uint16_t v, const void *data, size_t size)
{
    const byte_t *p = (const byte_t *)data;
    for (; size > 0 ; size--, p++)
    {
        v = CRC16_UPDATE_BYTE(v, *p);
    }
    return v;
}

uint16_t MY_FAST_CALL RCUdfCrc16::Crc16Calc(const void *data, size_t size)
{
    return Crc16_Update(CRC16_INIT_VAL, data, size);
}

static RCUdfCrc16TableInit m_CrcTableInit;

END_NAMESPACE_RCZIP
