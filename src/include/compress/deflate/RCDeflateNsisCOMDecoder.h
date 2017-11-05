/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCDeflateNsisCOMDecoder_h_
#define __RCDeflateNsisCOMDecoder_h_ 1

#include "compress/deflate/RCDeflateDecoder.h"

BEGIN_NAMESPACE_RCZIP

/** Deflate NSIS 解码器
*/
class RCDeflateNsisCOMDecoder:
    public RCDeflateDecoder
{
public:

    /** 默认构造函数
    */
    RCDeflateNsisCOMDecoder():
        RCDeflateDecoder(false,true)
    {
    };
    
    /** 默认析构函数
    */
    virtual ~RCDeflateNsisCOMDecoder() {} ;
};

END_NAMESPACE_RCZIP

#endif //__RCDeflateNsisCOMDecoder_h_
