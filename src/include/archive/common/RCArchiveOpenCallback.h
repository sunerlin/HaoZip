/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCArchiveOpenCallback_h_
#define __RCArchiveOpenCallback_h_ 1

#include "archive/common/IOpenCallbackUI.h"
#include "interface/IPassword.h"
#include "interface/IArchive.h"
#include "interface/IUnknownImpl.h"
#include "base/RCString.h"
#include "common/RCVector.h"
#include "filesystem/RCFileInfo.h"

BEGIN_NAMESPACE_RCZIP

/** 文档打开回调接口
*/
class RCArchiveOpenCallback:
    public IUnknownImpl4< IArchiveOpenCallback,
                          IArchiveOpenVolumeCallback,
                          IArchiveOpenSetSubArchiveName,
                          ICryptoGetTextPassword
                        >
{
public:

    /** 默认构造函数
    */
    RCArchiveOpenCallback() ;
    
    /** 默认析构函数
    */
    ~RCArchiveOpenCallback() ;

public:
    
    /** 初始化
    @param [in] folderPrefix 文件路径，结束应该是路径分隔符
    @param [in] fileName 文件名
    @return 成功返回RC_S_OK,否则返回错误号
    */
    HResult Init(const RCString& folderPrefix,  const RCString& fileName) ;
    
    /** 在m_fileNames中查找文件名，不区分大小写
    @param [in] name 文件名
    @return 返回下标，如果失败返回-1
    */
    int32_t FindName(const RCString& name);

public:
    
    /** 总数量
    @param [in] files 总的文件个数
    @param [in] bytes 总的文件大小
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult SetTotal(uint64_t files, uint64_t bytes) ;
    
    /** 完成数量
    @param [in] files 已经完成的文件个数
    @param [in] bytes 已经完成的文件大小
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult SetCompleted(uint64_t files, uint64_t bytes) ;

    /** 检查是否终止
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult CheckBreak();
    
    /** 修复分卷
    @param [in] items 修复前的分卷序列
    @param [in] volumeFirst 第一个分卷
    @param [out] outputFilenames 修复后的分卷序列
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult VolumeRepair(const RCVector<RCArchiveVolumeItem>& items, RCString& volumeFirst, RCVector<RCString>& outputFilenames);
    
    /** 设置错误
    @param [in] errorCode 错误号
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult SetError(int32_t errorCode) ;

    /** 获取属性
    @param [in] propID 属性ID
    @param [out] value 属性值
    @return 成功返回RC_S_OK，否则返回错误号
    */
    virtual HResult GetProperty(RCPropertyID propID, RCVariant& value) ;
    
    /** 获取流
    @param [in] name 流的属性名称
    @param [out] inStream 返回输入流接口，引用计数加1
    @return 成功返回RC_S_OK，否则返回错误号
    */
    virtual HResult GetStream(const RCString& name, IInStream** inStream) ;
    
    /** 获取路径
    @param [out] folder 流的路径
    @return 成功返回RC_S_OK，否则返回错误号
    */
    virtual HResult GetFolderPrefix(RCString& folder);

    /** 设置名称
    @param [in] name 文档名称
    @return 成功返回RC_S_OK，否则返回错误号
    */
    virtual HResult SetSubArchiveName(const RCString& name) ;
    
    /** 获取密码
    @param [out] password 获取的密码
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult CryptoGetTextPassword(RCString& password) ;
    
    /** 获取文件列表
    @return 返回文件列表
    */
    RCVector<RCString>& GetFileNames() ;
    
    /** 获取文件列表
    @return 返回文件列表
    */
    const RCVector<RCString>& GetFileNames() const ;
    
    /** 获取 IOpenCallbackUI 接口
    @return 返回界面打开回调接口指针
    */
    IOpenCallbackUI* GetCallback() ;
    
    /** 获取 IOpenCallbackUI 接口
    @return 返回界面打开回调接口指针
    */
    const IOpenCallbackUI* GetCallback() const ;
    
    /** 设置 IOpenCallbackUI 接口
    @param [in] callback 界面打开回调接口
    */
    void SetCallback(IOpenCallbackUI* callback) ;
    
    /** 获取 IArchiveOpenCallback 接口
    @return 返回文档打开回调接口指针
    */
    const IArchiveOpenCallbackPtr& GetReOpenCallback() const ;
    
    /** 设置 IArchiveOpenCallback 接口
    @param [in] reOpenCallback 文件打开回调接口指针
    */
    void SetReOpenCallback( const IArchiveOpenCallbackPtr& reOpenCallback) ;
    
    /** 设置TotalSize
    @param [in] totalSize 设置总大小
    */
    void SetTotalSize(uint64_t totalSize) ;
    
    /** 获取TotalSize
    @return 返回总大小
    */
    uint64_t GetTotalSize() const ;
    
    /** 获取TotalSize引用
    @return 返回总大小的引用
    */
    uint64_t& TotalSize() ;
    
private:
    
    /** 文件名列表
    */
    RCVector<RCString> m_fileNames ;
    
    /** 打开界面回调接口指针
    */
    IOpenCallbackUI* m_callback ;
    
    /** 文档打开回调接口指针
    */
    IArchiveOpenCallbackPtr m_reOpenCallback ;
    
    /** 总大小
    */
    uint64_t m_totalSize ;
    
private:
    
    /** 目录前缀
    */
    RCString m_folderPrefix ;
    
    /** 文件信息结构
    */
    RCFileInfo m_fileInfo;
    
    /** 子文档模式
    */
    bool m_subArchiveMode ;
    
    /** 子文档名称
    */
    RCString m_subArchiveName ;
};

END_NAMESPACE_RCZIP

#endif //__RCArchiveOpenCallback_h_
