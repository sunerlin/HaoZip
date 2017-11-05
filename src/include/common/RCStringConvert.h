/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCStringConvert_h_
#define __RCStringConvert_h_ 1

#include "base/RCString.h"
#include "base/RCNonCopyable.h"
#include "common/RCDynamicBuffer.h"

BEGIN_NAMESPACE_RCZIP

typedef enum
{
    RC_ENCODING_LOCAL,
    RC_CN_GB2312,
    RC_CN_GBK,
    RC_CN_GB18030,
    RC_CN_BIG5,
    RC_UNICODE_UTF7,
    RC_UNICODE_UTF8,
    RC_UNICODE_UTF16,
    RC_UNICODE_UTF16_BE,
    RC_UNICODE_UTF16_LE,
    RC_UNICODE_UTF32,
    RC_UNICODE_UTF32_BE,
    RC_UNICODE_UTF32_LE,
    RC_UNICODE_UCS2,
    RC_UNICODE_UCS2_BE,
    RC_UNICODE_UCS2_LE,
    RC_UNICODE_UCS4,
    RC_UNICODE_UCS4_BE,
    RC_UNICODE_UCS4_LE,
    
    RC_ENCODING_UNKNOWN
}RCEncodingType ;

/** 字符编码代码页
*/
typedef enum
{
    RC_CP_ACP   = 0 ,
    RC_CP_OEMCP = 1 ,
    RC_CP_UTF8  = 65001
}RCCodePage ;

class RCStringConvert:
    private RCNonCopyable
{   
public:
    
    /** 默认构造函数
    */
    RCStringConvert() ;
    
    /** 默认析构函数
    */
    ~RCStringConvert() ;
    
    /** 打开编码转换器
    @param [in] toCode 目标编码
    @param [in] fromCode 源编码
    @return 成功返回true，失败返回false
    */
    bool Open(RCEncodingType toCode, RCEncodingType fromCode) ;
    
    /** 转换编码
    @param [in] inBuf 输入数据缓冲区
    @param [in] inBufLen 输入数据长度
    @param [out] result 转换结果
    @return 成功返回true，失败返回false
    */
    bool Convert(const char* inBuf, size_t inBufLen, RCDynamicCharBuffer& result) ;
   
   /** 转换编码
    @param [in] inBuf 输入数据缓冲区
    @param [in] inBufLen 输入数据长度
    @param [out] result 转换结果, 以多字节方式存储
    @return 成功返回true，失败返回false
    */
    bool Convert(const char* inBuf, size_t inBufLen, RCStringA& result) ;
   
   /** 转换编码
    @param [in] inBuf 输入数据缓冲区
    @param [in] inBufLen 输入数据长度
    @param [out] result 转换结果, 以宽字节方式存储
    @return 成功返回true，失败返回false
    */
    bool Convert(const char* inBuf, size_t inBufLen, RCStringW& result) ;
    
    /** 关闭转换器
    @return 成功返回true，失败返回false
    */
    bool Close(void) ;

    /** UTF8编码转成Unicode编码
    @param [in] inBuf 输入数据缓冲区
    @param [in] inBufLen 输入数据长度
    @param [out] result 转换结果
    @return 成功返回true,失败返回false
    */
    static bool ConvertUTF8ToUnicode(const char* inBuf, size_t inBufLen, RCStringW& result) ;
    
    /** Unicode编码转成UTF8编码
    @param [in] inBuf 输入数据缓冲区
    @param [in] inBufLen 输入数据长度
    @param [out] result 转换结果
    @return 成功返回true,失败返回false
    */
    static bool ConvertUnicodeToUTF8(const wchar_t* inBuf, size_t inBufLen, RCStringA& result) ;
    
    /** 多字节转换成宽字节
    @param [in] src 源字符串编码
    @param [in] codePage 字符串编码的代码页
    @return 转换后的宽字节编码字符串
    */
    static RCStringW MultiByteToUnicodeString(const RCStringA& src, uint32_t codePage = RC_CP_ACP) ;
    
    /** 多字节转换成宽字节
    @param [in] src 源字符串编码
    @param [in] len 字符串的长度，如果为-1则认为src是以0结尾的字符串
    @param [in] codePage 字符串编码的代码页
    @return 转换后的宽字节编码字符串
    */
    static RCStringW MultiByteToUnicodeString(const char* src, int32_t len = -1, uint32_t codePage = RC_CP_ACP) ;
    
    /** 宽字节转换成多字节
    @param [in] src 源字符串编码
    @param [in] codePage 字符串编码的代码页
    @param [in] defaultChar 当无法转码时，替换的默认字符
    @param [out] defaultCharWasUsed 默认字符是否使用
    @return 转换后的多字节编码字符串
    */
    static RCStringA UnicodeStringToMultiByte(const RCStringW& src,
                                              uint32_t codePage, 
                                              char defaultChar, 
                                              bool& defaultCharWasUsed) ;
    
    /** 宽字节转换成多字节
    @param [in] src 源字符串编码
    @param [in] codePage 字符串编码的代码页
    @return 转换后的多字节编码字符串
    */                                 
    static RCStringA UnicodeStringToMultiByte(const RCStringW& src, uint32_t codePage = RC_CP_ACP);
    
    /** 获取编码字符串
    @param [in] type 字符串编码
    @return 返回编码字符串
    */
    static RCStringA GetEncodingString(RCEncodingType type) ;
    
public:
    
    /** 判断字节序是否为LITTLE ENDIAN
    */
    static bool IsLittleEndian(void) ;
    
    /** 根据BOM值获取编码类型
    @param [in] inBuf 缓冲区地址
    @param [in] inBufLen 缓冲区数据长度
    @param [out] type 返回编码类型
    @param [out] bomLen 可以为NULL，返回BOM标记的长度
    @return 如果成功返回true，失败返回false
    */
    static bool GetEncodingType(const char* inBuf, 
                                size_t inBufLen,
                                RCEncodingType& type,
                                uint32_t* bomLen) ;

private:
    
    /** 打开编码转换器
    @param [in] toCode 目标编码，参见libiconv文档
    @param [in] fromCode 源编码，参见libiconv文档
    @return 成功返回true，失败返回false
    */
    bool Open(const char* toCode, const char* fromCode) ;
    
private:
    
    /** 实现数据
    */
    struct TImpl ;
    TImpl* m_impl ;
};

END_NAMESPACE_RCZIP

#endif //__RCStringConvert_h_
