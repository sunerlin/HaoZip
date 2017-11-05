/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCEnumDirItems_h_
#define __RCEnumDirItems_h_ 1

#include "base/RCWindowsDefs.h"
#include "base/RCString.h"
#include "common/RCVector.h"
#include "filesystem/RCFileEnumerator.h"
#include "filesystem/RCFileInfo.h"
#include "filesystem/RCFindFile.h"
#include "archive/common/RCWildcardCensorNode.h"
#include "archive/common/RCWildcardCensor.h"
#include "archive/common/RCDirItems.h"
#include "archive/common/IEnumDirItemFilter.h"

BEGIN_NAMESPACE_RCZIP

/** 枚举目录时的回调接口
*/
class IEnumDirItemCallback
{
public:
    
    /** 进度回调
    @param [in] numFolders 目录数
    @param [in] numFiles 文件数
    @param [in] path 文件路径
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult ScanProgress(uint64_t numFolders, uint64_t numFiles, const RCString& path) = 0 ;
    
protected:
    
    /** 析构函数
    */
    ~IEnumDirItemCallback() {} ;
};

/** 通配符匹配
*/
class RCWildcardCensor ;

/** 目录项集合
*/
class RCDirItems ;

/** 枚举目录
*/
class RCEnumDirItems
{
public:
    
    /** 设置被压缩的文件过滤器
    @param [in] spEnumDirItemFilter 文件过滤器指针
    */
    void SetEnumDirItemFilter(const IEnumDirItemFilterPtr& spEnumDirItemFilter) ;

    /** 枚举文件目录信息
    @param [in] censor 通配符匹配处理
    @param [in] dirItems 目录项
    @param [in] callback 枚举操作回调接口指针
    @param [in] isForUpdateArchive 是否为更新压缩文件模式
    @param [out] errorPaths 枚举操作错误路径列表
    @param [out] errorCodes 枚举操作错误号列表
    @return 成功返回RC_S_OK,否则返回错误号
    */
    HResult EnumerateItems( const RCWildcardCensor& censor,
                            RCDirItems& dirItems,
                            IEnumDirItemCallback* callback,
                            bool isForUpdateArchive,
                            RCVector<RCString>& errorPaths,
                            RCVector<HResult>& errorCodes) ;

private:
    
    /** 枚举目录
    @param [in] curNode 当前目录
    @param [in] phyParent 物理父节点
    @param [in] logParent 逻辑父节点
    @param [in] phyPrefix 物理前缀
    @param [in] addArchivePrefix 增加的文档前缀
    @param [out] dirItems 目录信息列表
    @param [in] enterToSubFolders 枚举子目录
    @param [in] callback 枚举操作回调
    @param [in] isForUpdateArchive 是否更新文档
    @param [out] errorPaths 枚举操作错误路径列表
    @param [out] errorCodes 枚举操作错误号列表
    @return 成功返回RC_S_OK,否则返回错误号
    */
    HResult EnumerateDirItems(  const RCWildcardCensorNode& curNode,
                                int32_t phyParent, 
                                int32_t logParent, 
                                const RCString& phyPrefix,
                                const RCVector<RCString>& addArchivePrefix,
                                RCDirItems& dirItems,
                                bool enterToSubFolders,
                                IEnumDirItemCallback* callback,
                                bool isForUpdateArchive,
                                RCVector<RCString>& errorPaths,
                                RCVector<HResult>& errorCodes) ;

    /** 枚举目录项
    @param [in] curNode 当前目录
    @param [in] phyParent 物理父节点
    @param [in] logParent 逻辑父节点
    @param [in] curFolderName 当前目录名
    @param [in] phyPrefix 物理前缀
    @param [in] addArchivePrefix 增加的文档前缀
    @param [out] dirItems 目录信息列表
    @param [in] enterToSubFolders 是否枚举子目录
    @param [in] callback 枚举回调
    @param [in] isForUpdateArchive 是否更新文档
    @param [out] errorPaths 枚举操作错误路径列表
    @param [out] errorCodes 枚举操作错误号列表
    @return 成功返回RC_S_OK,否则返回错误号
    */
    HResult EnumerateDirItems_Spec( const RCWildcardCensorNode& curNode,
                                    int32_t phyParent, 
                                    int32_t logParent, 
                                    const RCString& curFolderName,
                                    const RCString& phyPrefix,
                                    const RCVector<RCString>& addArchivePrefix,
                                    RCDirItems& dirItems,
                                    bool enterToSubFolders,
                                    IEnumDirItemCallback* callback,
                                    bool isForUpdateArchive,
                                    RCVector<RCString>& errorPaths,
                                    RCVector<HResult>& errorCodes) ;

private:
    
    /** 被压缩的文件过滤器
    */
    IEnumDirItemFilterPtr m_spEnumDirItemFilter ;
};

END_NAMESPACE_RCZIP

#endif //__RCEnumDirItems_h_
