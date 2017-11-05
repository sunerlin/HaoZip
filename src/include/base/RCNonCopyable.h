/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCNonCopyable_h_
#define __RCNonCopyable_h_ 1

#include "base/RCDefs.h"

BEGIN_NAMESPACE_RCZIP

#ifdef _DEBUG

/** 禁止对象复制的公用基类
*/
class RCNonCopyable
{    
protected:
    
    /** 默认构造函数
    */
    RCNonCopyable() {} ;
    
    /** 默认析构函数
    */
    ~RCNonCopyable() {} ;

private:

    /** 拷贝构造函数
    */
    RCNonCopyable(const RCNonCopyable& from) ;
    
    /** 赋值操作符
    */
    const RCNonCopyable& operator= (const RCNonCopyable& from) ;
};

#else

/** Release版本的空基类优化
*/
class RCNonCopyable
{

};

#endif

END_NAMESPACE_RCZIP

#endif //__RCNonCopyable_h_
