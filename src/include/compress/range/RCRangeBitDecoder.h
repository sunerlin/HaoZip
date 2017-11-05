/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCRangeBitDecoder_h_
#define __RCRangeBitDecoder_h_ 1

#include "compress/range/RCRangeBitModel.h"
#include "compress/range/RCRangeDecoder.h"

BEGIN_NAMESPACE_RCZIP

/** Range Bit 解码器
*/
template <int32_t numMoveBits>
class RCRangeBitDecoder:
    public RCRangeBitModel<numMoveBits>
{
public:

    /** 解码
    @param [in] decoder 解码器
    @return 返回解码字节数
    */
    uint32_t Decode(RCRangeDecoder *decoder) ;
};

END_NAMESPACE_RCZIP

#include "RCRangeBitDecoder.inl"

#endif //__RCRangeBitDecoder_h_
