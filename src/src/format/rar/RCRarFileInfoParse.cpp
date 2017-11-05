/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#include "format/rar/RCRarFileInfoParse.h"
#include "format/rar/RCRarIn.h"
#include "format/common/RCFindSignature.h"
#include "filesystem/RCStreamUtils.h"
#include "filesystem/RCLimitedSequentialInStream.h"
#include "common/RCStringConvert.h"
#include "common/RCCreateCoder.h"
#include "interface/RCMethodDefs.h"

#define  MHD_VOLUME         0x0001
#define  MHD_COMMENT        0x0002
#define  MHD_LOCK           0x0004
#define  MHD_SOLID          0x0008
#define  MHD_PACK_COMMENT   0x0010
#define  MHD_NEWNUMBERING   0x0010
#define  MHD_AV             0x0020
#define  MHD_PROTECT        0x0040
#define  MHD_PASSWORD       0x0080
#define  MHD_FIRSTVOLUME    0x0100
#define  MHD_ENCRYPTVER     0x0200

#define  LHD_SPLIT_BEFORE   0x0001
#define  LHD_SPLIT_AFTER    0x0002
#define  LHD_PASSWORD       0x0004
#define  LHD_COMMENT        0x0008
#define  LHD_SOLID          0x0010

#define  LHD_WINDOWMASK     0x00e0
#define  LHD_WINDOW64       0x0000
#define  LHD_WINDOW128      0x0020
#define  LHD_WINDOW256      0x0040
#define  LHD_WINDOW512      0x0060
#define  LHD_WINDOW1024     0x0080
#define  LHD_WINDOW2048     0x00a0
#define  LHD_WINDOW4096     0x00c0
#define  LHD_DIRECTORY      0x00e0

#define  LHD_LARGE          0x0100
#define  LHD_UNICODE        0x0200
#define  LHD_SALT           0x0400
#define  LHD_VERSION        0x0800
#define  LHD_EXTTIME        0x1000
#define  LHD_EXTFLAGS       0x2000

#define  LONG_BLOCK         0x8000

#define  EARC_NEXT_VOLUME   0x0001 // not last volume
#define  EARC_DATACRC       0x0002 // store CRC32 of RAR archive (now used only in volumes)
#define  EARC_REVSPACE      0x0004 // reserve space for end of REV file 7 byte record
#define  EARC_VOLNUMBER     0x0008 // store a number of current volume

#define  PACK_VER               29
#define  PACK_CRYPT_VER         29
#define  UNP_VER                36
#define  CRYPT_VER              29
#define  AV_VER                 20
#define  PROTECT_VER            20

BEGIN_NAMESPACE_RCZIP

// RCMarkHeader ---------------------------------------------------------------------------------------------------------

bool RCRarFileInfoParse::RCMarkHeader::IsSignature()
{
    bool signature=false;

    if (Data[0]==0x52)
    {
        if (Data[1]==0x45 && Data[2]==0x7e && Data[3]==0x5e)
        {
            signature=true;
        }
        else if (Data[1]==0x61 && Data[2]==0x72 && Data[3]==0x21 && Data[4]==0x1a && Data[5]==0x07 && Data[6]==0x00)
        {
            signature=true;
        }
    }

    return signature;
}

bool RCRarFileInfoParse::RCMarkHeader::IsOldFormat()
{
    bool oldFormat=false;

    if (Data[0]==0x52)
    {
        if (Data[1]==0x45 && Data[2]==0x7e && Data[3]==0x5e)
        {
            oldFormat=true;
        }
        else if (Data[1]==0x61 && Data[2]==0x72 && Data[3]==0x21 && Data[4]==0x1a && Data[5]==0x07 && Data[6]==0x00)
        {
            oldFormat=false;
        }
    }

    return oldFormat;
}

// RCBlockHeader ---------------------------------------------------------------------------------------------------------

bool RCRarFileInfoParse::RCBlockHeader::IsSubBlock()
{
    bool subBlock = false;

    if (HeadType()==SUB_HEAD)
    {
        subBlock = true;
    }
    if (HeadType()==NEWSUB_HEAD && (Flags() & LHD_SOLID)!=0)
    {
        subBlock = true;
    }
    
    return subBlock;
}

unsigned short RCRarFileInfoParse::RCBlockHeader::HeadCRC()
{
    return *(unsigned short*)(Data);
}

