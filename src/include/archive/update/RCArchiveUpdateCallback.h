/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCArchiveUpdateCallback_h_
#define __RCArchiveUpdateCallback_h_ 1

#include "base/RCTypes.h"
#include "interface/ICoder.h"
#include "interface/IUnknownImpl.h"
#include "interface/IArchive.h"
#include "interface/IPassword.h"
#include "interface/RCPropertyID.h"
#include "filesystem/RCFileInfo.h"
#include "archive/common/RCDirItem.h"
#include "archive/common/RCDirItems.h"
#include "archive/common/RCArcItem.h"
#include "archive/update/RCUpdateCallbackUI.h"
#include "archive/update/RCUpdatePair2.h"
#include "archive/update/RCUpdateOptions.h"
#include "archive/update/RCArchiveUpdateFilter.h"
#include "common/RCVariant.h"

BEGIN_NAMESPACE_RCZIP

/** 输出流事件接口
*/
class IOutStreamEvent ;

/** 文档压缩，更新回调接口
*/
class RCArchiveUpdateCallback : 
    public IUnknownImpl6<IArchiveUpdateCallback2, 
                         ICryptoGetTextPassword2, 
                         ICryptoGetTextPassword,
                         ICompressProgressInfo,
                         IMtCompressProgressInfo,
                         ISetMultiThreadMode>
{
public:
    
    /** 构造函数
    @param [in] outStreamEvent 输出流事件接口
    */
    RCArchiveUpdateCallback(IOutStreamEvent* outStreamEvent);
    
public:
    
    /** 完成百分比
    @param [in] inSize 输入字节数
    @param [in] outSize 输出字节数
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult SetRatioInfo(uint64_t inSize, uint64_t outSize);
    
    /** 设置文件总数
    @param [in] total 文件总数
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult SetTotal(uint64_t total);
    
    /** 设置完成数量
    @param [in] completeValue 完成数量
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult SetCompleted(uint64_t completeValue);
    
    /** 设置更新项目信息
    @param [in] index 编号
    @param [out] newData 新数据
    @param [out] newProperties 新属性
    @param [out] indexInArchive 包内编号
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult GetUpdateItemInfo(uint32_t index, 
                                      int32_t& newData, 
                                      int32_t& newProperties, 
                                      uint32_t& indexInArchive ) ;
    
    /** 取得属性
    @param [in] index 文件编号
    @param [in] propID 属性编号
    @param [in] value 属性值
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult GetProperty(uint32_t index, RCPropertyID propID, RCVariant& value) ;
    
    /** 取得文档属性
    @param [in] propID 属性编号
    @param [in] value 属性值
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult GetArchiveProperty(RCPropertyID propID, RCVariant& value) ;
    
    /** 取得文件输入流
    @param [in] index 文件编号
    @param [out] inStream 文件输入流
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult GetStream(uint32_t index, ISequentialInStream **inStream) ;
    
    /** 设置文件操作结果
    @param [in] index 文件编号
    @param [in] operationResult 操作结果
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult SetOperationResult(uint32_t index, int32_t operationResult);
    
    /** 取得分卷大小
    @param [in] index 分卷编号
    @param [out] size 分卷大小
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult GetVolumeSize(uint32_t index, uint64_t& size);
    
    /** 取得分卷流
    @param [in] index 分卷编号
    @param [out]  volumeStream 分卷流
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult GetVolumeStream(uint32_t index, ISequentialOutStream **volumeStream) ;
    
    /** 取得密码
    @param [out] passwordIsDefined 密码是否定义 返回0表示没有定义密码，返回1表示有密码
    @param [out] password 密码
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult CryptoGetTextPassword2(int32_t *passwordIsDefined, RCString& password);
    
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
    
    /** 设置压缩解压多线程模式
    @param [in] isMultThread 是否为多线程
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult SetMultiThread(bool isMultThread) ;

public:
    
    /** 取得分卷名称
    @return 返回分卷名称
    */
    const RCString& GetVolName(void) const;
    
    /** 设置分卷名称
    @param [in] volName 分卷名称
    */
    void SetVolName(const RCString& volName);

    /** 取得分卷扩展名
    @return 返回分卷扩展名
    */
    const RCString& GetVolExt(void) const;
    
    /** 设置分卷扩展名
    @param [in] volExt 分卷扩展名
    */
    void SetVolExt(const RCString& volExt);

    /** 返回界面更新回调接口
    @return 界面更新回调接口
    */
    const RCUpdateCallbackUI* GetCallback() const;
    
    /** 设置界面更新回调接口
    @param [in] callback 界面更新回调接口
    */
    void SetCallback(RCUpdateCallbackUI* callback);
    
    /** 返回是否共享写
    @return 共享写返回true,否则返回false
    */
    bool GetShareForWrite(void) const;
    
    /** 设置共享写
    @param [in] shareForWrite
    */
    void SetShareForWrite(bool shareForWrite);

    /** 返回是否标准输入模式
    @return 返回是否标准输入模式
    */
    bool GetStdInMode(void) const;
    
    /** 设置是否标准输入模式
    @param [in] stdInMode 标准输入模式
    */
    void SetStdInMode(bool stdInMode);

    /** 返回更新项列表
    @return 返回更新项列表
    */
    const RCDirItems* GetDirItems(void) const;
    
    /** 设置更新项
    @param [in] dirItems 更新项
    */
    void SetDirItems(const RCDirItems* dirItems);

    /** 取得包内项列表
    @return 返回包内项列表
    */
    const std::vector<RCArcItem>* GetArcItems(void) const;
        
    /** 设置包内项列表
    @param [in] arcItems 项列表
    */
    void SetArcItems(const std::vector<RCArcItem>* arcItems);

    /** 取得更新比较列表
    @return 返回更新比较列表
    */
    const std::vector<RCUpdatePair2>* GetUpdatePairs(void) const;
        
    /** 设置更新比较列表
    @param [in] updatePairs2 更新比较列表
    */
    void SetUpdatePairs(const std::vector<RCUpdatePair2>* updatePairs2);

    /** 取得新名称列表
    @return 返回新名称列表
    */
    const std::vector<RCString>* GetNewNames(void) const;
    
    /** 设置新名称列表
    @param [in] newNames 新名称列表
    */
    void SetNewNames(const std::vector<RCString>* newNames) ;

    /** 取得输入文档指针
    @return 输入文档指针
    */
    const IInArchivePtr& GetArchive(void) const;
    
    /** 取得输入文档指针引用
    @return 输入文档指针引用
    */
    IInArchivePtr& GetArchive(void);

    /** 设置保存路径模式
    @param [in] pathMode 路径保存模式
    */
    void SetPathMode(RCUpdateOptions::path_mode pathMode) ;
        
    /** 设置压缩文件过滤器
    @param [in] spUpdateFilter 文件过滤器指针
    */
    void SetUpdateFileter(const RCArchiveUpdateFilterPtr& spUpdateFilter) ;
    
    /** 获取压缩文件过滤器
    @return 返回压缩文件过滤器
    */
    RCArchiveUpdateFilterPtr GetUpdateFileter(void) const ;

private:

    /** 分卷大小
    */
    std::vector<uint64_t> m_volumesSizes;

    /** 分卷名称
    */
    RCString m_volName;

    /** 分卷后缀
    */
    RCString m_volExt;

    /** 更新显示回调
    */
    RCUpdateCallbackUI* m_callback;

    /** 共享写
    */
    bool m_shareForWrite;

    /** 标准输入模式
    */
    bool m_stdInMode;

    /** 目录集合
    */
    const RCDirItems *m_dirItems;

    /** 内容项集合
    */
    const std::vector<RCArcItem>* m_arcItems;

    /**
    */
    const std::vector<RCUpdatePair2>* m_updatePairs;

    /** 
    */
    const std::vector<RCString> *m_newNames;

    /** 输入文档接口
    */
    IInArchivePtr m_archive;
    
    /** 保存路径模式
    */
    RCUpdateOptions::path_mode m_pathMode ;
    
    /** IO 错误回调处理
    */
    IOutStreamEvent* m_outStreamEvent ;
    
    /** 压缩文件过滤器
    */
    RCArchiveUpdateFilterPtr m_spUpdateFilter ;
};

END_NAMESPACE_RCZIP

#endif //__RCArchiveUpdateCallback_h_
