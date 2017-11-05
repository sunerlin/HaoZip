/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCDeflateEncoderValue_h_
#define __RCDeflateEncoderValue_h_ 1

#include "base/RCDefs.h"

BEGIN_NAMESPACE_RCZIP

/** Deflate 编码值
*/
class RCDeflateEncoderValue
{
public:

    /** 默认构造函数
    */
    RCDeflateEncoderValue() ;
    
    /** 默认析构函数
    */
    ~RCDeflateEncoderValue() ;
    
public:
    
    /** 设置是否字符
    */
    void SetAsLiteral() ;
    
    /** 返回是否字符
    */
    bool IsLiteral() const ;

public:
    
    /** 长度
    */
    uint16_t m_len ;
    
    /** 位置
    */
    uint16_t m_pos ;
};

END_NAMESPACE_RCZIP

#endif //__RCDeflateEncoderValue_h_
