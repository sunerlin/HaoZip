/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __IArchiveInfo_h_
#define __IArchiveInfo_h_ 1

#include "interface/RCArchiveType.h"
#include "interface/IRefCounted.h"
#include "base/RCString.h"
#include <vector>

BEGIN_NAMESPACE_RCZIP

class IInArchive ;
class IOutArchive ;

/** 创建输入文档的函数声明
@return 返回IInArchive接口指针
*/
typedef IInArchive* (*PFNCreateInArchive)(void) ;

/** 创建输出文档的函数声明
@return 返回IOutArchive接口指针
*/
typedef IOutArchive* (*PFNCreateOutArchive)(void) ;

/** 文档信息接口 
*/
class IArchiveInfo:
    public IRefCounted
{
public:
    
    /** 文档类型唯一ID
    @return 文档ID
    */
    virtual RCArchiveID GetArchiveID() const = 0 ;

    /** 获取文档名称
    @return 文档名称
    */
    virtual RCString GetName() const = 0 ;
    
    /** 获取文档扩展名
    @return 文档扩展名
    */
    virtual RCString GetExt() const = 0 ;
    
    /** 获取文档附加扩展名
    @return 文档附加扩展名
    */
    virtual RCString GetAddExt() const = 0 ;
    
    /** 获取文档签名
    @param [out] signature 返回文件签名
    */
    virtual void GetSignature(std::vector<byte_t>& signature) const = 0 ;
        
    /** 获取KeepName属性
    @return 如果KeepName则返回true，否则返回false
    */
    virtual bool IsKeepName() const = 0 ;
    
    /** 获取创建输入文档函数
    @return 函数指针
    */
    virtual PFNCreateInArchive GetCreateInArchiveFunc(void) const = 0 ;
    
    /** 获取创建输出文档函数
    @return 函数指针
    */
    virtual PFNCreateOutArchive GetCreateOutArchiveFunc(void) const = 0 ;

protected:
        
    /** 默认析构函数
    */
    ~IArchiveInfo() {} ;
};

/** 智能指针定义
*/
typedef RCComPtr<IArchiveInfo>  IArchiveInfoPtr ;

END_NAMESPACE_RCZIP

#endif //__IArchiveInfo_h_
