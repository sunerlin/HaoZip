/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCPPMDDecodeInfo_h_
#define __RCPPMDDecodeInfo_h_ 1

#include "compress/ppmd/RCPPMDInfo.h"
#include "compress/ppmd/RCPPMDRangeDecoderVirt.h"

BEGIN_NAMESPACE_RCZIP

/** PPMD 解码器
*/
class RCPPMDDecodeInfo:
    public RCPPMDInfo
{
public:

    /** 解码
    @param [in] rangeDecoder 解码器
    */
    void DecodeBinSymbol(RCPPMDRangeDecoderVirt* rangeDecoder) ;
    
    /** 解码
    @param [in] rangeDecoder 解码器
    */
    void DecodeSymbol1(RCPPMDRangeDecoderVirt* rangeDecoder) ;
    
    /** 解码
    @param [in] rangeDecoder 解码器
    */
    void DecodeSymbol2(RCPPMDRangeDecoderVirt* rangeDecoder) ;
    
    /** 解码
    @param [in] rangeDecoder 解码器
    @return 返回解码字节数
    */
    int32_t DecodeSymbol(RCPPMDRangeDecoderVirt* rangeDecoder) ;
};

END_NAMESPACE_RCZIP

#endif //__RCPPMDDecodeInfo_h_
