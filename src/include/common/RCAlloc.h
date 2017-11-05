/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCMemoryAlloc_h_
#define __RCMemoryAlloc_h_ 1

#include "base/RCNonCopyable.h"
#include "base/RCSingleton.h"

BEGIN_NAMESPACE_RCZIP

class RCMemoryAlloc:
    private RCNonCopyable
{
protected:

    /** 默认构造函数
    */
    RCMemoryAlloc() ;
    
    /** 默认析构函数
    */
    ~RCMemoryAlloc() ;
    
public:
    
    /** 分配内存
    @param [in] size 需要分配的内存空间大小，字节数
    @return 返回已分配的内存地址，如果失败返回NULL
    */
    void* MyAlloc(size_t size) ;
    
    /** 释放由MyAlloc分配的内存
    @param [in] address 需要释放的内存地址
    */
    void MyFree(void* address) ;
    
    /** 设置LargePageSize模式, 只有Windows支持该属性的版本有效
    */
    void SetLargePageSize() ;

    /** 分配内存
    @param [in] size 需要分配的内存空间大小，字节数
    @return 返回已分配的内存地址，如果失败返回NULL
    */
    void* MidAlloc(size_t size);
    
    /** 释放由MidAlloc分配的内存
    @param [in] address 需要释放的内存地址
    */
    void MidFree(void* address);
    
    /** 分配内存
    @param [in] size 需要分配的内存空间大小，字节数
    @return 返回已分配的内存地址，如果失败返回NULL
    */
    void* BigAlloc(size_t size);
    
    /** 释放内存
    @param [in] address 需要释放的内存地址
    */
    void BigFree(void* address);
    
private:
    
    /** LargePage 的大小，单位为字节
    */
    uint64_t m_largePageSize ;
};

/** 单件实例
*/
typedef RCSingleton<RCMemoryAlloc> RCAlloc ;

END_NAMESPACE_RCZIP

#endif //__RCMemoryAlloc_h_
