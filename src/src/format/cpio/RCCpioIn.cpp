/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "format/cpio/RCCpioIn.h"
#include "filesystem/RCStreamUtils.h"
#include "common/RCStringUtilA.h"

/////////////////////////////////////////////////////////////////
//RCCpioIn class implementation

BEGIN_NAMESPACE_RCZIP

HResult RCCpioIn::ReadBytes(void* data, uint32_t size, uint32_t& processedSize)
{
    size_t realProcessedSize = size;
    HResult hr = RCStreamUtils::ReadStream(m_inStream.Get(), data, &realProcessedSize);
    if (hr != RC_S_OK)
    {
        return hr;
    }

    processedSize = (uint32_t)realProcessedSize;
    m_position += processedSize;

    return RC_S_OK;
}

byte_t RCCpioIn::ReadByte()
{
    if (m_blockPos >= m_blockSize)
    {
        _ThrowCode(RC_E_ReadStreamError) ;
    }

    return m_block[m_blockPos++];
}

uint16_t RCCpioIn::ReadUInt16()
{
    uint16_t value = 0;
    for (int32_t i = 0; i < 2; i++)
    {
        byte_t b = ReadByte();
        value |= (uint16_t(b) << (8 * i));
    }
    return value;
}

uint32_t RCCpioIn::ReadUInt32()
{
    uint32_t value = 0;
    for (int32_t i = 0; i < 4; i++)
    {
        byte_t b = ReadByte();
        value |= (uint32_t(b) << (8 * i));
    }

    return value;
}

HResult RCCpioIn::Open(IInStream* inStream)
{
    HResult hr = inStream->Seek(0, RC_STREAM_SEEK_CUR, &m_position);
    if (hr != RC_S_OK)
    {
        return hr;
    }

    m_inStream = inStream;
    return RC_S_OK;
}

bool RCCpioIn::ReadNumber(uint32_t &resultValue)
{
    resultValue = 0;
    for (int32_t i = 0; i < 8; i++)
    {
        char c = char(ReadByte());
        int32_t d;
        if (c >= '0' && c <= '9')
        {
            d = c - '0';
        }
        else if (c >= 'A' && c <= 'F')
        {
            d = 10 + c - 'A';
        }
        else if (c >= 'a' && c <= 'f')
        {
            d = 10 + c - 'a';
        }
        else
        {
            return false;
        }

        resultValue *= 0x10;
        resultValue += d;
    }
    return true;
}

static bool OctalToNumber(const char *s, uint64_t &res)
{
    const char *end;
    res = RCStringUtilA::ConvertOctStringToUInt64(s, &end);
    return (*end == ' ' || *end == 0);
}

static bool OctalToNumber32(const char *s, uint32_t &res)
{
    uint64_t res64;
    if (!OctalToNumber(s, res64))
    {
        return false;
    }
    res = (uint32_t)res64;
    return (res64 <= 0xFFFFFFFF);
}

bool RCCpioIn::ReadOctNumber(int32_t size, uint32_t& resultValue)
{
    char sz[32 + 4];
    int32_t i;
    for (i = 0; i < size && i < 32; i++)
    {
        sz[i] = (char)ReadByte();
    }
    sz[i] = 0;
    return OctalToNumber32(sz, resultValue);
}

#define GetFromHex(y) { if (!ReadNumber(y)) return RC_S_FALSE; }
#define GetFromOct6(y) { if (!ReadOctNumber(6, y)) return RC_S_FALSE; }
#define GetFromOct11(y) { if (!ReadOctNumber(11, y)) return RC_S_FALSE; }

static unsigned short ConvertValue(unsigned short value, bool convert)
{
    if (!convert)
    {
        return value;
    }
    return (unsigned short)((((unsigned short)(value & 0xFF)) << 8) | (value >> 8));
}

static uint32_t GetAlignedSize(uint32_t size, uint32_t align)
{
    while ((size & (align - 1)) != 0)
    {
        size++;
    }
    return size;
}


