/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __IUnknownImpl_h_
#define __IUnknownImpl_h_ 1

#include "interface/RCIIDDefs.h"
#include "base/RCRefCounted.h"
#include "base/RCComPtr.h"

BEGIN_NAMESPACE_RCZIP

/** IUnknown接口实现类
*/
template <
            class T1
         >
class IUnknownImpl:
    public RCRefCounted,
    public T1
{
public:
    
    /** 根据ID查询接口
    @param [in] iid 被查询的接口ID
    @param [out] outObject 如果查询成功，则保存结果接口指针，并增加引用计数
    @return 如果成功则返回RC_S_OK，否则返回错误号
    */
    virtual HResult QueryInterface(RC_IID iid, void** outObject)
    {
        if(outObject == NULL)
        {
            return RC_E_INVALIDARG ;
        }
        switch(iid){
            case IUnknown::IID:
                {
                    *outObject = (IUnknown*)(T1*)this ;
                }
                break ;
            case T1::IID:
                {
                    *outObject = (T1*)this ;                    
                }
                break ;
            default:
                return RC_E_NOINTERFACE ;
                break ;                
        }
        this->AddRef() ;
        return RC_S_OK ;
    }
    
    /** 增加引用计数
    */
    virtual void AddRef(void)
    {
        RCRefCounted::Increase() ;
    }
    
    /** 减少引用计数
    */
    virtual void Release(void)
    {
        RCRefCounted::Decrease() ;
    }
    
protected:
      
    /** 默认析构函数
    */
    ~IUnknownImpl() {} ;
};

/** IUnknown接口实现类
*/
template <
            class T1,
            class T2
         >
class IUnknownImpl2:
    public RCRefCounted,
    public T1,
    public T2
{
public:
    
    /** 根据ID查询接口
    @param [in] iid 被查询的接口ID
    @param [out] outObject 如果查询成功，则保存结果接口指针，并增加引用计数
    @return 如果成功则返回RC_S_OK，否则返回错误号
    */
    virtual HResult QueryInterface(RC_IID iid, void** outObject)
    {
        if(outObject == NULL)
        {
            return RC_E_INVALIDARG ;
        }
        switch(iid){
            case IUnknown::IID:
                {
                    *outObject = (IUnknown*)(T1*)this ;
                }
                break ;
            case T1::IID:
                {
                    *outObject = (T1*)this ;
                }
                break ;
            case T2::IID:
                {
                    *outObject = (T2*)this ;
                }
                break ;
            default:
                return RC_E_NOINTERFACE ;
                break ;                
        }
        this->AddRef() ;
        return RC_S_OK ;
    }
    
    /** 增加引用计数
    */
    virtual void AddRef(void)
    {
        RCRefCounted::Increase() ;
    }
    
    /** 减少引用计数
    */
    virtual void Release(void)
    {
        RCRefCounted::Decrease() ;
    }
    
protected:
      
    /** 默认析构函数
    */
    ~IUnknownImpl2() {} ;
};

/** IUnknown接口实现类
*/
template <
            class T1,
            class T2,
            class T3
         >
class IUnknownImpl3:
    public RCRefCounted,
    public T1,
    public T2,
    public T3
{
public:
    
    /** 根据ID查询接口
    @param [in] iid 被查询的接口ID
    @param [out] outObject 如果查询成功，则保存结果接口指针，并增加引用计数
    @return 如果成功则返回RC_S_OK，否则返回错误号
    */
    virtual HResult QueryInterface(RC_IID iid, void** outObject)
    {
        if(outObject == NULL)
        {
            return RC_E_INVALIDARG ;
        }
        switch(iid){
            case IUnknown::IID:
                {
                    *outObject = (IUnknown*)(T1*)this ;
                }
                break ;
            case T1::IID:
                {
                    *outObject = (T1*)this ;
                }
                break ;
            case T2::IID:
                {
                    *outObject = (T2*)this ;
                }
                break ;
            case T3::IID:
                {
                    *outObject = (T3*)this ;
                }
                break ;
            default:
                return RC_E_NOINTERFACE ;
                break ;                
        }
        this->AddRef() ;
        return RC_S_OK ;
    }
    
    /** 增加引用计数
    */
    virtual void AddRef(void)
    {
        RCRefCounted::Increase() ;
    }
    
    /** 减少引用计数
    */
    virtual void Release(void)
    {
        RCRefCounted::Decrease() ;
    }
    
protected:
      
    /** 默认析构函数
    */
    ~IUnknownImpl3() {} ;
};

/** IUnknown接口实现类
*/
template <
            class T1,
            class T2,
            class T3,
            class T4
         >
