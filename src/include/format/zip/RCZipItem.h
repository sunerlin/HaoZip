/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCZipItem_h_
#define __RCZipItem_h_ 1

#include "base/RCDefs.h"
#include "base/RCString.h"
#include "base/RCSmartPtr.h"
#include "common/RCBuffer.h"
#include "common/RCVector.h"
#include "common/RCStringConvert.h"
#include "filesystem/RCFileDefs.h"
#include "RCZipHeader.h"

BEGIN_NAMESPACE_RCZIP

struct RCZipVersion
{
    /** 版本
    */
    byte_t m_version ;

    /** 操作系统
    */
    byte_t m_hostOS ;
};

bool operator==(const RCZipVersion& v1, const RCZipVersion& v2) ;

bool operator!=(const RCZipVersion& v1, const RCZipVersion& v2) ;

struct RCZipExtraSubBlock
{
    /** ID
    */
    uint16_t m_id ;

    /** DATA
    */
    RCByteBuffer m_data ;

    /** 解压NTFS时间
    */
    bool ExtractNtfsTime(int index, RC_FILE_TIME& ft) const ;

    /** 解压UNIX时间
    */
    bool ExtractUnixTime(int index, uint32_t& res) const ;
};

struct RCZipWzAESExtraField
{
    /** 版本
    */
    uint16_t m_vendorVersion ; // 0x0001 - AE-1, 0x0002 - AE-2,

    /** 强度
    */
    byte_t m_strength ;        // 1 - 128-bit , 2 - 192-bit , 3 - 256-bit

    /** 模式
    */
    uint16_t m_method ;

    /** 默认构造函数
    */
    RCZipWzAESExtraField(): m_vendorVersion(2), m_strength(3), m_method(0) {}

    /** 是否需要CRC
    @return 需要返回true，否则返回false
    */
    bool NeedCrc() const 
    { 
        return (m_vendorVersion == 1);
    }

    /** 从子块解析数据
    @param [in] sb 子块
    @return 成功返回true，否则返回false
    */
    bool ParseFromSubBlock(const RCZipExtraSubBlock& sb)
    {
        if (sb.m_id != RCZipHeader::NExtraID::kWzAES)
        {
            return false;
        }

        if (sb.m_data.GetCapacity() < 7)
        {
            return false;
        }

        const byte_t *p = sb.m_data.data() ;
        m_vendorVersion = (((uint16_t)p[1]) << 8) | p[0];
        if (p[2] != 'A' || p[3] != 'E')
        {
            return false;
        }

        m_strength = p[4];
        m_method = (((uint16_t)p[6]) << 16) | p[5];

        return true;
    }

    /** 设置子快
    @param [out] sb 返回子块信息
    */
    void SetSubBlock(RCZipExtraSubBlock& sb) const
    {
        sb.m_data.SetCapacity(7) ;
        sb.m_id = RCZipHeader::NExtraID::kWzAES;
        byte_t *p = sb.m_data.data() ;
        p[0] = (byte_t)m_vendorVersion;
        p[1] = (byte_t)(m_vendorVersion >> 8) ;
        p[2] = 'A';
        p[3] = 'E';
        p[4] = m_strength;
        p[5] = (byte_t)m_method;
        p[6] = (byte_t)(m_method >> 8) ;
    }
};

namespace RCZipStrongCryptoFlags
{
    const uint16_t kDES      = 0x6601 ;

    const uint16_t kRC2old   = 0x6602 ;

    const uint16_t k3DES168  = 0x6603 ;

    const uint16_t k3DES112  = 0x6609 ;

    const uint16_t kAES128   = 0x660E ;

    const uint16_t kAES192   = 0x660F;

    const uint16_t kAES256   = 0x6610 ;

    const uint16_t kRC2      = 0x6702 ;

    const uint16_t kBlowfish = 0x6720 ;

    const uint16_t kTwofish  = 0x6721 ;

    const uint16_t kRC4      = 0x6801 ;
}

struct RCZipStrongCryptoField
{
    /** 格式
    */
    uint16_t m_format ;

    /** ALG
    */
    uint16_t m_algId ;

    /** 长度
    */
    uint16_t m_bitLen ;

    /** 标志
    */
    uint16_t m_flags;

    /** 从子块解析数据
    @param [in] sb 子块
    @return 成功返回true，否则返回false
    */
    bool ParseFromSubBlock(const RCZipExtraSubBlock& sb)
    {
        if (sb.m_id != RCZipHeader::NExtraID::kStrongEncrypt)
        {
            return false;
        }

        const byte_t* p = sb.m_data.data();
        if (sb.m_data.GetCapacity() < 8)
        {
            return false;
        }

        m_format = (((uint16_t)p[1]) << 8) | p[0];
        m_algId  = (((uint16_t)p[3]) << 8) | p[2];
        m_bitLen = (((uint16_t)p[5]) << 8) | p[4];
        m_flags  = (((uint16_t)p[7]) << 8) | p[6];

        return (m_format == 2) ;
    }
};

struct RCZipExtraBlock
{
    /** 子快
    */
    RCVector<RCZipExtraSubBlock> m_subBlocks ;