HResult RCCpioIn::GetNextItem(bool& filled, RCCpioItemEx& item)
{
    filled = false;

    uint32_t processedSize;
    item.m_headerPosition = m_position;

    m_blockSize = s_maxBlockSize;
    HResult hr = ReadBytes(m_block, 2, processedSize);
    if (hr !=  RC_S_OK)
    {
        return hr;
    }

    if (processedSize != 2)
    {
        return RC_S_FALSE;
    }

    m_blockPos = 0;

    uint32_t nameSize;

    bool oldBE =
        m_block[0] == RCCpioHeader::NMagic::kMagicForRecord2[1] &&
        m_block[1] == RCCpioHeader::NMagic::kMagicForRecord2[0];

    bool binMode = (m_block[0] == RCCpioHeader::NMagic::kMagicForRecord2[0] &&
        m_block[1] == RCCpioHeader::NMagic::kMagicForRecord2[1]) ||
        oldBE;

    if (binMode)
    {
        hr = ReadBytes(m_block + 2, RCCpioHeader::kRecord2Size - 2, processedSize);
        if (hr !=  RC_S_OK)
        {
            return hr;
        }

        if (processedSize != RCCpioHeader::kRecord2Size - 2)
        {
            return RC_S_FALSE;
        }
        item.m_align = 2;
        m_blockPos = 2;
        item.m_devMajor = 0;
        item.m_devMinor = ConvertValue(ReadUInt16(), oldBE);
        item.m_inode = ConvertValue(ReadUInt16(), oldBE);
        item.m_mode = ConvertValue(ReadUInt16(), oldBE);
        item.m_uid = ConvertValue(ReadUInt16(), oldBE);
        item.m_gid = ConvertValue(ReadUInt16(), oldBE);
        item.m_numLinks = ConvertValue(ReadUInt16(), oldBE);
        item.m_rdevMajor =0;
        item.m_rdevMinor = ConvertValue(ReadUInt16(), oldBE);
        uint16_t timeHigh = ConvertValue(ReadUInt16(), oldBE);
        uint16_t timeLow = ConvertValue(ReadUInt16(), oldBE);
        item.m_mTime = (uint32_t(timeHigh) << 16) + timeLow;
        nameSize = ConvertValue(ReadUInt16(), oldBE);
        uint16_t sizeHigh = ConvertValue(ReadUInt16(), oldBE);
        uint16_t sizeLow = ConvertValue(ReadUInt16(), oldBE);
        item.m_size = (uint32_t(sizeHigh) << 16) + sizeLow;

        item.m_chkSum = 0;
        item.m_headerSize = GetAlignedSize(nameSize + RCCpioHeader::kRecord2Size, item.m_align);
        nameSize = item.m_headerSize - RCCpioHeader::kRecord2Size;
    }
    else
    {
        hr = ReadBytes(m_block + 2, 4, processedSize);
        if (hr != RC_S_OK)
        {
            return hr;
        }

        if (processedSize != 4)
        {
            return RC_S_FALSE;
        }

        bool magicOK =
            memcmp(m_block, RCCpioHeader::NMagic::kMagic1, 6) == 0 ||
            memcmp(m_block, RCCpioHeader::NMagic::kMagic2, 6) == 0;
        m_blockPos = 6;
        if (magicOK)
        {
            hr = ReadBytes(m_block + 6, RCCpioHeader::kRecordSize - 6, processedSize);
            if (hr != RC_S_OK)
            {
                return hr;
            }

            if (processedSize != RCCpioHeader::kRecordSize - 6)
            {
                return RC_S_FALSE;
            }
            item.m_align = 4;

            GetFromHex(item.m_inode);
            GetFromHex(item.m_mode);
            GetFromHex(item.m_uid);
            GetFromHex(item.m_gid);
            GetFromHex(item.m_numLinks);
            uint32_t mTime;
            GetFromHex(mTime);
            item.m_mTime = mTime;
            GetFromHex(item.m_size);
            GetFromHex(item.m_devMajor);
            GetFromHex(item.m_devMinor);
            GetFromHex(item.m_rdevMajor);
            GetFromHex(item.m_rdevMinor);
            GetFromHex(nameSize);
            GetFromHex(item.m_chkSum);
            item.m_headerSize = GetAlignedSize(nameSize + RCCpioHeader::kRecordSize, item.m_align);
            nameSize = item.m_headerSize - RCCpioHeader::kRecordSize;
        }
        else
        {
            if (!memcmp(m_block, RCCpioHeader::NMagic::kMagic3, 6) == 0)
            {
                return RC_S_FALSE;
            }

            hr = ReadBytes(m_block + 6, RCCpioHeader::kOctRecordSize - 6, processedSize);
            if (hr != RC_S_OK)
            {
                return hr;
            }

            if (processedSize != RCCpioHeader::kOctRecordSize - 6)
            {
                return RC_S_FALSE;
            }

            item.m_align = 1;
            item.m_devMajor = 0;
            GetFromOct6(item.m_devMinor);
            GetFromOct6(item.m_inode);
            GetFromOct6(item.m_mode);
            GetFromOct6(item.m_uid);
            GetFromOct6(item.m_gid);
            GetFromOct6(item.m_numLinks);
            item.m_rdevMajor = 0;
            GetFromOct6(item.m_rdevMinor);
            uint32_t mTime;
            GetFromOct11(mTime);
            item.m_mTime = mTime;
            GetFromOct6(nameSize);
            GetFromOct11(item.m_size);  // ?????
            item.m_headerSize = GetAlignedSize(nameSize + RCCpioHeader::kOctRecordSize, item.m_align);
            nameSize = item.m_headerSize - RCCpioHeader::kOctRecordSize;
        }
    }

    if (nameSize == 0 || nameSize >= (1 << 27))
    {
        return RC_E_FAIL;
    }

    char* buf = new char [nameSize];

    hr = ReadBytes(buf, nameSize, processedSize);
    if (hr != RC_S_OK)
    {
        return hr;
    }

    if (processedSize != nameSize)
    {
        return RC_E_FAIL;
    }

    item.m_name = buf;
    delete[] buf;

    if (strcmp(item.m_name.c_str(), RCCpioHeader::NMagic::kEndName) == 0)
    {
        return RC_S_OK;
    }
    filled = true;

    return RC_S_OK;
}

HResult RCCpioIn::Skip(uint64_t numBytes)
{
    uint64_t newPostion;
    HResult hr = m_inStream->Seek(numBytes, RC_STREAM_SEEK_CUR, &newPostion);
    if (hr != RC_S_OK)
    {
        return hr;
    }

    m_position += numBytes;
    if (m_position != newPostion)
    {
        return RC_E_FAIL;
    }

    return RC_S_OK;
}

HResult RCCpioIn::SkipDataRecords(uint64_t dataSize, uint32_t align)
{
    while ((dataSize & (align - 1)) != 0)
    {
        dataSize++;
    }

    return Skip(dataSize);
}

END_NAMESPACE_RCZIP
