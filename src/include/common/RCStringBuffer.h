/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCStringBuffer_h_
#define __RCStringBuffer_h_ 1

#include "base/RCNonCopyable.h"
#include "base/RCString.h"
#include "common/RCStringUtil.h"

BEGIN_NAMESPACE_RCZIP

template <class string_type>
class RCStringBufferBase:
    private RCNonCopyable
{
public:

    /** 默认构造函数
    @param [in] str 字符串指针
    */
    explicit RCStringBufferBase(string_type* str) ;
    
    /** 默认析构函数
    */
    ~RCStringBufferBase() ;
    
    /** 获取缓冲区
    @param [in] minBufLength 缓冲区长度
    @return 返回缓冲区地址, 如果minBufLength为0， 返回NULL
    */
    typename string_type::value_type* GetBuffer(uint32_t minBufLength) ;
    
    /** 释放缓冲区, 以尾零方式计算字符串长度
    */
    void ReleaseBuffer() ;
    
    /** 释放缓冲区,并把缓冲区数据赋值给字符串
    @param [in] newLength 字符串数据长度
    */
    void ReleaseBuffer(uint32_t newLength) ;

private:
    
    /** 字符串地址
    */
    string_type* m_str ;
    
    /** 缓冲区地址
    */
    typename string_type::value_type* m_buffer ;
        
    /** 缓冲区长度
    */
    uint32_t m_length ;
};

typedef RCStringBufferBase<RCStringA> RCStringBufferA ;
typedef RCStringBufferBase<RCStringW> RCStringBufferW ;

#ifdef RCZIP_UNICODE
    typedef RCStringBufferW RCStringBuffer ;
#else
    typedef RCStringBufferA RCStringBuffer ;
#endif

END_NAMESPACE_RCZIP

/** 模板实现文件
*/
#include "RCStringBuffer.inl"

#endif //__RCStringBuffer_h_