    /** 清除
    */
    void Clear() 
    { 
        m_subBlocks.clear();
    }

    /** 获取大小
    */
    size_t GetSize() const
    {
        size_t res = 0;
        for (uint32_t i = 0; i < (uint32_t)m_subBlocks.size(); i++)
        {
            res += m_subBlocks[i].m_data.GetCapacity() + 2 + 2;
        }
        return res;
    }

    /** 获取WzAES信息
    @param [out] aesField 返回WzAES信息
    @return 成功返回true，否则返回false
    */
    bool GetWzAESField(RCZipWzAESExtraField &aesField) const
    {
        for (uint32_t i = 0; i < (uint32_t)m_subBlocks.size(); i++)
        {
            if (aesField.ParseFromSubBlock(m_subBlocks[i]))
            {
                return true;
            }
        }
        return false;
    }

    /** 获取加密信息
    @param [out] f 返回加密信息
    @return 成功返回true，否则返回false
    */
    bool GetStrongCryptoField(RCZipStrongCryptoField &f) const
    {
        for (uint32_t i = 0; i < (uint32_t)m_subBlocks.size(); i++)
        {
            if (f.ParseFromSubBlock(m_subBlocks[i]))
            {
                return true;
            }
        }
        return false;
    }

    /** 是否有WzAES信息
    @return 有返回true，否则返回false
    */
    bool HasWzAesField() const
    {
        RCZipWzAESExtraField aesField;

        return GetWzAESField(aesField);
    }

    /** 获取ntfs时间
    @param [in] index 序号
    @param [in] ft 时间信息
    @return 成功返回true，否则返回false
    */
    bool GetNtfsTime(int32_t index, RC_FILE_TIME &ft) const
    {
        for (uint32_t i = 0; i < (uint32_t)m_subBlocks.size(); i++)
        {
            const RCZipExtraSubBlock &sb = m_subBlocks[i];

            if (sb.m_id == RCZipHeader::NExtraID::kNTFS)
            {
                return sb.ExtractNtfsTime(index, ft);
            }
        }
        return false;
    }

    /** 获取unix时间
    @param [in] index 序号
    @param [in] res 时间信息
    @return 成功返回true，否则返回false
    */ 
    bool GetUnixTime(int32_t index, uint32_t& res) const
    {
        for (int32_t i = 0; i < (int32_t)m_subBlocks.size(); i++)
        {
            const RCZipExtraSubBlock& sb = m_subBlocks[i];
            if (sb.m_id == RCZipHeader::NExtraID::kUnixTime)
            {
                return sb.ExtractUnixTime(index, res);
            }
        }
        return false;
    }

    /** 移除未知子块
    */ 
    void RemoveUnknownSubBlocks()
    {
        for (int32_t i = (int32_t)m_subBlocks.size() - 1; i >= 0; i--)
        { 
            if (m_subBlocks[i].m_id != RCZipHeader::NExtraID::kWzAES)
            {
                RCVectorUtils::Delete(m_subBlocks, i);
            }
        }
    }
};

class RCZipLocalItem
{
public:

    /** 解压版本
    */
    RCZipVersion m_extractVersion ;

    /** 标志
    */
    uint16_t m_flags ;

    /** 压缩模式
    */
    uint16_t m_compressionMethod ;

    /** 时间
    */
    uint32_t m_time ;

    /** 文件crc
    */
    uint32_t m_fileCRC ;

    /** 压缩大小
    */
    uint64_t m_packSize ;

    /** 解压大小
    */
    uint64_t m_unPackSize ;

    /** 文件名
    */
    RCStringA m_name ;

    /** 本地扩展信息
    */
    RCZipExtraBlock m_localExtra ;

public:

    /** 是否是UTF8编码
    @return 是返回true，否则返回false
    */
    bool IsUtf8() const
    {
        return (m_flags & RCZipHeader::NFlags::kUtf8) != 0; 
    }

    /** 是否是加密
    @return 是返回true，否则返回false
    */
    bool IsEncrypted() const
    { 
        return (m_flags & RCZipHeader::NFlags::kEncrypted) != 0; 
    }

    /** 是否是Strong加密
    @return 是返回true，否则返回false
    */
    bool IsStrongEncrypted() const
    { 
        return IsEncrypted() && (m_flags & RCZipHeader::NFlags::kStrongEncrypted) != 0;
    }

    /** 是否是acs加密
    @return 是返回true，否则返回false
    */  
    bool IsAesEncrypted() const 
    {
        return IsEncrypted() && (IsStrongEncrypted() || m_compressionMethod == RCZipHeader::NCompressionMethod::kWzAES); 
    }
 
    /** 是否是LzmaEOS
    @return 是返回true，否则返回false
    */ 
    bool IsLzmaEOS() const
    { 
        return (m_flags & RCZipHeader::NFlags::kLzmaEOS) != 0;
    }

    /** 是否是目录
    @return 是返回true，否则返回false
    */
    bool IsDir() const;

    /** 是否是忽略项
    @return 是返回true，否则返回false
    */
    bool IgnoreItem() const
    { 
        return false; 
    }

