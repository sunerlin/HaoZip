/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCRpmHeader_h_
#define __RCRpmHeader_h_ 1

#include "base/RCTypes.h"
#include "interface/IStream.h"

BEGIN_NAMESPACE_RCZIP

class RCRpmDefs
{
public:

    /** 引导大小
    */
    static const uint32_t s_leadSize = 96;

    /** 入口信息大小
    */
    static const uint32_t s_entryInfoSize = 16;

    /** 头部大小
    */
    static const uint32_t s_csigHeaderSigSize = 16;

    /** 字符拷贝
    @param [out] dest 返回拷贝的字符
    @param [in] src 源字符
    @return 返回拷贝的字符
    */
    template <class T>
    static inline T * MyStringCopy(T *dest, const T *src)
    {
        T *destStart = dest;
        while ((*dest++ = *src++) != 0);
        return destStart;
    }

    /** 字符长度
    @param [in] s 字符
    @return 返回字符长度
    */
    template <class T>
    static inline int32_t MyStringLen(const T *s)
    {
        int32_t i;
        for (i = 0; s[i] != _T('\0'); i++);
        return i;
    }
};

struct RCRpmLead
{
    /** 标识
    */
    unsigned char m_magic[4];

    /** 主版本
        not supported  ver1, only support 2,3 and lator
    */
    unsigned char m_major;

    /** 子版本
    */
    unsigned char m_minor;

    /** 类型
    */
    uint16_t m_type;

    /** 文档个数
    */
    uint16_t m_archNum;

    /** 名字
    */
    char m_name[66];

    /** 操作系统类型
    */
    uint16_t m_osNum;

    /** 签名类型
    */
    uint16_t m_signatureType;

    /** 保留项
        pad to 96 bytes -- 8 byte aligned
    */
    char m_reserved[16];

    /** 检查标识
    @return 通过检查返回true，否则返回false
    */
    bool MagicCheck() const;
};

struct RCRpmSigHeaderSig
{
    /** 标识
    */
    unsigned char m_magic[4];

    /** 保留项
    */
    uint32_t m_reserved;

    /** 索引长
        count of index entries
    */
    uint32_t m_indexLen;

    /** 数据长
        number of bytes
    */
    uint32_t m_dataLen;

    /** 检查标识
    @return 通过检查返回true，否则返回false
    */
    bool MagicCheck();

    /** 获取丢失头部长
    @return 返回丢失头部长
    */
    uint32_t GetLostHeaderLen();
};

struct RCRpmFuncWrapper
{
    /** 获取头部数据
    @param [in] inStream 输入流
    @param [out] h 返回头部数据
    @return 成功返回RC_S_OK，否则返回错误号
    */
    static HResult ReadSigHeaderSig(IInStream* inStream, RCRpmSigHeaderSig& h);

    /** 打开文档
    @param [in] inStream 输入流
    @return 成功返回RC_S_OK，否则返回错误号
    */
    static HResult OpenArchive(IInStream* inStream);
};

END_NAMESPACE_RCZIP

#endif //__RCRpmHeader_h_
