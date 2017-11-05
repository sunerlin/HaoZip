/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCRarItem_h_
#define __RCRarItem_h_ 1

#include "RCRarHeader.h"
#include "base/RCString.h"
#include "base/RCSmartPtr.h"

BEGIN_NAMESPACE_RCZIP

struct RCRarTime
{
    /** dos时间
    */
    uint32_t m_dosTime;

    /** 低位秒
    */
    byte_t m_lowSecond;

    /** 时间
    */
    byte_t m_subTime[3];
};

struct RCRarItem
{
    /** 大小
    */
    uint64_t m_size;

    /** 包大小
    */
    uint64_t m_packSize;

    /** 创建时间
    */
    RCRarTime m_cTime;

    /** 访问时间
    */
    RCRarTime m_aTime;

    /** 更改时间
    */
    RCRarTime m_mTime;

    /** crc
    */
    uint32_t m_fileCRC;

    /** 属性
    */
    uint32_t m_attrib;

    /** 风格
    */
    uint16_t m_flags;

    /** 平台系统
    */
    byte_t m_hostOS;

    /** 解压版本
    */
    byte_t m_unPackVersion;

    /** 方法
    */
    byte_t m_method;

    /** 是否有创建时间
    */
    bool m_cTimeDefined;

    /** 是否有访问时间
    */
    bool m_aTimeDefined;

    /** 名字
    */
    RCStringA m_name;

    /** unicode名字
    */
    RCString m_unicodeName;

    /** salt(加密用的)
    */
    byte_t m_salt[8];

    /** 是否加密
    @return 是返回true，否咋返回false
    */
    bool IsEncrypted() const { return (m_flags & RCRarHeader::NFile::kEncrypted) != 0; }

    /** 是否是固实压缩
    @return 是返回true，否咋返回false
    */
    bool IsSolid() const { return (m_flags & RCRarHeader::NFile::kSolid) != 0; }

    /** 是否有注释
    @return 是返回true，否咋返回false
    */
    bool IsCommented() const { return (m_flags & RCRarHeader::NFile::kComment) != 0; }

    /** 之前是否有分卷包
    @return 是返回true，否咋返回false
    */
    bool IsSplitBefore() const { return (m_flags & RCRarHeader::NFile::kSplitBefore) != 0; }

    /** 之后是否有分卷包
    @return 是返回true，否咋返回false
    */
    bool IsSplitAfter() const { return (m_flags & RCRarHeader::NFile::kSplitAfter) != 0; }

    /** 是否有salt
    @return 有返回true，否咋返回false
    */
    bool HasSalt() const { return (m_flags & RCRarHeader::NFile::kSalt) != 0; }

    /** 是否有扩展时间
    @return 有返回true，否咋返回false
    */
    bool HasExtTime() const { return (m_flags & RCRarHeader::NFile::kExtTime) != 0; }

    /** 是否有unicode名字
    @return 有返回true，否咋返回false
    */
    bool HasUnicodeName() const { return (m_flags & RCRarHeader::NFile::kUnicodeName) != 0; }

    /** 是否是老版本
    @return 是返回true，否咋返回false
    */
    bool IsOldVersion() const { return (m_flags & RCRarHeader::NFile::kOldVersion) != 0; }

    /** 获取字典大小
    @return 返回字典大小
    */
    uint32_t GetDictSize() const { return (m_flags >> RCRarHeader::NFile::kDictBitStart) & RCRarHeader::NFile::kDictMask; }

    /** 是否是路径
    @return 是返回true，否咋返回false
    */
    bool IsDir() const;

    /** 是否是忽略项
    @return 是返回true，否咋返回false
    */
    bool IgnoreItem() const;

    /** 获取windows下的属性
    @return 返回属性
    */
    uint32_t GetWinAttributes() const;

    /** 默认构造函数
    */
    RCRarItem(): 
      m_cTimeDefined(false), 
      m_aTimeDefined(false) {}
};

class RCRarItemEx: public RCRarItem
{
public:

    /** 获取整个大小
    @return 返回整个大小
    */
    uint64_t GetFullSize() const { return m_mainPartSize + m_commentSize + m_alignSize + m_packSize; };

    /** 获取注释位置
    @return 返回注释位置
    */
    uint64_t GetCommentPosition() const { return m_position + m_mainPartSize; };

    /** 获取数据位置
    @return 返回数据位置
    */
    uint64_t GetDataPosition() const { return GetCommentPosition() + m_commentSize + m_alignSize; };

public:

    /** 位置
    */
    uint64_t m_position;

    /** 主断大小
    */
    uint16_t m_mainPartSize;

    /** 注释大小
    */
    uint16_t m_commentSize;

    /** 对齐大小
    */
    uint16_t m_alignSize;
};

/** RCRarItemEx智能指针
*/
typedef RCSharedPtr<RCRarItemEx>  RCRarItemExPtr ;

END_NAMESPACE_RCZIP

#endif //__RCRarItem_h_
