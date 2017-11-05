/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCGZipHeader_h_
#define __RCGZipHeader_h_ 1

#include "base/RCDefs.h"

BEGIN_NAMESPACE_RCZIP

namespace RCGZipSignature
{
    static const uint16_t kSignature = 0x8B1F ;
}

namespace RCGZipHeader
{
    /** 标志
    */
    namespace NFlags
    {
        const byte_t kIsText  = 1 << 0;
        const byte_t kCrc     = 1 << 1;
        const byte_t kExtra   = 1 << 2;
        const byte_t kName    = 1 << 3;
        const byte_t kComment = 1 << 4;
    }

    /** 解压方法
    */
    namespace NExtraFlags
    {
        const byte_t kMaximum = 2;
        const byte_t kFastest = 4;
    }

    /** 压缩方法
    */
    namespace NCompressionMethod
    {
        const byte_t kDeflate = 8;
    }

    /** 平台系统
    */
    namespace NHostOS
    {
        enum EEnum
        {
            kFAT = 0,
            kAMIGA,
            kVMS,
            kUnix,
            kVM_CMS,
            kAtari,
            kHPFS,
            kMac,
            kZ_System,
            kCPM,
            kTOPS20,
            kNTFS,
            kQDOS,
            kAcorn,
            kVFAT,
            kMVS,
            kBeOS,
            kTandem,
            
            kUnknown = 255
        };
    }
}

END_NAMESPACE_RCZIP

#endif //__RCGZipHeader_h_
