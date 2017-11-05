/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCTempFile_h_
#define __RCTempFile_h_ 1

#include "base/RCNonCopyable.h"
#include "base/RCString.h"

BEGIN_NAMESPACE_RCZIP

class RCTempFile:
    private RCNonCopyable
{
public:

    /** 默认构造函数
    */
    RCTempFile() ;
    
    /** 默认析构函数
    */
    ~RCTempFile() ;
    
public:
    /** 析构的时候禁止删除文件
    */
    void DisableDeleting() ;
    
    /** 创建临时文件名，同时设置删除标记
    @param [in] dirPath 创建临时文件的目录
    @param [out] resultPath 返回创建的临时文件名，包含完整路径
    @return 成功返回true，否则返回false
    */
    bool CreateTempFile(const RCString& dirPath, RCString& resultPath);
    
    /** 创建临时文件名，同时设置删除标记
    @param [out] resultPath 返回创建的临时文件名，包含完整路径
    @return 成功返回true，否则返回false
    */
    bool CreateTempFile(RCString& resultPath);
       
    /** 删除文件
    @return 成功删除返回true，否则返回false
    */
    bool Remove();
    
    /** 获取临时文件名
    @return 返回临时文件名，含完整路径
    */
    const RCString& GetFileName(void) const ;
    
private:
    
    /** 创建临时文件名，同时设置删除标记
    @param [in] dirPath 创建临时文件的目录
    @param [in] prefix 创建临时文件用的文件前缀
    @param [out] resultPath 返回创建的临时文件名，包含完整路径
    @return 成功返回true，否则返回false
    */
    bool Create(const RCString& dirPath, const RCString& prefix, RCString& resultPath) ;
    
    /** 创建临时文件名，同时设置删除标记
    @param [in] prefix 创建临时文件的目录前缀
    @param [out] resultPath 返回创建的临时文件名，包含完整路径
    @return 成功返回true，否则返回false
    */
    bool Create(const RCString& prefix, RCString& resultPath) ;
    
private:
    
    /** 析构的时候是否需要删除的标记
    */
    bool m_mustBeDeleted ;
    
    /** 临时文件名
    */
    RCString m_fileName ;
};

END_NAMESPACE_RCZIP

#endif //__RCTempFile_h_
