/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __IRefCounted_h_
#define __IRefCounted_h_ 1

#include "interface/IUnknown.h"

BEGIN_NAMESPACE_RCZIP

/** 只含有引用计数，不提供接口查询的基类 
*/
class IRefCounted:
    public IUnknown
{
public:
    
    /** 根据ID查询接口
    @param [in] iid 被查询的接口ID
    @param [out] outObject 如果查询成功，则保存结果接口指针，并增加引用计数
    @return 始终返回RC_E_NOTIMPL
    */
    virtual HResult QueryInterface(RC_IID iid, void** outObject)
    {
        return RC_E_NOTIMPL ;
    }
    
    /** 增加引用计数
    */
    virtual void AddRef(void) = 0 ;
    
    /** 减少引用计数
    */
    virtual void Release() = 0 ;

protected:
      
    /** 默认析构函数
    */
    ~IRefCounted() {} ;
};

END_NAMESPACE_RCZIP

#endif //__IRefCounted_h_
