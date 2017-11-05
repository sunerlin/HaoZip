/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCGZipDefs_h_
#define __RCGZipDefs_h_ 1

#include "base/RCDefs.h"

BEGIN_NAMESPACE_RCZIP

class RCGZipDefs
{
public:

    /** algo 1
    */
    static const uint32_t s_algo1 = 0;

    /** algo 5
    */
    static const uint32_t s_algo5 = 1;

    /** passes 1
    */
    static const uint32_t s_passes1  = 1;

    /** passes 7
    */
    static const uint32_t s_passes7  = 3;

    /** passes 9
    */
    static const uint32_t s_passes9  = 10;

    /** fastbytes 1
    */
    static const uint32_t s_fastBytes1 = 32;

    /** fastbytes 7
    */
    static const uint32_t s_fastBytes7 = 64;

    /** fastbytes 9
    */
    static const uint32_t s_fastBytes9 = 128;
};

END_NAMESPACE_RCZIP

#endif //__RCGZipDefs_h_