class IUnknownImpl4:
    public RCRefCounted,
    public T1,
    public T2,
    public T3,
    public T4
{
public:
    
    /** 根据ID查询接口
    @param [in] iid 被查询的接口ID
    @param [out] outObject 如果查询成功，则保存结果接口指针，并增加引用计数
    @return 如果成功则返回RC_S_OK，否则返回错误号
    */
    virtual HResult QueryInterface(RC_IID iid, void** outObject)
    {
        if(outObject == NULL)
        {
            return RC_E_INVALIDARG ;
        }
        switch(iid){
            case IUnknown::IID:
                {
                    *outObject = (IUnknown*)(T1*)this ;
                }
                break ;
            case T1::IID:
                {
                    *outObject = (T1*)this ;
                }
                break ;
            case T2::IID:
                {
                    *outObject = (T2*)this ;
                }
                break ;
            case T3::IID:
                {
                    *outObject = (T3*)this ;
                }
                break ;
            case T4::IID:
                {
                    *outObject = (T4*)this ;
                }
                break ;
            default:
                return RC_E_NOINTERFACE ;
                break ;                
        }
        this->AddRef() ;
        return RC_S_OK ;
    }
    
    /** 增加引用计数
    */
    virtual void AddRef(void)
    {
        RCRefCounted::Increase() ;
    }
    
    /** 减少引用计数
    */
    virtual void Release(void)
    {
        RCRefCounted::Decrease() ;
    }
    
protected:
      
    /** 默认析构函数
    */
    ~IUnknownImpl4() {} ;
};

/** IUnknown接口实现类
*/
template <
            class T1,
            class T2,
            class T3,
            class T4,
            class T5
         >
class IUnknownImpl5:
    public RCRefCounted,
    public T1,
    public T2,
    public T3,
    public T4,
    public T5
{
public:
    
    /** 根据ID查询接口
    @param [in] iid 被查询的接口ID
    @param [out] outObject 如果查询成功，则保存结果接口指针，并增加引用计数
    @return 如果成功则返回RC_S_OK，否则返回错误号
    */
    virtual HResult QueryInterface(RC_IID iid, void** outObject)
    {
        if(outObject == NULL)
        {
            return RC_E_INVALIDARG ;
        }
        switch(iid){
            case IUnknown::IID:
                {
                    *outObject = (IUnknown*)(T1*)this ;
                }
                break ;
            case T1::IID:
                {
                    *outObject = (T1*)this ;
                }
                break ;
            case T2::IID:
                {
                    *outObject = (T2*)this ;
                }
                break ;
            case T3::IID:
                {
                    *outObject = (T3*)this ;
                }
                break ;
            case T4::IID:
                {
                    *outObject = (T4*)this ;
                }
                break ;
            case T5::IID:
                {
                    *outObject = (T5*)this ;
                }
                break ;
            default:
                return RC_E_NOINTERFACE ;
                break ;                
        }
        this->AddRef() ;
        return RC_S_OK ;
    }
    
    /** 增加引用计数
    */
    virtual void AddRef(void)
    {
        RCRefCounted::Increase() ;
    }
    
    /** 减少引用计数
    */
    virtual void Release(void)
    {
        RCRefCounted::Decrease() ;
    }
    
protected:
      
    /** 默认析构函数
    */
    ~IUnknownImpl5() {} ;
};

/** IUnknown接口实现类
*/
template <
            class T1,
            class T2,
            class T3,
            class T4,
            class T5,
            class T6
         >
class IUnknownImpl6:
    public RCRefCounted,
    public T1,
    public T2,
    public T3,
    public T4,
    public T5,
    public T6
{
public:
    
    /** 根据ID查询接口
    @param [in] iid 被查询的接口ID
    @param [out] outObject 如果查询成功，则保存结果接口指针，并增加引用计数
    @return 如果成功则返回RC_S_OK，否则返回错误号
    */
    virtual HResult QueryInterface(RC_IID iid, void** outObject)
    {
        if(outObject == NULL)
        {
            return RC_E_INVALIDARG ;
        }
        switch(iid){
            case IUnknown::IID:
                {
                    *outObject = (IUnknown*)(T1*)this ;
                }
                break ;
            case T1::IID:
                {
                    *outObject = (T1*)this ;
                }
                break ;
            case T2::IID:
                {
                    *outObject = (T2*)this ;
                }
                break ;
            case T3::IID:
                {
                    *outObject = (T3*)this ;
                }
                break ;
            case T4::IID:
                {
                    *outObject = (T4*)this ;
                }
                break ;
            case T5::IID:
                {
                    *outObject = (T5*)this ;
                }
                break ;
            case T6::IID:
                {
                    *outObject = (T6*)this ;
                }
                break ;
            default:
                return RC_E_NOINTERFACE ;
                break ;                
        }
        this->AddRef() ;
        return RC_S_OK ;
    }
    
    /** 增加引用计数
    */
    virtual void AddRef(void)
    {
        RCRefCounted::Increase() ;
    }
    
    /** 减少引用计数
    */
    virtual void Release(void)
    {
        RCRefCounted::Decrease() ;
    }
    
protected:
      
    /** 默认析构函数
    */
    ~IUnknownImpl6() {} ;
};

