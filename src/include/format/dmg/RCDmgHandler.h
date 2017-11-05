/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCDmgHandler_h_
#define __RCDmgHandler_h_ 1

#include "base/RCDefs.h"
#include "interface/IArchive.h"
#include "interface/ICoder.h"
#include "interface/IUnknownImpl.h"
#include "interface/IStream.h"
#include "common/RCVector.h"
#include "base/RCString.h"
#include "format/dmg/RCDmgFile.h"

BEGIN_NAMESPACE_RCZIP

class RCXmlItem ;

class RCDmgHandler:
    public IUnknownImpl2<IInArchive,
                         ISetCompressCodecsInfo>
{
public:

    /** 默认构造函数
    */
    RCDmgHandler() ;
    
    /** 析构函数
    */
    virtual ~RCDmgHandler() ;
    
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

public:

    /** 设置编码解码器管理接口指针
    @param [in] compressCodecsInfo 编码解码器指针
    @return 成功返回RC_S_OK，否则返回错误号 
    */
    virtual HResult SetCompressCodecsInfo(ICompressCodecsInfo* compressCodecsInfo) ;

private:

    /** 打开文档
    @param [in] stream 输入流接口
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult Open2(IInStream *stream) ;

    /** 从BASE64编码的字节获取原始字节
    @param [in] c BASE64编码的字节
    @return 返回原始字节
    */
    static int32_t Base64ToByte(char c) ;

    /** 从BASE64编码的内存获取二进制
    @param [out] dest 返回原始字节
    @param [in] src BASE64编码的内存
    @param [in] srcLen 内存长度
    @return 返回原始字节长度
    */
    static int32_t Base64ToBin(byte_t* dest, const char* src, int32_t srcLen) ;

    /** 查找密匙 
    @param [in] item xml项
    @param [out] key 返回密匙
    @param [in] nextTag 下一个标签
    @return 返回索引
    */
    static int32_t FindKeyPair(const RCXmlItem& item, const RCStringA& key, const RCStringA& nextTag) ;

    /** 从密匙中获取字符串
    @param [in] item xml项
    @param [out] key 返回密匙
    @param [in] nextTag 下一个标签
    @return 返回字符串
    */
    static RCStringA GetStringFromKeyPair(const RCXmlItem& item, const RCStringA& key, const RCStringA& nextTag) ;
       
private:

    /** 编码解码管理器
    */
    ICompressCodecsInfoPtr m_compressCodecsInfo ;

    /** 输入流
    */
    IInStreamPtr m_inStream;

    /** Xml
    */
    RCStringA m_xml ;

    /** 文件列表
    */
    RCVector<RCDmgFilePtr> m_files;

    /** 文件Indicators
    */
    RCVector<int32_t> m_fileIndices;
};

END_NAMESPACE_RCZIP

#endif //__RCDmgHandler_h_
