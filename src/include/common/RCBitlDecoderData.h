/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCBitlDecoderData_h_
#define __RCBitlDecoderData_h_ 1

#include "base/RCDefs.h"
#include "base/RCSingleton.h"

BEGIN_NAMESPACE_RCZIP

class RCBitlDecoderData
{
public:
    
    /** 默认构造函数
    */
    RCBitlDecoderData() ;

public:
    
    /** 大数值位数
    */
    static const int32_t s_kNumBigValueBits = 8 * 4;
    
    /** 数值字节数
    */
    static const int32_t s_kNumValueBytes = 3;
    
    /** 数值位数
    */
    static const int32_t s_kNumValueBits = 8  * RCBitlDecoderData::s_kNumValueBytes ;
    
    /** 数值掩码
    */
    static const uint32_t s_kMask = (1 << s_kNumValueBits) - 1 ;
    
    /** Invert表
    */
    byte_t s_kInvertTable[256] ;    
};

typedef RCSingleton<RCBitlDecoderData>  RCBitlDefs ;

END_NAMESPACE_RCZIP

#endif //__RCBitlDecoderData_h_
