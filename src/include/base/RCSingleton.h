/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCSingleton_h_
#define __RCSingleton_h_ 1

#include "base/RCDefs.h"

BEGIN_NAMESPACE_RCZIP

/** Singleton 模板
    用法：
        class A ;
        typedef RCSingleton<T> B ;
*/
template <class T>
class RCSingleton:
    private T
{
public:
    
    /** 获取Singleton对象
    */
    static T& Instance() ;

#ifdef _DEBUG
private:
    
    /** 默认构造函数
    */
    RCSingleton() {} ;
    
    /** 默认析构函数
    */
    ~RCSingleton() {} ;

    /** 拷贝构造函数
    */
    RCSingleton(const RCSingleton& rhs) ;
    
    /** 赋值操作符
    */
    RCSingleton& operator= (const RCSingleton& rhs) ;
#endif

};

/** 单例对象实现，非线程安全，需要调用者控制线程安全性
@return 返回单例对象的引用
*/
template <class T>
T& RCSingleton<T>::Instance()
{
    static RCSingleton<T> sObject ;
    return (sObject);
}

END_NAMESPACE_RCZIP

#endif //__RCSingleton_h_
