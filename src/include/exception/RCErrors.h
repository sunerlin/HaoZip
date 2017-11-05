/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCErrors_h_
#define __RCErrors_h_ 1

#include "base/RCNonCopyable.h"
#include "base/RCString.h"

BEGIN_NAMESPACE_RCZIP

/** 错误号和错误信息处理
*/
class RCErrors:
    private RCNonCopyable
{
public:

    /** 默认构造函数
    */
    RCErrors() ;
    
    /** 默认析构函数
    */
    ~RCErrors() ;

public:

#ifndef RCZIP_OS_WIN
    /** 获取标准错误号(errno)
    @return 返回错误号
    */
    static int32_t GetStdErrorCode(void) ;
#endif
    
    /** 获取系统相关的错误号(如Windows下的GetLastError())
    @return 返回HResult格式的错误号
    */
    static HResult GetSystemErrorCode(void) ;

#ifndef RCZIP_OS_WIN    
    /** 根据错误号取得错误信息
    @param [in] errorCode 错误号
    @return 与错误号对应的错误信息
    */
    static RCString GetStdErrorMessage(int32_t errorCode) ;
#endif
    
    /** 根据错误号取得错误信息
    @param [in] hr HResult格式的错误号
    @return 与错误号对应的错误信息
    */
    static RCString GetSystemErrorMessage(HResult hr) ;
    
    /** HResult与系统错误号之间的转换
    @param [in] hr HResult格式的错误号
    @return 返回操作系统相关的错误号
    */
    static int32_t ConvertToSystemError(HResult hr) ;
    
    /** 系统错误号与HResult之间的转换
    @param [in] errorCode 操作系统相关的错误号
    @return 返回HResult格式的错误号
    */
    static HResult ConvertToHResult(int32_t errorCode) ;
    
    /** bool 类型转换成HResult
    @param [in] result 需要转换的bool值
    @return 返回HResult格式的错误号
    */
    static HResult ConvertBoolToHRESULT(bool result) ;
    
    /** 获取当前系统错误号，如Windows平台，为GetLastError()返回结果
    @return 返回HResult格式的错误号
    */
    static HResult GetLastErrorCode(void) ;
    
    /** 获取当前的系统错误信息，如Windows平台，为GetLastError()对应的错误信息
    @return 返回最后一次错误号对应的错误信息，如果不是错误则返回空串
    */
    static RCString GetLastErrorMessage(void) ;
};

END_NAMESPACE_RCZIP

#endif //__RCErrors_h_
