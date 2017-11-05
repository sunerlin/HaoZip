/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCZipHeader_h_
#define __RCZipHeader_h_ 1

#include "base/RCDefs.h"

BEGIN_NAMESPACE_RCZIP

namespace RCZipSignature
{
    extern uint32_t kLocalFileHeader;
    extern uint32_t kDataDescriptor;
    extern uint32_t kCentralFileHeader;
    extern uint32_t kEndOfCentralDir;
    extern uint32_t kZip64EndOfCentralDir;
    extern uint32_t kZip64EndOfCentralDirLocator;
    extern uint32_t kZipSingleVolumeSignature;

    static const uint32_t kMarkerSize = 4;
}

const uint32_t g_ecdSize = 22;
const uint32_t g_zip64EcdSize = 44;
const uint32_t g_zip64EcdLocatorSize = 20;

namespace RCZipHeader
{
    namespace NCompressionMethod
    {
        /** 压缩类型
        */
        enum EType
        {
            kStored = 0,
            kShrunk = 1,
            kReduced1 = 2,
            kReduced2 = 3,
            kReduced3 = 4,
            kReduced4 = 5,
            kImploded = 6,
            kReservedTokenizing = 7, // reserved for tokenizing
            kDeflated = 8,
            kDeflated64 = 9,
            kPKImploding = 10,

            kBZip2 = 12,
            kLZMA = 14,
            kTerse = 18,
            kLz77 = 19,
            kJpeg = 0x60,
            kWavPack = 0x61,
            kPPMd = 0x62,
            kWzAES = 0x63
        };
        const int32_t kNumCompressionMethods = 11;
        const byte_t kMadeByProgramVersion = 20;

        const byte_t kDeflateExtractVersion = 20;
        const byte_t kStoreExtractVersion = 10;

        const byte_t kSupportedVersion   = 20;
    }

    /** 额外ID信息
    */
    namespace NExtraID
    {
        enum
        {
            kZip64 = 0x01,
            kNTFS = 0x0A,
            kStrongEncrypt = 0x17,
            kUnixTime = 0x5455,
            kWzAES = 0x9901
        };
    }

    /** NTFS时间信息
    */
    namespace NNtfsExtra
    {
        const uint16_t kTagTime = 1;
        enum
        {
            kMTime = 0,
            kATime ,
            kCTime
        };
    }

    /** UNIX时间信息
    */
    namespace NUnixTime
    {
        enum
        {
            kMTime = 0,
            kATime,
            kCTime
        };
    }

    const uint32_t kLocalBlockSize      = 26;
    const uint32_t kDataDescriptorSize  = 16;
    const uint32_t kCentralBlockSize    = 42;

    /** 标志信息
    */
    namespace NFlags
    {
        const int32_t kEncrypted = 1 << 0;
        const int32_t kLzmaEOS = 1 << 1;
        const int32_t kDescriptorUsedMask = 1 << 3;
        const int32_t kStrongEncrypted = 1 << 6;
        const int32_t kUtf8 = 1 << 11;

        const int32_t kImplodeDictionarySizeMask = 1 << 1;
        const int32_t kImplodeLiteralsOnMask     = 1 << 2;

        const int32_t kDeflateTypeBitStart = 1;
        const int32_t kNumDeflateTypeBits = 2;
        const int32_t kNumDeflateTypes = (1 << kNumDeflateTypeBits);
        const int32_t kDeflateTypeMask = (1 << kNumDeflateTypeBits) - 1;
    }

