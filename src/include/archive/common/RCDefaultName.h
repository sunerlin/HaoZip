/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#include "base/RCString.h"

#ifndef __RCDefaultName_h_
#define __RCDefaultName_h_ 1

BEGIN_NAMESPACE_RCZIP

/** 缺省名称
*/
class RCDefaultName
{
public:

    /** 返回缺省名称2
    @param [in] fileName 文件名
    @param [in] extension 文件扩展名
    @param [in] addSubExtension 添加子扩展名
    @return 返回缺省文件名
    */
    static RCString GetDefaultName2(const RCString& fileName,
                                    const RCString& extension,
                                    const RCString& addSubExtension) ;
private:

    /** 返回缺省名称3
    @param [in] fileName 文件名
    @param [in] extension 文件扩展名
    @param [in] addSubExtension 添加子扩展名
    @return 返回缺省文件名
    */
    static RCString GetDefaultName3(const RCString& fileName,
                                    const RCString& extension, 
                                    const RCString& addSubExtension) ;

private:
    
    /** 空文件别名
    */
    static const RCString s_emptyFileAlias;
};

END_NAMESPACE_RCZIP

#endif //__RCDefaultName_h_
