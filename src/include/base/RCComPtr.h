/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCComPtr_h_
#define __RCComPtr_h_ 1

#include "base/RCDefs.h"

BEGIN_NAMESPACE_RCZIP

/** 仿COM智能指针宏定义
*/
template <class T>
class RCComPtr
{
public:
    
    /** 默认构造函数
    */
    RCComPtr() ;
    
    /** 指针数据构造，构造的同时对接口指针引用计数增加
    @param [in] p 接口指针
    */
    RCComPtr(T* p) ;
    
    /** 拷贝构造函数
    @param [in] rhs 被复制的对象
    */
    RCComPtr(const RCComPtr<T>& rhs) ;
    
    /** 默认析构函数
    */
    ~RCComPtr() ;
    
    /** 释放指针,并递减引用计数
    */
    void Release() ;
    
    /** 取得指针的地址
    @return 返回存贮的接口指针的地址
    */
    T** GetAddress() ;
    
    /** 操作符 ->
    @return 返回接口指针
    */
    T* operator->() const ;
    
    /** 操作符 *
    @return 返回接口指针的解引用
    */
    T& operator *() const ;
    
    /** 指针赋值操作符，同时增加接口的引用计数
    @param [in] p 接口指针
    @return 返回智能指针对象的引用
    */
    RCComPtr<T>& operator = (T* p) ;
    
    /** 指针赋值操作符
    @param [in] rhs 被复制的对象
    @return 返回智能指针对象的引用
    */
    RCComPtr<T>& operator = (const RCComPtr<T>& rhs) ;
    
    /** 操作符 !
    @return 如果指针不为空则返回false, 否则返回true
    */
    bool operator!() const ;
    
    /** 操作符 bool
    @return 如果指针不为空则返回true, 否则返回false
    */
    operator bool () const ;

    /** Attach，递减当前的指针引用计数，并赋新值为p,不增加p的引用计数
    @param [in] p 接口指针
    */
    void Attach(T* p) ;
    
    /** Detach，释放当前接口指针，不改变引用计数
    @return 返回当前的接口指针
    */
    T* Detach() ;
    
    /** 接口查询
    @param [in] iid 接口ID
    @param [out] pp 返回被查询的接口指针，如果接口ID不被实现，不改变值
    @return 成功返回RC_S_OK，失败则返回错误号
    */  
    template <class Q>
    HResult QueryInterface(RC_IID iid, Q** pp) const
    {
        return m_ptr->QueryInterface(iid, (void**)pp) ;
    }
    
    /* 获取指针
    @return 返回当前的接口指针
    */
    T* Get() const ;
    
private:
    
    /** 指针地址
    */
    T* m_ptr ;
};

END_NAMESPACE_RCZIP

#include "RCComPtr.inl"

#endif //__RCComPtr_h_