    /** 操作系统类型
    */
    namespace NHostOS
    {
        enum EEnum
        {
            kFAT      = 0,
            kAMIGA    = 1,
            kVMS      = 2,  // VAX/VMS
            kUnix     = 3,
            kVM_CMS   = 4,
            kAtari    = 5,  // what if it's a minix filesystem? [cjh]
            kHPFS     = 6,  // filesystem used by OS/2 (and NT 3.x)
            kMac      = 7,
            kZ_System = 8,
            kCPM      = 9,
            kTOPS20   = 10, // pkzip 2.50 NTFS
            kNTFS     = 11, // filesystem used by Windows NT
            kQDOS     = 12, // SMS/QDOS
            kAcorn    = 13, // Archimedes Acorn RISC OS
            kVFAT     = 14, // filesystem used by Windows 95, NT
            kMVS      = 15,
            kBeOS     = 16, // hybrid POSIX/database filesystem
            kTandem   = 17,
            kOS400    = 18,
            kOSX      = 19
        };
    }

    /** UNIX文件属性类型
    */
    namespace NUnixAttribute
    {
        const uint32_t kIFMT   =   0170000;     /* Unix file type mask */

        const uint32_t kIFDIR  =   0040000;     /* Unix directory */
        const uint32_t kIFREG  =   0100000;     /* Unix regular file */
        const uint32_t kIFSOCK =   0140000;     /* Unix socket (BSD, not SysV or Amiga) */
        const uint32_t kIFLNK  =   0120000;     /* Unix symbolic link (not SysV, Amiga) */
        const uint32_t kIFBLK  =   0060000;     /* Unix block special       (not Amiga) */
        const uint32_t kIFCHR  =   0020000;     /* Unix character special   (not Amiga) */
        const uint32_t kIFIFO  =   0010000;     /* Unix fifo    (BCC, not MSC or Amiga) */

        const uint32_t kISUID  =   04000;       /* Unix set user id on execution */
        const uint32_t kISGID  =   02000;       /* Unix set group id on execution */
        const uint32_t kISVTX  =   01000;       /* Unix directory permissions control */
        const uint32_t kENFMT  =   kISGID;   /* Unix record locking enforcement flag */
        const uint32_t kIRWXU  =   00700;       /* Unix read, write, execute: owner */
        const uint32_t kIRUSR  =   00400;       /* Unix read permission: owner */
        const uint32_t kIWUSR  =   00200;       /* Unix write permission: owner */
        const uint32_t kIXUSR  =   00100;       /* Unix execute permission: owner */
        const uint32_t kIRWXG  =   00070;       /* Unix read, write, execute: group */
        const uint32_t kIRGRP  =   00040;       /* Unix read permission: group */
        const uint32_t kIWGRP  =   00020;       /* Unix write permission: group */
        const uint32_t kIXGRP  =   00010;       /* Unix execute permission: group */
        const uint32_t kIRWXO  =   00007;       /* Unix read, write, execute: other */
        const uint32_t kIROTH  =   00004;       /* Unix read permission: other */
        const uint32_t kIWOTH  =   00002;       /* Unix write permission: other */
        const uint32_t kIXOTH  =   00001;       /* Unix execute permission: other */
    }

    /** AMIGA属性
    */
    namespace NAmigaAttribute
    {
        const uint32_t kIFMT     = 06000;       /* Amiga file type mask */
        const uint32_t kIFDIR    = 04000;       /* Amiga directory */
        const uint32_t kIFREG    = 02000;       /* Amiga regular file */
        const uint32_t kIHIDDEN  = 00200;       /* to be supported in AmigaDOS 3.x */
        const uint32_t kISCRIPT  = 00100;       /* executable script (text command file) */
        const uint32_t kIPURE    = 00040;       /* allow loading into resident memory */
        const uint32_t kIARCHIVE = 00020;       /* not modified since bit was last set */
        const uint32_t kIREAD    = 00010;       /* can be opened for reading */
        const uint32_t kIWRITE   = 00004;       /* can be opened for writing */
        const uint32_t kIEXECUTE = 00002;       /* executable image, a loadable runfile */
        const uint32_t kIDELETE  = 00001;      /* can be deleted */
    }
}

END_NAMESPACE_RCZIP

#endif //__RCZipHeader_h_
