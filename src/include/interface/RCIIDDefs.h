/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCIIDDefs_h_
#define __RCIIDDefs_h_ 1

#include "base/RCDefs.h"

BEGIN_NAMESPACE_RCZIP

/** Unknown的接口ID起始值
*/
#define IID_IUNKNOWN_BASE   (0)

/** 流相关的接口ID起始值
*/
#define IID_ISTREAM_BASE    (1024*1)

/** 操作进度相关的接口ID起始值
*/
#define IID_IPROGRESS_BASE  (1024*2)

/** 密码相关的接口ID起始值
*/
#define IID_IPASSWORD_BASE  (1024*3)

/** 压缩编解码相关的接口ID起始值
*/
#define IID_ICODER_BASE     (1024*4)

/** 文件操作相关的接口ID起始值
*/
#define IID_IARCHIVE_BASE   (1024*5)

/** 覆盖模式相关的接口ID起始值
*/
#define IID_IOVERWRITE_BASE (1024*6)

END_NAMESPACE_RCZIP

#endif //__RCIIDDefs_h_
