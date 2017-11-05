/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RC7zHeader_h_
#define __RC7zHeader_h_ 1

#include "base/RCDefs.h"

//#define _7Z_VOL

BEGIN_NAMESPACE_RCZIP

class RC7zHeader
{
public:

    static const int32_t s_k7zSignatureSize = 6;
    static byte_t s_k7zSignature[s_k7zSignatureSize];

    // #define _7Z_VOL
    // 7z-MultiVolume is not finished yet.
    // It can work already, but I still do not like some
    // things of that new multivolume format.
    // So please keep it commented.
    
    static const byte_t s_k7zMajorVersion = 0;
    static const uint32_t s_k7zStartHeaderSize = 20;

#ifdef _7Z_VOL
    static byte_t s_k7zFinishSignature[s_k7zSignatureSize];
    static const uint32_t s_k7zFinishHeaderSize = s_k7zStartHeaderSize + 16;
#endif
    
};

struct RC7zArchiveVersion
{
    /** 主版本号
    */
    byte_t m_major ;

    /** 子版本号
    */
    byte_t m_minor ;
};

struct RC7zStartHeader
{
    /** 下一个头的偏移
    */
    uint64_t m_nextHeaderOffset;

    /** 下一个头的大小
    */
    uint64_t m_nextHeaderSize;

    /** 下一个头的crc
    */
    uint32_t m_nextHeaderCRC;
};

#ifdef _7Z_VOL

struct RC7zFinishHeader: 
    public RC7zStartHeader
{
    uint64_t m_archiveStartOffset;       // data offset from end if that struct

    uint64_t m_additionalStartBlockSize; // start  signature & start header size
};

#endif

class RC7zID
{
public:
    enum Enum
    {
        kEnd,
        kHeader,
        kArchiveProperties,    
        kAdditionalStreamsInfo,
        kMainStreamsInfo,
        kFilesInfo,    
        kPackInfo,
        kUnpackInfo,
        kSubStreamsInfo,
        kSize,
        kCRC,
        kFolder,
        kCodersUnpackSize,
        kNumUnpackStream,
        kEmptyStream,
        kEmptyFile,
        kAnti,
        kName,
        kCTime,
        kATime,
        kMTime,
        kWinAttributes,
        kComment,
        kEncodedHeader,
        kStartPos,
        kDummy
    };
} ;

END_NAMESPACE_RCZIP

#endif //__RC7zHeader_h_
