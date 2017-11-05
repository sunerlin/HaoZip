/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCCodecsDefs_h_
#define __RCCodecsDefs_h_ 1

#include "base/RCDefs.h"

BEGIN_NAMESPACE_RCZIP

#define RC_CREATE_CODECS_FUN_NAME   "CreateCodecs"
#define RC_CREATE_CODECS_FUN_TNAME  _T("CreateCodecs")

extern "C"{
    /** 创建编码器管理对象, 返回对象类型为ICompressCodecsInfo接口指针
    @param [out] outObject 返回ICompressCodecsInfo接口指针，并且引用计数加1
    @return 如果成功则返回RC_S_OK，否则返回错误号
    */
    typedef HResult (*RCCreateCodecsFunction)(void** outObject) ;
}

END_NAMESPACE_RCZIP

#endif //__RCCodecsDefs_h_
