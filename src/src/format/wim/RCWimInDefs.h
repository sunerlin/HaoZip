/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCWimInDefs_h_
#define __RCWimInDefs_h_ 1

#include "base/RCDefs.h"

BEGIN_NAMESPACE_RCZIP

static const int32_t  s_kWimChunkSizeBits = 15;
static const uint32_t s_kWimChunkSize = (1 << s_kWimChunkSizeBits);

END_NAMESPACE_RCZIP

#endif //__RCWimInDefs_h_
