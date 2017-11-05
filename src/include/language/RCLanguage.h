/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCLanguage_h_
#define __RCLanguage_h_ 1

#include "base/RCString.h"
#include "base/RCSingleton.h"
#include "language/RCLanguageRes.h"
#include "common/RCSharedLibrary.h"
#include <stdarg.h>

BEGIN_NAMESPACE_RCZIP

class RCLanguage
{
protected:

    /** 默认构造函数
    */
    RCLanguage() ;
    
    /** 默认析构函数
    */
    ~RCLanguage() ;

public:

    /** 初始化资源动态链接库
    @param [in] resFileName 资源文件名
    @return 加载成功则返回true, 否则返回false
    */
    bool SetResFileName(const RCString& resFileName) ;
    
    /** 获取资源文件名
    @return 返回资源文件名，含完整路径
    */
    RCString GetResFileName(void) const ;
    
    /** 加载字符串, 字符串最长限制为 511 字节
    @param [in] resourceID 资源ID
    @return 返回加载的字符串，失败则返回空串
    */
    RCString LoadStr(uint32_t resourceID) ;
    
    /** 加载格式化字符串，参数可变，加载字符串最长限制为 1023 字节
    @param [in] resourceID 资源ID
    @return 返回加载的字符串，失败则返回空串
     */
    RCString LoadFormatStr(uint32_t resourceID, ...) ;
    
    /** 加载格式化字符串，参数可变，加载字符串最长限制为 1023 字节
    @param [in] resourceID 资源ID
    @param [in] args 可变参数列表
    @return 返回加载的字符串，失败则返回空串
     */
    RCString LoadFormatStr(uint32_t resourceID, va_list args) ;

    /** 获取资源模块的句柄
    @return 返回资源模块的句柄,失败则返回空
    */
    RCSharedLibrary::shared_lib_module GetResHandle(void) const;
        
    /** 卸载载语言模块
    */
    bool UnLoadLanguage(void) ;

private:
    
    /** 实现类
    */
    class TImpl ;
    
    /** 实现对象
    */
    TImpl* m_impl ;
};

/** 语言管理类的单例
*/
typedef RCSingleton<RCLanguage> RCLang ;

/** 加载字符串
@param [in] resourceID 资源ID
@return 返回加载的字符串值
*/
inline RCString LoadStr(uint32_t resourceID)
{
    return RCLang::Instance().LoadStr(resourceID) ;
}

/** 加载格式化字符串，参数可变
@param [in] resourceID 资源ID
@param [in] ... 可变参数列表
@return 返回加载的字符串值
*/
inline RCString LoadFormatStr(uint32_t resourceID, ...)
{
    va_list args;
    va_start(args, resourceID) ;
    RCString result = RCLang::Instance().LoadFormatStr(resourceID, args) ;
    va_end(args) ; 
    return result ;
}

/** 获取资源模块的句柄
@return 返回资源文件对应的系统平台句柄
*/
inline RCSharedLibrary::shared_lib_module GetResHandle(void)
{
    return RCLang::Instance().GetResHandle();
}


/** 设置资源文件名的导出函数名
*/
#define RC_SET_RESOURCE_FUNCTION "SetResFileName"

/** 设置资源文件名的导出函数原型
@param [in] szFileName 资源文件名，含完整路径，字符串以'\0'结尾
*/
extern "C"{
    typedef RCZIP_API void (*PFNSetResurceFileNameFunc)(const RCString::value_type* szFileName) ;
}

END_NAMESPACE_RCZIP

#endif //__RCLanguage_h_
