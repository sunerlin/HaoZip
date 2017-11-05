/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCTypes_h_
#define __RCTypes_h_ 1

#include <tchar.h>
#include "base/RCNameSpace.h"
#include "base/RCConfig.h"

#ifndef RCZIP_HAS_STDINT
    #define BOOST__STDC_CONSTANT_MACROS_DEFINED 1
    #define __STDC_CONSTANT_MACROS 1
    #include <base/stdint.h>
#else
    #include <cstdint>
#endif

#include <vector>

/** 返回值类型
*/
typedef long HResult ;

BEGIN_NAMESPACE_RCZIP

/** 字符类型
*/
#if defined RCZIP_UNICODE
    typedef wchar_t char_t ;
#else 
    typedef char    char_t ;
#endif

/** 字节类型
*/
typedef unsigned char  byte_t ;

/** 接口ID类型
*/
typedef uint32_t RC_IID ;

/** 压缩解压编码ID类型
*/
typedef uint64_t RCMethodID ;

/** 返回值定义
*/
enum HResultCode
{
    /** 系统错误号
    */
    RC_S_OK                    = ((HResult)0x00000000L),
    RC_S_FALSE                 = ((HResult)0x00000001L),
    RC_E_NOTIMPL               = ((HResult)0x80004001L),
    RC_E_NOINTERFACE           = ((HResult)0x80004002L),
    RC_E_ABORT                 = ((HResult)0x80004004L),
    RC_E_FAIL                  = ((HResult)0x80004005L),
    RC_E_OUTOFMEMORY           = ((HResult)0x8007000EL),
    RC_E_INVALIDARG            = ((HResult)0x80070057L),
    RC_E_CLASSNOTAVAILABLE     = ((HResult)0x80040111L),
    
    /** 自定义错误号
    */
    RC_E_CodeBegin             = ((HResult)0x00000002L) ,   //自定义错误号起始值
    RC_E_Password                                       ,   //密码错误
    RC_E_UnexpectedEndOfArchive                         ,   //意外的文件结尾
    RC_E_ArchiveHeaderCRCError                          ,   //文件数据头CRC错误  
    RC_E_FileHeaderCRCError                             ,   //文件头CRC错误
    RC_E_DataCRCError                                   ,   //文件CRC校验错误
    RC_E_IncorrectArchive                               ,   //文件格式错误
    RC_E_MultiVolumeArchiveAreNotSupported              ,   //不支持分卷
    RC_E_ReadStreamError                                ,   //读取文件流失败
    RC_E_SeekStreamError                                ,   //Seek文件失败
    RC_E_WriteStreamError                               ,   //写文件流失败
    RC_E_DataError                                      ,   //数据错误
    RC_E_VersionNotSupported                            ,   //不支持该版本
    RC_E_Skip                                           ,   //跳过该操作
    RC_E_Password_Too_Long                              ,   //密码长度超过限制
    RC_E_NonSupport_Isz_Password                        ,   //不支持带密码的isz文件
    
    RC_E_CodeEnd                                            //自定义错误号结束值
};

/** 判断HResult是否为成功
@param [in] value 需要判断的值
@return 如果为成功值则返回true，否则返回false
*/
inline bool IsSuccess(const HResult& value)
{
    return (value == RC_S_OK) ;
}

END_NAMESPACE_RCZIP

#endif //__RCTypes_h_
