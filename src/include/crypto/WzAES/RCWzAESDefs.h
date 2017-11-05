/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCWzAESDefs_h_
#define __RCWzAESDefs_h_ 1

#include "base/RCDefs.h"

BEGIN_NAMESPACE_RCZIP

/** WzAES 常量定义
*/
class RCWzAESDefs
{
public:
    static const uint32_t s_kSaltSizeMax = 16;
    static const uint32_t s_kMacSize = 10;
    static const uint32_t s_kPasswordSizeMax = 99 ; // 128;

    /** Password Verification Code Size
    */
    static const uint32_t s_kPwdVerifCodeSize = 2 ;
    static const uint32_t s_kAesKeySizeMax = 32;
    static const uint32_t s_kNumKeyGenIterations = 1000;
};

/** 优化选项定义
*/
//#define _NO_WZAES_OPTIMIZATIONS 1

END_NAMESPACE_RCZIP

#endif //__RCWzAESDefs_h_
