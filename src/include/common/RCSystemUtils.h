/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCSystemUtils_h_
#define __RCSystemUtils_h_ 1

#include "base/RCDefs.h"

BEGIN_NAMESPACE_RCZIP

class RCSystemUtils
{
public:

    /** 默认构造函数
    */
    RCSystemUtils() ;
    
    /** 默认析构函数
    */
    ~RCSystemUtils() ;
    
public:
    
    /** 获取CPU数量
    @return 返回当前机器的CPU数
    */
    static uint32_t GetNumberOfProcessors() ;
    
    /** 获取RAM大小
    @return 返回当前机器的RAM大小，单位为字节
    */
    static uint64_t GetRamSize() ;
};

END_NAMESPACE_RCZIP

#endif //__RCSystemUtils_h_
