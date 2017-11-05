/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCFileName_h_
#define __RCFileName_h_ 1

#include "base/RCString.h"

BEGIN_NAMESPACE_RCZIP

class RCFileName
{
public:
    
    /** 获取路径分隔符: 如windows平台的 '\\'
    @return 返回路径分隔符
    */
    static char_t GetDirDelimiter(void) ; 
    
    /** 获取路径分隔符: 返回'/'
    @return 返回路径分隔符'/'
    */
    static char_t GetPosixDirDelimiter(void) ; 
    
    /** 获取多个路径之间的分隔符
    @return 返回多个路径之间的分隔符
    */
    static char_t GetDirsSplitChar(void) ; 
    
    /** 获取通配符：* 字符
    @return 返回 * 字符
    */
    static char_t GetAnyStringWildcard(void) ; 
    
    /** 对路径处理，确保以路径分隔符为最后一个字符
    @param [in,out] dirPath 需要处理的路径名
    */
    static void NormalizeDirPathPrefix(RCString& dirPath) ;
    
    /** 对路径处理，确保以路径分隔符为平台标准的分隔符
    @param [in,out] dirPath 需要处理的路径名
    */
    static void NormalizeDirPathDelimiter(RCString& dirPath) ;
    
    /** 连接路径
    @param [in] dirPath 路径名
    @param [in] fileName 路径名或者文件名
    @return 返回连接的路径名
    */
    static RCString JoinFileName(const RCString& dirPath, const RCString& fileName) ;
    
    /** 分隔路径
    @param [in] fullName 完整路径名
    @param [out] pureName 文件名部分
    @param [out] extensionDelimiter 扩展名分隔符
    @param [out] extension 扩展名
    */
    static void SplitNameToPureNameAndExtension(const RCString& fullName,
                                                RCString& pureName,
                                                RCString& extensionDelimiter,
                                                RCString& extension) ;
    /** 在路径两边加上双引号
    @param [in] filePath 文件路径
    @return 返回处理过的文件路径
    */
    static RCString GetQuotedFilePath(const RCString& filePath) ;
    
    /** 在路径两边加上双引号
    @param [in] filePath 文件路径
    @return 返回处理过的文件路径
    */
    static RCString RemoveQuoteFromFilePath(const RCString& filePath) ;
    
    /** 判断是否为绝对路径
    @param [in] filePath 文件路径
    @return 如果为绝对路径，返回true, 否则返回false
    */
    static bool IsCompletedPath(const RCString& filePath) ;

    /** 获取文件名中禁止使用的字符。
    @param [in] path 文件名是否带路径
    @return 返回含禁止使用字符的字符串
    */
    static RCString GetFileNameForbidChar(bool path = false);
    
    /** 判断是否含有通配符
    @param [in] fileName 文件路径
    @return 如果含有通配符返回true, 否则返回false
    */
    static bool HasWildcardChar(const RCString& fileName) ;
};

END_NAMESPACE_RCZIP

#endif //__RCFileName_h_
