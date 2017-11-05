/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCUpdate_h_
#define __RCUpdate_h_ 1

#include "base/RCtypes.h"
#include "base/RCWindowsDefs.h"
#include "archive/manager/RCArchiveCodecs.h"
#include "archive/update/RCUpdateOptions.h"
#include "archive/update/RCUpdateErrorInfo.h"
#include "archive/update/RCUpdateCallbackUI.h"
#include "archive/update/RCCompressMethodMode.h"
#include "archive/common/IOpenCallbackUI.h"
#include "archive/common/RCArcItem.h"
#include "archive/common/RCDirItems.h"
#include "archive/common/RCTempFiles.h"

BEGIN_NAMESPACE_RCZIP

/** 通配符匹配
*/
class RCWildcardCensor ;

/** 输出流事件接口
*/
class IOutStreamEvent ;

/** 更新流程封装
*/
class RCUpdate
{
public:

    /** 压缩数据
    @param [in] codecs 编码管理器
    @param [in] censor 通配符匹配
    @param [in] options 更新参数
    @param [out]  errorInfo 错误信息
    @param [in] openCallback 打开回调接口
    @param [in] callback 界面回调接口
    @return 成功返回RC_S_OK,否则返回错误号
    */
    static HResult UpdateArchive(const RCArchiveCodecsPtr& codecs,
                                 const RCWildcardCensor& censor,
                                 RCUpdateOptions& options,
                                 RCUpdateErrorInfo& errorInfo,
                                 IOpenCallbackUI* openCallback,
                                 RCUpdateCallbackUI2* callback);
private:
    
    /** 更新压缩文件数据
    @param [in] codecs 编码管理器
    @param [in] options 更新参数
    @param [in] archive 输入文档
    @param [in] arcItems 包内文件列表
    @param [in] dirItems 更新项列表
    @param [in] tempFiles 临时文件
    @param [out] errorInfo 错误信息
    @param [in] callback 更新回调接口
    @return 成功返回RC_S_OK,否则返回错误号
    */
    static HResult UpdateWithItemLists(const RCArchiveCodecsPtr& codecs,
                                       RCUpdateOptions& options,
                                       IInArchive* archive,
                                       const RCVector<RCArcItem>& arcItems,
                                       RCDirItems& dirItems,
                                       RCTempFiles& tempFiles,
                                       RCUpdateErrorInfo& errorInfo,
                                       RCUpdateCallbackUI2* callback);

    /** 读取输入文档中的内容
    @param [in] censor 通配符匹配
    @param [in] archive 输入文档
    @param [in] defaultItemName 缺省文件名
    @param [out] archiveFileInfo 文档信息
    @param [out] arcItems 包内项列表
    @return 成功返回RC_S_OK,否则返回错误号
    */
    static HResult EnumerateInArchiveItems(const RCWildcardCensor& censor,
                                           IInArchive* archive,
                                           const RCString& defaultItemName,
                                           RCFileInfo& archiveFileInfo,
                                           RCVector<RCArcItem>& arcItems);

    /** 压缩数据
    @param [in] codecs 编码管理器
    @param [in] actionSet 更新方式
    @param [in] archive 输入文档
    @param [in] compressionMethod 压缩模式
    @param [out] archivePath 文档路径
    @param [in] arcItems 包内项列表
    @param [in] shareForWrite 共享写
    @param [in] stdInMode 标准输入模式
    @param [in] stdOutMode 标准输出模式
    @param [in] isTestMode 测试模式
    @param [in] dirItems 更新项列表
    @param [in] sfxMode 自解压模式
    @param [in] sfxModule 自解压模块文件名
    @param [in] volumesSizes 分卷大小
    @param [in] tempFiles 临时文件
    @param [out] errorInfo 错误信息
    @param [in] callback 界面回调接口
    @param [in] pathMode 路径保存模式
    @param [in] outStreamEvent 输出流事件接口
    @param [in] options 更新参数
    @return 成功返回RC_S_OK,否则返回错误号
    */
    static HResult Compress(const RCArchiveCodecsPtr& codecs,
                            const RCActionSet& actionSet,
                            IInArchive* archive,
                            const RCCompressMethodMode& compressionMethod,
                            RCArchivePath& archivePath,
                            const RCVector<RCArcItem>& arcItems,
                            bool shareForWrite,
                            bool stdInMode,
                            bool stdOutMode,
                            bool isTestMode,
                            const RCDirItems& dirItems,
                            bool sfxMode,                            
                            const RCString& sfxModule,
                            const RCVector<uint64_t>& volumesSizes,
                            RCTempFiles& tempFiles,
                            RCUpdateErrorInfo& errorInfo,
                            RCUpdateCallbackUI* callback,
                            RCUpdateOptions::path_mode pathMode,
                            IOutStreamEvent* outStreamEvent,
                            const RCUpdateOptions& options );


    /** 复制数据
    @param [in] inStream 输入流
    @param [in] outStream 输出流
    @return 成功返回RC_S_OK,否则返回错误号
    */
    static HResult CopyBlock(ISequentialInStream* inStream, 
                             ISequentialOutStream* outStream);

    /** 比较文件时间
    @param [in] arcItems 包内项列表
    @param [in] arcItemIndex 项目编号
    @param [in] mTime 修改时间
    @return = 0, 相等
            < 0, 包内 < mTime
            > 0, 包内 > mTime
    */
    static int32_t CompareArchiveItemFileTime(const RCVector<RCArcItem>& arcItems,
                                              size_t arcItemIndex,
                                              const RC_FILE_TIME& mTime) ;

    /** 检查需要更新的新文件，进行过滤
    @param [in] arcItems 包内项列表
    @param [in] dirItems 更新项列表
    @param [in] deleteNoneExists 删除不存在
    */
    static void FilterDirItems(const RCVector<RCArcItem>& arcItems,
                               RCDirItems& dirItems,
                               bool deleteNoneExists) ;

    /** 检查已经存在文档中的文件，进行过滤
    @param [in] arcItems 包内项列表
    @param [in] dirItems 更新项列表
    */
    static void FilterArcItems(RCVector<RCArcItem>& arcItems,
                               const RCDirItems& dirItems) ;
};

END_NAMESPACE_RCZIP

#endif //__RCUpdate_h_
