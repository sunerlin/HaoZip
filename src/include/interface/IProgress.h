/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __IProgress_h_
#define __IProgress_h_ 1

#include "interface/IUnknown.h"

BEGIN_NAMESPACE_RCZIP

/** 接口ID定义
*/
enum
{
    IID_IProgress = IID_IPROGRESS_BASE
};

/** 进度显示相关接口声明
*/
class IProgress:
    public IUnknown
{
public:
    
    /** 接口ID
    */
    enum { IID = IID_IProgress } ;
    
public:

    /** 总体数量
    @param [in] total 总体的数量
    @return 成功返回RC_S_OK, 否则返回错误码
    */
    virtual HResult SetTotal(uint64_t total) = 0 ;
    
    /** 当前完成
    @param [in] completed 当前完成的数量
    @return 成功返回RC_S_OK, 否则返回错误码
    */
    virtual HResult SetCompleted(uint64_t completed) = 0 ;

protected:
        
    /** 默认析构函数
    */
    ~IProgress(){} ;
};

/** 智能指针定义
*/
typedef RCComPtr<IProgress> IProgressPtr ;

END_NAMESPACE_RCZIP

#endif //__IProgress_h_
