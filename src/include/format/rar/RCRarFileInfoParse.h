/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCRarFileInfoParse_h_
#define __RCRarFileInfoParse_h_ 1

#include "interface/IArchive.h"
#include "interface/ICoder.h"
#include "interface/IUnknownImpl.h"
#include "RCRarVolumeInStream.h"
#include "common/RCSystemUtils.h"

BEGIN_NAMESPACE_RCZIP

const unsigned int SALT_SIZE              = 8;
const unsigned int SIZEOF_MARKHEAD        = 7;
const unsigned int SIZEOF_OLDMHD          = 7;
const unsigned int SIZEOF_NEWMHD          = 13;
const unsigned int SIZEOF_OLDLHD          = 21;
const unsigned int SIZEOF_NEWLHD          = 32;
const unsigned int SIZEOF_SHORTBLOCKHEAD  = 7;
const unsigned int SIZEOF_LONGBLOCKHEAD   = 11;
const unsigned int SIZEOF_SUBBLOCKHEAD    = 14;
const unsigned int SIZEOF_COMMHEAD        = 13;
const unsigned int SIZEOF_PROTECTHEAD     = 26;
const unsigned int SIZEOF_AVHEAD          = 14;
const unsigned int SIZEOF_SIGNHEAD        = 15;
const unsigned int SIZEOF_UOHEAD          = 18;
const unsigned int SIZEOF_MACHEAD         = 22;
const unsigned int SIZEOF_EAHEAD          = 24;
const unsigned int SIZEOF_BEEAHEAD        = 24;
const unsigned int SIZEOF_STREAMHEAD      = 26;

enum 
{
    MARK_HEAD=0x72,MAIN_HEAD=0x73,FILE_HEAD=0x74,COMM_HEAD=0x75,AV_HEAD=0x76,
    SUB_HEAD=0x77,PROTECT_HEAD=0x78,SIGN_HEAD=0x79,NEWSUB_HEAD=0x7a,
    ENDARC_HEAD=0x7b
};

/** RAR文件信息分析器，输入流的头部地址即为RAR文件地址，不在文件内部搜索RAR标记头。
*/
class RCRarFileInfoParse
{
public:

    /** 构造函数
    @param [in] stream 输入流
    */
    RCRarFileInfoParse(IInStream* stream) : m_stream(0)
    {
        if (!stream)
        {
            return;
        }

        uint64_t SFXSize = 0;
        if (stream->Seek(0, RC_STREAM_SEEK_CUR, &SFXSize) != RC_S_OK)
        {
            return;
        }

        m_sfxSize = SFXSize;

        m_stream = stream;
    }

    /** 默认析构函数
    */
    ~RCRarFileInfoParse()
    {
        if (!m_stream)
        {
            return;
        }

        uint64_t position = 0;
        if (m_stream->Seek(m_sfxSize, RC_STREAM_SEEK_SET, &position) != RC_S_OK)
        {
            return;
        }
    }

    /** 头部信息
    */
    class RCMarkHeader
    {
    public:

        /** 头部信息
        */
        unsigned char Data[SIZEOF_MARKHEAD];

        /** 文件标签是否正确
        @return 是返回true，否则返回false
        */
        bool IsSignature();

        /** 是否是旧版本
        @return 是返回true，否则返回false
        */
        bool IsOldFormat();

    }m_markHeader;

    /** 块头部信息
    */
    struct RCBlockHeader
    {
    public:

        /** 头部信息
        */
        unsigned char Data[SIZEOF_SHORTBLOCKHEAD];

        /** 是否是子块
        @return 是返回true，否则返回false
        */
        bool IsSubBlock();

        /** 块CRC
        @return 返回crc
        */
        unsigned short HeadCRC();

        /** 块类型
        @return 返回类型
        */
        unsigned char HeadType();

        /** 块风格
        @return 返回风格
        */
        unsigned short Flags();

        /** 设置快风格
        @param [in] flags 风格
        */
        void SetFlags(unsigned short flags);

        /** 块头部大小
        @return 返回大小
        */
        unsigned short HeadSize();

        /** 设置头部大小
        @param [in] size 大小
        */
        void SetHeadSize(unsigned short size);

        /** 是否子块
        @return 是返回true，否则返回false
        */
        bool SubBlock();
    };

    /** Main块头部信息
    */
    struct RCMainHeader : public RCBlockHeader
    {
    public:

        /** 头部信息
        @param [in] data 数据
        @param [in] size 大小
        @return 成功返回true，否则返回false
        */
        bool ReadFrom(unsigned char* data, uint64_t size) ;

        /** 是否加密
        @return 是返回true，否则返回false
        */
        bool IsEncrypted();

        /** 是否分卷
        @return 是返回true，否则返回false
        */
        bool IsVolume();

        /** 是否是第一个卷
        @return 是返回true，否则返回false
        */
        bool IsFirstVolume();

        /** 是否固始
        @return 是返回true，否则返回false
        */
        bool IsSolid();

        /** 是否在Main块保存注释
        @return 是返回true，否则返回false
        */
        bool IsMainComment();

        /** 是否锁定
        @return 是返回true，否则返回false
        */
        bool IsLocked();

        /** 是否标记
        @return 是返回true，否则返回false
        */
        bool IsSigned();

