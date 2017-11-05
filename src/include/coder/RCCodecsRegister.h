/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCCodecsRegister_h_
#define __RCCodecsRegister_h_ 1

#include "coder/RCCodecsManager.h"

BEGIN_NAMESPACE_RCZIP

/** 从代码中静态载入编解码 
*/
class RCCodecsRegister:
    public RCCodecsManager
{
public:

    /** 默认构造函数
    */
    RCCodecsRegister() ;
    
    /** 默认析构函数
    */
    virtual ~RCCodecsRegister() ;
    
public:
    
    /** 载入编码解码器
    @return 成功返回RC_S_OK，否则返回错误号
    */
    virtual HResult LoadCodecs(void) ;
};

END_NAMESPACE_RCZIP

#endif //__RCCodecsRegister_h_
