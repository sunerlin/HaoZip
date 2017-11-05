/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __IOutStreamEvent_h_
#define __IOutStreamEvent_h_ 1

#include "base/RCString.h"

#ifdef RCZIP_OS_WIN
    #include "base/RCWindowsDefs.h"
#endif

BEGIN_NAMESPACE_RCZIP

/** 输出流的IO事件
*/
class IOutStreamEvent
{
public:
    
    /** 创建新文件
    @param [in] fileName 新创建的文件完整路径
    @return 成功返回RC_S_OK, 否则返回错误码
    */
    virtual HResult OnCreateFile(RCString fileName) = 0 ;
    
    /** 创建新文件夹
    @param [in] folderName 新创建的文件夹完整路径
    @return 成功返回RC_S_OK, 否则返回错误码
    */
    virtual HResult OnCreateFolder(RCString folderName) = 0 ;
    
    /** 通知Windows资源浏览器
    @return 成功返回RC_S_OK, 否则返回错误码
    */
    virtual HResult NotifyChange(void) = 0 ;
    
    /** IO写入错误
    @param [in] fileName 当前正在写入文件的完整路径
    @param [in] fileSize 当前正在写入的文件大小
    @param [in] errorCode 发生错误的错误码
    @return 成功返回RC_S_OK, 否则返回错误码
    */
    virtual HResult OnWriteFileError(RCString fileName, uint64_t fileSize, uint64_t errorCode) = 0 ;

#ifdef RCZIP_OS_WIN    
    
    /** 获取窗口句柄，仅Windows有效
    @return 返回窗口句柄
    */
    virtual HWND GetHwnd() const = 0 ;
    
#endif 

protected:
       
    /** 默认析构函数
    */
    virtual ~IOutStreamEvent() {} ;
};

END_NAMESPACE_RCZIP

#endif //__IOutStreamEvent_h_
