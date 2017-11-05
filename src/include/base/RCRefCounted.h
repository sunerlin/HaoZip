/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCRefCounted_h_
#define __RCRefCounted_h_ 1

#include "base/RCNonCopyable.h"

BEGIN_NAMESPACE_RCZIP

/** 引用计数基类
*/
class RCRefCounted:
    private RCNonCopyable
{
public:

    /** 增加引用计数
    */
    void Increase(void) const ;
    
    /** 减小引用计数,如果引用计数减小到0，则调用Destroy
    */
    void Decrease(void) const ;
    
protected:
    
    /** 默认构造函数
    */
    RCRefCounted() ;
    
    /** 默认析构函数
    */
    virtual ~RCRefCounted() ;
    
    /** 销毁对象
    */
    virtual void Destroy() ;
    
private:
    
    /** 引用计数
    */
    mutable long m_lRefCount ;
};

END_NAMESPACE_RCZIP

#endif //__RCRefCounted_h_
