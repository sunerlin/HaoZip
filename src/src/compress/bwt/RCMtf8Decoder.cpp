/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "compress/bwt/RCMtf8Decoder.h"

/////////////////////////////////////////////////////////////////
//RCMtf8Decoder class implementation

BEGIN_NAMESPACE_RCZIP

#define MTF_MASK ((1 << RC_MTF_MOVS) - 1)

RCMtf8Decoder::RCMtf8Decoder()
{
}

RCMtf8Decoder::~RCMtf8Decoder()
{
}

void RCMtf8Decoder::StartInit()
{
    memset(Buffer, 0, sizeof(Buffer)) ;
}

void RCMtf8Decoder::Add(uint32_t pos, byte_t value)
{
    RCZIP_ASSERT(pos < POS_LEN ) ;
    Buffer[pos >> RC_MTF_MOVS] |= ((RCMtfVar)value << ((pos & MTF_MASK) << 3));
}

byte_t RCMtf8Decoder::GetHead() const
{
    return (byte_t)Buffer[0] ;
}

byte_t RCMtf8Decoder::GetAndMove(uint32_t pos)
{
    RCZIP_ASSERT(pos < POS_LEN ) ;
    uint32_t lim = ((uint32_t)pos >> RC_MTF_MOVS);
    pos = (pos & MTF_MASK) << 3;
    RCMtfVar prev = (Buffer[lim] >> pos) & 0xFF;

    uint32_t i = 0;
    if ((lim & 1) != 0)
    {
        RCMtfVar next = Buffer[0];
        Buffer[0] = (next << 8) | prev;
        prev = (next >> (MTF_MASK << 3));
        i = 1;
        lim -= 1;
    }
    for (; i < lim; i += 2)
    {
        RCMtfVar n0 = Buffer[i];
        RCMtfVar n1 = Buffer[i + 1];
        Buffer[i    ] = (n0 << 8) | prev;
        Buffer[i + 1] = (n1 << 8) | (n0 >> (MTF_MASK << 3));
        prev = (n1 >> (MTF_MASK << 3));
    }
    RCMtfVar next = Buffer[i];
    RCMtfVar mask = (((RCMtfVar)0x100 << pos) - 1);
    Buffer[i] = (next & ~mask) | (((next << 8) | prev) & mask);
    return (byte_t)Buffer[0] ;
}

END_NAMESPACE_RCZIP
