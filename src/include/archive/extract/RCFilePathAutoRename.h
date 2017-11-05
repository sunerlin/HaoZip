/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCFilePathAutoRename_h_
#define __RCFilePathAutoRename_h_ 1

#include "base/RCString.h"

BEGIN_NAMESPACE_RCZIP

/** 文件自动重命名
*/
class RCFilePathAutoRename
{
public:

    /** 自动重命名
    @param [in] fullProcessedPath 原路径
    @return 成功返回true,否则返回false
    */
    static bool AutoRenamePath(RCString& fullProcessedPath) ;

private:

    /** 生成重命名文件名
    @param [in] name 原文件名
    @param [in] extension 扩展名
    @param [in] value 数值
    @param [out] path 重命名文件名
    @return 成功返回true,否则返回false
    */
    static  bool MakeAutoName(const RCString& name,
                              const RCString& extension,
                              int32_t value,
                              RCString& path) ;
};

END_NAMESPACE_RCZIP

#endif //__RCFilePathAutoRename_h_
