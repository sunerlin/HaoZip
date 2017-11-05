/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCString_h_
#define __RCString_h_ 1

#include "base/RCDefs.h"
#include <string>

BEGIN_NAMESPACE_RCZIP

/** Unicode版本的String
*/
typedef std::wstring RCStringW ; 
    
/** Ansi版本的String
*/
typedef std::string  RCStringA ;

/** String 类型宏定义
*/
#if defined (RCZIP_UNICODE)
    typedef RCStringW  RCString ;
#else
    typedef RCStringA  RCString ;
#endif

END_NAMESPACE_RCZIP

#endif //__RCString_h_
