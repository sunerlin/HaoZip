/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCTarItem_h_
#define __RCTarItem_h_ 1

#include "format/common/RCItemNameUtils.h"
#include "common/RCStringConvert.h"
#include "base/RCString.h"
#include "RCTarHeader.h"

BEGIN_NAMESPACE_RCZIP

/** Tar 格式文件项
*/
struct RCTarItem
{
    /** 名称
    */
    RCStringA m_name;

    /** 大小
    */
    uint64_t m_size;

    /** 模式
    */
    uint32_t m_mode;

    /** UID
    */
    uint32_t m_uid;

    /** GID
    */
    uint32_t m_gid;

    /** 修改时间
    */
    uint32_t m_mTime;

    /** 主设备
    */
    uint32_t m_deviceMajor;

    /** 子设备
    */
    uint32_t m_deviceMinor;

    /** 链接名
    */
    RCStringA m_linkName;

    /** 用户
    */
    RCStringA m_user ;

    /** 用户组
    */
    RCStringA m_group ;

    /** Magic
    */
    char m_magic[8];

    /** 链接标志
    */
    char m_linkFlag;

    /** 主设备是否定义
    */
    bool m_deviceMajorDefined;

    /** 子设备是否定义
    */
    bool m_deviceMinorDefined;

    /** 检查是否为目录
    @return 目录返回true，否则返回false
    */
    bool IsDir() const
    {
        switch(m_linkFlag)
        {
          case RCTarHeader::NLinkFlag::s_directory:
          case RCTarHeader::NLinkFlag::s_dumpDir:
            return true;
          case RCTarHeader::NLinkFlag::s_oldNormal:
          case RCTarHeader::NLinkFlag::s_normal:
            return RCItemNameUtils::HasTailSlash(m_name, RC_CP_OEMCP);
        }
        return false;
    }

    /** 检查是否Magic
    @return Magic返回true，否则返回false
    */
    bool IsMagic() const
    {
        for (int i = 0; i < 5; i++)
          if (m_magic[i] != RCTarHeader::NMagic::s_usTar[i])
            return false;
        return true;
    }

    /** 取得压缩大小
    @return 返回压缩大小
    */
    uint64_t GetPackSize() const { return (m_size + 0x1FF) & (~((uint64_t)0x1FF)); }
};

/** Tar 文件项
*/
struct RCTarItemEx: public RCTarItem
{
    /** 头位置
    */
    uint64_t m_headerPosition;

    /** 链接大小
    */
    unsigned m_longLinkSize;

    /** 取得数据位置
    @return 返回数据位置
    */
    uint64_t GetDataPosition() const { return m_headerPosition + m_longLinkSize + RCTarHeader::s_recordSize; }

    /** 取得大小
    @return 返回大小
    */
    uint64_t GetFullSize() const { return m_longLinkSize + RCTarHeader::s_recordSize + m_size; }
};

END_NAMESPACE_RCZIP

#endif //__RCItem_h_
