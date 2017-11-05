/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __IOverwrite_h
#define __IOverwrite_h 1

#include "interface/IUnknown.h"
#include "base/RCString.h"
#include "filesystem/RCFileDefs.h"
#include "archive/extract/RCExtractDefs.h"

BEGIN_NAMESPACE_RCZIP

/** 接口ID定义
*/
enum
{
    IID_IAskOverwrite = IID_IOVERWRITE_BASE
};

/** 获取密码接口
*/
class IAskOverwrite:
    public IUnknown
{
public:
    
    /** 接口ID
    */
    enum { IID = IID_IAskOverwrite } ;
    
public:
    
    /** 文件覆盖确认接口
    @param [in] existName 已经存在的文件路径
    @param [in] existTime 已经存在的文件时间
    @param [in] existSize 已经存在的文件大小
    @param [in] newName 新文件名
    @param [in] newTime 新文件时间
    @param [in] newSize 新文件大小
    @param [in] isEnableRename 是否支持重命名操作
    @param [out] answer 确认结果
    @param [out] newFileName 如果是重命名单个文件时，返回用户指定名称
    @return 成功返回RC_S_OK, 否则返回错误码
    */
    virtual HResult AskOverwrite(const RCString& existName,
                                 const RC_FILE_TIME* existTime, 
                                 const uint64_t* existSize,
                                 const RCString& newName,
                                 const RC_FILE_TIME* newTime, 
                                 const uint64_t* newSize,
                                 bool isEnableRename,
                                 RCExractOverwriteAnswer& answer,
                                 RCString& newFileName) = 0 ;
    
protected:
    
    /** 默认析构函数
    */
    ~IAskOverwrite(){} ;
};

/** 智能指针定义
*/
typedef RCComPtr<IAskOverwrite> IAskOverwritePtr ;

END_NAMESPACE_RCZIP

#endif //__IPassword_h