    /** 获取windows下的属性
    @return 属性
    */
    uint32_t GetWinAttributes() const ;

    /** 是否有描述
    @return 是返回true，否则返回false
    */
    bool HasDescriptor() const
    { 
        return (m_flags & RCZipHeader::NFlags::kDescriptorUsedMask) != 0;
    }

    /** 获取unicode字符串
    @return unicode字符串
    */
    RCString GetUnicodeString(const RCStringA &s) const
    {
        RCString res;
        if (IsUtf8())
        {    
            if (!RCStringConvert::ConvertUTF8ToUnicode(s.c_str(), s.length(), res))
            {
                res.clear();
            }
        }

        if (res.empty())
        {
            res = RCStringConvert::MultiByteToUnicodeString(s, GetCodePage());
        }

        return res;
    }

public:

    /** 清理标志
    */
    void ClearFlags() 
    {
        m_flags = 0;
    }

    /** 设置是否加密
    */
    void SetEncrypted(bool encrypted) ;

    /** 设置UTF8
    */
    void SetUtf8(bool isUtf8) ;

    /** 获取字符编码
    @return 返回字符编码
    */
    uint16_t GetCodePage() const
    { 
        return  RC_CP_OEMCP;
    }
  
private:

    /** 设置标志
    @param [in] startBitNumber
    @param [in] numBits
    @param [in] value
    */
    void SetFlagBits(int startBitNumber, int numBits, int value) ;

    /** 设置bit mask
    @param [in] bitMask
    @param [in] enable
    */
    void SetBitMask(int bitMask, bool enable) ;
};

class RCZipItem:
    public RCZipLocalItem
{
public:

    /** 版本
    */
    RCZipVersion m_madeByVersion ;

    /** 内部属性
    */
    uint16_t m_internalAttributes ;

    /** 外部属性
    */
    uint32_t m_externalAttributes ;

    /** 磁盘起始号
    */
    uint16_t m_diskNumberStart;

    /** 本地头位置
    */
    uint64_t m_localHeaderPosition ;

    /** ntfs修改时间
    */
    RC_FILE_TIME m_ntfsMTime ;

    /** ntfs访问时间
    */
    RC_FILE_TIME m_ntfsATime ;

    /** ntfs创建时间
    */
    RC_FILE_TIME m_ntfsCTime ;

    /** 扩展快信息
    */
    RCZipExtraBlock m_centralExtra ;

    /** 注释
    */
    RCByteBuffer m_comment ;

    /** from local
    */
    bool m_fromLocal ;

    /** from central
    */
    bool m_fromCentral ;

    /** 是否定义ntfs时间
    */
    bool m_ntfsTimeIsDefined ;

public:

    /** 是否是目录
    @return 是目录返回true，否则返回false
    */
    bool IsDir() const ;

    /** 得到windows下的属性
    @return 属性
    */
    uint32_t GetWinAttributes() const ;

    /** 是否有crc
    @return 是返回true，否则返回false
    */
    bool IsThereCrc() const
    {
        if (m_compressionMethod == RCZipHeader::NCompressionMethod::kWzAES)
        {
            RCZipWzAESExtraField aesField;
            if (m_centralExtra.GetWzAESField(aesField))
            {
                return aesField.NeedCrc() ;
            }
        }
        return (m_fileCRC != 0 || !IsDir());
    }

    /** 获取字符编码
    @return 返回字符编码
    */
    uint16_t GetCodePage() const
    {
        return (uint16_t)((m_madeByVersion.m_hostOS == RCZipHeader::NHostOS::kFAT
                          || m_madeByVersion.m_hostOS == RCZipHeader::NHostOS::kNTFS) ? 
                          RC_CP_OEMCP : RC_CP_ACP);
    }

    /** 默认构造函数
    */
    RCZipItem() : m_fromLocal(false), m_fromCentral(false), m_ntfsTimeIsDefined(false) {}
};

class RCZipItemEx:
    public RCZipItem
{
public:

    /** 文件头与名字的大小
    */
    uint32_t m_fileHeaderWithNameSize ;

    /** 本地扩展信息
    */
    uint16_t m_localExtraSize ;

public:

    /** 获取本地所有大小
    @return 本地所有大小
    */
    uint64_t GetLocalFullSize() const
    { 
        return m_fileHeaderWithNameSize + m_localExtraSize + m_packSize +
            (HasDescriptor() ? RCZipHeader::kDataDescriptorSize : 0); 
    };

    /** 本地扩展信息的位置
    @return  扩展信息的位置
    */
    uint64_t GetLocalExtraPosition() const
    { 
        return m_localHeaderPosition + m_fileHeaderWithNameSize; 
    };

    /** 获取本地数据位置
    @return数据位置
    */
    uint64_t GetDataPosition() const
    { 
        return GetLocalExtraPosition() + m_localExtraSize; 
    };
};

/** RCZipItem智能指针
*/
typedef RCSharedPtr<RCZipItem>  RCZipItemPtr ;

/** RCZipItemEx智能指针
*/
typedef RCSharedPtr<RCZipItemEx>  RCZipItemExPtr ;

END_NAMESPACE_RCZIP

#endif //__RCZipItem_h_
