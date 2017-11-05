/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCZipCompressionMethodMode_h_
#define __RCZipCompressionMethodMode_h_ 1

#include "base/RCDefs.h"
#include "base/RCString.h"
#include "common/RCVector.h"

BEGIN_NAMESPACE_RCZIP

struct RCZipCompressionMethodMode
{
    /** 压缩模式的顺序
    */
    RCByteVector m_methodSequence ;

    /** 匹配搜索
    */
    RCStringW m_matchFinder;

    /** algo
    */
    uint32_t m_algo ;

    /** passes
    */
    uint32_t m_numPasses ;

    /** fastbytes
    */
    uint32_t m_numFastBytes ;

    /** 是否匹配搜索周期
    */
    bool m_numMatchFinderCyclesDefined ;

    /** 匹配搜索周期
    */
    uint32_t m_numMatchFinderCycles ;

    /** 字典大小
    */
    uint32_t m_dicSize ;

#ifdef COMPRESS_MT
    /** 线程数
    */
    uint32_t m_numThreads ;
#endif

    /** 是否有密码
    */
    bool m_passwordIsDefined ;

    /** 密码
    */
    RCStringA m_password ;

    /** 是否是AES模式
    */
    bool m_isAesMode ;

    /** AES模式密匙
    */
    byte_t m_aesKeyMode ;

    /** 默认构造函数
    */ 
    RCZipCompressionMethodMode():
        m_numMatchFinderCyclesDefined(false),
        m_passwordIsDefined(false),
        m_isAesMode(false),
        m_aesKeyMode(3)
    {}
} ;

END_NAMESPACE_RCZIP

#endif //__RCZipCompressionMethodMode_h_
