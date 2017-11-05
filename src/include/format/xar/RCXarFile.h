/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCXarFile_h_
#define __RCXarFile_h_ 1

#include "base/RCString.h"

BEGIN_NAMESPACE_RCZIP

/** Xar 文件
*/
class RCXarFile
{
public:

    /** 默认构造函数
    */
    RCXarFile() ;
    
    /** 默认析构函数
    */
    ~RCXarFile() ;
    
public:

    /** 名称
    */
    RCStringA m_name;

    /** 模式
    */
    RCStringA m_method;

    /** 大小
    */
    uint64_t m_size;

    /** 压缩大小
    */
    uint64_t m_packSize;

    /** 偏移量
    */
    uint64_t m_offset;

    /** 创建时间
    */
    uint64_t m_cTime;

    /** 修改时间
    */
    uint64_t m_mTime;

    /** 访问时间
    */
    uint64_t m_aTime;    

    /** 是否目录
    */
    bool m_isDir;

    /** 是否有数据
    */
    bool m_hasData;    

    /** 是否定义sha1
    */
    bool m_sha1IsDefined;

    /** sha1
    */
    byte_t m_sha1[20];

    /** 父编号
    */
    int32_t m_parent;
};

END_NAMESPACE_RCZIP

#endif //__RCXarFile_h_
