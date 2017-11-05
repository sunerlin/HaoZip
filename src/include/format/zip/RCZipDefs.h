/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCZipDefs_h_
#define __RCZipDefs_h_ 1

#include "base/RCDefs.h"

BEGIN_NAMESPACE_RCZIP

namespace RCZipDefs
{
    /** lz algo x1
    */
    static const uint32_t s_lzAlgoX1 = 0;

    /** lz algo x5
    */
    static const uint32_t s_lzAlgoX5 = 1;

    /** deflate passes x1
    */
    static const uint32_t s_deflateNumPassesX1  = 1;

    /** deflate passes x7
    */
    static const uint32_t s_deflateNumPassesX7  = 3;

    /** deflate passes x9
    */
    static const uint32_t s_deflateNumPassesX9  = 10;

    /** deflate fastbytes x1
    */
    static const uint32_t s_deflateNumFastBytesX1 = 32;

    /** deflate fastbytes x7
    */
    static const uint32_t s_deflateNumFastBytesX7 = 64;

    /** deflate fastbytes x9
    */
    static const uint32_t s_deflateNumFastBytesX9 = 128;

    /** 匹配搜索 x1
    */
    static const char_t* s_lzmaMatchFinderX1 = _T("HC4");

    /** 匹配搜索 x5
    */
    static const char_t* s_lzmaMatchFinderX5 = _T("BT4");

    /** lzma fastbytes x1
    */
    static const uint32_t s_lzmaNumFastBytesX1 = 32;

    /** deflate fastbytes x7
    */
    static const uint32_t s_lzmaNumFastBytesX7 = 64;

    /** lzma 字典大小 x1
    */
    static const uint32_t s_lzmaDicSizeX1 = 1 << 16;

    /** lzma 字典大小 x3
    */
    static const uint32_t s_lzmaDicSizeX3 = 1 << 20;

    /** lzma 字典大小 x5
    */
    static const uint32_t s_lzmaDicSizeX5 = 1 << 24;

    /** lzma 字典大小 x7
    */
    static const uint32_t s_lzmaDicSizeX7 = 1 << 25;

    /** lzma 字典大小 x9
    */
    static const uint32_t s_lzmaDicSizeX9 = 1 << 26;

    /** bzip2 passes x1
    */
    static const uint32_t s_bzip2NumPassesX1 = 1;

    /** bzip2 passes x7
    */
    static const uint32_t s_bzip2NumPassesX7 = 2;

    /** bzip2 passes x9
    */
    static const uint32_t s_bzip2NumPassesX9 = 7;

    /** bzip2 字典大小 x1
    */
    static const uint32_t s_bzip2DicSizeX1 = 100000;

    /** bzip2 字典大小 x3
    */
    static const uint32_t s_bzip2DicSizeX3 = 500000;

    /** bzip2 字典大小 x5
    */
    static const uint32_t s_bzip2DicSizeX5 = 900000;

    /** 最多线程数
    */
    static const uint32_t s_maxThreadNums = 10;
};

END_NAMESPACE_RCZIP

#endif //__RCZipDefs_h_
