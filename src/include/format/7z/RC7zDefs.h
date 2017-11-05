/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RC7zDefs_h_
#define __RC7zDefs_h_ 1

#include "base/RCDefs.h"

BEGIN_NAMESPACE_RCZIP

typedef uint32_t RC7zNum ;

class RC7zDefs
{
public:

    /** 最大个数
    */
    static const RC7zNum  s_7zNumMax     = 0x7FFFFFFF;

    /** 最大索引
    */
    static const RC7zNum  s_7zNumNoIndex = 0xFFFFFFFF;

    /** 头大小
    */
    static const uint32_t s_headerSize   = 32 ;
};

END_NAMESPACE_RCZIP

#endif //__RC7zDefs_h_
