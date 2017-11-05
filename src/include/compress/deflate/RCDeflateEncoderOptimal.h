/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCDeflateEncoderOptimal_h_
#define __RCDeflateEncoderOptimal_h_ 1

#include "base/RCDefs.h"

BEGIN_NAMESPACE_RCZIP

/** Defalte 编码优化
*/
class RCDeflateEncoderOptimal
{
public:

    /** 默认构造函数
    */
    RCDeflateEncoderOptimal() ;
    
    /** 默认析构函数
    */
    ~RCDeflateEncoderOptimal() ;
    
public:
    
    /** 权重
    */
    uint32_t m_price ;
    
    /** Pos Prev
    */
    uint16_t m_posPrev ;
    
    /** Back Prev
    */
    uint16_t m_backPrev ;
};

END_NAMESPACE_RCZIP

#endif //__RCDeflateEncoderOptimal_h_
