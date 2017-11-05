/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCUpdateArchiveCommand_h_
#define __RCUpdateArchiveCommand_h_ 1

#include "base/RCString.h"
#include "archive/update/RCArchivePath.h"
#include "archive/update/RCUpdateAction.h"

BEGIN_NAMESPACE_RCZIP
   
/** 文档更新命令结构
*/
class RCUpdateArchiveCommand
{
public:
    
    /** 设置用户文档路径
    @param [in] userArchivePath
    */
    void SetUserArchivePath(const RCString& userArchivePath);
    
    /** 取得用户文档路径
    @return 返回用户文档路径
    */
    const RCString& GetUserArchivePath(void) const;

    /** 返回文档路径
    @return 返回文档路径
    */
    RCArchivePath& GetArchivePath(void);
    
    /** 返回文档路径
    @return 返回文档路径
    */
    const RCArchivePath& GetArchivePath(void) const;

    /** 返回更新操作
    @return 返回更新操作
    */
    RCActionSet& GetActionSet(void);
    
    /** 返回更新操作
    @return 返回更新操作
    */
    const RCActionSet& GetActionSet(void) const;

private:

    /** 使用文档路径
    */
    RCString m_userArchivePath;

    /** 文档路径
    */
    RCArchivePath m_archivePath;

    /** 更新操作集合
    */
    RCActionSet m_actionSet;
};

END_NAMESPACE_RCZIP

#endif //__RCUpdateArchiveCommand_h_
