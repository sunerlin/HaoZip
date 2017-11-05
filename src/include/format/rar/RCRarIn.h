/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCRarIn_h_
#define __RCRarIn_h_ 1

#include "RCRarHeader.h"
#include "RCRarItem.h"
#include "interface/IArchive.h"
#include "interface/IStream.h"
#include "interface/IPassword.h"
#include "common/RCBuffer.h"
#include "crypto/RarAES/RCRarAESDecoder.h"

BEGIN_NAMESPACE_RCZIP

class RCRarInArchiveInfo
{
public:

    /** 起始位置
    */
    uint64_t m_startPosition;

    /** 风格
    */
    uint16_t m_flags;

    /** 注释位置
    */
    uint64_t m_commentPosition;

    /** 注释大小
    */
    uint16_t m_commentSize;

    /** 注释
    */
    RCString m_comment;

public:

    /** 是否是固实压缩
    @return 是返回true，否则返回false
    */
    bool IsSolid() const
    {
        return (m_flags & RCRarHeader::NArchive::kSolid) != 0;
    }

    /** 是否有注释
    @return 有返回true，否则返回false
    */
    bool IsCommented() const
    {
        //return (Flags & RCRarHeader::NArchive::kComment) != 0;
        return !m_comment.empty();
    }

    /** 是否是分卷
    @return 是返回true，否则返回false
    */
    bool IsVolume() const
    {
        return (m_flags & RCRarHeader::NArchive::kVolume) != 0;
    }

    /** 是否是新分卷名
    @return 是返回true，否则返回false
    */
    bool HaveNewVolumeName() const
    {
        return (m_flags & RCRarHeader::NArchive::kNewVolName) != 0;
    }

    /** 是否是加密
    @return 是返回true，否则返回false
    */
    bool IsEncrypted() const
    {
        return (m_flags & RCRarHeader::NArchive::kBlockEncryption) != 0;
    }
};

class RCRarIn
{
public:

    /** 打开文档
    @param [in] stream 输入流
    @param [in] searchHeaderSizeLimit 查找文件格式签名的最大偏移位置
    @param [in] openArchiveCallback 打开文档回调接口指针
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult Open(IInStream* inStream, const uint64_t* searchHeaderSizeLimit, IArchiveOpenCallback* openArchiveCallback = 0);

    /** 关闭
    */
    void Close();

    /** 获取下一项
    @param [in] codecsInfo 编码管理器
    @param [in] item 项
    @param [in] getTextPassword 密码管理器
    @param [in] openArchiveCallback 打开文档回调接口指针
    @param [out] decryptionError 是否解密错误
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult GetNextItem(ICompressCodecsInfo* codecsInfo,
                        RCRarItemExPtr& item,
                        ICryptoGetTextPassword* getTextPassword,
                        IArchiveOpenCallback* openArchiveCallback,
                        bool& decryptionError);

    /** 跳过文档注释
    */
    void SkipArchiveComment();

    /** 获取文档信息
    @param [out] archiveInfo 文档信息  
    */
    void GetArchiveInfo(RCRarInArchiveInfo &archiveInfo) const;

    /** 在文档中定位
    @param [in] position 位置
    @return 成功返回true，否则返回false
    */
    bool SeekInArchive(uint64_t position);

    /** 创建最大输出流
    @param [in] position 位置
    @param [in] size 大小
    @return 输出流
    */
    ISequentialInStream* CreateLimitedStream(uint64_t position, uint64_t size);

protected:

    /** 读一个字节
    @return 返回一个字节
    */
    byte_t ReadByte();

    /** 读两个字节转成16位无符号整型
    @return 返回16位无符号整型
    */
    uint16_t ReadUInt16();

    /** 读四个字节转成32位无符号整型
    @return 返回32位无符号整型
    */
    uint32_t ReadUInt32();

    /** 读时间
    @param [in] mask 掩码
    @param [out] rarTime 返回时间
    */
    void ReadTime(byte_t mask, RCRarTime& rarTime);

    /** 完成加密块
    */
    void FinishCryptoBlock()
    {
        if (m_cryptoMode)
        {
            while ((m_cryptoPos & 0xF) != 0)
            {
                m_cryptoPos++;
                m_position++;
            }
        }
    }

protected:

    /** 文件头部数据
    */
    RCByteBuffer m_fileHeaderData;

    /** 文件头部块
    */
    RCRarHeader::NBlock::CBlock m_blockHeader;

    /** rarAES解码器
    */
    RCRarAESDecoder* m_rarAESSpec;

    /** rarAES解码器
    */
    ICompressFilterPtr m_rarAES;

    /** 当前数据(it must point to start of Rar::Block)
    */
    byte_t* m_curData;

    /** 当前位置
    */
    uint32_t m_curPos;

    /** 最大位置
    */
    uint32_t m_posLimit;

    /** 解密数据
    */
    RCBuffer<byte_t> m_decryptedData;

    /** 解密数据大小
    */
    uint32_t m_decryptedDataSize;

    /** 是否是加密模式
    */
    bool m_cryptoMode;

    /** 解密位置
    */
    uint32_t m_cryptoPos;

private:

    /** 读名字
    @param [in] item 项信息
    @param [in] nameSize 名字大小
    */
    void ReadName(RCRarItemExPtr& item, int nameSize);

    /** 读真实头
    @param [in] item 项信息
    */
    void ReadHeaderReal(RCRarItemExPtr& item);

    /** 读字节
    @param [out] data 返回读取的内容
    @param [in] size 读取的大小
    @param [in] aProcessedSize 返回实际读到的大小
    */
    HResult ReadBytes(void* data, uint32_t size, uint32_t* aProcessedSize);

    /** 读字节并检查是否读到指定大小
    @param [out] data 返回实际读到的内容
    @param [in] size 读取的大小
    @return 成功返回true，否则返回false
    */
    bool ReadBytesAndTestSize(void* data, uint32_t size);

    /** 读字节并检查是否正确
    @param [out] data 返回实际读到的内容
    @param [in] size 读取的大小
    */
    void ReadBytesAndTestResult(void* data, uint32_t size);

    /** 查找并读取标志
    @param [in] stream 输入流
    @param [in] searchHeaderSizeLimit 最大查找范围
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult FindAndReadMarker(IInStream* stream, const uint64_t* searchHeaderSizeLimit);

    /** 打开文档
    @param [in] stream 输入流
    @param [in] searchHeaderSizeLimit 查找文件格式签名的最大偏移位置
    @param [in] openArchiveCallback 打开文档回调接口指针
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult Open2(IInStream* stream, const uint64_t* searchHeaderSizeLimit, IArchiveOpenCallback* openArchiveCallback);

    /** 添加定位值
    @param [in] addValue 添加的值
    */
    void AddToSeekValue(uint64_t addValue);

private:

    /** 输入流
    */
    IInStreamPtr m_stream;

    /** 输入流起始位置
    */
    uint64_t m_streamStartPosition;

    /** 位置
    */
    uint64_t m_position;

    /** 文档流起始位置
    */
    uint64_t m_archiveStartPosition;

    /** 文档头
    */
    RCRarHeader::NArchive::CHeader360 m_archiveHeader;

    /** 名字
    */
    RCCharBuffer m_nameBuffer;

    /** unicode名字
    */
    RCBuffer<char_t> m_unicodeNameBuffer;

    /** 是否定位文档中的注释
    */
    bool m_seekOnArchiveComment;

    /** 文档中的注释位置
    */
    uint64_t m_archiveCommentPosition;
};

END_NAMESPACE_RCZIP

#endif //__RCRarIn_h_
