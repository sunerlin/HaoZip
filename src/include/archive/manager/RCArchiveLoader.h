/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCArchiveLoader_h_
#define __RCArchiveLoader_h_ 1

#include "archive/manager/RCArchiveManager.h"

BEGIN_NAMESPACE_RCZIP

/** 载入文档格式信息 
*/
class RCArchiveLoader:
    public RCArchiveManager
{
public:

    /** 默认构造函数
    */
    RCArchiveLoader() ;
    
    /** 默认析构函数
    */
    virtual ~RCArchiveLoader() ;
    
public:
    
    /** 从IArchiveInfo接口载入文档管理器
    @param [in] archiveInfo IArchiveInfo接口指针
    @return 如果成功则返回RC_S_OK，否则返回错误号
    */
    HResult LoadFormat(IArchiveInfo* archiveInfo) ;
};

END_NAMESPACE_RCZIP

#endif //__RCArchiveLoader_h_
