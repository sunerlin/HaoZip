/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCRarHeader_h_
#define __RCRarHeader_h_ 1

#include "base/RCDefs.h"

BEGIN_NAMESPACE_RCZIP

namespace RCRarHeader
{

    const int32_t kMarkerSize = 7;
    extern byte_t kMarker[kMarkerSize];

    const int32_t kArchiveSolid = 0x1;

    namespace NBlockType
    {
        enum EBlockType
        {
            kMarker        = 0x72,
            kArchiveHeader = 0x73,
            kFileHeader    = 0x74,
            kCommentHeader = 0x75,
            kOldAuthenticity = 0x76,
            kSubBlock       = 0x77,
            kRecoveryRecord = 0x78,
            kAuthenticity   = 0x79,

            kEndOfArchive   = 0x7B // Is not safe
        };
    }

    namespace NArchive
    {
        const uint16_t kVolume  = 1;
        const uint16_t kComment = 2;
        const uint16_t kLock    = 4;
        const uint16_t kSolid   = 8;
        const uint16_t kNewVolName = 0x10; // ('volname.partN.rar')
        const uint16_t kAuthenticity  = 0x20;
        const uint16_t kRecovery = 0x40;
        const uint16_t kBlockEncryption  = 0x80;
        const uint16_t kFirstVolume = 0x100; // (set only by RAR 3.0 and later)
        const uint16_t kEncryptVer = 0x200; // RAR 3.6 there is EncryptVer byte_t in End of MainHeader

        const int32_t kHeaderSizeMin = 7;

        /** 块信息
        */
        struct CBlock
        {
            /** crc
            */
            uint16_t m_crc;

            /** 类型
            */
            byte_t m_type;

            /** 风格
            */
            uint16_t m_flags;

            /** 大小
            */
            uint16_t m_size;

            /** 保留一
            */
            uint16_t m_reserved1;

            /** 保留二
            */
            uint32_t m_reserved2;

            // uint16_t GetRealCRC() const;
        };

        const int32_t kArchiveHeaderSize = 13;

        const int32_t kBlockHeadersAreEncrypted = 0x80;

        struct CHeader360: public CBlock
        {
            /** 加密版本
            */
            byte_t m_encryptVersion;

            /** 是否是加密
            @return 是返回true，否则返回false
            */
            bool IsEncrypted() const { return (m_flags & RCRarHeader::NArchive::kBlockEncryption) != 0; }

            /** 是否本地加密版本
            @return 是返回true，否则返回false
            */
            bool IsThereEncryptVer() const { return (m_flags & RCRarHeader::NArchive::kEncryptVer) != 0; }

            /** 是否是老的加密版本
            @return 是返回true，否则返回false
            */
            bool IsEncryptOld() const { return (!IsThereEncryptVer() || m_encryptVersion < 36); }

            /** 获取基本大小
            @return 返回大小
            */
            uint32_t GetBaseSize() const { return kArchiveHeaderSize + (IsEncryptOld() ? 0 : 1); }
        };
    }

    namespace NFile
    {
        const int32_t kSplitBefore = 1 << 0;
        const int32_t kSplitAfter  = 1 << 1;
        const int32_t kEncrypted   = 1 << 2;
        const int32_t kComment     = 1 << 3;
        const int32_t kSolid       = 1 << 4;

        const int32_t kDictBitStart     = 5;
        const int32_t kNumDictBits  = 3;
        const int32_t kDictMask         = (1 << kNumDictBits) - 1;
        const int32_t kDictDirectoryValue  = 0x7;

        const int32_t kSize64Bits    = 1 << 8;
        const int32_t kUnicodeName   = 1 << 9;
        const int32_t kSalt          = 1 << 10;
        const int32_t kOldVersion    = 1 << 11;
        const int32_t kExtTime       = 1 << 12;
        // const int32_t kExtFlags      = 1 << 13;
        // const int32_t kSkipIfUnknown = 1 << 14;

        const int32_t kLongBlock    = 1 << 15;

        /*
        struct CBlock
        {
        // uint16_t HeadCRC;
        // byte_t Type;
        // uint16_t Flags;
        // uint16_t HeadSize;
        uint32_t PackSize;
        uint32_t UnPackSize;
        byte_t HostOS;
        uint32_t FileCRC;
        uint32_t Time;
        byte_t UnPackVersion;
        byte_t Method;
        uint16_t NameSize;
        uint32_t Attributes;
        };
        */

        /*
        struct CBlock32
        {
        uint16_t HeadCRC;
        byte_t Type;
        uint16_t Flags;
        uint16_t HeadSize;
        uint32_t PackSize;
        uint32_t UnPackSize;
        byte_t HostOS;
        uint32_t FileCRC;
        uint32_t Time;
        byte_t UnPackVersion;
        byte_t Method;
        uint16_t NameSize;
        uint32_t Attributes;
        uint16_t GetRealCRC(const void *aName, uint32_t aNameSize,
        bool anExtraDataDefined = false, byte_t *anExtraData = 0) const;
        };
        struct CBlock64
        {
        uint16_t HeadCRC;
        byte_t Type;
        uint16_t Flags;
        uint16_t HeadSize;
        uint32_t PackSizeLow;
        uint32_t UnPackSizeLow;
        byte_t HostOS;
        uint32_t FileCRC;
        uint32_t Time;
        byte_t UnPackVersion;
        byte_t Method;
        uint16_t NameSize;
        uint32_t Attributes;
        uint32_t PackSizeHigh;
        uint32_t UnPackSizeHigh;
        uint16_t GetRealCRC(const void *aName, uint32_t aNameSize) const;
        };
        */

        const int32_t kLabelFileAttribute            = 0x08;
        const int32_t kWinFileDirectoryAttributeMask = 0x10;

        enum CHostOS
        {
            kHostMSDOS = 0,
            kHostOS2   = 1,
            kHostWin32 = 2,
            kHostUnix  = 3,
            kHostMacOS = 4,
            kHostBeOS = 5
        };
    }

    namespace NBlock
    {
        const uint16_t kLongBlock = 1 << 15;

        struct CBlock
        {
           /** crc
           */
            uint16_t m_crc;

           /** 类型
           */
            byte_t m_type;

           /** 风格
           */
            uint16_t m_flags;

           /** 头大小
           */
            uint16_t m_headSize;

            //uint32_t DataSize;
        };
    }

    /*
    struct CSubBlock
    {
    uint16_t HeadCRC;
    byte_t HeadType;
    uint16_t Flags;
    uint16_t HeadSize;
    uint32_t DataSize;
    uint16_t SubType;
    byte_t Level; // Reserved : Must be 0
    };

    struct CCommentBlock
    {
    uint16_t HeadCRC;
    byte_t HeadType;
    uint16_t Flags;
    uint16_t HeadSize;
    uint16_t UnpSize;
    byte_t UnpVer;
    byte_t Method;
    uint16_t CommCRC;
    };


    struct CProtectHeader
    {
    uint16_t HeadCRC;
    byte_t HeadType;
    uint16_t Flags;
    uint16_t HeadSize;
    uint32_t DataSize;
    byte_t Version;
    uint16_t RecSectors;
    uint32_t TotalBlocks;
    byte_t Mark[8];
    };
    */

    static const uint32_t kMaxThreadNums = 10;
};

END_NAMESPACE_RCZIP

#endif //__RCRarHeader_h_
