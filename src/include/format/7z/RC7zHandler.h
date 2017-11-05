/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RC7zHandler_h_
#define __RC7zHandler_h_ 1

#include "RCArchiveOutHandler.h"
#include "format/7z/RC7zArchiveDatabaseEx.h"
#include "format/7z/RC7zCompressionMode.h"
#include "interface/IArchive.h"
#include "interface/IStream.h"
#include "interface/ICoder.h"
#include "interface/IUnknownImpl.h"
#include "common/RCVector.h"
#include "format/7z/RC7zCompressionMode.h"
#include "format/7z/RC7zItem.h"

BEGIN_NAMESPACE_RCZIP

#ifdef RC_STATIC_SFX  

class RC7zHandler:
    public IUnknownImpl2<IInArchive,
                         ISetCompressCodecsInfo>

#else

class RC7zHandler:
    public RCArchiveOutHandler,
    public IUnknownImpl4<IInArchive,
                         IOutArchive,
                         ISetProperties,
                         ISetCompressCodecsInfo>
#endif
{
public:

    /** 默认构造函数
    */
    RC7zHandler() ;
    
    /** 默认析构函数
    */
    ~RC7zHandler() ;
    
public:

    //@{
    /**@name IInArchive 接口
    */
    
    /** 打开文档
    @param [in] stream 输入流接口
    @param [in] maxCheckStartPosition 查找文件格式签名的最大偏移位置
    @param [in] openArchiveCallback 打开文档回调接口指针
    @return 成功返回RC_S_OK，否则返回错误号
    */
    virtual HResult Open(IInStream* stream, uint64_t maxCheckStartPosition, IArchiveOpenCallback* openArchiveCallback) ;
    
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
    virtual HResult Extract(const std::vector<uint32_t>& indices, int32_t testMode, IArchiveExtractCallback* extractCallback) ;
    
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
    
    //@}

#ifndef RC_STATIC_SFX

    //@{
    /**@name IOutArchive 接口
    */
    
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
    
    //@} 
    
    //@{
    /**@name ISetProperties 接口
    */
    
    /** 设置属性
    @param [in] propertyArray 文档属性数值
    @return 成功返回RC_S_OK，否则返回错误号
    */
    virtual HResult SetProperties(const RCPropertyNamePairArray& propertyArray) ;
    
    //@}
    
#endif
    
    //@{
    /**@name ISetCompressCodecsInfo 接口
    */
    
    /** 设置编码解码器管理接口指针
    @param [in] compressCodecsInfo 编码解码器指针
    @return 成功返回RC_S_OK，否则返回错误号
    */
    virtual HResult SetCompressCodecsInfo(ICompressCodecsInfo* compressCodecsInfo) ;
    
    //@}
    
private:
    
    /** 是否加密
    @param [in] index2 索引
    @return 是返回true，否则返回false
    */
    bool IsEncrypted(uint32_t index2) const ;

#ifndef RC_STATIC_SFX 

    /** 设置密码
    @param [in] methodMode 压缩模式
    @param [in] updateCallback 解压回调管理器
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult SetPassword(RC7zCompressionMethodMode& methodMode, IArchiveUpdateCallback* updateCallback) ;

    /** 设置压缩模式
    @param [in] methodMode 压缩模式
    @param [in] methodsInfo 模式信息
    @param [in] numThreads 线程数
    @param [in] totalSize 总大小
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult SetCompressionMethod(RC7zCompressionMethodMode& methodMode, 
                                 RCVector<RCOneMethodInfo>& methodsInfo, 
                                 uint32_t numThreads,
                                 uint64_t totalSize) ;

    /** 设置压缩模式 
    @param [in] methodMode 压缩模式
    @param [in] headerMethod 头压缩模式
    @param [in] totalSize 总大小
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult SetCompressionMethod(RC7zCompressionMethodMode& methodMode,
                                 RC7zCompressionMethodMode& headerMethod,
                                 uint64_t totalSize) ;
#endif

    /** 获取属性
    @param [in] index 索引
    @param [in] propID 属性ID
    @param [in] value 属性值
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult DoGetProperty(uint32_t index, RCPropertyID propID, RCVariant& value) ;

private:

    /** 是否是copy模式
    @param [in] methodName 模式名
    @return 是返回true，否则返回false
    */
    static bool IsCopyMethod(const RCString& methodName) ;
    
private:

    /** 输入流
    */
    IInStreamPtr m_inStream ;

    /** 文档基本数据
    */
    RC7zArchiveDatabaseEx m_db ;

    /** 是否有密码
    */
    bool m_passwordIsDefined ;

    /** 线程数
    */
    uint32_t m_numThreads ;

    /** crc大小
    */
    uint32_t m_crcSize ; 

    /** 绑定信息
    */
    RCVector<RC7zBind> m_binds ;

    /** 编码管理器
    */
    ICompressCodecsInfoPtr m_codecsInfo ;
    
#ifndef RC_STATIC_SFX

    /** 从小字节序获取Uint32
    @param [in] p 内存数据
    @return uint32
    */
    static uint32_t GetUInt32FromMemLE(const byte_t* p) ;

    /** Uint32转成字符串
    @param [in] value 数值
    @return 字符串
    */
    static RCString ConvertUInt32ToString(uint32_t value) ;

    /** 大小转成字符串
    @param [in] value 数值
    @return 字符串
    */
    static RCString GetStringForSizeValue(uint32_t value) ;

    /** 获取二进制
    @param [in] value 数值
    @return 字符串
    */
    static RCString::value_type GetHex(byte_t value) ;

    /** 字符串中添加二进制
    @param [in,out] result 输入字符串，返回字符串
    @param [in] value 数值
    */
    static void AddHexToString(RCString& result, byte_t value) ; 

#endif

    /** 文件信息
    */
    RCVector<uint64_t> m_fileInfoPopIDs;

    /** 初始化属性ID
    */
    void FillPopIDs() ;

    /** 设置属性
    @param [in] v RC7zUInt64DefVector
    @param [in] index 索引
    @param [in] prop 属性
    */
    static void SetPropFromUInt64Def(RC7zUInt64DefVector& v, int32_t index, RCVariant& prop) ;
};

END_NAMESPACE_RCZIP

#endif //__RC7zHandler_h_