/** IUnknown接口实现类
*/
template <
            class T1,
            class T2,
            class T3,
            class T4,
            class T5,
            class T6,
            class T7
         >
class IUnknownImpl7:
    public RCRefCounted,
    public T1,
    public T2,
    public T3,
    public T4,
    public T5,
    public T6,
    public T7
{
public:
    
    /** 根据ID查询接口
    @param [in] iid 被查询的接口ID
    @param [out] outObject 如果查询成功，则保存结果接口指针，并增加引用计数
    @return 如果成功则返回RC_S_OK，否则返回错误号
    */
    virtual HResult QueryInterface(RC_IID iid, void** outObject)
    {
        if(outObject == NULL)
        {
            return RC_E_INVALIDARG ;
        }
        switch(iid){
            case IUnknown::IID:
                {
                    *outObject = (IUnknown*)(T1*)this ;
                }
                break ;
            case T1::IID:
                {
                    *outObject = (T1*)this ;
                }
                break ;
            case T2::IID:
                {
                    *outObject = (T2*)this ;
                }
                break ;
            case T3::IID:
                {
                    *outObject = (T3*)this ;
                }
                break ;
            case T4::IID:
                {
                    *outObject = (T4*)this ;
                }
                break ;
            case T5::IID:
                {
                    *outObject = (T5*)this ;
                }
                break ;
            case T6::IID:
                {
                    *outObject = (T6*)this ;
                }
                break ;
            case T7::IID:
                {
                    *outObject = (T7*)this ;
                }
                break ;
            default:
                return RC_E_NOINTERFACE ;
                break ;                
        }
        this->AddRef() ;
        return RC_S_OK ;
    }
    
    /** 增加引用计数
    */
    virtual void AddRef(void)
    {
        RCRefCounted::Increase() ;
    }
    
    /** 减少引用计数
    */
    virtual void Release(void)
    {
        RCRefCounted::Decrease() ;
    }
    
protected:
      
    /** 默认析构函数
    */
    ~IUnknownImpl7() {} ;
};

/** IUnknown接口实现类
*/
template <
            class T1,
            class T2,
            class T3,
            class T4,
            class T5,
            class T6,
            class T7,
            class T8,
            class T9,
            class T10,
            class T11
         >
class IUnknownImpl11:
    public RCRefCounted,
    public T1,
    public T2,
    public T3,
    public T4,
    public T5,
    public T6,
    public T7,
    public T8,
    public T9,
    public T10,
    public T11
{
public:
    
    /** 根据ID查询接口
    @param [in] iid 被查询的接口ID
    @param [out] outObject 如果查询成功，则保存结果接口指针，并增加引用计数
    @return 如果成功则返回RC_S_OK，否则返回错误号
    */
    virtual HResult QueryInterface(RC_IID iid, void** outObject)
    {
        if(outObject == NULL)
        {
            return RC_E_INVALIDARG ;
        }
        switch(iid){
            case IUnknown::IID:
                {
                    *outObject = (IUnknown*)(T1*)this ;
                }
                break ;
            case T1::IID:
                {
                    *outObject = (T1*)this ;
                }
                break ;
            case T2::IID:
                {
                    *outObject = (T2*)this ;
                }
                break ;
            case T3::IID:
                {
                    *outObject = (T3*)this ;
                }
                break ;
            case T4::IID:
                {
                    *outObject = (T4*)this ;
                }
                break ;
            case T5::IID:
                {
                    *outObject = (T5*)this ;
                }
                break ;
            case T6::IID:
                {
                    *outObject = (T6*)this ;
                }
                break ;
            case T7::IID:
                {
                    *outObject = (T7*)this ;
                }
                break ;
            case T8::IID:
                {
                    *outObject = (T8*)this ;
                }
                break ;
            case T9::IID:
                {
                    *outObject = (T9*)this ;
                }
                break ; 
            case T10::IID:
                {
                    *outObject = (T10*)this ;
                }
                break ;
            case T11::IID:
                {
                    *outObject = (T11*)this ;
                }
                break ;
            default:
                return RC_E_NOINTERFACE ;
                break ;                
        }
        this->AddRef() ;
        return RC_S_OK ;
    }
    
    /** 增加引用计数
    */
    virtual void AddRef(void)
    {
        RCRefCounted::Increase() ;
    }
    
    /** 减少引用计数
    */
    virtual void Release(void)
    {
        RCRefCounted::Decrease() ;
    }
    
protected:
      
    /** 默认析构函数
    */
    ~IUnknownImpl11() {} ;
};

END_NAMESPACE_RCZIP

#endif //__IUnknownImpl_h_