unsigned char RCRarFileInfoParse::RCBlockHeader::HeadType()
{
    return Data[2];
}

unsigned short RCRarFileInfoParse::RCBlockHeader::Flags()
{
    return *(unsigned short*)(Data + 3);
}
void RCRarFileInfoParse::RCBlockHeader::SetFlags(unsigned short flags)
{
    *(unsigned short*)(Data + 3) = flags;
}

unsigned short RCRarFileInfoParse::RCBlockHeader::HeadSize()
{
    return *(unsigned short*)(Data + 5);
}

void RCRarFileInfoParse::RCBlockHeader::SetHeadSize(unsigned short size)
{
    *(unsigned short*)(Data + 5) = size;
}

// RCMainHeader ---------------------------------------------------------------------------------------------------------

bool RCRarFileInfoParse::RCMainHeader::IsEncrypted()
{
    return ((Flags() & MHD_PASSWORD) != 0);
}

bool RCRarFileInfoParse::RCMainHeader::IsVolume()
{
    return ((Flags() & MHD_VOLUME) != 0);
}

bool RCRarFileInfoParse::RCMainHeader::IsFirstVolume()
{
    return ((Flags() & MHD_FIRSTVOLUME) != 0);
}

bool RCRarFileInfoParse::RCMainHeader::IsSolid()
{
    return ((Flags() & MHD_SOLID) != 0);
}

bool RCRarFileInfoParse::RCMainHeader::IsMainComment()
{
    return ((Flags() & MHD_COMMENT) != 0);
}

bool RCRarFileInfoParse::RCMainHeader::IsLocked()
{
    return ((Flags() & MHD_LOCK) != 0);
}

bool RCRarFileInfoParse::RCMainHeader::IsSigned()
{
    return (m_posAV != 0);
}

bool RCRarFileInfoParse::RCMainHeader::IsProtected()
{
    return ((Flags() & MHD_PROTECT) != 0);
}

bool RCRarFileInfoParse::RCMainHeader::ReadFrom(unsigned char* data, uint64_t size)
{
    uint64_t position = 0;

    if (position + 2 > size)
    {
        return false;
    }
    m_highPosAV = *(unsigned short*)(data + position);
    position += 2;

    if (position + 2 > size)
    {
        return false;
    }
    m_posAV = *(unsigned short*)(data + position);
    position += 2;

    if (Flags() & MHD_ENCRYPTVER)
    {
        if (position + 2 > size)
        {
            return false;
        }
        m_encryptVer = *(unsigned short*)(data + position);
        position += 2;
    }
    else
    {
        m_encryptVer = 0;
    }

    return true;
}

// RCEndHeader ---------------------------------------------------------------------------------------------------------

bool RCRarFileInfoParse::RCEndHeader::ReadFrom(unsigned char* data, uint64_t size)
{
    uint64_t position = 0;

    if (position + 4 > size)
    {
        return false;
    }
    m_arcDataCRC = *(uint32_t*)(data + position);
    position += 4;

    if (position + 2 > size)
    {
        return false;
    }
    m_volNumber = *(unsigned short*)(data + position);
    position += 2;

    return true;
}

bool RCRarFileInfoParse::RCEndHeader::IsLastVolume()
{
    return (!(Flags() & EARC_NEXT_VOLUME));
}

// RCProtectHeader ---------------------------------------------------------------------------------------------------------

bool RCRarFileInfoParse::RCProtectHeader::ReadFrom(unsigned char* data, uint64_t size)
{
    uint64_t position = 0;

    if (position + 4 > size)
    {
        return false;
    }
    m_dataSize = *(uint32_t*)(data + position);
    position += 4;

    return true;
}

// RCSubHeader ---------------------------------------------------------------------------------------------------------

bool RCRarFileInfoParse::RCSubHeader::ReadFrom(unsigned char* data, uint64_t size)
{
    uint64_t position = 0;

    if (position + 4 > size)
    {
        return false;
    }
    m_dataSize = *(uint32_t*)(data + position);
    position += 4;

    return true;
}

// RCFileHeader ---------------------------------------------------------------------------------------------------------

