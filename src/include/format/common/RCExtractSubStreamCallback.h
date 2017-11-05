/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/
#include "interface/IArchive.h"
#include "interface/IStream.h"
#include "interface/IUnknownImpl.h"

BEGIN_NAMESPACE_RCZIP

/** 嵌套包解压进度回调接口
*/
class RCExtractSubStreamCallback :
    public IUnknownImpl<IArchiveExtractCallback>
{
public:
    
    /** 默认构造函数
    */
    RCExtractSubStreamCallback() ;
    
    /** 析构函数
    */
    virtual ~RCExtractSubStreamCallback() ;
    
    /** 初始化
    @param [in] destFileName 解压文件的目标文件名，含完整路径
    @param [in] fileSize 文件解压后大小， 如果为 uint64_t(-1) 表示未知大小
    @param [in] openCallback 打开文档进度回调接口
    @return 成功返回true，否则返回false
    */
    bool Init(const RCString& destFileName, 
              uint64_t fileSize,
              IArchiveOpenCallback* openCallback) ;
    
public:
    
    /** 总体数量
    @param [in] total 总体的数量
    @return 成功返回RC_S_OK，否则返回错误号
    */
    virtual HResult SetTotal(uint64_t total) ;
    
    /** 当前完成
    @param [in] completed 当前完成的数量
    @return 成功返回RC_S_OK，否则返回错误号
    */
    virtual HResult SetCompleted(uint64_t completed) ;
    
public:
    
    /** 获取输出流
    @param [in] index 解压文件或者目录的下标值
    @param [out] outStream 输出流的接口指针，引用计数加1
    @param [in] askExtractMode 解压模式
    @return 成功返回RC_S_OK，否则返回错误号
    */
    virtual HResult GetStream(uint32_t index, ISequentialOutStream** outStream, int32_t askExtractMode) ;
    
    /** 准备解压操作
    @param [in] index 解压文件或者目录的下标值
    @param [in] askExtractMode 解压模式
    @return 成功返回RC_S_OK，否则返回错误号
    */
    virtual HResult PrepareOperation(uint32_t index, int32_t askExtractMode) ;
    
    /** 设置结果
    @param [in] index 解压文件或者目录的下标值
    @param [in] resultEOperationResult 操作结果
    @return 成功返回RC_S_OK，否则返回错误号
    */
    virtual HResult SetOperationResult(uint32_t index, int32_t resultEOperationResult) ;
    
    /** 设置多线程解压标记
    @param [in] isMultiThreads 如果为true，当前为多线程解压模式，否则为单线程解压模式
    @return 成功返回RC_S_OK，否则返回错误号
    */
    virtual HResult SetMultiThreadMode(bool isMultiThreads) ;

    /** 获取解压路径
    @param [out] path 解压路径
    @return 成功返回RC_S_OK，否则返回错误号
    */
    virtual HResult GetDestinationDirectory(RCString& path) ;
    
    /** 获取需要从解压路径中去除的路径部分
    @param [out] removePathParts 需要从解压路径中去除的路径部分
    @return 成功返回RC_S_OK，否则返回错误号
    */
    virtual HResult GetRemovePathParts(RCVector<RCString>& removePathParts) ;

    /** 获取解压路径选项
    @param [out] pathMode 解压路径选项
    @return 成功返回RC_S_OK，否则返回错误号
    */
    virtual HResult GetPathMode(RCExractPathMode& pathMode) ;

    /** 获取解压覆盖选项
    @param [out] overwriteMode 解压覆盖选项
    @return 成功返回RC_S_OK，否则返回错误号
    */
    virtual HResult GetOverwriteMode(RCExractOverwriteMode& overwriteMode) ;

    /** 获取解压更新方式
    @param [out] updateMode 解压更新方式
    @return 成功返回RC_S_OK，否则返回错误号
    */
    virtual HResult GetUpdateMode(RCExractUpdateMode& updateMode) ;
    
    /** 是否保留损坏文件
    @return 是返回true，否则返回false
    */
    virtual bool IsKeepDamageFiles(void) ;
    
private:

    /** 打开文档接口
    */
    IArchiveOpenCallback* m_openCallback ;
    
    /** 解压目标文件名
    */ 
    RCString m_destFileName ;
    
    /** 文件大小
    */
    uint64_t m_fileSize ;
    
    /** 目标输出流的接口指针
    */
    ISequentialOutStreamPtr m_spOutFileStream ;
};

END_NAMESPACE_RCZIP
