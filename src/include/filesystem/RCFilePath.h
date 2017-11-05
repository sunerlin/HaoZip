/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCFilePath_h_
#define __RCFilePath_h_ 1

#include "base/RCString.h"

BEGIN_NAMESPACE_RCZIP

/** Unicode版本路径操作类 
*/
typedef RCStringW RCFilePathW ;

/** Ansi版本路径操作类 
*/
typedef RCStringA  RCFilePathA ;

/** 默认路径操作类 
*/
#if defined (RCZIP_UNICODE)
    typedef RCFilePathW  RCFilePath ;
#else
    typedef RCFilePathA  RCFilePath ;
#endif

END_NAMESPACE_RCZIP

#endif //__RCFilePath_h_
