/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCConfig_h_
#define __RCConfig_h_ 1
/*
#pragma auto_inline(off)
#pragma intrinsic(memcmp)
#pragma intrinsic(memcpy)
#pragma intrinsic(memset)
#pragma intrinsic(strcat)
#pragma intrinsic(strcmp)
#pragma intrinsic(strcpy)
#pragma intrinsic(strlen)
#pragma intrinsic(strset)
#pragma intrinsic(wcscat)
#pragma intrinsic(wcscmp)
#pragma intrinsic(wcscpy)
#pragma intrinsic(wcslen)
*/
#if defined(_M_X64) || defined(_M_AMD64) || defined(__x86_64__) 

#ifndef _WIN64
    #define _WIN64 
#endif

#endif

/** Unicode or Ansi 版本
*/
//#if defined UNICODE || defined _UNICODE
    #define RCZIP_UNICODE 1
//#endif

/** stdint.h 选项，VC中没有这个文件
*/
#if !defined (_MSC_VER)
    #define RCZIP_HAS_STDINT 1
#endif

#if defined (_MSC_VER) || defined (_WIN32) || defined (_WIN64) 
    #define RCZIP_OS_WIN 1
#endif

#ifdef RCZIP_OS_WIN
    #pragma warning (disable: 4819)
    #pragma warning (disable: 4131)
    #pragma warning (disable: 4100)
    #pragma warning (disable: 4512)
    #pragma warning (disable: 4510)
    #pragma warning (disable: 4610)
    #pragma warning (disable: 4239)
    #pragma warning (disable: 4189)
    #pragma warning (disable: 4702)
    #pragma warning (disable: 4127)
    #pragma warning (disable: 4099)
#endif

#ifndef RC_STATIC_SFX
    /** 多线程版本压缩与解压支持
    */
    #ifndef COMPRESS_MT
        #define COMPRESS_MT 1
    #endif
    
    #ifndef COMPRESS_MF_MT
        #define COMPRESS_MF_MT 1
    #endif
    
    #ifndef COMPRESS_BZIP2_MT
        #define COMPRESS_BZIP2_MT 1
    #endif
#endif

/** 64位操作系统标识
*/
#ifdef _WIN64
    #define RCZIP_BIT_64 1
#endif

#endif //__RCConfig_h_
