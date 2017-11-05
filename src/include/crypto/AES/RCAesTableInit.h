/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCAesTableInit_h_
#define __RCAesTableInit_h_ 1

#include "base/RCSingleton.h"

BEGIN_NAMESPACE_RCZIP

/** Aes 表初始化
*/
class RCAesTableInitImpl
{
protected:

    /** 默认构造函数
    */
    RCAesTableInitImpl() ;
    
    /** 默认析构函数
    */
    ~RCAesTableInitImpl() ;
};

typedef RCSingleton<RCAesTableInitImpl> RCAesTableInit ;

END_NAMESPACE_RCZIP

#endif //__RCAesTableInit_h_
