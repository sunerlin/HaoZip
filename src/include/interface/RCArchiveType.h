/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCArchiveType_h_
#define __RCArchiveType_h_ 1

#include "base/RCDefs.h"

BEGIN_NAMESPACE_RCZIP

/** 文档类型ID枚举值
*/
enum RCArchiveType
{
    RC_ARCHIVE_UNKNOWN  = -1 ,
    RC_ARCHIVE_ZIP      =  1 ,
    RC_ARCHIVE_7Z       =  2 ,
    RC_ARCHIVE_TAR      =  3 ,
    RC_ARCHIVE_RAR      =  4 ,
    RC_ARCHIVE_CAB      =  5 ,
    RC_ARCHIVE_ISO      =  6 ,
    RC_ARCHIVE_BZIP2    =  7 ,
    RC_ARCHIVE_UDF      =  8 ,
    RC_ARCHIVE_ARJ      =  9 ,
    RC_ARCHIVE_Z        = 10 ,
    RC_ARCHIVE_GZIP     = 11 ,
    RC_ARCHIVE_LZH      = 12 ,
    RC_ARCHIVE_RPM      = 13 ,
    RC_ARCHIVE_LZMA     = 14 ,
    RC_ARCHIVE_NSIS     = 15 ,
    RC_ARCHIVE_CHM      = 16 ,
    RC_ARCHIVE_DMG      = 17 ,
    RC_ARCHIVE_HFS      = 18 ,
    RC_ARCHIVE_WIM      = 19 ,
    RC_ARCHIVE_DEB      = 20 ,
    RC_ARCHIVE_COM      = 21 ,
    RC_ARCHIVE_CPIO     = 22 ,
    RC_ARCHIVE_PE       = 23 ,
    RC_ARCHIVE_XAR      = 24 ,
    RC_ARCHIVE_SPLIT    = 25 ,
    RC_ARCHIVE_UUE      = 26 ,
    RC_ARCHIVE_ACE      = 27 ,
    RC_ARCHIVE_ZIP_SPLIT= 28 ,
    RC_ARCHIVE_LZMA86   = 29 ,
    RC_ARCHIVE_ISZ      = 30
};

/** 文档类型ID
*/
typedef uint32_t RCArchiveID ;

/** 文档类型，字符串形式
*/
#define RC_ARCHIVE_TYPE_ZIP             _T("zip")       
#define RC_ARCHIVE_TYPE_7Z              _T("7z")       
#define RC_ARCHIVE_TYPE_TAR             _T("tar")       
#define RC_ARCHIVE_TYPE_RAR             _T("rar")       
#define RC_ARCHIVE_TYPE_CAB             _T("cab")       
#define RC_ARCHIVE_TYPE_ISO             _T("iso")       
#define RC_ARCHIVE_TYPE_UDF             _T("udf")       
#define RC_ARCHIVE_TYPE_BZIP2           _T("bzip2")       
#define RC_ARCHIVE_TYPE_ARJ             _T("arj")       
#define RC_ARCHIVE_TYPE_Z               _T("z")       
#define RC_ARCHIVE_TYPE_GZIP            _T("gzip")       
#define RC_ARCHIVE_TYPE_LZH             _T("lzh")       
#define RC_ARCHIVE_TYPE_RPM             _T("rpm")       
#define RC_ARCHIVE_TYPE_LZMA            _T("lzma")      
#define RC_ARCHIVE_TYPE_LZMA86          _T("lzma86")   
#define RC_ARCHIVE_TYPE_NSIS            _T("nsis")       
#define RC_ARCHIVE_TYPE_CHM             _T("chm")       
#define RC_ARCHIVE_TYPE_DMG             _T("dmg")       
#define RC_ARCHIVE_TYPE_HFS             _T("hfs")       
#define RC_ARCHIVE_TYPE_WIM             _T("wim")       
#define RC_ARCHIVE_TYPE_DEB             _T("deb")       
#define RC_ARCHIVE_TYPE_COM             _T("com")       
#define RC_ARCHIVE_TYPE_CPIO            _T("cpio")       
#define RC_ARCHIVE_TYPE_PE              _T("PE")       
#define RC_ARCHIVE_TYPE_XAR             _T("xar")       
#define RC_ARCHIVE_TYPE_SPLIT           _T("split") 
#define RC_ARCHIVE_TYPE_UUE             _T("uue")   
#define RC_ARCHIVE_TYPE_ACE             _T("ace")
#define RC_ARCHIVE_TYPE_ZIP_SPLIT       _T("zip_split")
#define RC_ARCHIVE_TYPE_ISZ             _T("isz")

/** 压缩格式枚举值
*/
enum RCCompressFormatEnum
{
    RC_COMPRESS_FORMAT_ZIP = 0 ,
    RC_COMPRESS_FORMAT_7Z  = 1 ,
    RC_COMPRESS_FORMAT_TAR = 2 
};

END_NAMESPACE_RCZIP

#endif //__RCArchiveType_h_
