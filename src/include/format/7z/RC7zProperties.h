/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RC7zProperties_h_
#define __RC7zProperties_h_ 1

#include "interface/RCPropertyID.h"

BEGIN_NAMESPACE_RCZIP

class RC7zPropID
{
public:

    enum
    {
        kpidPackedSize0 = RCPropID::kpidUserDefined,
        kpidPackedSize1,
        kpidPackedSize2,
        kpidPackedSize3,
        kpidPackedSize4
    };
};

END_NAMESPACE_RCZIP

#endif //__RC7zProperties_h_
