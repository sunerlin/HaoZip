/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCRangeCoderDefs_h_
#define __RCRangeCoderDefs_h_ 1

#include "base/RCDefs.h"
#include "base/RCSingleton.h"

BEGIN_NAMESPACE_RCZIP

/** Range 编码常量定义
*/
class RCRangeCoderDefs
{
public:

    static const int32_t    s_kNumTopBits = 24 ;
    static const uint32_t   s_kTopValue = (1 << s_kNumTopBits) ;
    
    static const int32_t    s_kNumBitModelTotalBits  = 11;
    static const uint32_t   s_kBitModelTotal = (1 << s_kNumBitModelTotalBits);
    static const int32_t    s_kNumMoveReducingBits = 4;
    static const int32_t    s_kNumBitPriceShiftBits = 4;
    static const uint32_t   s_kBitPrice = 1 << s_kNumBitPriceShiftBits;
    static uint32_t         s_probPrices[s_kBitModelTotal >> s_kNumMoveReducingBits] ;
    
protected:

    /** 默认构造函数
    */
    RCRangeCoderDefs() ;
    
    /** 默认析构函数
    */
    ~RCRangeCoderDefs() ;    
};

typedef RCSingleton<RCRangeCoderDefs> RCRangeCoderInit ;

END_NAMESPACE_RCZIP

#endif //__RCRangeCoderDefs_h_
