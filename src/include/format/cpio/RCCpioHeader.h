/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCCpioHeader_h_
#define __RCCpioHeader_h_ 1

#include "base/RCDefs.h"

BEGIN_NAMESPACE_RCZIP

namespace RCCpioHeader
{
    namespace NMagic
    {
        extern const char* kMagic1 ;
        extern const char* kMagic2 ;
        extern const char* kMagic3 ;
        extern const char* kEndName ;
        extern const byte_t kMagicForRecord2[2] ;
    }

    const uint32_t kRecord2Size     = 26 ;
    const uint32_t kRecordSize      = 110 ;
    const uint32_t kOctRecordSize   = 76 ;
}

END_NAMESPACE_RCZIP

#endif //__RCCpioHeader_h_
