/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCFormatDefs_h_
#define __RCFormatDefs_h_ 1

#include "base/RCDefs.h"

BEGIN_NAMESPACE_RCZIP

#define RC_CREATE_ARCHIVE_FUN_NAME      "CreateArchiveInfoObjects"
#define RC_CREATE_ARCHIVE_FUN_TNAME  _T("CreateArchiveInfoObjects")

extern "C"{
    /** 创建文档信息对象函数, 返回对象类型为IArchiveInfo接口指针数组的起始地址，最后一个元素后面为NULL
    @param [out] outObject 返回IArchiveInfo接口指针，引用计数不加1
    */
    typedef HResult (*RCCreateArchiveFunction)(void** outObject) ;
}

END_NAMESPACE_RCZIP

#endif //__RCFormatDefs_h_
