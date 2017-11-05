/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCPPMDEncodeInfo_h_
#define __RCPPMDEncodeInfo_h_ 1

#include "compress/ppmd/RCPPMDInfo.h"
#include "compress/range/RCRangeEncoder.h"

BEGIN_NAMESPACE_RCZIP

/** PPMD 编码器信息
*/
class RCPPMDEncodeInfo:
    public RCPPMDInfo
{
public:

    /** 编码
    @param [in] symbol 符号
    @param [in] rangeEncoder 编码器
    */
    void EncodeBinSymbol(int32_t symbol, RCRangeEncoder* rangeEncoder) ;
    
    /** 编码
    @param [in] symbol 符号
    @param [in] rangeEncoder 编码器
    */
    void EncodeSymbol1(int32_t symbol, RCRangeEncoder* rangeEncoder) ;
    
    /** 编码
    @param [in] symbol 符号
    @param [in] rangeEncoder 编码器
    */
    void EncodeSymbol2(int32_t symbol, RCRangeEncoder* rangeEncoder) ;
    
    /** 编码
    @param [in] c 
    @param [in] rangeEncoder 编码器
    */
    void EncodeSymbol(int32_t c, RCRangeEncoder* rangeEncoder) ;
};

END_NAMESPACE_RCZIP

#endif //__RCPPMDEncodeInfo_h_
