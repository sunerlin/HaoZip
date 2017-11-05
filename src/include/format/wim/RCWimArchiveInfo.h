/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCWimArchiveInfo_h_
#define __RCWimArchiveInfo_h_ 1

#include "common/RCArchiveInfoBase.h"

BEGIN_NAMESPACE_RCZIP

/** Wim 格式
*/
class RCWimArchiveInfo:
    public RCArchiveInfoBase
{
public:

    /** 默认构造函数
    */
    RCWimArchiveInfo() ;
    
    /** 默认析构函数
    */
    ~RCWimArchiveInfo() ;
 
public:

    /** 文档类型唯一ID
    @return 文档ID
    */
    virtual RCArchiveID GetArchiveID() const ;
    
    /** 获取Archive名称
    @return 返回Archive名称
    */
    virtual RCString GetName() const ;
    
    /** 获取Archive扩展名
    @return 返回Archive扩展名
    */
    virtual RCString GetExt() const ;
    
    /** 获取Archive扩展名
    @return 返回Archive附加扩展名
    */
    virtual RCString GetAddExt() const ;
    
    /** 获取Archive签名
    @param [in] signature 格式签名
    */
    virtual void GetSignature(std::vector<byte_t>& signature) const ;
        
    /** 获取KeepName属性
    @return 返回KeepName属性
    */
    virtual bool IsKeepName() const ;
    
    /** 获取创建InArchive函数
    @return 返回InArchive创建函数
    */
    virtual PFNCreateInArchive GetCreateInArchiveFunc(void) const ;
    
    /** 获取创建OutArchive函数
    @return 返回OutArchive创建函数
    */
    virtual PFNCreateOutArchive GetCreateOutArchiveFunc(void) const ;
};

END_NAMESPACE_RCZIP

#endif //__RCWimArchiveInfo_h_
