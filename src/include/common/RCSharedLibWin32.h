/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCSharedLibWin32_h_
#define __RCSharedLibWin32_h_ 1

#include "base/RCNonCopyable.h"
#include "base/RCString.h"

#include "base/RCWindowsDefs.h"

BEGIN_NAMESPACE_RCZIP

class RCSharedLibWin32:
    private RCNonCopyable
{
public:
    
    /** 类型定义
    */
    typedef HINSTANCE   shared_lib_module ;
    typedef FARPROC     shared_lib_proc ;
    
public:

    /** 默认构造函数
    */
    RCSharedLibWin32() ;
    
    /** 默认析构函数
    */
    ~RCSharedLibWin32() ;
    
    /** 载入动态链接库
    @param [in] fileName 动态链接库的文件名，包含完整路径
    @return 加载成功返回true，否则返回false
    */
    bool LoadSharedLib(const RCString& fileName) ;
    
    /** 载入动态链接库
    @param [in] fileName 动态链接库的文件名，包含完整路径
    @param [in] flags 载入的标记
    @return 加载成功返回true，否则返回false
    */
    bool LoadSharedLibEx(const RCString& fileName, DWORD flags) ;
    
    /** 卸载动态链接库
    @return  卸载成功返回true，否则返回false
    */
    bool UnloadSharedLib(void) ;
    
    /** 动态链接库是否已经加载
    @return 如果已经加载返回true，否则返回false
    */
    bool IsLoaded(void) const ;
    
    /** 获取动态链接库中的函数地址
    @param [in] procName 函数名称
    @return 返回获取的函数地址指针，如果失败返回NULL
    */
    shared_lib_proc GetSharedLibProc(const char* procName) const ;

    /** 获取模块句柄
    @return 返回当前已加载模块的句柄
    */
    shared_lib_module GetHandle() const ;
    
    /** 获取模块对应路径
    @param [in] module 该模块的句柄
    @return 完整的文件名，包含路径，如果失败返回空串
    */
    static RCString GetModuleFullName(shared_lib_module module) ;
    
private:
    
    /** module 句柄
    */
    shared_lib_module m_module ;
};

END_NAMESPACE_RCZIP

#endif //__RCSharedLibWin32_h_
