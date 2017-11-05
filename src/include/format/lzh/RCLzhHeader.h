/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCLzhHeader_h_
#define __RCLzhHeader_h_ 1

#include "base/RCDefs.h"

BEGIN_NAMESPACE_RCZIP

#ifdef RCZIP_OS_WIN
#define CHAR_PATH_SEPARATOR '\\'
#define WCHAR_PATH_SEPARATOR L'\\'
#define WSTRING_PATH_SEPARATOR L"\\"
#else
#define CHAR_PATH_SEPARATOR '/'
#define WCHAR_PATH_SEPARATOR L'/'
#define WSTRING_PATH_SEPARATOR L"/"
#endif

class RCLzhHeader
{
public:

    /** 方法id大小
    */
    static const int s_methodIdSize = 5;

    /** 扩展id文件名
    */
    static const byte_t s_extIdFileName = 0x01;

    /** 扩展id目录名
    */
    static const byte_t s_extIdDirName  = 0x02;

    /** 扩展id unix时间
    */
    static const byte_t s_extIdUnixTime = 0x54;
};


END_NAMESPACE_RCZIP

#endif //__RCLzhHeader_h_
