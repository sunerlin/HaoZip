/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCFindFile_h_
#define __RCFindFile_h_ 1

#include "base/RCNonCopyable.h"
#include "base/RCString.h"

BEGIN_NAMESPACE_RCZIP

class RCFileInfo ;
class RCFindFile:
    private RCNonCopyable
{
public:

    /** 默认构造函数
    */
    RCFindFile() ;
    
    /** 默认析构函数
    */
    ~RCFindFile() ;

public:
    
    /** 句柄是否已经分配
    @return 如果已经分配返回true, 否则返回false
    */
    bool IsHandleAllocated() const ;
    
    /** 查找文件
    @param [in] wildcard 含通配符的路径
    @param [out] fileInfo 取到的文件信息
    @return 成功返回true, 否则返回false
    */
    bool FindFirst(const RCString& wildcard, RCFileInfo& fileInfo) ;
    
    /** 查找下一个文件
    @param [out] fileInfo 取到的文件信息
    @return 成功返回true, 否则返回false
    */
    bool FindNext(RCFileInfo& fileInfo) ;
    
    /** 关闭查找
    @return 成功返回true, 否则返回false
    */
    bool Close(void) ;
    
    /** 查找文件
    @param [in] wildcard 含通配符的路径
    @param [out] fileInfo 取到的文件信息
    @return 成功返回true, 否则返回false
    */
    static bool FindFile(const RCString& wildcard, RCFileInfo& fileInfo);
    
private:
    
    /** 实现类
    */
    class TImpl ;
    
    /** 实现对象
    */
    TImpl* m_impl ;
};

END_NAMESPACE_RCZIP

#endif //__RCFindFile_h_
