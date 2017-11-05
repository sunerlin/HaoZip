/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "RCRar3VmMemBitDecoder.h"

/////////////////////////////////////////////////////////////////
//RCRar3VmMemBitDecoder class implementation

BEGIN_NAMESPACE_RCZIP

RCRar3VmMemBitDecoder::RCRar3VmMemBitDecoder():
    _data(NULL),
    _bitSize(0),
    _bitPos(0)
{
}

RCRar3VmMemBitDecoder::~RCRar3VmMemBitDecoder()
{
}

void RCRar3VmMemBitDecoder::Init(const byte_t *data, uint32_t byteSize)
{
    _data = data ;
    _bitSize = (byteSize << 3);
    _bitPos = 0 ;
}

uint32_t RCRar3VmMemBitDecoder::ReadBits(int32_t numBits)
{
    uint32_t res = 0 ;
    for (;;)
    {
        byte_t b = _bitPos < _bitSize ? _data[_bitPos >> 3] : 0;
        int32_t avail = (int32_t)(8 - (_bitPos & 7));
        if (numBits <= avail)
        {
            _bitPos += numBits;
            return res | (b >> (avail - numBits)) & ((1 << numBits) - 1);
        }
        numBits -= avail;
        res |= (uint32_t)(b & ((1 << avail) - 1)) << numBits;
        _bitPos += avail;
    }
}

uint32_t RCRar3VmMemBitDecoder::ReadBit()
{
    return ReadBits(1) ;
}

bool RCRar3VmMemBitDecoder::Avail() const
{
    return (_bitPos < _bitSize);
}

END_NAMESPACE_RCZIP
