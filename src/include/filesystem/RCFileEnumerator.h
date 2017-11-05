/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCFileEnumerator_h_
#define __RCFileEnumerator_h_ 1

#include "base/RCNonCopyable.h"
#include "base/RCString.h"
#include "filesystem/RCFindFile.h"

BEGIN_NAMESPACE_RCZIP

class RCFileInfo ;

/** 遍历获取一个目录中的所有文件或者目录项
    不会递归遍历子目录
*/
class RCFileEnumerator:
    private RCNonCopyable
{
public:

    /** 默认构造函数
    */
    RCFileEnumerator() ;
    
    /** 构造函数
    @param [in] wildcard 含通配符的目录名
    */
    explicit RCFileEnumerator(const RCString& wildcard) ;
    
    /** 默认析构函数
    */
    ~RCFileEnumerator() ;
    
public:
    
    /** 读取一个文件
    @param [out] fileInfo 返回取到的文件信息
    @return 成功返回true, 如果失败或者已经没有其他文件时，返回false
    */
    bool Next(RCFileInfo& fileInfo) ;
    
    /** 读取一个文件
    @param [out] fileInfo 返回取到的文件信息
    @param [out] found 如果取到文件返回true，否则返回false
    @return 成功返回true, 如果失败返回false
    */
    bool Next(RCFileInfo& fileInfo, bool& found);

private:
    
    /** 读取下一个文件
    @param [out] fileInfo 返回取到的文件信息
    */
    bool NextAny(RCFileInfo& fileInfo);
    
private:
    
    /** 通配符路径，默认为*
    */
    RCString m_wildcard ;
    
    /** 文件查找方法
    */
    RCFindFile m_findFile ;
};

END_NAMESPACE_RCZIP

#endif //__RCFileEnumerator_h_
