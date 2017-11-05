/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef  __RCUpdateErrorInfo_h_
#define __RCUpdateErrorInfo_h_ 1

#include "base/RCTypes.h"
#include "base/RCWindowsDefs.h"
#include "base/RCString.h"

BEGIN_NAMESPACE_RCZIP

/** 错误信息
*/
class RCErrorInfo
{
public:
    
    /** 默认构造函数
    */
    RCErrorInfo() ;

public:
    
    /** 设置系统错误号
    @param [in] systemError 系统错误号
    */
    void SetSystemError(HResult systemError);
    
    /** 取得系统错误号
    @return 返回系统错误号
    */
    const HResult GetSystemError(void) const;

    /** 设置错误文件名
    @param [in] fileName 文件名
    */
    void SetFileName(const RCString& fileName);
    
    /** 取得错误文件名
    @return 返回错误文件名
    */
    const RCString& GetFileName(void) const;

    /** 设置错误文件名
    @param [in] fileName2 文件名
    */
    void SetFileName2(const RCString& fileName2);
    
    /** 取得错误文件名
    @return 返回错误文件名
    */
    const RCString& GetFileName2(void) const;

    /** 设置错误信息
    @param [in] message 错误信息
    */
    void SetMessage(const RCString& message);
    
    /** 返回错误信息
    @return 返回错误信息
    */
    const RCString& GetMessage(void) const;

    /** 将错误信息转换成字符串
    @return 返回错误信息串
    */
    RCString ToString(void) const ;

private:
    
    /** 系统错误号
    */
    HResult m_systemError;

    /** 文件名
    */
    RCString m_fileName;

    /** 文件名2
    */
    RCString m_fileName2;

    /** 错误信息
    */
    RCString m_message;
    
};

/** 压缩，更新错误信息结构
*/
struct RCUpdateErrorInfo : 
    public RCErrorInfo
{
};

END_NAMESPACE_RCZIP

#endif //__RCUpdateErrorInfo_h_
