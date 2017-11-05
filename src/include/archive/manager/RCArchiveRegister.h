/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCArchiveRegister_h_
#define __RCArchiveRegister_h_ 1

#include "archive/manager/RCArchiveManager.h"

BEGIN_NAMESPACE_RCZIP

/** 从代码中静态载入文档格式信息 
*/
class RCArchiveRegister:
    public RCArchiveManager
{
public:

    /** 默认构造函数
    */
    RCArchiveRegister() ;
    
    /** 默认析构函数
    */
    virtual ~RCArchiveRegister() ;
    
public:
    
    /** 载入文档格式
    @return 如果成功则返回RC_S_OK，否则返回错误号
    */
    virtual HResult LoadArchives(void) ;
};

END_NAMESPACE_RCZIP

#endif //__RCArchiveRegister_h_
