/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCExtractingFilePath_h_
#define __RCExtractingFilePath_h_ 1

#include "common/RCVector.h"
#include "base/RCString.h"

BEGIN_NAMESPACE_RCZIP

/** 解压文件路径
*/
class RCExtractingFilePath
{
public:

    /** 筛选正确路径
    @param [in,out] pathParts 文件路径列表
    */
    static void MakeCorrectPath(RCVector<RCString>& pathParts) ;
    
    /** 生成路径
    @param [in] parts 各级路径
    @return 返回生成的路径
    */
    static RCString MakePathNameFromParts(const RCVector<RCString>& parts) ;
    
    /** 生成路径
    @param [in] path 原路径
    @return 返回生成的路径
    */
    static RCString GetCorrectFsPath(const RCString& path) ;
    
    /** 生成路径
    @param [in] path 原路径
    @return 返回生成的路径
    */
    static RCString GetCorrectFullFsPath(const RCString& path) ;
    
private:

    /** 取正确的文件名
    @param [in] path 原文件名
    @return 返回正确的文件名
    */
    static RCString GetCorrectFileName(const RCString& path) ;

    /** 替换字符
    @param [in] s 原字符串
    @return 替换后字符串
    */
    static RCString ReplaceIncorrectChars(const RCString& s) ;

#ifdef RCZIP_OS_WIN

    /** 检查结尾
    @param [in] name 文件名
    @param [in] len 文件名长度
    @return 正确返回true,否则返回false
    */
    static bool CheckTail(const RCString& name, int32_t len) ;

    /** 检查文件数字
    @param [in] name 文件名
    @param [in] reservedName 保留名称
    @return 正确返回true,否则返回false
    */
    static bool CheckNameNum(const RCString& name, char_t* reservedName) ;

    /** 检查是否支持
    @param [in] name 文件名
    @return 支持返回true,否则返回false
    */
    static bool IsSupportedName(const RCString& name) ;
#endif
};

END_NAMESPACE_RCZIP

#endif //__RCExtractingFilePath_h_
