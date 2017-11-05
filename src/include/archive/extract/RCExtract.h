/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCExtract_h_
#define __RCExtract_h_ 1

#include "archive/manager/RCArchiveCodecs.h"
#include "archive/extract/RCExtractOptions.h"
#include "archive/extract/RCExtractStatus.h"
#include "archive/common/RCWildcardCensorNode.h"
#include "interface/IArchiveInfo.h"
#include "base/RCNonCopyable.h"

BEGIN_NAMESPACE_RCZIP

/** 打开界面回调接口
*/
class IOpenCallbackUI ;

/** 解压界面回调接口
*/
class IExtractCallbackUI ;

/** 通配符匹配
*/
class RCWildcardCensorNode ;

/** 解压回调接口
*/
class RCExtractArchiveCallback;

/** 输出流事件接口
*/
class IOutStreamEvent ;

/** 解压操作
*/
class RCExtract:
    private RCNonCopyable
{
public:

    /** 默认构造函数
    */
    RCExtract() ;
    
    /** 默认析构函数
    */
    ~RCExtract() ;

public:
    
    /** 解压操作
    @param [in] spCodecs 编码管理器
    @param [in] formatIndices 文件格式列表
    @param [in] archivePaths 压缩包路径列表
    @param [in] archivePathsFull 压缩包全路径列表
    @param [in] wildcardCensor 通配符匹配
    @param [in] options 解压参数
    @param [in] openCallbackUI 压缩包打开回调界面
    @param [in] extractCallbackUI 解压回调界面
    @param [out] errorMessage 错误信息
    @param [out] stat 解压状态
    @param [in] outStreamEvent 输出流事件接口
    @param [in] overWriteMode 文件覆盖模式
    @return 成功返回RC_S_OK,否则返回错误号
    */
    HResult DecompressArchives(const RCArchiveCodecsPtr& spCodecs, 
                               const RCVector<RCArchiveID>& formatIndices,
                               RCVector<RCString>& archivePaths, 
                               RCVector<RCString>& archivePathsFull,
                               const RCWildcardCensorNode& wildcardCensor,
                               const RCExtractOptions& options,
                               IOpenCallbackUI* openCallbackUI,
                               IExtractCallbackUI* extractCallbackUI,
                               RCString& errorMessage,
                               RCExtractStatus& stat,
                               IOutStreamEvent* outStreamEvent,
                               RCExractOverwriteMode* overWriteMode = 0);
private:
    
    /** 单个文件解压操作
    @param [in] archive 输入文档
    @param [in] packSize 压缩大小
    @param [in] wildcardCensor 通配符匹配
    @param [in] options 解压参数
    @param [in] extractCallbackUI 解压界面回调接口
    @param [in] extractArchiveCallback 文档解压回调接口
    @param [out] errorMessage 错误信息
    @return 成功返回RC_S_OK,否则返回错误号
    */
    HResult DecompressArchive(IInArchive* archive,
                              uint64_t packSize,
                              const RCWildcardCensorNode& wildcardCensor,
                              const RCExtractOptions& options,
                              IExtractCallbackUI* extractCallbackUI,
                              RCExtractArchiveCallback* extractArchiveCallback,
                              RCString& errorMessage);

private:
    
    /** TRar文件
    */
    struct TRarFile;

    /** Rar文件名
    */
    class RCRarFileName;

};

END_NAMESPACE_RCZIP

#endif //__RCExtract_h_
