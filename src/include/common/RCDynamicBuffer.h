/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCDynamicBuffer_h_
#define __RCDynamicBuffer_h_ 1

#include "common/RCBuffer.h"

BEGIN_NAMESPACE_RCZIP

template <typename T> 
class RCDynamicBuffer:
    public RCBuffer<T>
{
public:

    /** 默认构造函数
    */
    RCDynamicBuffer() ;
    
    /** 默认析构函数
    */
    virtual ~RCDynamicBuffer() ;
    
    /** 指定大小的构造函数
    @param [in] size 缓冲区大小
    */
    explicit RCDynamicBuffer(size_t size) ;
    
    /** 拷贝构造函数
    @param [in] from 复制对象
    */
    RCDynamicBuffer(const RCDynamicBuffer& from) ;
    
    /** 赋值操作符
    @param [in] from 复制对象
    */
    RCDynamicBuffer& operator= (const RCDynamicBuffer& from) ;
    
public:
    
    /** 确保缓冲区大小不小于capacity
    @param [in] capacity 缓冲区大小
    */
    void EnsureCapacity(size_t capacity) ;
    
    /** 增加缓冲区大小
    @param [in] size 增加的大小
    */
    void GrowLength(size_t size) ;
};

/** 字节类型的缓冲区
*/
typedef RCDynamicBuffer<byte_t> RCDynamicByteBuffer ;

/** 字符类型的缓冲区
*/
typedef RCDynamicBuffer<char> RCDynamicCharBuffer ;

END_NAMESPACE_RCZIP

#include "RCDynamicBuffer.inl"

#endif //__RCDynamicBuffer_h_
