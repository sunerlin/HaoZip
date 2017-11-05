/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCItemNameUtils_h_
#define __RCItemNameUtils_h_ 1

#include "base/RCNonCopyable.h"
#include "base/RCString.h"

BEGIN_NAMESPACE_RCZIP

class RCItemNameUtils:
    private RCNonCopyable
{
public:

    /** 默认构造函数
    */
    RCItemNameUtils() ;
    
    /** 默认析构函数
    */
    ~RCItemNameUtils() ;
    
public:

    /** 转成合法文件名：路径中'/'转成'\'
    @param [in] name 输入文件名字，返回合法文件名
    @return 合法文件名
    */
    static RCString MakeLegalName(const RCString& name);

    /** 转成平台相关文件名
    @param [in] name 输入文件名字，返回平台相关文件名
    @return 平台相关文件名
    */
    static RCString GetOSName(const RCString& name);

    /** 转成平台相关文件名
    @param [in] name 输入文件名字，返回平台相关文件名
    @return 平台相关文件名
    */
    static RCString GetOSName2(const RCString& name);

    /** 末尾是否有分割符
    @param [in] name 文件名
    @param [in] codePage 编码
    @return 是返回true，否则返回false
    */
    static bool HasTailSlash(const RCStringA& name, uint32_t codePage);

    /** windows文件名转成平台相关文件名
    @param [in] name 输入文件名字，返回平台相关文件名
    @return 平台相关文件名
    */
    static RCString WinNameToOSName(const RCString& name) ;
};

END_NAMESPACE_RCZIP

#endif //__RCItemNameUtils_h_
