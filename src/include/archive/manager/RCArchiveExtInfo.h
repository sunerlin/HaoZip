/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCArchiveExtInfo_h_
#define __RCArchiveExtInfo_h_ 1

#include "base/RCString.h"

BEGIN_NAMESPACE_RCZIP

/** 文档格式扩展信息
*/
class RCArchiveExtInfo
{
public:

    /** 默认构造函数
    */
    RCArchiveExtInfo() ;
    
    /** 构造函数
    @param [in] ext 扩展名
    @param [in] addExt 附加扩展名
    */
    RCArchiveExtInfo(const RCString& ext, const RCString& addExt) ;
    
    /** 默认析构函数
    */
    ~RCArchiveExtInfo() ;
    
    /** 拷贝构造函数
    @param [in] rhs 拷贝原型
    */
    RCArchiveExtInfo(const RCArchiveExtInfo& rhs) ;
    
    /** 赋值操作符
    @param [in] rhs 复制原型
    */
    RCArchiveExtInfo& operator= (const RCArchiveExtInfo& rhs) ;

public:
    
    /** 获取文件扩展名
    @return 返回扩展名
    */
    const RCString& GetExt(void) const ;
    
    /** 获取附加扩展名
    @return 返回附加扩展名
    */
    const RCString& GetAddExt(void) const ;
    
    /** 设置扩展名
    @param [in] ext 扩展名
    */
    void SetExt(const RCString& ext) ;
    
    /** 设置附加扩展名
    @param [in] addExt 附加扩展名
    */
    void SetAddExt(const RCString& addExt) ;
    
private:
    
    /** 文件扩展名
    */
    RCString m_ext ;
    
    /** 附加扩展名
    */
    RCString m_addExt ;
};

END_NAMESPACE_RCZIP

#endif //__RCArchiveExtInfo_h_
