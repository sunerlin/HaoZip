/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCBZip2Handler_h_
#define __RCBZip2Handler_h_ 1

#include "interface/IArchive.h"
#include "interface/ICoder.h"
#include "interface/IUnknownImpl.h"
#include "interface/IStream.h"
#include "common/RCSystemUtils.h"
#include "filesystem/RCTempFile.h"

BEGIN_NAMESPACE_RCZIP

class RCBZip2Handler:
    public IUnknownImpl5<IInArchive,
                         IOutArchive,
                         ISetCompressCodecsInfo,
                         ISetProperties,
                         IInArchiveGetStream>

{
public:

    /** 默认构造函数
    */
    RCBZip2Handler() ;

public:

    /** 打开文档
    @param [in] stream 输入流接口
    @param [in] maxCheckStartPosition 查找文件格式签名的最大偏移位置
    @param [in] openArchiveCallback 打开文档回调接口指针
    @return 成功返回RC_S_OK，否则返回错误号
    */
    virtual HResult Open(IInStream* stream, uint64_t maxCheckStartPosition, IArchiveOpenCallback* openArchiveCallback);
    
    /** 关闭文档
    @return 成功返回RC_S_OK，否则返回错误号
    */
    virtual HResult Close() ;
    
    /** 获取内容个数
    @param [out] numItems 返回文档内容个数
    @return 成功返回RC_S_OK，否则返回错误号
    */
    virtual HResult GetNumberOfItems(uint32_t& numItems) ;
    
    /** 获取属性
    @param [in] index 属性的下标
    @param [in] propID 属性ID
    @param [out] value 属性值
    @return 成功返回RC_S_OK，否则返回错误号
    */
    virtual HResult GetProperty(uint32_t index, RCPropertyID propID, RCVariant& value) ;
    
    /** 解压
    @param [in] indices 需要解压的Item 下标数组，如果数组内容为空表示解压全部内容
    @param [in] testMode 1为测试模式，0 为正常解压模式
    @param [in] extractCallback 解压回调接口指针
    @return 成功返回RC_S_OK，否则返回错误号
    */
    virtual HResult Extract(const RCVector<uint32_t>& indices, int32_t testMode, IArchiveExtractCallback* extractCallback);
    
    /** 获取文档属性
    @param [in] propID 属性ID
    @param [out] value 属性值
    @return 成功返回RC_S_OK，否则返回错误号
    */
    virtual HResult GetArchiveProperty(RCPropertyID propID, RCVariant& value) ;
    
    /** 获取属性个数
    @param [out] numProperties 返回属性个数
    @return 成功返回RC_S_OK，否则返回错误号
    */
    virtual HResult GetNumberOfProperties(uint32_t& numProperties) ;
    
    /** 获取属性信息
    @param [in] index 属性下标值
    @param [out] name 返回属性名称
    @param [out] propID 返回属性ID
    @param [out] varType 返回属性类型
    @return 成功返回RC_S_OK，否则返回错误号
    */
    virtual HResult GetPropertyInfo(uint32_t index, RCString& name, RCPropertyID& propID, RCVariantType::RCVariantID& varType) ;
    
    /** 获取文档属性个数
    @param [out] numProperties 返回属性个数
    @return 成功返回RC_S_OK，否则返回错误号
    */
    virtual HResult GetNumberOfArchiveProperties(uint32_t& numProperties) ;
    
    /** 获取文档属性信息
    @param [in] index 属性下标值
    @param [out] name 返回属性名称
    @param [out] propID 返回属性ID
    @param [out] varType 返回属性类型
    @return 成功返回RC_S_OK，否则返回错误号
    */
    virtual HResult GetArchivePropertyInfo(uint32_t index, RCString& name, RCPropertyID& propID, RCVariantType::RCVariantID& varType) ;

    /** 获取流
    @param [in] index 流的下标值
    @param [out] stream 返回输入流接口，引用计数加1
    @param [in] openCallback 打开文档回调接口指针
    @return 成功返回RC_S_OK，否则返回错误号
    */
    virtual HResult GetStream(uint32_t index, ISequentialInStream** stream, IArchiveOpenCallback* openCallback);  
        
public:

    /** 更新内容
    @param [in] outStream 输出流的接口指针
    @param [in] numItems 文档内容的个数
    @param [in] updateCallback 文档更新接口指针
    @return 成功返回RC_S_OK，否则返回错误号
    */
    virtual HResult UpdateItems(ISequentialOutStream* outStream, 
                                uint32_t numItems, 
                                IArchiveUpdateCallback* updateCallback) ;
    
    /** 获取文件时间类型
    @param [out] type 时间类型数值
    @return 成功返回RC_S_OK，否则返回错误号
    */
    virtual HResult GetFileTimeType(uint32_t& type) ;

public:

    /** 设置编码解码器管理接口指针
    @param [in] compressCodecsInfo 编码解码器指针
    @return 成功返回RC_S_OK，否则返回错误号 
    */
    virtual HResult SetCompressCodecsInfo(ICompressCodecsInfo* compressCodecsInfo) ;

public:

    /** 设置属性
    @param [in] propertyArray 文档属性数值
    @return 成功返回RC_S_OK，否则返回错误号
    */
    virtual HResult SetProperties(const RCPropertyNamePairArray& propertyArray) ;

private:    
    
    /** 初始化压缩算法属性
    */
    void InitMethodProperties() ;
    
private:

    /** 压缩包文件流
    */
    IInStreamPtr m_stream ;

    /** 输出流
    */
    ISequentialInStreamPtr m_seqStream ;
    
    /** 压缩后大小
    */
    uint64_t m_packSize ;

    /** 数据起始位置
    */
    uint64_t m_startPosition ;
    
    /** 压缩后大小值是否有效
    */
    bool m_packSizeDefined ;

    /** 编解码信息管理器
    */
    ICompressCodecsInfoPtr m_compressCodecsInfo ;

    /** 嵌套包解压临时文件
    */
    RCTempFile m_tmpFile;

    /** 嵌套包子文件流
    */
    IInStreamPtr m_subStream;

    //@{

    /** @name 
    */

    /** 压缩级别
    */
    uint32_t m_level;

    /** 字典大小
    */
    uint32_t m_dicSize;

    /** passes
    */
    uint32_t m_numPasses;

    /** 线程数
    */
    uint32_t m_numThreads;

    //@}
};

END_NAMESPACE_RCZIP

#endif //__RCBZip2Handler_h_