bool RCRarFileInfoParse::RCFileHeader::ReadFrom(unsigned char* data, uint64_t size)
{
    uint64_t position = 0;

    if (position + 4 > size)
    {
        return false;
    }
    m_packSize = *(uint32_t*)(data + position);
    position += 4;

    if (position + 4 > size)
    {
        return false;
    }
    m_unpSize = *(uint32_t*)(data + position);
    position += 4;

    if (position + 1 > size)
    {
        return false;
    }
    HostOS = *(data + position);
    position += 1;

    if (position + 4 > size)
    {
        return false;
    }
    m_fileCRC = *(uint32_t*)(data + position);
    position += 4;

    if (position + 4 > size)
    {
        return false;
    }
    m_fileTime = *(uint32_t*)(data + position);
    position += 4;

    if (position + 1 > size)
    {
        return false;
    }
    m_unpVer = *(data + position);
    position += 1;

    if (position + 1 > size)
    {
        return false;
    }
    m_method = *(data + position);
    position += 1;

    if (position + 2 > size)
    {
        return false;
    }
    m_nameSize = *(unsigned short*)(data + position);
    position += 2;

    if (position + 4 > size)
    {
        return false;
    }
    m_fileAttr = *(uint32_t*)(data + position);
    position += 4;

    if (Flags() & LHD_LARGE)
    {
        if (position + 4 > size)
        {
            return false;
        }
        m_highPackSize = *(uint32_t*)(data + position);
        position += 4;

        if (position + 4 > size)
        {
            return false;
        }
        m_highUnpSize = *(uint32_t*)(data + position);
        position += 4;
    }
    else
    {
        m_highPackSize = 0;
        m_highUnpSize = 0;

        if (m_unpSize == 0xffffffff)
        {
            m_unpSize=0;
            m_highUnpSize=0x7fffffff;
        }
    }

    return true;
}

// RCRarFileInfoParse ---------------------------------------------------------------------------------------------------------

uint64_t RCRarFileInfoParse::Tell()
{
    if (!m_stream)
    {
        return 0;
    }

    uint64_t position = 0;

    if (m_stream->Seek(0, RC_STREAM_SEEK_CUR, &position) != RC_S_OK)
    {
        position = 0;
    }

    return position;
}

bool RCRarFileInfoParse::Read(void* data, size_t size)
{
    if (!m_stream)
    {
        return false;
    }

    HResult result = RCStreamUtils::ReadStream_FALSE(m_stream, data, size);
    if (result != RC_S_OK )
    {
        return false;
    }

    return true;
}

bool RCRarFileInfoParse::ReadInfoFromStream()
{
    if (!m_stream)
    {
        return false;
    }

    // 得到头部信息
    if (!Read(m_markHeader.Data, SIZEOF_MARKHEAD))
    {
        return false;
    }

    if (!m_markHeader.IsSignature())
    {
        return false;
    }

    if (m_markHeader.IsOldFormat())
    {
        uint64_t position = 0;
        if (m_stream->Seek(0, RC_STREAM_SEEK_SET, &position) != RC_S_OK)
        {
            return false;
        }
    }

    // 读取第一个块，也就是Main块
    unsigned char BlockType = 0;
    uint64_t CurBlockPos = 0;
    uint64_t NextBlockPos = 0;
    if (ReadHeader(BlockType, CurBlockPos, NextBlockPos) == 0)
    {
        return false;
    }

    uint64_t position = 0;
    if (m_stream->Seek(NextBlockPos, RC_STREAM_SEEK_SET, &position) != RC_S_OK)
    {
        return false;
    }

    if (m_mainHeader.m_encryptVer > UNP_VER)
    {
        return false;
    }
    
    // 如果不是第一个分卷，读出分卷号
    if (m_mainHeader.IsVolume())
    {
        if (SearchBlock(ENDARC_HEAD) == 0)
        {
            return false;
        }
    }

    return true;
}

