/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCSetProperties_h_
#define __RCSetProperties_h_ 1

#include "interface/IUnknown.h"
#include "common/RCVariant.h"
#include "common/RCVector.h"
#include "archive/common/RCArchiveProperty.h"

BEGIN_NAMESPACE_RCZIP

/** 属性设置
*/
class RCSetProperties
{
public:

    /** 设置属性
    @param [in] unknown Known接口
    @param [in] properties 属性数组
    @return 成功返回RC_S_OK，否则返回错误号
    */
    static HResult SetProperties(IUnknown* unknown, const RCVector<RCArchiveProperty>& properties) ;

private:

    /** 解析字符串
    @param [in] s 字符串
    @param [in] prop variant
    */
    static void ParseNumberString(const RCString& s, RCVariant& prop) ;    
};

END_NAMESPACE_RCZIP

#endif //__RCSetProperties_h_
