/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCWildcardUtils_h_
#define __RCWildcardUtils_h_ 1

#include "base/RCString.h"
#include "common/RCVector.h"

BEGIN_NAMESPACE_RCZIP

/** 通配辅助类
*/
class RCWildcardUtils
{
public:
    
    /** 比较文件名，大小写是否区分以s_isCaseSensitive进行判断
    @param [in] s1 第一个文件名
    @param [in] s2 第二个文件名
    @return 返回值定义如下：
            0  - 相等
            1  - lhs > rhs
            -1 - lhs < rhs
    */
    static int32_t CompareFileNames(const RCString& s1, const RCString& s2);
    
    /** 分解路径字符串为字符串数组，分隔符为文件路径分隔符
    @param [in] path 文件路径
    @param [out] pathParts 返回分隔结果
    */
    static void SplitPathToParts(const RCString& path, RCVector<RCString>& pathParts);
    
    /** 分解文件名为文件路径和文件名两个部分
    @param [in] path 文件名
    @param [out] dirPrefix 路径名，不含文件名
    @param [out] name 文件名，不含路径
    */
    static void SplitPathToParts(const RCString& path, RCString& dirPrefix, RCString& name);
    
    /** 从文件名中分解出文件路径
    @param [in] path 文件名，含路径
    @return 返回文件路径
    */
    static RCString ExtractDirPrefixFromPath(const RCString& path);
    
    /** 从文件名中分解出文件名
    @param [in] path 文件名，含路径
    @return 返回文件名，不含路径
    */
    static RCString ExtractFileNameFromPath(const RCString& path);
    
    /** 文件名中是否含有通配符
    @param [in] path 文件名
    @return 如果含有通配符返回true，否则返回false
    */
    static bool DoesNameContainWildCard(const RCString& path);
    
    /** 判断文件名是否匹配通配符
    @param [in] mask 通配符文件名
    @param [in] name 文件名
    @return 如果匹配返回true，否则返回false
    */
    static bool CompareWildCardWithName(const RCString& mask, const RCString& name);
    
    /** 判断字符是否为路径分隔符
    @param [in] ch 需要判断的字符
    @return 如果是分隔符返回true，否则返回false
    */
    static bool IsCharDirLimiter(char_t ch) ;
    
    /** 获取路径分隔符
    @return 返回路径分隔符
    */
    static char_t GetDirLimiter(void) ;
    
    /** 获取是否区分大小写
    @return 区分大小写返回true,否则返回false
    */
    static bool IsCaseSensitive(void) ;
    
    /** 设置是否区分大小写
    @param [in] isCaseSensitive 是否区分大小写
    */
    static void SetCaseSensitive(bool isCaseSensitive) ;

private:
    
    /** 比较文件名时，是否区分大小写
    */
    static bool s_isCaseSensitive ;

private:
    
    /** 判断文件名是否匹配通配符
    @param [in] mask 通配符文件名
    @param [in] name 文件名
    @return 如果匹配返回true，否则返回false
    */
    static bool EnhancedMaskTest(const char_t* mask, const char_t* name) ;
};

END_NAMESPACE_RCZIP

#endif //__RCWildcardUtils_h_
