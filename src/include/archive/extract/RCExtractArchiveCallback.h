/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCExtractArchiveCallback_h_
#define __RCExtractArchiveCallback_h_ 1

#include "archive/extract/IExtractFileCallback.h"
#include "archive/extract/RCExtractDefs.h"
#include "interface/IArchive.h"
#include "interface/IPassword.h"
#include "interface/ICoder.h"
#include "interface/IOverwrite.h"
#include "interface/IUnknownImpl.h"
#include "interface/RCPropertyID.h"
#include "common/RCVector.h"
#include "filesystem/RCOutFileStream.h"
#include "thread/RCMutex.h"
#include <set>
#include <list>

BEGIN_NAMESPACE_RCZIP

/** 解压进度显示
*/
class RCLocalProgress ;

/** 输出流事件接口
*/
class IOutStreamEvent ;

/** 文档解压回调接口
*/
class RCExtractArchiveCallback:
    public IUnknownImpl4< IArchiveExtractCallback,
                          ICryptoGetTextPassword,
                          IMtCompressProgressInfo,
                          IAskOverwrite
                        >
{
public:

    /** 默认构造函数
    */
    RCExtractArchiveCallback() ;
    
    /** 默认析构函数
    */
    virtual ~RCExtractArchiveCallback() ;
    
public:
    
    /** 总体数量
    @param [in] total 总体的数量
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult SetTotal(uint64_t total) ;
    
    /** 当前完成
    @param [in] completed 当前完成的数量
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult SetCompleted(uint64_t completed) ;
    
    /** 获取输出流
    @param [in] index 流的下标值
    @param [out] outStream 输出流的接口指针，引用计数加1
    @param [in] askExtractMode 解压模式
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult GetStream(uint32_t index, ISequentialOutStream** outStream, int32_t askExtractMode) ;
    
    /** 准备解压操作
    @param [in] index 解压文件或者目录的下标值
    @param [in] askExtractMode 解压模式
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult PrepareOperation(uint32_t index, int32_t askExtractMode) ;
    
    /** 设置结果
    @param [in] index 解压文件或者目录的下标值
    @param [in] resultEOperationResult 操作结果
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult SetOperationResult(uint32_t index, int32_t resultEOperationResult) ;
    
    /** 设置多线程解压标记
    @param [in] isMultiThreads 如果为true，当前为多线程解压模式，否则为单线程解压模式
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult SetMultiThreadMode(bool isMultiThreads) ;
    
    /** 获取密码
    @param [out] password 获取的密码
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult CryptoGetTextPassword(RCString& password) ;
    
    /** 设置压缩进度
    @param [in] index 当前流的下标
    @param [in] inSize 输入数据大小
    @param [in] outSize 输出数据大小
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult SetRatioInfo(uint32_t index, uint64_t inSize, uint64_t outSize) ;

    /** 获取解压路径
    @param [out] path 解压路径
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult GetDestinationDirectory(RCString& path) ;

    /** 获取需要从解压路径中去除的路径部分
    @param [out] removePathParts 需要从解压路径中去除的路径部分
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult GetRemovePathParts(RCVector<RCString>& removePathParts) ;

    /** 获取解压路径选项
    @param [out] pathMode 解压路径选项
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult GetPathMode(RCExractPathMode& pathMode) ;

    /** 获取解压覆盖选项
    @param [out] overwriteMode 解压覆盖选项
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult GetOverwriteMode(RCExractOverwriteMode& overwriteMode) ;

    /** 获取解压更新方式
    @param [out] updateMode 解压更新方式
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult GetUpdateMode(RCExractUpdateMode& updateMode) ;

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
                                 RCString& newFileName) ;

    /** 是否保留损坏文件
    @return 保留损坏文件返回true,否则返回false
    */
    virtual bool IsKeepDamageFiles(void) ;
    
public:
    
    /** 初始化
    @param [in] archiveHandler 解压文档管理接口
    @param [in] extractCallback2 解压回调接口
    @param [in] stdOutMode 是否为控制台输出模式
    @param [in] directoryPath 解压路径
    @param [in] removePathParts 解压路径中需要移除的路径部分
    @param [in] itemDefaultName 默认的文件名
    @param [in] utcMTimeDefault 默认的文件修改时间
    @param [in] attributesDefault 默认文件属性
    @param [in] packSize 压缩包大小
    @param [in] fileTimeFlag 解压文件时间属性
    */
    void Init( IInArchive* archiveHandler,
               IExtractFolderArchiveCallback* extractCallback2,
               bool stdOutMode,
               const RCString& directoryPath,
               const RCVector<RCString>& removePathParts,
               const RCString& itemDefaultName,
               const RC_FILE_TIME& utcMTimeDefault,
               uint64_t attributesDefault,
               uint64_t packSize,
               RCExractFileTimeFlag fileTimeFlag);
    
    /** 初始化解压选项
    @param [in] multiArchives 是否同时解压多个文档
    @param [in] pathMode 解压路径选项
    @param [in] overwriteMode 解压覆盖选项
    @param [in] updateMode 解压更新方式
    @param [in] isKeepDamageFiles 是否保留损坏文件
    @param [in] extractToSubDir 解压每个文件到子目录中
    @param [in] outStreamEvent 解压时，输出IO的事件处理
    */
    void InitForMulti(bool multiArchives,
                      RCExractPathMode pathMode,
                      RCExractOverwriteMode overwriteMode,
                      RCExractUpdateMode updateMode,
                      bool isKeepDamageFiles,
                      bool extractToSubDir,
                      IOutStreamEvent* outStreamEvent) ;
    
    /** 设置需要移除的路径组，平面浏览模式解压使用
    @param [in] removePathList 需要移除的路径列表
    */
    void SetRemovePathList(const RCVector<RCString>& removePathList) ;
    
    /** 获取解压进度管理实现接口
    @return 返回进度显示接口指针
    */
    RCLocalProgress* GetLocalProgress() ;
    
    /** 获取已经解压的文件总大小
    @return 返回已经解压文件总大小
    */
    uint64_t GetUnpackSize() const ;
    
    /** 获取文件夹总数
    @return 返回文件夹总数
    */
    uint64_t GetNumFolders() const ;
    
    /** 获取文件总数
    @return 返回文件总数
    */
    uint64_t GetNumFiles() const ;
    
private: 
    
    /** 创建多级目录
    @param [in] dirPathParts 各级目录列表
    @param [out] fullPath 返回最末级目录
    @return 成功返回true,否则返回false
    */
    bool CreateComplexDirectory(const RCVector<RCString>& dirPathParts, 
                                RCString& fullPath) ;

    /** 获取文件时间属性
    @param [in] index 文件编号
    @param [in] propID 属性编号
    @param [in] filetime 文件时间
    @param [out] filetimeIsDefined 文件时间是否定义
    @return 成功返回RC_S_OK,否则返回错误号
    */
    HResult GetTime(int32_t index, 
                    RCPropertyID propID, 
                    RC_FILE_TIME& filetime, 
                    bool& filetimeIsDefined) ;
    
    /** 创建单个目录
    @param [in] fullPath 目录全路径
    @return 成功返回true,否则返回false
    */             
    bool CreateOneDirectory(const RCString& fullPath) ;
    
    /** 清除解压过程文件数据
    */
    void ClearExtractingFileInfo() ;
    
    /** 诊断错误
    @param [in] index 编号
    @param [in] errMsg 错误信息
    @return 成功返回RC_S_OK,否则返回错误号
    */
    HResult OnMessageError(int32_t index,const RCString& errMsg) ;
    
    /** 检查移除目录前缀项
    @param [in] removePathParts 需要移除的目录前缀
    @param [in] pathParts 当前的目录或者文件
    @param [out] numRemovePathParts 需要移除路径前缀项个数
    @return 成功返回RC_S_OK,否则返回错误号
    */
    HResult CheckRemovePathParts(const RCVector<RCString>& removePathParts,
                                 const RCVector<RCString>& pathParts,
                                 RCString::size_type& numRemovePathParts) const ;
    
private:
    
    /** 输入文档接口
    */
    IInArchivePtr m_spInArchive ;
    
    /** 解压回调接口
    */
    IExtractFolderArchiveCallbackPtr m_extractCallback2 ;
    
    /** 解压密码接口
    */
    ICryptoGetTextPasswordPtr m_cryptoGetTextPassword;
    
    /** 更新方式
    */
    RCExractUpdateMode m_updateMode ;
    
    /** 解压路径选项
    */
    RCExractPathMode m_pathMode;
    
    /** 覆盖选项
    */
    RCExractOverwriteMode m_overwriteMode;
    
    /** 是否保留损坏文件
    */
    bool m_keepDamageFiles ;
    
    /** 解压到子目录
    */
    bool m_extractToSubDir;

    /** 解压目标路径
    */
    RCString m_directoryPath;
    
    /** 解压后是否需要按照包内的创建时间更新解压后的文件
    */
    bool m_writeCTime;
    
    /** 解压后是否需要按照包内的访问时间更新解压后的文件
    */
    bool m_writeATime;
    
    /** 解压后是否需要按照包内的修改时间更新解压后的文件
    */
    bool m_writeMTime;
    
    /** 解压后需要从包内路径中移除的路径部分
    */
    RCVector<RCString> m_removePathParts;
    
    /** 解压后需要移除的路径组，平面浏览模式解压使用
    */
    RCVector<RCString> m_removePathList ;
    
    /** 默认的包内项名称
    */
    RCString m_itemDefaultName;
    
    /** 默认的文件修改时间
    */
    RC_FILE_TIME m_utcMTimeDefault;
    
    /** 默认文件属性
    */
    uint64_t m_attributesDefault;
    
    /** 是否为控制台输出模式
    */
    bool m_stdOutMode ;
                              
private:

    /** 解压进度管理实现接口
    */
    RCLocalProgress* m_localProgressSpec;
    
    /** 解压进度接口
    */
    ICompressProgressInfoPtr m_localProgress;
    
    /** 压缩后总大小
    */
    uint64_t m_packTotal;
    
    /** 解压后总大小
    */
    uint64_t m_unpackTotal;

    /** 是否为同时解压多个文件
    */
    bool m_multiArchives;
    
    /** 文件夹总数
    */
    uint64_t m_numFolders;
    
    /** 文件总数
    */
    uint64_t m_numFiles;
    
    /** 已经解压的文件总大小
    */
    uint64_t m_unpackSize;
    
    /** 错误总数
    */
    uint64_t m_numErrors ;
    
    /** 本次压缩中创建的目录缓存，为优化IO性能
    */
    typedef std::set<RCString> TCreatedDirectoryCache ;
    TCreatedDirectoryCache m_createdDirCache ;
    
    /** IO输出事件处理
    */
    IOutStreamEvent* m_outStreamEvent ;
    
private:
    
    /** 当前解压的文件信息
    */
    struct TExtractingFileInfo ;
    
    /** 当前解压的文件信息数据容器
    */
    RCVector<TExtractingFileInfo*> m_extractingFileInfoArray ;
    
    /** 是否为多线程压缩
    */
    bool m_isMultiThreads ;
    
    /** 多线程同步锁
    */
    RCMutex m_extractMutex ;
    
private:
    /** 获取默认使用的解压文件信息
    */
    TExtractingFileInfo* GetDefaultExtractingFileInfo() ;
    
    /** 获取默认使用的解压文件信息
    */
    TExtractingFileInfo* GetExtractingFileInfo(uint32_t index) ;
    
    /** 释放解压文件信息
    */
    void ReleaseExtractingFileInfo(uint32_t index) ;
    
private:
    /** 待删除文件
    */
    typedef std::list<RCString> TDeleteFileList ;
    TDeleteFileList m_deleteFileList ;  
    
    /** 析构时需要设置的目录时间
    */
    struct TPendingDirTime
    {
        RCString m_dirPath ;
        RCFileTimeValue m_createTime ;
        RCFileTimeValue m_accessTime ;
        RCFileTimeValue m_modifyTime ;
    };
    typedef std::list<TPendingDirTime> TPendingDirTimeList ;
    TPendingDirTimeList m_pendingDirTimeList ;
};

/** 智能指针定义
*/
typedef RCComPtr<RCExtractArchiveCallback> RCExtractArchiveCallbackPtr ;

END_NAMESPACE_RCZIP

#endif //__RCExtractArchiveCallback_h_