uint64_t RCRarFileInfoParse::ReadHeader(unsigned char& BlockType, uint64_t &CurBlockPos, uint64_t& NextBlockPos)
{
    if (!m_stream)
    {
        return 0;
    }

    BlockType = 0;
    NextBlockPos = 0;

    // 记录当前位置
    CurBlockPos = Tell();

    if (m_markHeader.IsOldFormat())
    {
        /** TODO 旧版本没有发现分卷序号的存放地点，暂不予处理。
        */
        return false;

        return ReadOldHeader(BlockType, CurBlockPos, NextBlockPos);
    }

    return ReadNewHeader(BlockType, CurBlockPos, NextBlockPos);
}
uint64_t RCRarFileInfoParse::ReadOldHeader(unsigned char& BlockType, uint64_t CurBlockPos, uint64_t& NextBlockPos)
{
    BlockType = 0;
    NextBlockPos = 0;
    uint64_t CurBlockSize = 0;

    if (!m_stream)
    {
        return 0;
    }

    // 临时变量，用于接收当前位置
    uint64_t position = 0;
    uint32_t processedSize = 0;

    // 首个块读法不一样
    bool FirstBlock = (CurBlockPos <= m_sfxSize);
    if (FirstBlock)
    {
        unsigned char Data[SIZEOF_OLDMHD];

        if ((m_stream->Read(Data, SIZEOF_OLDMHD, &processedSize) != RC_S_OK) || (processedSize != SIZEOF_OLDMHD))
        {
            return 0;
        }

        
        m_mainHeader.SetHeadSize(*(unsigned short*)(Data + 4));
        m_mainHeader.SetFlags((unsigned short)(*(Data + 6) & 0x3f));

        NextBlockPos=CurBlockPos+m_mainHeader.HeadSize();
        BlockType = MAIN_HEAD;
        CurBlockSize = SIZEOF_OLDMHD;
    }
    else
    {
        unsigned char Data[SIZEOF_OLDLHD];

        if ((m_stream->Read(Data, SIZEOF_OLDLHD, &processedSize) != RC_S_OK) || (processedSize != SIZEOF_OLDLHD))
        {
            return 0;
        }

        uint32_t PackSize = *(unsigned int*)(Data + 0);
        uint32_t UnpSize = *(unsigned int*)(Data + 4);
        unsigned short FileCRC = *(unsigned short*)(Data + 8);
        unsigned short HeadSize = *(unsigned short*)(Data + 10);
        uint32_t FileTime = *(unsigned int*)(Data + 12);
        unsigned char FileAttr = *(Data + 16);
        unsigned char Flags = *(Data + 17);
        unsigned char UnpVer = *(Data + 18);
        unsigned char NameSize = *(Data + 19);
        unsigned char Method = *(Data + 20);



        NextBlockPos=CurBlockPos + HeadSize + PackSize;
        BlockType = FILE_HEAD;
        CurBlockSize = SIZEOF_OLDLHD;
    }

    return 0;
}

