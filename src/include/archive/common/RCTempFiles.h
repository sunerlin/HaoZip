/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCTempFiles_h_
#define __RCTempFiles_h_ 1

#include "base/RCNonCopyable.h"
#include "base/RCString.h"
#include "common/RCVector.h"

BEGIN_NAMESPACE_RCZIP

/** 临时文件
*/
class RCTempFiles:
    private RCNonCopyable
{
public:

    /** 默认构造函数
    */
    RCTempFiles() ;
    
    /** 默认析构函数
    */
    ~RCTempFiles() ;
    
public:
    
    /** 获取临时文件列表
    @return 返回临时文件列表
    */
    const RCVector<RCString>& GetPaths() const ;
    
    /** 获取临时文件列表
    @return 返回临时文件列表引用
    */
    RCVector<RCString>& GetPaths() ;
    
private:
    
    /** 删除所有临时文件后,清空列表
    */
    void Clear() ;
    
private:
    
    /** 临时文件列表
    */
    RCVector<RCString> m_paths ;
};

END_NAMESPACE_RCZIP

#endif //__RCTempFiles_h_
