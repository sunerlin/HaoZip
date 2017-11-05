/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCUpdateExtFilter_h_
#define __RCUpdateExtFilter_h_ 1

#include "base/RCDefs.h"
#include "base/RCSmartPtr.h"

BEGIN_NAMESPACE_RCZIP

/** 按扩展名过滤
*/
class RCExtFilter
{
public:

    /** 文件扩展名
    */
    RCString m_fileExt;

    /** 压缩算法ID
    */
    RCMethodID m_methodID;

    /** 级别
    */
    int32_t m_level;
};

END_NAMESPACE_RCZIP

#endif //__RCUpdateExtFilter_h_
