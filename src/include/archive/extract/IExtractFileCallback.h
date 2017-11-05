/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __IExtractFileCallback_h_
#define __IExtractFileCallback_h_ 1

#include "interface/IProgress.h"
#include "base/RCString.h"
#include "archive/extract/RCExtractDefs.h"
#include "filesystem/RCFileDefs.h"

BEGIN_NAMESPACE_RCZIP

/** 从压缩文件中解压文件回调，作用单位：包内文件
*/
class IExtractFolderArchiveCallback:
    public IProgress
{
public:

    /** 文件覆盖确认接口
    @param [in] existName 已经存在的文件路径
    @param [in] existTime 已经存在的文件时间
    @param [in] existSize 已经存在的文件大小
    @param [in] newName 新文件名
    @param [in] newTime 新文件时间
    @param [in] newSize 新文件大小
    @param [in] isEnableRename 是否支持重命名操作
    @param [out] answer 确认结果
    @param [out] newFileName 如果是重命名单个文件时，返回用户指定名称
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult AskOverwrite(const RCString& existName,
                                 const RC_FILE_TIME* existTime, 
                                 const uint64_t* existSize,
                                 const RCString& newName,
                                 const RC_FILE_TIME* newTime, 
                                 const uint64_t* newSize,
                                 bool isEnableRename,
                                 RCExractOverwriteAnswer& answer,
                                 RCString& newFileName) = 0 ;
    
    /** 解压文件前回调函数
    @param [in] index 文件编号
    @param [in] name 文件名称
    @param [in] isFolder 是否为文件夹
    @param [in] askExtractMode 解压询问模式
    @param [in] unPackSize 原大小
    @param [in] position 完成大小
    @return 成功返回RC_S_OK,否则返回错误号
    */                            
    virtual HResult PrepareOperation( uint32_t index,
                                      const RCString& name, 
                                      bool isFolder,                                       
                                      RCExractAskMode askExtractMode, 
                                      uint64_t unPackSize,
                                      const uint64_t* position) = 0 ;
    
    /** 解压文件错误信息
    @param [in] index 编号
    @param [in] message 错误信息
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult MessageError(uint32_t index, const RCString& message) = 0 ;
    
    /** 解压文件操作结果
    @param [in] index 文件编号
    @param [in] operationResult 解压结果
    @param [in] encrypted 是否加密
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult SetOperationResult(uint32_t index,
                                       RCExractOperationResult operationResult,
                                       bool encrypted) = 0 ;

protected:
    
    /** 默认析构函数
    */
    virtual ~IExtractFolderArchiveCallback() {} ;
};

/** 压缩文件解压界面回调，作用单位：压缩包
*/
class IExtractCallbackUI : 
    public IExtractFolderArchiveCallback
{
public:
    
    /** 打开压缩文件之前事件
    @param [in] name 压缩文件名
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult BeforeOpen(const RCString& name) = 0 ;
    
    /** 打开压缩文件结果事件
    @param [in] name 压缩文件名
    @param [in] result 打开压缩文件结果
    @param [in] encrypted 压缩文件是否为加密文件
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult OpenResult(const RCString& name, HResult result, bool encrypted) = 0 ;
    
    /** 压缩文件中没有文件可以解压
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult ThereAreNoFiles() = 0 ;
    
    /** 压缩文件解压结果
    @param [in] result 解压结果
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult ExtractResult(HResult result) = 0;
    
    /** 设置解压密码
    @param [in] password 解压密码
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult SetPassword(const RCString& password) = 0 ;

    /** 返回解压过程中的错误数
    @return 返回解压过程错误数
    */
    virtual uint64_t GetErrorCount(void) = 0 ;

protected:
    
    /** 默认析构函数
    */
    virtual ~IExtractCallbackUI() {} ;
};

/** 智能指针定义
*/
typedef RCComPtr<IExtractFolderArchiveCallback> IExtractFolderArchiveCallbackPtr ;

END_NAMESPACE_RCZIP

#endif //__IExtractFileCallback_h_
