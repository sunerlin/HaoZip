/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCTempDirectory_h_
#define __RCTempDirectory_h_ 1

#include "base/RCNonCopyable.h"
#include "base/RCString.h"

BEGIN_NAMESPACE_RCZIP

class RCTempDirectory:
    private RCNonCopyable
{
public:

    /** 默认构造函数
    */
    RCTempDirectory() ;
    
    /** 默认析构函数
    */
    ~RCTempDirectory() ;
    
public:
    
    /** 获取临时目录路径
    @return 返回临时路径
    */
    const RCString& GetPath() const ;
    
    /** 删除临时目录
    @return 成功返回true, 失败则返回false
    */
    bool Remove() ;
    
    /** 析构时，禁止删除临时目录
    */
    void DisableDeleting() ;
    
    /** 创建临时目录
    @return 成功返回true，否则返回false
    */
    bool CreateTempDirectory(void) ;
    
    /** 创建临时文件名，同时设置删除标记
    @param [in] dirPath 创建临时目录的目录
    @return 成功返回true，否则返回false
    */
    bool CreateTempDirectory(const RCString& dirPath) ;
    
private:
    
    /** 创建临时目录
    @param [in] prefix 创建临时目录用的文件前缀
    @return 成功返回true，否则返回false
    */
    bool Create(const RCString& prefix) ;
    
    /** 创建临时文件名，同时设置删除标记
    @param [in] dirPath 创建临时目录的目录
    @param [in] prefix 创建临时文件用的文件前缀
    @return 成功返回true，否则返回false
    */
    bool Create(const RCString& dirPath, const RCString& prefix) ;
    
private:
    
    /** 是否删除临时目录标记
    */
    bool m_mustBeDeleted;
    
    /** 临时目录路径
    */
    RCString m_tempDir;
};

END_NAMESPACE_RCZIP

#endif //__RCTempDirectory_h_
