/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCWimHeader_h_
#define __RCWimHeader_h_ 1

#include "format/wim/RCWimResource.h"

BEGIN_NAMESPACE_RCZIP

/** Wim 常量定义
*/
namespace NWimHeaderFlags
{
    const uint32_t s_compression     = 2;
    const uint32_t s_spanned         = 8;
    const uint32_t s_rpFix           = 0x80;
    const uint32_t s_xpress          = 0x20000;
    const uint32_t s_lzx             = 0x40000;
}

/** Wim 头结构
*/
class RCWimHeader
{
public:

    /** 默认构造函数
    */
    RCWimHeader() ;
    
    /** 默认析构函数
    */
    ~RCWimHeader() ;

public:

    /** 解析
    @param [in] p 数据
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult Parse(const byte_t* p);

    /** 是否压缩
    @return 压缩返回true,否则返回false
    */
    bool IsCompressed() const ;

    /** 是否支持
    @return 支持返回true,否则返回false
    */
    bool IsSupported() const ;

    /** 是否lzx模式
    @return lzx返回true,否则返回false
    */
    bool IsLzxMode() const ;

    /** 是否Span
    @return Span返回true,否则返回false
    */
    bool IsSpanned() const ;

    /** 是否新版本
    @return 新版返回true,否则返回false
    */
    bool IsNewVersion() const ;

    /** 是否From Archive
    @return From Archive返回true,否则返回false
    */
    bool AreFromOnArchive(const RCWimHeader& h) ;
        
public:

    /** 标记
    */
    uint32_t m_flags;

    /** 版本
    */
    uint32_t m_version;

    /** 编号
    */
    uint16_t m_partNumber;

    /** 分块数
    */
    uint16_t m_numParts;

    /** Image 数
    */
    uint32_t m_numImages;

    /** Guid
    */
    byte_t m_guid[16];
    
    /** Offset Resource
    */
    RCWimResource m_offsetResource;

    /** Xml
    */
    RCWimResource m_xmlResource;

    /** Meta Data
    */
    RCWimResource m_metadataResource;
};

END_NAMESPACE_RCZIP

#endif //__RCWimHeader_h_
