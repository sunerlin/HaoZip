/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCBZip2CRCTableInit_h_
#define __RCBZip2CRCTableInit_h_ 1

#include "base/RCSingleton.h"

BEGIN_NAMESPACE_RCZIP

/** BZip2 CRC表初始化
*/
class RCBZip2CRCTableInit
{
protected:

    /** 默认构造函数
    */
    RCBZip2CRCTableInit() ;
    
    /** 默认析构函数
    */
    ~RCBZip2CRCTableInit() ;
};

typedef RCSingleton<RCBZip2CRCTableInit> RCBZip2CRCInit ;

END_NAMESPACE_RCZIP

#endif //__RCBZip2CRCTableInit_h_
