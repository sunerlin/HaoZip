/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCZipHandler_h_
#define __RCZipHandler_h_ 1

#include "interface/IArchive.h"
#include "interface/ICoder.h"
#include "interface/IUnknownImpl.h"
#include "interface/IStream.h"
#include "common/RCSystemUtils.h"
#include "RCZipExtFilter.h"
#include "RCZipFileFilter.h"
#include "RCZipItem.h"
#include "RCZipIn.h"

BEGIN_NAMESPACE_RCZIP

class RCZipHandler:
    public IUnknownImpl4<IInArchive,IOutArchive,ISetCompressCodecsInfo,ISetProperties>
{
public:

    /** 默认构造函数
    */
    RCZipHandler() ;

    /** 默认析构函数
    */
    ~RCZipHandler() ;

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
    virtual HResult Extract(const std::vector<uint32_t>& indices, int32_t testMode, IArchiveExtractCallback* extractCallback);
    
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

public:

    /** 文件扩展名算法过滤器
    */
    static RCVector<RCZipExtFilterPtr> s_extFilters;

    /** 文件算法过滤器
    */
    static RCZipFileFilter s_fileFilter;

private:
    /** 是否设置了智能文件类型过滤
    */
    bool m_hasExtFilters ;

    /** 是否设置了文件压缩算法过滤
    */
    bool m_hasFileFilters ;

private:

    /** zip items
    */
    RCVector<RCZipItemExPtr> m_zipItems;

#ifdef COMPRESS_MT
    /** 输入文档
    */
    RCVector<RCZipInPtr> m_zipInArchive;

    /** 输入流
    */
    RCVector<IInStreamPtr> m_spInStream;
#else
    /** 输入文档
    */
    RCZipIn m_zipInArchive;
#endif

    /** 编码解码器管理接口指针
    */
    ICompressCodecsInfoPtr m_compressCodecsInfo;

    //@{

    /** @name method mode 
    */

    /** 压缩等级
    */
    int32_t m_level;

    /** 主模式
    */
    int32_t m_mainMethod;

    /** 字典大小
    */
    uint32_t m_dicSize;

    /** algo
    */
    uint32_t m_algo;

    /** passes
    */
    uint32_t m_numPasses;

    /** fastbytes
    */
    uint32_t m_numFastBytes;

    /** 匹配搜索周期
    */
    uint32_t m_numMatchFinderCycles;

    /** 是否匹配搜索周期
    */
    bool m_numMatchFinderCyclesDefined;

    //@}

    //@{

    /** @name AES
    */

    /** 强制ACE模式
    */
    bool m_forceAesMode;

    /** 是否是ACE模式
    */
    bool m_isAesMode;

    /** AES模式密匙
    */
    byte_t m_aesKeyMode;

    //@}

    //@{

    /** @name local
    */

    /** ntfs时间
    */
    bool m_writeNtfsTimeExtra;

    /** local
    */
    bool m_forseLocal;

    /** utf8
    */
    bool m_forseUtf8;

    //@}

#ifdef COMPRESS_MT
    /** 线程数
    */
    uint32_t m_numThreads;
#endif

    void InitMethodProperties()
    {
        m_level      = -1;
        m_mainMethod = -1;
        m_algo       = 0xFFFFFFFF;
        m_dicSize    = 0xFFFFFFFF;
        m_numPasses  = 0xFFFFFFFF;
        m_numFastBytes = 0xFFFFFFFF;
        m_numMatchFinderCycles = 0xFFFFFFFF;
        m_numMatchFinderCyclesDefined = false;

        m_forceAesMode = false;
        m_isAesMode  = false;
        m_aesKeyMode = 3; // aes-256

        m_writeNtfsTimeExtra = true;
        m_forseLocal = false;
        m_forseUtf8  = false;

#ifdef COMPRESS_MT
        m_numThreads = RCSystemUtils::GetNumberOfProcessors();
#endif
    }
};

END_NAMESPACE_RCZIP

#endif //__RCZipHandler_h_
