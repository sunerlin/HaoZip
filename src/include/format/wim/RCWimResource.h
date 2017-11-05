/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCWimResource_h_
#define __RCWimResource_h_ 1

#include "base/RCDefs.h"

BEGIN_NAMESPACE_RCZIP

namespace NWimResourceFlags
{
    const byte_t s_compressed = 4;
    const byte_t s_metadata = 2;
}

class RCWimResource
{
public:

    /** 默认构造函数
    */
    RCWimResource() ;
    
    /** 默认析构函数
    */
    ~RCWimResource() ;
    
public:

    /** 解析
    @param [in] p 数据
    */
    void Parse(const byte_t* p) ; 

    /** 是否压缩
    @return 压缩返回true,否则返回false
    */
    bool IsCompressed() const ;

    /** 是否meta数据
    @return meta数据返回true,否则返回false
    */
    bool IsMetadata() const ;

    /** 是否空
    @return 空返回true,否则返回false
    */
    bool IsEmpty() const ;
    
public:

    /** 压缩大小
    */
    uint64_t m_packSize;

    /** 偏移
    */
    uint64_t m_offset;

    /** 解压后大小
    */
    uint64_t m_unpackSize;

    /** 标志
    */
    byte_t   m_flags;
};

END_NAMESPACE_RCZIP

#endif //__RCWimResource_h_
