/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCStringUtilA_h_
#define __RCStringUtilA_h_ 1

#include "base/RCNonCopyable.h"
#include "base/RCString.h"

BEGIN_NAMESPACE_RCZIP

/** 字符串转换工具类，支持Ansi字符串
*/
class RCStringUtilA:
    private RCNonCopyable
{
public:

    /** 默认构造函数
    */
    RCStringUtilA() ;
    
    /** 默认析构函数
    */
    ~RCStringUtilA() ;
    
public:
    
    /** 64位无符号整型转成多字节字符串
    @param [in] value 整型值
    @param [out] s 保存转换结果的缓冲区，不做越界检查
    @param [in] base 进制，默认为十进制
    @return 返回转换后的字符串值
    */
    static void ConvertUInt64ToString(uint64_t value, char* s, uint32_t base = 10) ;

    /** 64位有符号整型转成多字节字符串
    @param [in] value 整型值
    @param [out] s 保存转换结果的缓冲区，不做越界检查
    */
    static void ConvertInt64ToString(int64_t value, char* s) ;
    
    /** 多字节十进制字符串转成64位无符号整型
    @param [in] s 需要转换的字符串
    @param [out] end 转换后，指向当前处理的字符地址，可以为NULL
    @return 返回整型值
    */
    static uint64_t ConvertStringToUInt64(const char* s, const char** end);
    
    /** 多字节八进制字符串转成64位无符号整型
    @param [in] s 需要转换的字符串
    @param [out] end 转换后，指向当前处理的字符地址，可以为NULL
    @return 返回整型值
    */
    static uint64_t ConvertOctStringToUInt64(const char* s, const char** end);
    
    /** 多字节十六进制字符串转成64位无符号整型
    @param [in] s 需要转换的字符串
    @param [out] end 转换后，指向当前处理的字符地址，可以为NULL
    @return 返回整型值
    */
    static uint64_t ConvertHexStringToUInt64(const char* s, const char** end);

    /** 多字节字符串转成64位有符号整型
    @param [in] s 需要转换的字符串
    @param [out] end 转换后，指向当前处理的字符地址，可以为NULL
    @return 返回整型值
    */
    static int64_t ConvertStringToInt64(const char* s, const char** end);
    
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
    static char* StringCopy(char* dest, const char* src) ;
    
    /** 字符转换成大写
    @param [in] ch 需要转换的字符串
    @return 转换结果字符
    */
    static char CharToUpper(char ch) ;
    
    /** 字符转换成小写
    @param [in] ch 需要转换的字符串
    @return 转换结果字符
    */
    static char CharToLower(char ch) ;
    
    /** 在字符串中查找字符
    @param [in] data 字符串起始地址
    @param [in] ch 需要查找的字符
    @return 查找到的字符串地址，找不到则返回NULL
    */
    static const char* StringChr(const char* data, char ch) ;

    /** 取左侧子串
    @param [in] str 需要操作的字符串
    @param [in] count 需要取的字符个数
    @return 返回子串
    */
    static RCStringA Left(const RCStringA& str, int32_t count) ;
    
    /** 取右侧子串
    @param [in] str 需要操作的字符串
    @param [in] count 需要取的字符个数
    @return 返回子串
    */
    static RCStringA Right(const RCStringA& str, int32_t count) ;
    
    /** 取子串
    @param [in] str 需要操作的字符串
    @param [in] startIndex 起始下标值
    @param [in] count 字符个数，如果为RCString::npos，则表示从startIndex到字符串结束
    @return 返回子串
    */
    static RCStringA Mid(const RCStringA& str, int32_t startIndex, RCStringA::size_type count = RCStringA::npos) ;

    /** 在字符串中替换子字符串
    @param [in,out] str 需要操作的字符串
    @param [in] from 被替换的字符串
    @param [in] to   替换成的字符串
    */
    static void Replace(RCStringA& str ,const RCStringA::value_type* from, const RCStringA::value_type* to) ;
    
    /** 去除左侧指定字符
    @param [in,out] str 需要操作的字符串
    @param [in] value 需要Trim的字符列表
    */
    static void TrimLeft(RCStringA& str, const RCStringA::value_type* value = " \r\t\n" ) ;
        
    /** 去除右侧指定字符
    @param [in,out] str 需要操作的字符串
    @param [in] value 需要Trim的字符列表
    */
    static void TrimRight(RCStringA& str, const RCStringA::value_type* value = " \r\t\n" ) ;
        
    /** 去除左右两侧指定字符
    @param [in,out] str 需要操作的字符串
    @param [in] value 需要Trim的字符列表
    */
    static void Trim(RCStringA& str, const RCStringA::value_type* value = " \r\t\n" ) ;
        
    /** 转换为大写
    @param [in,out] str 需要操作的字符串
    */
    static void MakeUpper(RCStringA& str) ;
    
    /** 转换为小写
    @param [in,out] str 需要操作的字符串
    */
    static void MakeLower(RCStringA& str) ;
    
    /** 比较字符串，不区分大小写
    @param [in] lhs 左值字符串
    @param [in] rhs 右值字符串
    @return 返回值定义如下：
            0  - 相等
            1  - lhs > rhs
            -1 - lhs < rhs
    */
    static int32_t CompareNoCase(const RCStringA& lhs, const RCStringA& rhs) ;
    
    /** 比较字符串，区分大小写
    @param [in] lhs 左值字符串
    @param [in] rhs 右值字符串
    @return 返回值定义如下：
            0  - 相等
            1  - lhs > rhs
            -1 - lhs < rhs
    */
    static int32_t Compare(const RCStringA& lhs, const RCStringA& rhs) ;
        
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
    static int32_t CompareNoCase(const char* cs, const char* ct, size_t count) ;
};

END_NAMESPACE_RCZIP

#endif //__RCStringUtilA_h_
