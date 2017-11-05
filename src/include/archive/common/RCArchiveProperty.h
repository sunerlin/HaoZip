/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCArchiveProperty_h_
#define __RCArchiveProperty_h_ 1

#include "base/RCString.h"

BEGIN_NAMESPACE_RCZIP

/** 文档属性
*/
class RCArchiveProperty
{
public:

    /** 默认构造函数
    */
    RCArchiveProperty() ;
    
    /** 构造函数
    @param [in] name 属性名称
    @param [in] value 属性值
    */
    RCArchiveProperty(const RCString& name, const RCString& value) ;
    
    /** 默认析构函数
    */
    ~RCArchiveProperty() ;
    
    /** 拷贝构造函数
    @param [in] rhs 拷贝值
    */
    RCArchiveProperty(const RCArchiveProperty& rhs) ;
    
    /** 赋值操作符
    @param [in] rhs 复制原型
    */
    RCArchiveProperty& operator= (const RCArchiveProperty& rhs) ;

public:    
    
    /** 设置属性名称
    @param [in] name 属性名称
    */
    void SetName(const RCString& name) ;
    
    /** 设置属性值
    @param [in] value 属性值
    */
    void SetValue(const RCString& value) ;
    
    /** 获取属性名称
    @return 返回属性名称
    */
    const RCString& GetName(void) const ;
    
    /** 获取属性值
    @return 返回属性值
    */
    const RCString& GetValue(void) const ;
    
private:
    
    /** 属性名称
    */
    RCString m_name;
    
    /** 属性值
    */
    RCString m_value;
};

END_NAMESPACE_RCZIP

#endif //__RCArchiveProperty_h_
