/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCEnumDirItemUpdateCallback_h_
#define __RCEnumDirItemUpdateCallback_h_ 1

#include "archive/common/RCEnumDirItems.h"
#include "archive/update/RCUpdateCallbackUI.h"

BEGIN_NAMESPACE_RCZIP

/** 目录枚举回调接口实现
*/
struct RCEnumDirItemUpdateCallback : 
    public IEnumDirItemCallback
{
    /** 界面更新回调接口指针
    */
    RCUpdateCallbackUI2 *m_callback;
    
    /** 设置扫描进度
    @param [in] numFolders 目录数
    @param [in] numFiles 文件数
    @param [in] path 文件路径
    @return 成功返回RC_S_OK,否则返回错误号
    */
    HResult ScanProgress(uint64_t numFolders, uint64_t numFiles, const RCString& path)
    {
        if(m_callback)
        {
            return m_callback->ScanProgress(numFolders, numFiles, path);
        }
        else
        {
            return RC_S_OK ;
        }
    }
};

END_NAMESPACE_RCZIP

#endif //__RCEnumDirItemUpdateCallback_h_
