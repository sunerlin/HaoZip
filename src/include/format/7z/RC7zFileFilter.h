/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RC7zFileFilter_h_
#define __RC7zFileFilter_h_ 1

#include "archive/update/RCFileFilter.h"

BEGIN_NAMESPACE_RCZIP

class RC7zFileFilter:
    public RCFileFilter
{
public:

    struct
    {
        /** 字典大小
        */
        uint32_t m_dicSize;

        /** algo
        */
        uint32_t m_algo;

        /** fastbytes
        */
        uint32_t m_fastBytes;

        /** 匹配字符
        */
        const RCString::value_type* m_matchFinder;

    }m_lzma;

    struct
    {
        /** fastbytes
        */
        uint32_t m_fastBytes;

        /** passes
        */
        uint32_t m_numPasses;

        /** algo
        */
        uint32_t m_algo;

    }m_deflate;

    struct
    {
        /** passes
        */
        uint32_t m_numPasses;

        /** 字典大小
        */
        uint32_t m_dicSize;

    }m_bzip2;

    struct
    {
        /** 内存大小
        */
        uint32_t m_useMemSize;

        /** 顺序
        */
        uint32_t m_order;

    }m_ppmd;
};

/** RC7zFileFilter智能指针
*/
typedef RCSharedPtr<RC7zFileFilter> RC7zFileFilterPtr ;

END_NAMESPACE_RCZIP

#endif //__RC7zFileFilter_h_
