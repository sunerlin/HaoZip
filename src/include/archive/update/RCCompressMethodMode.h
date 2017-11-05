/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCCompressMethodMode_h_
#define __RCCompressMethodMode_h_ 1

#include "base/RCTypes.h"
#include "common/RCVector.h"
#include "archive/common/RCArchiveProperty.h"

BEGIN_NAMESPACE_RCZIP

/** 压缩模式
*/
class RCCompressMethodMode
{
public:

    /** 默认构造函数
    */
    RCCompressMethodMode() :
        m_formatIndex(-1) 
    {}
    
public:
    
    /** 设置文档格式编号
    @param [in] formatIndex 格式编号
    */
    void SetFormatIndex(int32_t formatIndex);
    
    /** 取得文档格式编号
    @return 返回文档格式编号
    */
    const int32_t GetFormatIndex(void) const;

    /** 取得属性列表
    @return 返回文档属性列表引用
    */
    RCVector<RCArchiveProperty>& GetProperties(void);
    
    /** 取得属性列表
    @return 返回文档属性列表
    */
    const RCVector<RCArchiveProperty>& GetProperties(void) const;

private:
    
    /** 文档格式编号
    */
    int32_t m_formatIndex;
    
    /** 属性列表
    */
    RCVector<RCArchiveProperty> m_properties;
};

END_NAMESPACE_RCZIP

#endif //__RCCompressMethodMode_h_
