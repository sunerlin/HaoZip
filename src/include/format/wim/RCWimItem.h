/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCWimItem_h_
#define __RCWimItem_h_ 1

#include "format/wim/RCWimResource.h"
#include "filesystem/RCFileDefs.h"
#include "base/RCString.h"

BEGIN_NAMESPACE_RCZIP

const uint32_t s_kWimHashSize = 20;
const uint32_t s_kWimStreamInfoSize = 24 + 2 + 4 + s_kWimHashSize ;

/** Wim Stream Info
*/
struct RCWimStreamInfo
{
    /** Resource
    */
    RCWimResource m_resource;

    /** Part Number
    */
    uint16_t m_partNumber;

    /** Ref Count
    */
    uint32_t m_refCount;

    /** Hash
    */
    byte_t m_hash[s_kWimHashSize];
};

class RCWimItem
{
public:

    /** 默认构造函数
    */
    RCWimItem() ;
    
    /** 默认析构函数
    */
    ~RCWimItem() ;
    
public:

    /** 是否为目录
    @return 目录返回true,否则返回false
    */
    bool isDir() const ;

    /** 是否有流
    @return 有数据流返回true,否则返回false
    */
    bool HasStream() const ;
    
public:

    /** 名称
    */
    RCString m_name;

    /** 属性
    */
    uint32_t m_attrib;

    /** Hash
    */
    byte_t m_hash[s_kWimHashSize];

    /** 创建时间
    */
    RC_FILE_TIME m_cTime;

    /** 访问时间
    */
    RC_FILE_TIME m_aTime;

    /** 修改时间
    */
    RC_FILE_TIME m_mTime;

    /** 编号
    */
    int32_t m_streamIndex;

    /** Meta Data
    */
    bool m_hasMetadata ;
};

END_NAMESPACE_RCZIP

#endif //__RCWimItem_h_
