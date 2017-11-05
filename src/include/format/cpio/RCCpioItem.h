/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCCpioItem_h_
#define __RCCpioItem_h_ 1

#include "base/RCDefs.h"
#include "base/RCString.h"

#ifdef RCZIP_OS_WIN
#include <sys/stat.h>
#endif

BEGIN_NAMESPACE_RCZIP

struct RCCpioItem
{
    /** 名字
    */
    RCStringA m_name;

    /** 索引节点
    */
    uint32_t m_inode;

    /** 模式 
    */
    uint32_t m_mode;

    /** UID
    */
    uint32_t m_uid;

    /** GID
    */
    uint32_t m_gid;

    /** 大小
    */
    uint32_t m_size;

    /** 修改时间
    */
    uint32_t m_mTime;

    // char_t m_linkFlag;
    // RCStringA m_linkName; ?????

    /** 签名 
    */
    char m_magic[8];

    /** 链接数
    */
    uint32_t m_numLinks;

    /** 压缩主版本
    */
    uint32_t m_devMajor;

    /** 压缩子版本
    */
    uint32_t m_devMinor;

    /** 解压主版本
    */
    uint32_t m_rdevMajor;

    /** 解压子版本
    */
    uint32_t m_rdevMinor;

    /** 校验和
    */
    uint32_t m_chkSum;

    /** 是否对齐
    */
    uint32_t m_align;

    /** 是否是目录
    @return 是返回vtrue，否则返回false
    */
    bool IsDir() const
    {
        return (m_mode & 0170000) == 0040000 ;
    }
};

class RCCpioItemEx: 
    public RCCpioItem
{
public:

    /** 获取数据偏移位置
    @return 返回数据偏移位置
    */
    uint64_t GetDataPosition() const
    {
        return m_headerPosition + m_headerSize;
    };

public:

    /** 头部偏移位置
    */
    uint64_t m_headerPosition;

    /** 头部大小
    */
    uint32_t m_headerSize;
};

END_NAMESPACE_RCZIP

#endif //__RCCpioItem_h_
