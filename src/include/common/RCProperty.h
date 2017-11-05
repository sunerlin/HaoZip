/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCProperty_h_
#define __RCProperty_h_ 1

#include "base/RCString.h"
#include "interface/RCPropertyID.h"
#include "common/RCVariant.h"
#include <utility>
#include <vector>

BEGIN_NAMESPACE_RCZIP

/** 属性名称，属性值的结构
    Key   是属性名称
    Value 是属性值
*/
typedef std::pair<RCString,RCVariant> RCPropertyNamePair ;

/** 属性数组
*/    
typedef std::vector<RCPropertyNamePair> RCPropertyNamePairArray ;
    
/** 属性名称，属性值的结构
    Key   是属性ID
    Value 是属性值
*/
typedef std::pair<RCPropertyID,RCVariant> RCPropertyIDPair ;

/** 属性数组
*/    
typedef std::vector<RCPropertyIDPair> RCPropertyIDPairArray ;

END_NAMESPACE_RCZIP

#endif //__RCProperty_h_
