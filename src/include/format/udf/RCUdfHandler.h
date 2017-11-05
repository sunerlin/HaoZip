/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCUdfHandler_h_
#define __RCUdfHandler_h_ 1

#include "base/RCTypes.h"
#include "interface/IArchive.h"
#include "interface/ICoder.h"
#include "interface/IUnknownImpl.h"
#include "interface/IStream.h"
#include "interface/RCMethodDefs.h"
#include "RCUdfIn.h"
#include "RCUdfProgressImp.h"
#include "RCUdfExtentsStream.h"

BEGIN_NAMESPACE_RCZIP

/** Udf 格式处理
*/
class RCUdfHandler:
    public IUnknownImpl2<IInArchive, ISetCompressCodecsInfo>
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
    virtual HResult Close();

    /** 获取内容个数
    @param [out] numItems 返回文档内容个数
    @return 成功返回RC_S_OK，否则返回错误号
    */
    virtual HResult GetNumberOfItems(uint32_t& numItems);

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

    /** 设置编码解码器管理接口指针
    @param [in] compressCodecsInfo 编码解码器指针
    @return 成功返回RC_S_OK，否则返回错误号 
    */
    virtual HResult SetCompressCodecsInfo(ICompressCodecsInfo* compressCodecsInfo) ;
    
private:
    
    /** 将UDF文件时间转换成本地文件时间
    @param [in] t 时间
    @param [out] prop 时间属性
    */
    static void UdfTimeToFileTime(const RCUdfTime &t, RCVariant& prop);
    
    /** 获取子文件流
    @param [in] index 流的下标值
    @param [out] stream 返回输入流接口，引用计数加1
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult GetStream(uint32_t index, ISequentialInStream** stream) ; 

    /** UDF文件数据偏移
    */
    struct RCUdfRef2
    {
        /** vol
        */
        int32_t m_vol;

        /** fs
        */
        int32_t m_fs;

        /** ref
        */
        int32_t m_ref;
    };
 
private:
    
    /** 编码解码管理器
    */
    ICompressCodecsInfoPtr m_compressCodecsInfo;
    
    /** 文件输入流
    */
    IInStreamPtr m_inStream ;
    
    /** UDF文件管理器
    */
    RCUdfInArchive m_archive ;
    
    /** UDF文件偏移数据
    */
    RCVector<RCUdfRef2> m_refs2 ;
};

END_NAMESPACE_RCZIP

#endif //__RCUdfHandler_h_
