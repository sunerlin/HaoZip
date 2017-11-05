/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCHandle_h_
#define __RCHandle_h_ 1

#include "base/RCNonCopyable.h"

#ifdef RCZIP_OS_WIN
#include "base/RCWindowsDefs.h"
#endif

BEGIN_NAMESPACE_RCZIP

class RCHandle:
    private RCNonCopyable
{

    /** 句柄类型定义
    */
#ifndef RCZIP_OS_WIN
    typedef HANDLE void*
#endif

public:

    /** 默认构造函数
    */
    RCHandle();

    /** 默认析构函数
    */
    ~RCHandle();

public:

    /** 关闭句柄
    @return 成功返回true, 否则返回false
    */
    bool Close();

    /** Attach 到句柄
    @param [in] handle 句柄值
    */
    void Attach(HANDLE handle);

    /** Detach 句柄
    @return 返回内部句柄值
    */
    HANDLE Detach();

    /** HANDLE 操作符
    @return 返回当前的句柄值
    */
    operator HANDLE() const ;

protected:

    /** 句柄值
    */
    HANDLE m_handle ;
};
END_NAMESPACE_RCZIP

#endif //__RCHandle_h_
