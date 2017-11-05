/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCExtractStatus_h_
#define __RCExtractStatus_h_ 1

#include "base/RCDefs.h"

BEGIN_NAMESPACE_RCZIP

/** 解压状态
*/
class RCExtractStatus
{
public:
    
    /** 文档总数
    */
    uint64_t m_numArchives ;
    
    /** 解压后的数据大小
    */
    uint64_t m_unpackSize ;
    
    /** 解压压缩数据大小
    */
    uint64_t m_packSize ;
    
    /** 已经解压的文件夹个数
    */
    uint64_t m_numFolders ;
    
    /** 已经解压的文件个数
    */
    uint64_t m_numFiles ;

public:
    
    /** 清空成员变量
    */
    void Clear()
    {
        m_numArchives = 0;
        m_unpackSize  = 0;
        m_packSize    = 0;
        m_numFolders  = 0;
        m_numFiles    = 0;
    }
};

END_NAMESPACE_RCZIP

#endif //__RCExtractStatus_h_