        /** 是否保护
        @return 是返回true，否则返回false
        */
        bool IsProtected();

        /** 加密版本
        @return 是返回true，否则返回false
        */
        unsigned short m_encryptVer;

        /** posAV的高位
        */
        unsigned short m_highPosAV;

        /** posAV
        */
        unsigned short m_posAV;

    }m_mainHeader;

    /** End块头部信息
    */
    struct RCEndHeader : public RCBlockHeader
    {
    public:

        /** 头部信息
        @param [in] data 数据
        @param [in] size 大小
        @return 成功返回true，否则返回false
        */
        bool ReadFrom(unsigned char* data, uint64_t size) ;

        /** 是否有下一个分卷
        @return 是返回true，否则返回false
        */
        bool IsLastVolume();

        /** 压缩包CRC32校验
        */
        uint32_t m_arcDataCRC;

        /** 当前分卷序号
        */
        unsigned short m_volNumber;

    }m_endHeader;

    /** Protect块头部信息
    */
    struct RCProtectHeader
    {
    public:

        /** 头部信息
        @param [in] data 数据
        @param [in] size 大小
        @return 成功返回true，否则返回false
        */
        bool ReadFrom(unsigned char* data, uint64_t size) ;

        /** 数据大小
        */
        uint32_t m_dataSize;

        /** 其他
        unsigned char m_version;
        unsigned short m_recSectors;
        uint32_t m_totalBlocks;
        unsigned char m_mark[8];
        */
    };


    /** Sub块头部信息
    */
    struct RCSubHeader
    {
    public:

        /** 头部信息
        @param [in] data 数据
        @param [in] size 大小
        @return 成功返回true，否则返回false
        */
        bool ReadFrom(unsigned char* data, uint64_t size) ;

        /** 数据大小
        */
        uint32_t m_dataSize;

        /** 其他
        unsigned short m_subType;
        unsigned char m_level;
        */
    };

    /** File块头部信息
    */
    struct RCFileHeader : public RCBlockHeader
    {
    public:

        /** 头部信息
        @param [in] data 数据
        @param [in] size 大小
        @return 成功返回true，否则返回false
        */
        bool ReadFrom(unsigned char* data, uint64_t size) ;

        /** 包大小(低位)
        */
        uint32_t m_packSize;

        /** 解压后大小(低位)
        */
        uint32_t m_unpSize;

        /** 平台类型
        */
        unsigned char HostOS;

        /** crc
        */
        uint32_t m_fileCRC;

        /** 文件时间
        */
        uint32_t m_fileTime;

        /** 解压版本
        */
        unsigned char m_unpVer;

        /** 方法
        */
        unsigned char m_method;

        /** 名字大小
        */
        unsigned short m_nameSize;

        union 
        {
            /** 文件属性
            */
            uint32_t m_fileAttr;

            /** 子风格
            */
            uint32_t m_subFlags;
        };

        /** 包大小(高位)
        */
        uint32_t m_highPackSize;

        /** 解压后大小(高位)
        */
        uint32_t m_highUnpSize;
    
        /** 其他
        char m_fileName[NM];
        wchar m_fileNameW[NM];
        Array<byte> m_subData;
        byte m_salt[SALT_SIZE];
        RarTime m_mTime;
        RarTime m_cTime;
        RarTime m_aTime;
        RarTime m_arcTime;
        */
    };

    /** 从文件流读取文件信息
    @return 成功返回true，否则返回false
    */
    bool ReadInfoFromStream();

private:

    /** 输入流
    */
    IInStream* m_stream;

    /** 输入流的起始地址
    */
    uint64_t m_sfxSize;

    /** 读取块头部
    @param [out] BlockType 块类型
    @param [out] CurBlockPos 当前块头部的位置
    @param [out] NextBlockPos 下一个块头部的位置
    @return 返回块头部大小
    */
    uint64_t ReadHeader(unsigned char& BlockType, uint64_t &CurBlockPos, uint64_t& NextBlockPos);

    /** 读取老的头部
    @param [out] BlockType 块类型
    @param [out] CurBlockPos 当前块头部的位置
    @param [out] NextBlockPos 下一个块头部的位置
    @return 返回块头部大小
    */
    uint64_t ReadOldHeader(unsigned char& BlockType, uint64_t CurBlockPos, uint64_t& NextBlockPos);

    /** 读取新的头部
    @param [out] BlockType 块类型
    @param [out] CurBlockPos 当前块头部的位置
    @param [out] NextBlockPos 下一个块头部的位置
    @return 返回块头部大小
    */
    uint64_t ReadNewHeader(unsigned char& BlockType, uint64_t CurBlockPos, uint64_t& NextBlockPos);

    /** 搜索块
    @param [in] BlockType 块类型
    @return 返回位置
    */
    uint64_t SearchBlock(unsigned char BlockType);

    /** 获取当前位置
    @return 返回当前位置
    */
    uint64_t Tell();

    /** 读取数据
    @param [in] data 地址
    @param [in] size 大小
    @return 成功返回true，否则返回false
    */
    bool Read(void* data, size_t size);
};

END_NAMESPACE_RCZIP

#endif //RCRarFileInfoParse_h_
