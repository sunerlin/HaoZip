/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCPPMDRangeDecoder_h_
#define __RCPPMDRangeDecoder_h_ 1

#include "compress/ppmd/RCPPMDRangeDecoderVirt.h"
#include "compress/range/RCRangeDecoder.h"

BEGIN_NAMESPACE_RCZIP

/** PPMD 序列解码器
*/
class RCPPMDRangeDecoder:
    public RCPPMDRangeDecoderVirt,
    public RCRangeDecoder
{
public:

    /** 返回阀值
    @param [in] total 总数
    @return 返回阀值
    */
    virtual uint32_t GetThreshold(uint32_t total) ;

    /** 解码
    @param [in] start 开始
    @param [in] size 大小
    */
    virtual void Decode(uint32_t start, uint32_t size) ;

    /** 位解码
    @param [in] size0 大小
    @param [in] numTotalBits 位数
    */
    virtual uint32_t DecodeBit(uint32_t size0, uint32_t numTotalBits) ;
};

END_NAMESPACE_RCZIP

#endif //__RCPPMDRangeDecoder_h_
