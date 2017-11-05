/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCRangeBitEncoder_h_
#define __RCRangeBitEncoder_h_ 1

#include "compress/range/RCRangeBitModel.h"
#include "compress/range/RCRangeEncoder.h"

BEGIN_NAMESPACE_RCZIP

/** Range Bit 编码器
*/
template <int32_t numMoveBits>
class RCRangeBitEncoder:
    public RCRangeBitModel<numMoveBits>
{
public:

    /** 编码
    @param [in] encoder 编码器
    @param [in] symbol 符号
    */
    void Encode(RCRangeEncoder* encoder, uint32_t symbol) ;

    /** 返回权重
    @param [in] symbol 符号
    @return 返回权重
    */
    uint32_t GetPrice(uint32_t symbol) const ;

    /** 返回权重
    @return 返回权重
    */
    uint32_t GetPrice0() const ;

    /** 返回权重
    @return 返回权重
    */
    uint32_t GetPrice1() const ;    
};

END_NAMESPACE_RCZIP

#include "RCRangeBitEncoder.inl"

#endif //__RCRangeBitEncoder_h_
