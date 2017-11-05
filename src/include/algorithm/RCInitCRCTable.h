/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCInitCRCTable_h_
#define __RCInitCRCTable_h_ 1

#include "base/RCSingleton.h"

BEGIN_NAMESPACE_RCZIP

class RCInitCRCTableImpl
{
public:

    /** 默认构造函数
    */
    RCInitCRCTableImpl() ;
};

typedef RCSingleton<RCInitCRCTableImpl> RCInitCRCTable ;

END_NAMESPACE_RCZIP

#endif //__RCInitCRCTable_h_