uint64_t RCRarFileInfoParse::ReadNewHeader(unsigned char& BlockType, uint64_t CurBlockPos, uint64_t& NextBlockPos)
{
    BlockType = 0;
    NextBlockPos = 0;
    uint64_t CurBlockSize = 0;

    if (!m_stream)
    {
        return 0;
    }
    // 临时变量，用于接收当前位置
    uint64_t position = 0;
    uint32_t processedSize = 0;

    // 首个块不加密，所以除加密标记外还需判定当前位置
    bool Decrypt = m_mainHeader.IsEncrypted() && (CurBlockPos >= m_sfxSize + SIZEOF_MARKHEAD + SIZEOF_NEWMHD);

    // 跳过加密头
    if (Decrypt)
    {
        /** TODO 加密文件都得信息没有调用解密算法解密，暂不予处理。
        */
        return 0;

        if (m_stream->Seek(SALT_SIZE, RC_STREAM_SEEK_CUR, &position) != RC_S_OK)
        {
            return 0;
        }
    }
    
    // 读取块部分头部
    RCBlockHeader blockheader;
    if (!Read(blockheader.Data, SIZEOF_SHORTBLOCKHEAD))
    {
        return 0;
    }
    CurBlockSize += SIZEOF_SHORTBLOCKHEAD;

    // 校验块大小
    if (blockheader.HeadSize() < SIZEOF_SHORTBLOCKHEAD)
    {
        return 0;
    }

    // 读取头部的剩余部分
    uint64_t leaveHeadSize = 0;
    if (blockheader.HeadType()==COMM_HEAD)
    {
        // Comment头部的剩余部分
        leaveHeadSize = SIZEOF_COMMHEAD-SIZEOF_SHORTBLOCKHEAD;
    }
    else if (blockheader.HeadType()==MAIN_HEAD && (blockheader.Flags() & MHD_COMMENT)!=0)
    {
        // Main头部的剩余部分
        leaveHeadSize = SIZEOF_COMMHEAD-SIZEOF_SHORTBLOCKHEAD;
    }
    else
    {
        // 其他类型头部的剩余部分
        leaveHeadSize = blockheader.HeadSize()-SIZEOF_SHORTBLOCKHEAD;
    }
    unsigned char* leaveHeadData = new unsigned char[static_cast<size_t>(leaveHeadSize)];
    if (!leaveHeadData)
    {
        return 0;
    }
    if ((m_stream->Read(leaveHeadData, static_cast<uint32_t>(leaveHeadSize), &processedSize) != RC_S_OK) || (processedSize != static_cast<uint32_t>(leaveHeadSize)))
    {
        delete leaveHeadData;
        return 0;
    }
    CurBlockSize += leaveHeadSize;

    NextBlockPos = CurBlockPos+blockheader.HeadSize();

    BlockType = blockheader.HeadType();

    // 分析头部的剩余部分
    switch (blockheader.HeadType())
    {
    case MAIN_HEAD:
        {
            *(RCBlockHeader*)&m_mainHeader = blockheader;

            if (!m_mainHeader.ReadFrom(leaveHeadData, leaveHeadSize))
            {
                delete leaveHeadData;
                return 0;
            }
        }
        break;
    case ENDARC_HEAD:
        {
            *(RCBlockHeader*)&m_endHeader = blockheader;

            if (!m_endHeader.ReadFrom(leaveHeadData, leaveHeadSize))
            {
                delete leaveHeadData;
                return 0;
            }
        }
        break;
    case FILE_HEAD:
        {
            RCFileHeader file;
            *(RCBlockHeader*)&file = blockheader;

            if (!file.ReadFrom(leaveHeadData, leaveHeadSize))
            {
                delete leaveHeadData;
                return 0;
            }
            uint64_t fullPackSize = ((((uint64_t)(file.m_highPackSize))<<32)+(file.m_packSize));
            NextBlockPos += fullPackSize;
        }
        break;
    case NEWSUB_HEAD:
        {
            
        }
        break;
    case COMM_HEAD:
        {
            /**
            ushort UnpSize;
            byte UnpVer;
            byte Method;
            ushort CommCRC;
            */
        }
        break;
    case SIGN_HEAD:
        {
            /**
            uint CreationTime;
            ushort ArcNameSize;
            ushort UserNameSize;
            */
        }
        break;
    case AV_HEAD:
        {
            /**
            byte UnpVer;
            byte Method;
            byte AVVer;
            uint AVInfoCRC;
            */
        }
        break;
    case PROTECT_HEAD:
        {
            RCProtectHeader protect;
            if (!protect.ReadFrom(leaveHeadData, leaveHeadSize))
            {
                delete leaveHeadData;
                return 0;
            }
            NextBlockPos += protect.m_dataSize;
        }
        break;
    case SUB_HEAD:
        {
            RCSubHeader sub;
            if (!sub.ReadFrom(leaveHeadData, leaveHeadSize))
            {
                delete leaveHeadData;
                return 0;
            }
            NextBlockPos += sub.m_dataSize;
        }
        break;
    default:
        if (blockheader.Flags() & LONG_BLOCK)
        {
            uint64_t position = 0;

            if (position + 4 > leaveHeadSize)
            {
                return false;
            }
            uint32_t DataSize = *(uint32_t*)(leaveHeadData + position);
            position += 4;

            NextBlockPos += DataSize;
        }
        break;
    }

    delete leaveHeadData;

    if (Decrypt)
    {
        NextBlockPos += SALT_SIZE;

        /** TODO 加密后数据与实际数据大小未必一样，需要另外处理
        */
    }

    return CurBlockSize;
}

uint64_t RCRarFileInfoParse::SearchBlock(unsigned char BlockType)
{
    if (!m_stream)
    {
        return 0;
    }

    uint64_t size = 0;
    uint64_t NextBlockPos = 0;
    uint64_t CurBlockPos = 0;

    unsigned char readBlockType = 0;
    while ((size = ReadHeader(readBlockType, CurBlockPos, NextBlockPos)) != 0 && 
           (BlockType == ENDARC_HEAD || readBlockType != ENDARC_HEAD))
    {
        if (readBlockType == BlockType)
        {
            return CurBlockPos;
        }

        uint64_t position = 0;
        if (m_stream)
        {
            if (m_stream->Seek(NextBlockPos, RC_STREAM_SEEK_SET, &position) != RC_S_OK)
            {
                return 0;
            }
        }
    }
    return(0);
}

END_NAMESPACE_RCZIP