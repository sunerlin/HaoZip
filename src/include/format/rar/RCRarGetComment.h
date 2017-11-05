/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCRarGetComment_h_
#define __RCRarGetComment_h_ 1

#include "base/RCTypes.h"
#include "base/RCString.h"
#include "interface/IPassword.h"

BEGIN_NAMESPACE_RCZIP

class RCRarGetComment
{
public:

#ifndef RCZIP_UNICODE
    /** 获取RAR注释
    @param [in] filename 文件名
    @param [out] comment 返回注释
    @param [in] getTextPassword 密码管理器
    @return 成功返回true，否则返回false
    */
    static bool GetRarComment(const char* filename, RCStringA& comment, ICryptoGetTextPassword* getTextPassword);

#else
    /** 获取RAR注释
    @param [in] filename 文件名
    @param [out] comment 返回注释
    @param [in] getTextPassword 密码管理器
    @return 成功返回true，否则返回false
    */
    static bool GetRarComment(const wchar_t* filename, RCStringW& comment, ICryptoGetTextPassword* getTextPassword);
    
#endif
};

END_NAMESPACE_RCZIP

#endif //__RCRarGetComment_h_