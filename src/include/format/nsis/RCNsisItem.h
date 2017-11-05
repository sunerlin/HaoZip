/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCNsisItem_h_
#define __RCNsisItem_h_ 1

#include "common/RCStringUtil.h"
#include "common/RCStringConvert.h"
#include "filesystem/RCFileDefs.h"
#include "RCNsisHeader.h"

BEGIN_NAMESPACE_RCZIP

struct RCNsisFirstHeader
{
public:

    /** 标志
    */
    uint32_t m_flags;

    /** 头部长度
    */
    uint32_t m_headerLength;

    /** 文档大小
    */
    uint32_t m_archiveSize;

public:

    /** 是否crc
    @return 是返回true，否则返回false
    */
    bool ThereIsCrc() const
    {
        if ((m_flags & RCNsisHeader::NFlags::kForceCrc) != 0)
        {
            return true;
        }
        return ((m_flags & RCNsisHeader::NFlags::kNoCrc) == 0);
    }

    /** 获取数据大小
    @return 返回数据大小
    */
    uint32_t GetDataSize() const
    {
        return m_archiveSize - (ThereIsCrc() ? 4 : 0);
    }
};

struct RCNsisBlockHeader
{
    /** 偏移
    */
    uint32_t m_offset;

    /** 个数
    */
    uint32_t m_num;
};

struct RCNsisItem
{
public:

    /** 前缀
    */
    RCStringA m_prefixA;

    /** 前缀
    */
    RCString m_prefixU;

    /** 名字
    */
    RCStringA m_nameA;

    /** 名字
    */
    RCString m_nameU;

    /** 修改时间
    */
    RC_FILE_TIME m_mTime;

    /** 是否是unicode
    */
    bool m_isUnicode;

    /** 是否过滤
    */
    bool m_useFilter;

    /** 是否是压缩
    */
    bool m_isCompressed;

    /** 大小是否定义
    */
    bool m_sizeIsDefined;

    /** 压缩大小是否定义
    */
    bool m_compressedSizeIsDefined;

    /** 估计大小是否定义
    */
    bool m_estimatedSizeIsDefined;

    /** 位置
    */
    uint32_t m_pos;

    /** 大小
    */
    uint32_t m_size;

    /** 压缩大小
    */
    uint32_t m_compressedSize;

    /** 估计大小
    */
    uint32_t m_estimatedSize;

    /** 字典大小
    */
    uint32_t m_dictionarySize;

public:

    /** 构造函数
    */
    RCNsisItem(): 
      m_isUnicode(false),
      m_useFilter(false),
      m_isCompressed(true),
      m_sizeIsDefined(false),
      m_compressedSizeIsDefined(false),
      m_estimatedSizeIsDefined(false),
      m_size(0)
    {
    }

public:

    /** 是否是安装目录
    @return 是返回true，否则返回false
    */
    bool IsINSTDIR() const
    {
        return (m_prefixA.length() >= 3 || m_prefixU.length() >= 3);
    }

    /** 获取简化名
    @param [in] unicode 是否是unicode
    @return 返回名字
    */
    RCString GetReducedName(bool unicode) const
    {
        RCString  s;
        if (unicode)
        {
            s = m_prefixU;
        }
        else
        {
            s = RCStringConvert::MultiByteToUnicodeString(m_prefixA);
        }

        if (s.length() > 0)
        {
            if (s[s.length() - 1] != _T('\\'))
            {
                s += _T('\\');
            }
        }

        if (unicode)
        {
            s += m_nameU;
        }
        else
        {
            s += RCStringConvert::MultiByteToUnicodeString(m_nameA);
        }

        const int32_t len = 9;
        if (RCStringUtil::CompareNoCase(RCStringUtil::Left(s, len), _T("$INSTDIR\\")) == 0)
        {
            s = RCStringUtil::Mid(s, len);
        }
        return s;
    }
};

END_NAMESPACE_RCZIP

#endif //__RCNsisItem_h_
