/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __IUnknown_h_
#define __IUnknown_h_ 1

#include "interface/RCIIDDefs.h"
#include "base/RCComPtr.h"

BEGIN_NAMESPACE_RCZIP

/** 接口ID定义
*/
enum
{
    IID_INVALID  = IID_IUNKNOWN_BASE -1,
    IID_IUnknown = IID_IUNKNOWN_BASE
};

/** 接口基类，所有接口均属于该类的子类 
*/
class IUnknown
{
public:
    
    /** 接口ID
    */
    enum { IID = IID_IUnknown } ;
    
public:
    
    /** 根据ID查询接口
    @param [in] iid 被查询的接口ID
    @param [out] outObject 如果查询成功，则保存结果接口指针，并增加引用计数
    @return 如果成功则返回RC_S_OK，否则返回错误号
    */
    virtual HResult QueryInterface(RC_IID iid, void** outObject) = 0 ;
    
    /** 增加引用计数
    */
    virtual void AddRef(void) = 0 ;
    
    /** 减少引用计数
    */
    virtual void Release(void) = 0 ;

protected:
      
    /** 默认析构函数
    */
    virtual ~IUnknown() {} ;
};

/** 智能指针定义
*/
typedef RCComPtr<IUnknown> IUnknownPtr ;


END_NAMESPACE_RCZIP

#endif //__IUnknown_h_
