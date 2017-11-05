/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCPropData_h_
#define __RCPropData_h_ 1

#include "base/RCString.h"
#include "common/RCVariant.h"

BEGIN_NAMESPACE_RCZIP

struct RCPropData
{
public:

    /** 属性名
    */
    RCString m_propName ;

    /** 属性ID
    */
    uint32_t m_propID ;

    /** 属性类型
    */
    RCVariantType::RCVariantID m_varType ; 
};

END_NAMESPACE_RCZIP

#endif //__RCPropData_h_
