/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCWildcardItem_h_
#define __RCWildcardItem_h_ 1

#include "base/RCNonCopyable.h"
#include "base/RCString.h"
#include "common/RCVector.h"

BEGIN_NAMESPACE_RCZIP

/** 通配符匹配项
*/
class RCWildcardItem
{
public:

    /** 默认构造函数
    */
    RCWildcardItem() ;
    
    /** 默认析构函数
    */
    ~RCWildcardItem() ;
    
    /** 拷贝构造函数
    @param [in] rhs 拷贝参数
    */
    RCWildcardItem(const RCWildcardItem& rhs) ;
    
    /** 赋值操作符
    @param [in] rhs 复制参数
    */
    RCWildcardItem& operator= (const RCWildcardItem& rhs) ;

public:
    
    /** 检查路径
    @param [in] pathParts 各级路径列表
    @param [in] isFile 是否为文件
    @return 匹配返回true,否则返回false
    */
    bool CheckPath(const RCVector<RCString>& pathParts, bool isFile) const ;
    
    /** 返回各级路径列表
    @return 返回各级路径列表
    */
    const RCVector<RCString>& GetPathParts(void) const ;
    
    /** 返回各级路径列表
    @return 返回各级路径列表引用
    */
    RCVector<RCString>& GetPathParts(void) ;
    
    /** 返回是否递归
    @return 递归返回true,否则返回false
    */
    bool IsRecursive(void) const ;
    
    /** 返回是否for文件
    @return for文件返回true,否则返回false
    */
    bool IsForFile(void) const ;
    
    /** 返回是否for目录
    @return for目录返回true,否则返回false
    */
    bool IsForDir(void) const ;
    
    /** 设置是否递归
    @param [in] isRecursive 是否递归
    */
    void SetIsRecursive(bool isRecursive) ;
    
    /** 设置是否for文件
    @param [in] isForFile for文件
    */
    void SetIsForFile(bool isForFile) ;
    
    /** 设置是否for目录
    @param [in] isForDir for目录
    */
    void SetIsForDir(bool isForDir) ;
    
private:
      
    /** 文件路径列表
    */ 
    RCVector<RCString> m_pathParts ;
    
    /** 是否递归
    */
    bool m_isRecursive ;
    
    /** 是否for文件
    */
    bool m_isForFile ;
    
    /** 是否for目录
    */
    bool m_isForDir ;
};

END_NAMESPACE_RCZIP

#endif //__RCWildcardItem_h_
