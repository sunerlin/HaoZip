/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCChmDefs_h_
#define __RCChmDefs_h_ 1

#include "archive/common/RCGuidDefs.h"

BEGIN_NAMESPACE_RCZIP

namespace RCChmDefs
{
    static const RC_GUID s_chmLzxGuid =
    { 0x7FC28940, 0x9D31, 0x11D0, 0x9B, 0x27, 0x00, 0xA0, 0xC9, 0x1E, 0x9C, 0x7C };

    static const RC_GUID s_help2LzxGuid =
    { 0x0A9007C6, 0x4076, 0x11D3, 0x87, 0x89, 0x00, 0x00, 0xF8, 0x10, 0x57, 0x54 };

    static const RC_GUID s_desGuid =
    { 0x67F6E4A2, 0x60BF, 0x11D3, 0x85, 0x40, 0x00, 0xC0, 0x4F, 0x58, 0xC3, 0xCF };
};

END_NAMESPACE_RCZIP

#endif //__RCChmDefs_h_
