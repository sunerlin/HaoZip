/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCDefs_h_
#define __RCDefs_h_ 1

#include "base/RCTypes.h"

#ifdef _DEBUG
    #include <assert.h>
#endif

BEGIN_NAMESPACE_RCZIP

/** Unicode版本的字符串宏定义
*/
#if !defined(_T)
    #if defined (RCZIP_UNICODE)
        #define _T(x)   L##x 
    #else 
        #define _T(x)   x
    #endif
#endif

/** 重新抛出异常
*/
#define _ThrowAgain()       throw

/** 抛出错误号
*/
#define _ThrowCode(ex)      throw ( static_cast<HResult>(ex) )

/** 抛出异常
*/
#define _Throw(ex)          throw ( ex )

/** 内部使用的Assert宏定义
*/
#ifdef _DEBUG
    #define RCZIP_ASSERT(x) { assert(x); }
#else
    #define RCZIP_ASSERT(x)
#endif

/** 导入/导出 定义
*/
#if !defined (RCZIP_NO_WINDOWS_API)
    #define RCZIP_API_EXPORT __declspec(dllexport)
    #define RCZIP_API_IMPORT __declspec(dllimport)
#else
    #define RCZIP_API_EXPORT
    #define RCZIP_API_IMPORT
#endif

/** RCZIP API
*/
#ifdef _RCZIP_API_EXPORTS
    #define RCZIP_API    RCZIP_API_EXPORT
#else
    #define RCZIP_API    RCZIP_API_IMPORT
#endif

/** 取最小值, 保持代码兼容性而引入
@param [in] a 第一个值
@param [in] b 第二个值
@return 返回 a 与 b 中比较小的一个
*/
template <class T> 
inline T MyMin(const T& a, const T& b)
{
    return a < b ? a : b;
}

/** 取最大值, 保持代码兼容性而引入
@param [in] a 第一个值
@param [in] b 第二个值
@return 返回 a 与 b 中比较大的一个
*/
template <class T> 
inline T MyMax(const T& a, const T& b)
{
    return a > b ? a : b;
}

/** 比较函数
@param [in] a 第一个值
@param [in] b 第二个值
@return 若 a == b 返回 0; 若a > b 返回 1; 若 a < b 返回 -1
*/
template <class T> 
inline int MyCompare(const T& a, const T& b)
{
    return a < b ? -1 : (a == b ? 0 : 1);
}

/** 将bool值转换为int值
@param [in] value 需要转换的bool值
@return 返回转换后的int值，用 1 表示true, 0 表示false
*/
inline int BoolToInt(const bool& value)
{
    return (value ? 1: 0);
}

/** 将int值转换为bool值
@param [in] value 需要转换的int值
@return 返回转换后的bool值
*/
inline bool IntToBool(const int& value)
{
    return (value != 0) ;
}

END_NAMESPACE_RCZIP

#endif //__RCDefs_h_
