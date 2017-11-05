/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#include "interface/IArchive.h"
#include "interface/ICoder.h"
#include "interface/IUnknownImpl.h"
#include "interface/IStream.h"
#include "RCTarItem.h"

#ifndef __RCTarHandler_h_
#define __RCTarHandler_h_ 1

BEGIN_NAMESPACE_RCZIP

/** Tar 格式处理
*/
class RCTarHandler:
    public IUnknownImpl3<IInArchive,IOutArchive,ISetCompressCodecsInfo>
{
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
    virtual HResult GetArchiveProperty(RCPropertyID propID, RCVariant& value);
    
    /** 获取属性个数
    @param [out] numProperties 返回属性个数
    @return 成功返回RC_S_OK，否则返回错误号
    */
    virtual HResult GetNumberOfProperties(uint32_t& numProperties);
    
    /** 获取属性信息
    @param [in] index 属性下标值
    @param [out] name 返回属性名称
    @param [out] propID 返回属性ID
    @param [out] varType 返回属性类型
    @return 成功返回RC_S_OK，否则返回错误号
    */
    virtual HResult GetPropertyInfo(uint32_t index, RCString& name, RCPropertyID& propID, RCVariantType::RCVariantID& varType);
    
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
    virtual HResult GetArchivePropertyInfo(uint32_t index, RCString& name, RCPropertyID& propID, RCVariantType::RCVariantID& varType);
    
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

private:

    /** 打开文档
    @param [in] stream 输入流
    @param [in] openArchiveCallback 打开文档回调接口
    @return 成功返回RC_S_OK，否则返回错误号 
    */
    HResult Open2(IInStream *stream, IArchiveOpenCallback *openArchiveCallback);

    /** 跳至指定编号
    @param [in] index 指定编码
    @return 成功返回RC_S_OK，否则返回错误号 
    */
    HResult SkipTo(uint32_t index) ;
    
private:

    /** 编码解码器管理接口指针
    */
    ICompressCodecsInfoPtr m_compressCodecsInfo;

    /** 包内文件项信息列表
    */
    RCVector<RCTarItemEx> m_items;

    /** 输入流指针
    */
    IInStreamPtr m_stream;
    
    /** 序列输入流指针
    */
    ISequentialInStreamPtr m_seqStream ;

    /** 是否完好
    */
    bool m_isGood ;
    
    /** 当前编号
    */
    uint32_t m_curIndex;

    /** latest Read
    */
    bool m_latestIsRead;

    /** 项信息
    */
    RCTarItemEx m_latestItem ;
    
    /** 物理大小
    */
    uint64_t m_phySize;

    /** 物理大小是否定义
    */
    bool m_phySizeDefined;

    /** 复制操作指针
    */
    ICompressCoderPtr m_spCopyCoder;
};

END_NAMESPACE_RCZIP

#endif //__RCTarHandler_h_
