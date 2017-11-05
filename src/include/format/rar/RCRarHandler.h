/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCRarHandler_h_
#define __RCRarHandler_h_ 1

#include "interface/IArchive.h"
#include "interface/ICoder.h"
#include "interface/IUnknownImpl.h"
#include "RCRarVolumeInStream.h"
#include "common/RCSystemUtils.h"

BEGIN_NAMESPACE_RCZIP

class RCRarHandler:
    public IUnknownImpl3<IInArchive,
                         ISetCompressCodecsInfo,
                         IArchiveVolumeType>
{
public:

    /** 默认构造函数
    */
    RCRarHandler()
    {
#ifdef COMPRESS_MT
        m_numThreads = RCSystemUtils::GetNumberOfProcessors();
#endif
    }

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

    /** 设置编码解码器管理接口指针
    @param [in] compressCodecsInfo 编码解码器指针
    @return 成功返回RC_S_OK，否则返回错误号 
    */
    virtual HResult SetCompressCodecsInfo(ICompressCodecsInfo* compressCodecsInfo) ;

    /** 获取格式属性
    @param [in] inStream 文件输入流
    @param [in] maxCheckStartPosition 查找文件格式签名的最大偏移位置
    @param [out] isVolume 分卷压缩包返回true，否则返回false
    */
    virtual HResult GetVolumeType(IInStream* inStream, uint64_t maxCheckStartPosition, bool& isVolume) ;

private:

    /** 获取包的大小
    @param [in] refIndex 索引
    @return 返回包的大小
    */
    uint64_t GetPackSize(int32_t refIndex) const ;

    /** 是否是固实压缩
    @param [in] refIndex 索引
    @return 是返回true，否则返回false
    */
    bool IsSolid(int32_t refIndex)
    {
        const RCRarItemExPtr& item = m_items[m_refItems[refIndex]->m_itemIndex];
        if (item->m_unPackVersion < 20)
        {
            if (m_archiveInfo.IsSolid())
            {
                return (refIndex > 0);
            }
            return false;
        }
        return item->IsSolid();
    }

    /** 打开文档
    @param [in] stream 输入流
    @param [in] maxCheckStartPosition 查找文件格式签名的最大偏移位置
    @param [in] openArchiveCallback 打开文档回调接口指针
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult Open2(IInStream* stream, const uint64_t* maxCheckStartPosition, IArchiveOpenCallback* openArchiveCallback) ;

private:

    /** 编码解码器管理接口指针
    */
    ICompressCodecsInfoPtr m_compressCodecsInfo ;

    /** ref项
    */
    RCVector<RCRarRefItemPtr> m_refItems ;

    /** 项
    */
    RCVector<RCRarItemExPtr> m_items ;

#ifdef COMPRESS_MT

    /** 文档
    */
    RCVector<RCVector<RCRarIn>> m_archives;

    /** 输入流
    */
    RCVector<RCVector<IInStreamPtr>> m_spInStream;

#else

    /** 文档
    */
    RCVector<RCRarIn> m_archives ;

#endif

    /** 文档信息
    */
    RCRarInArchiveInfo m_archiveInfo ;

#ifdef COMPRESS_MT

    /** 线程数
    */
    uint32_t m_numThreads ;

#endif
};

END_NAMESPACE_RCZIP

#endif //__RCRarHandler_h_
