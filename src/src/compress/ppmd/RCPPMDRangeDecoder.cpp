/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "compress/ppmd/RCPPMDRangeDecoder.h"

/////////////////////////////////////////////////////////////////
//RCPPMDRangeDecoder class implementation

BEGIN_NAMESPACE_RCZIP

uint32_t RCPPMDRangeDecoder::GetThreshold(uint32_t total)
{
    return RCRangeDecoder::GetThreshold(total);
}

void RCPPMDRangeDecoder::Decode(uint32_t start, uint32_t size)
{
    RCRangeDecoder::Decode(start, size);
}

uint32_t RCPPMDRangeDecoder::DecodeBit(uint32_t size0, uint32_t numTotalBits)
{
    return RCRangeDecoder::DecodeBit(size0, numTotalBits);
}

END_NAMESPACE_RCZIP
