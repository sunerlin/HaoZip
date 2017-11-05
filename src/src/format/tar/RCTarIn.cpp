/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#include "format/tar/RCTarIn.h"
#include "common/RCStringUtilA.h"
#include "filesystem/RCStreamUtils.h"

BEGIN_NAMESPACE_RCZIP

static void MyStrNCpy(char *dest, const char *src, int size)
{
    for (int i = 0; i < size; i++)
    {
        char c = src[i];
        dest[i] = c;
        if (c == 0)
        {
            break;
        }
    }
}

static bool OctalToNumber(const char *srcString, int size, uint64_t &res)
{
    char sz[32];
    MyStrNCpy(sz, srcString, size);
    sz[size] = 0;
    const char *end;
    int i;
    for (i = 0; sz[i] == ' '; i++);
    res = RCStringUtilA::ConvertOctStringToUInt64(sz + i, &end);
    return (*end == ' ' || *end == 0);
}

static bool OctalToNumber32(const char *srcString, int size, uint32_t &res)
{
    uint64_t res64;
    if (!OctalToNumber(srcString, size, res64))
    {
        return false;
    }
    res = (uint32_t)res64;
    return (res64 <= 0xFFFFFFFF);
}

#define RIF(x) { if (!(x)) return RC_S_FALSE; }

static bool IsRecordLast(const char *buf)
{
    for (int i = 0; i < RCTarHeader::s_recordSize; i++)
    {
        if (buf[i] != 0)
        {
            return false;
        }
    }
    return true;
}

static void ReadString(const char *s, int size, RCStringA &result)
{
    char temp[RCTarHeader::s_recordSize + 1];
    MyStrNCpy(temp, s, size);
    temp[size] = '\0';
    result = temp;
}

static HResult GetNextItemReal(ISequentialInStream *stream, bool &filled, RCTarItemEx &item, size_t &processedSize)
{
    item.m_longLinkSize = 0;
    char buf[RCTarHeader::s_recordSize];
    char *p = buf;

    filled = false;

    processedSize = RCTarHeader::s_recordSize;

    HResult rs = RCStreamUtils::ReadStream(stream, buf, &processedSize);
    if (rs != RC_S_OK)
    {
        return rs;
    }
    if (processedSize == 0 || (processedSize == RCTarHeader::s_recordSize && IsRecordLast(buf)))
    {
        return RC_S_OK;
    }
    if (processedSize < RCTarHeader::s_recordSize)
    {
        return RC_S_FALSE;
    }

    ReadString(p, RCTarHeader::s_nameSize, item.m_name); p += RCTarHeader::s_nameSize;

    RIF(OctalToNumber32(p, 8, item.m_mode)); p += 8;

    if (!OctalToNumber32(p, 8, item.m_uid)) 
    {
        item.m_uid = 0;
    }
    p += 8;
    if (!OctalToNumber32(p, 8, item.m_gid)) 
    {
        item.m_gid = 0;
    }
    p += 8;

    RIF(OctalToNumber(p, 12, item.m_size)); p += 12;
    RIF(OctalToNumber32(p, 12, item.m_mTime)); p += 12;

    uint32_t checkSum;
    RIF(OctalToNumber32(p, 8, checkSum));
    memcpy(p, RCTarHeader::s_checkSumBlanks, 8); p += 8;

    item.m_linkFlag = *p++;

    ReadString(p, RCTarHeader::s_nameSize, item.m_linkName); p += RCTarHeader::s_nameSize;

    memcpy(item.m_magic, p, 8); p += 8;

    ReadString(p, RCTarHeader::s_userNameSize, item.m_user);  p += RCTarHeader::s_userNameSize;
    ReadString(p, RCTarHeader::s_groupNameSize, item.m_group); p += RCTarHeader::s_groupNameSize;

    item.m_deviceMajorDefined = (p[0] != 0); RIF(OctalToNumber32(p, 8, item.m_deviceMajor)); p += 8;
    item.m_deviceMinorDefined = (p[0] != 0); RIF(OctalToNumber32(p, 8, item.m_deviceMinor)); p += 8;

    RCStringA prefix;
    ReadString(p, RCTarHeader::s_prefixSize, prefix);
    p += RCTarHeader::s_prefixSize;
    if (!prefix.empty() && item.IsMagic() &&
        (item.m_linkFlag != 'L' /* || prefix != "00000000000" */ ))
    {
        item.m_name = prefix + RCStringA("/") + item.m_name;
    }

    if (item.m_linkFlag == RCTarHeader::NLinkFlag::s_link)
    {
        item.m_size = 0;
    }

    uint32_t checkSumReal = 0;
    for (int i = 0; i < RCTarHeader::s_recordSize; i++)
    {
        checkSumReal += (byte_t)buf[i];
    }

    if (checkSumReal != checkSum)
    {
        return RC_S_FALSE;
    }

    filled = true;
    return RC_S_OK;
}

HResult RCTarIn::ReadItem(ISequentialInStream *stream, bool &filled, RCTarItemEx &item)
{
    size_t processedSize;
    HResult rs = GetNextItemReal(stream, filled, item, processedSize);
    if (rs != RC_S_OK)
    {
        return rs;
    }
    if (!filled)
    {
        return RC_S_OK;
    }
    // GNUtar extension
    if (item.m_linkFlag == 'L' || // NEXT file has a long name
        item.m_linkFlag == 'K')   // NEXT file has a long linkname
    {
        if (item.m_name.compare(RCTarHeader::s_longLink) != 0)
        {
            if (item.m_name.compare(RCTarHeader::s_longLink2) != 0)
            {
                return RC_S_FALSE;
            }
        }

        RCCharBuffer fullName;
        if (item.m_size > (1 << 15))
        {
            return RC_S_FALSE;
        }

        int packSize = (int)item.GetPackSize();
        fullName.SetCapacity(packSize + 1);
        char *buffer = (char*)fullName.data();

        rs = RCStreamUtils::ReadStream_FALSE(stream, buffer, packSize);
        if (rs != RC_S_OK)
        {
            return rs;
        }

        processedSize += packSize;
        buffer[item.m_size] = '\0';

        uint64_t headerPosition = item.m_headerPosition;
        if (item.m_linkFlag == 'L')
        {
            size_t processedSize2;
            rs = GetNextItemReal(stream, filled, item, processedSize2);
            if (rs != RC_S_OK)
            {
                return rs;
            }
            item.m_longLinkSize = (unsigned)processedSize;
        }
        else
        {
            item.m_longLinkSize = (unsigned)processedSize - RCTarHeader::s_recordSize ;
            item.m_size = 0;
        }
        item.m_name = fullName.data();
        item.m_headerPosition = headerPosition;
    }
    else if (item.m_linkFlag == 'g' || item.m_linkFlag == 'x' || item.m_linkFlag == 'X')
    {
        // pax Extended Header
        return RC_S_OK;
    }
    else if (item.m_linkFlag == RCTarHeader::NLinkFlag::s_dumpDir)
    {
        // GNU Extensions to the Archive Format
        return RC_S_OK;
    }
    else if (item.m_linkFlag > '7' || (item.m_linkFlag < '0' && item.m_linkFlag != 0))
    {
        return RC_S_FALSE;
    }

    return RC_S_OK;
}

END_NAMESPACE_RCZIP