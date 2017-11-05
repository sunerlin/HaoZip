/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCStringUtil_h_
#define __RCStringUtil_h_ 1

#include "base/RCNonCopyable.h"
#include "base/RCString.h"
#include <stdarg.h>

BEGIN_NAMESPACE_RCZIP

/** 字符串转换工具类
*/
class RCStringUtil:
    private RCNonCopyable
{
public:

    /** 默认构造函数
    */
    RCStringUtil() ;
    
    /** 默认析构函数
    */
    ~RCStringUtil() ;
    
public:
    
    /** 32位有符号整型转成字符串
    @param [in] value 整型值
    @return 返回转换后的字符串值
    */
    static RCString ConvertInt32ToString(int32_t value ) ;
    
    /** 32位无符号整型转成字符串
    @param [in] value 整型值
    @return 返回转换后的字符串值
    */
    static RCString ConvertUInt32ToString(uint32_t value ) ;
    
    /** 32位无符号整型转成十六进制字符串
    @param [in] value 整型值
    @return 返回转换后的字符串值
    */
    static RCString ConvertUInt32ToHex(uint32_t value ) ;
    
    /** 64位无符号整型转成宽字节字符串
    @param [in] value 整型值
    @param [out] s 返回转换后的字符串，缓冲区大小内部不检查
    */
    static void ConvertUInt64ToString(uint64_t value, wchar_t* s) ;
    
    /** 64位有符号整型转成宽字节字符串
    @param [in] value 整型值
    @param [out] s 返回转换后的字符串，缓冲区大小内部不检查
    */
    static void ConvertInt64ToString(int64_t value, wchar_t *s) ;
    
    /** 64位有符号整型转成字符串
    @param [in] value 整型值
    @return 返回转换后的字符串值
    */
    static RCString ConvertInt64ToString(int64_t value ) ;
    
    /** 64位无符号整型转成字符串
    @param [in] value 整型值
    @return 返回转换后的字符串值
    */
    static RCString ConvertUInt64ToString(uint64_t value ) ;
    
    /** 宽字节字符串转成64位无符号整型
    @param [in] s 字符串起始地址
    @param [out] end 转换截止的字符串地址，可选参数
    @return 返回转换后整型值
    */
    static uint64_t ConvertStringToUInt64(const wchar_t* s, const wchar_t** end);
    
    /** 获取字符串长度
    @param [in] s 字符串起始地址
    @return 返回字符串长度
    */
    static size_t StringLen(const wchar_t* s) ;
    
    /** 获取字符串长度
    @param [in] s 字符串起始地址
    @return 返回字符串长度
    */
    static size_t StringLen(const char* s) ;
    
    /** 字符串复制, 不做缓冲区长度检查
    @param [in] src 源字符串地址
    @param [out] dest 目标缓冲区地址
    @return 返回dest
    */
    static wchar_t* StringCopy(wchar_t* dest, const wchar_t* src) ;
       
    /** 比较字符串，不区分大小写
    @param [in] lhs 左值字符串
    @param [in] rhs 右值字符串
    @return 返回值定义如下：
            0  - 相等
            1  - lhs > rhs
            -1 - lhs < rhs
    */
    static int32_t CompareNoCase(const RCString& lhs, const RCString& rhs) ;
    
    /** 比较字符串，区分大小写
    @param [in] lhs 左值字符串
    @param [in] rhs 右值字符串
    @return 返回值定义如下：
            0  - 相等
            1  - lhs > rhs
            -1 - lhs < rhs
    */
    static int32_t Compare(const RCString& lhs, const RCString& rhs) ;
    
    /** 字符转换成大写
    @param [in] ch 需要转换的字符串
    @return 转换结果字符
    */
    static wchar_t CharToUpper(wchar_t ch) ;
    
    /** 字符转换成小写
    @param [in] ch 需要转换的字符串
    @return 转换结果字符
    */
    static wchar_t CharToLower(wchar_t ch) ;
    
    /** 在字符串中查找字符
    @param [in] data 字符串起始地址
    @param [in] ch 需要查找的字符
    @return 查找到的字符串地址，找不到则返回NULL
    */
    static const wchar_t* StringChr(const wchar_t* data, wchar_t ch) ;
    
    /** 在字符串中替换子字符串
    @param [in,out] str 需要操作的字符串
    @param [in] from 被替换的字符串
    @param [in] to   替换成的字符串
    */
    static void Replace(RCString& str ,const RCString::value_type* from, const RCString::value_type* to) ;
        
    /** 在字符串中替换子字符
    @param [in,out] str 需要操作的字符串
    @param [in] from 被替换的字符串
    @param [in] to   替换成的字符串
    */
    static void Replace(RCString& str ,const RCString::value_type from, const RCString::value_type to) ;
    
    /** 去除左侧指定字符
    @param [in,out] str 需要操作的字符串
    @param [in] value 需要Trim的字符列表
    */
    static void TrimLeft(RCString& str, const RCString::value_type* value = _T(" \r\t\n")) ;
        
    /** 去除右侧指定字符
    @param [in,out] str 需要操作的字符串
    @param [in] value 需要Trim的字符列表
    */
    static void TrimRight(RCString& str, const RCString::value_type* value = _T(" \r\t\n")) ;
        
    /** 去除左右两侧指定字符
    @param [in,out] str 需要操作的字符串
    @param [in] value 需要Trim的字符列表
    */
    static void Trim(RCString& str, const RCString::value_type* value = _T(" \r\t\n")) ;
        
    /** 转换为大写
    @param [in,out] str 需要操作的字符串
    */
    static void MakeUpper(RCString& str) ;
    
    /** 转换为小写
    @param [in,out] str 需要操作的字符串
    */
    static void MakeLower(RCString& str) ;
    
    /** 取左侧子串
    @param [in] str 需要操作的字符串
    @param [in] count 需要取的字符个数
    @return 返回子串
    */
    static RCString Left(const RCString& str, int32_t count) ;
    
    /** 取右侧子串
    @param [in] str 需要操作的字符串
    @param [in] count 需要取的字符个数
    @return 返回子串
    */
    static RCString Right(const RCString& str, int32_t count) ;
    
    /** 取子串
    @param [in] str 需要操作的字符串
    @param [in] startIndex 起始下标值
    @param [in] count 字符个数，如果为RCString::npos，则表示从startIndex到字符串结束
    @return 返回子串
    */
    static RCString Mid(const RCString& str, int32_t startIndex, RCString::size_type count = RCString::npos) ;

    /** 格式化字符串
    @param [in,out] str 结果字符串
    @param [in] fmt 格式化字符
    @param [in] ... 可变参数列表
    */
    static void Format(RCString& str, const RCString::value_type* fmt , ...) ;
        
    /** 字符串的通配符比较, 通配符为 * 与 ?
    @param [in] sPattern 含有通配符的字符串
    @param [in] sFileName 需要进行匹配比较的字符串
    @param [in] bNoCase 是否区分大小写，true 表示不区分大小写, false表示区分大小写
    @return 返回true表示匹配成功，否则匹配失败
    */
    static bool WildcardCompare(const char_t* sPattern, 
                                const char_t* sFileName, 
                                bool bNoCase) ;    
       
private:

    /** 比较字符串，不区分大小写
    @param [in] cs 左值字符串
    @param [in] ct 右值字符串
    @param [in] count 最多比较的字符数
    @return 返回值定义如下：
            0  - 相等
            1  - lhs > rhs
            -1 - lhs < rhs
    */
    static int32_t CompareNoCase(const wchar_t* cs, const wchar_t* ct, size_t count) ;

    /** 格式化字符串
    @param [out] buf 结果缓冲区
    @param [in] size 缓冲区大小
    @param [in] format 格式化字符
    @param [in] args 可变参数列表
    @return 转换结果： 0 表示成功, -1 表示失败，1 表示缓冲区不够
    */
    static int32_t FormatToBuffer(RCString::value_type* buf, int32_t size ,const RCString::value_type* format, va_list args) ;
};

END_NAMESPACE_RCZIP

#endif //__RCStringUtil_h_
