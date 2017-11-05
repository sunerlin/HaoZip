/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCUpdateFileFilter_h_
#define __RCUpdateFileFilter_h_ 1

#include "base/RCDefs.h"
#include "base/RCSmartPtr.h"

BEGIN_NAMESPACE_RCZIP

/** 文件过滤
*/
class RCFileFilter
{
public:

    /** 文件
    */
    RCString m_fileList;

    /** 压缩算法ID
    */
    RCMethodID m_methodID;

    /** 级别
    */
    int32_t m_level;
};

END_NAMESPACE_RCZIP

#endif //__RCUpdateFileFilter_h_
