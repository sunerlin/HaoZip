/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCZipExtFilter_h_
#define __RCZipExtFilter_h_ 1

#include "archive/update/RCExtFilter.h"

BEGIN_NAMESPACE_RCZIP

class RCZipExtFilter:
    public RCExtFilter
{
public:

    struct
    {
        /** passes
        */
        uint32_t m_numPasses;

        /** fastbytes
        */
        uint32_t m_numFastBytes;

        /** algo
        */
        uint32_t m_algo;

    }m_deflate;

    struct
    {
        /** 字典大小
        */
        uint32_t m_dicSize;

        /** fastbytes
        */
        uint32_t m_numFastBytes;

        /** algo
        */
        uint32_t m_algo;

        /** 匹配搜索
        */
        RCStringW m_matchFinder;

    }m_lzma;

    struct
    {
        /** passes
        */
        uint32_t m_numPasses;

        /** 字典大小
        */
        uint32_t m_dicSize;

    }m_bzip2;
};

/** RCZipExtFilter智能指针
*/
typedef RCSharedPtr<RCZipExtFilter> RCZipExtFilterPtr ;

END_NAMESPACE_RCZIP

#endif //__RCZipExtFilter_h_
