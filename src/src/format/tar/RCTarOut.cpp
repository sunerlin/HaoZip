/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#include "format/tar/RCTarOut.h"
#include "common/RCStringUtilA.h"
#include "filesystem/RCStreamUtils.h"

BEGIN_NAMESPACE_RCZIP

HResult RCTarOut::WriteBytes(const void *buffer, uint32_t size)
{
    return RCStreamUtils::WriteStream(m_stream.Get(), buffer, size);
}

void RCTarOut::Create(ISequentialOutStream *outStream)
{
    m_stream = outStream;
}

static RCStringA MakeOctalString(uint64_t value)
{
    char s[32];
    RCStringUtilA::ConvertUInt64ToString(value, s, 8);
    return RCStringA(s) + ' ';
}

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

static bool MakeOctalString8(char *s, uint32_t value)
{
    RCStringA tempString = MakeOctalString(value);

    const size_t kMaxSize = 8;
    if (tempString.size() >= kMaxSize)
    {
        return false;
    }

    size_t numSpaces = kMaxSize - (tempString.size() + 1);
    for(size_t i = 0; i < numSpaces; i++)
    {
        s[i] = ' ';
    }

    RCStringUtilA::StringCopy(s + numSpaces, (const char *)tempString.c_str());

    return true;
}

static bool MakeOctalString12(char *s, uint64_t value)
{
    RCStringA tempString  = MakeOctalString(value);
    const size_t kMaxSize = 12;
    if (tempString.size() > kMaxSize)
    {
        return false;
    }

    size_t numSpaces = kMaxSize - tempString.size();
    for(size_t i = 0; i < numSpaces; i++)
    {
        s[i] = ' ';
    }

    memmove(s + numSpaces, (const char *)tempString.c_str(), tempString.length());

    return true;
}

static bool CopyString(char *dest, const RCStringA &src, int32_t maxSize)
{
    if ((int32_t)src.size() >= maxSize)
    {
        return false;
    }

    RCStringUtilA::StringCopy(dest, (const char *)src.c_str());

    return true;
}

#define RETURN_IF_NOT_TRUE(x) { if (!(x)) return RC_E_FAIL; }

HResult RCTarOut::WriteHeaderReal(const RCTarItem &item)
{
    char record[RCTarHeader::s_recordSize];
    char *cur = record;
    int i;
    for (i = 0; i < RCTarHeader::s_recordSize; i++)
    {
        record[i] = 0;
    }

    // RETURN_IF_NOT_TRUE(CopyString(header.Name, item.Name, RCTarHeader::s_nameSize));
    if (item.m_name.length() > RCTarHeader::s_nameSize)
    {
        return RC_E_FAIL;
    }

    MyStrNCpy(cur, item.m_name.c_str(), RCTarHeader::s_nameSize);
    cur += RCTarHeader::s_nameSize;

    RETURN_IF_NOT_TRUE(MakeOctalString8(cur, item.m_mode));
    cur += 8;
    RETURN_IF_NOT_TRUE(MakeOctalString8(cur, item.m_uid));
    cur += 8;
    RETURN_IF_NOT_TRUE(MakeOctalString8(cur, item.m_gid));
    cur += 8;

    RETURN_IF_NOT_TRUE(MakeOctalString12(cur, item.m_size));
    cur += 12;
    RETURN_IF_NOT_TRUE(MakeOctalString12(cur, item.m_mTime));
    cur += 12;

    memmove(cur, RCTarHeader::s_checkSumBlanks, 8);
    cur += 8;

    *cur++ = item.m_linkFlag;

    RETURN_IF_NOT_TRUE(CopyString(cur, item.m_linkName, RCTarHeader::s_nameSize));
    cur += RCTarHeader::s_nameSize;

    memmove(cur, item.m_magic, 8);
    cur += 8;

    RETURN_IF_NOT_TRUE(CopyString(cur, item.m_user, RCTarHeader::s_userNameSize));
    cur += RCTarHeader::s_userNameSize;
    RETURN_IF_NOT_TRUE(CopyString(cur, item.m_group, RCTarHeader::s_groupNameSize));
    cur += RCTarHeader::s_groupNameSize;

    if (item.m_deviceMajorDefined)
    {
        RETURN_IF_NOT_TRUE(MakeOctalString8(cur, item.m_deviceMajor));
    }
    cur += 8;

    if (item.m_deviceMinorDefined)
    {
        RETURN_IF_NOT_TRUE(MakeOctalString8(cur, item.m_deviceMinor));
    }
    cur += 8;

    uint32_t checkSumReal = 0;
    for(i = 0; i < RCTarHeader::s_recordSize; i++)
    {
        checkSumReal += byte_t(record[i]);
    }

    RETURN_IF_NOT_TRUE(MakeOctalString8(record + 148, checkSumReal));

    return WriteBytes(record, RCTarHeader::s_recordSize);
}

HResult RCTarOut::WriteHeader(const RCTarItem &item)
{
    int32_t nameSize = (int32_t)item.m_name.size();
    if (nameSize < RCTarHeader::s_nameSize)
    {
        return WriteHeaderReal(item);
    }

    RCTarItem modifiedItem = item;
    int32_t nameStreamSize = nameSize + 1;
    modifiedItem.m_size = nameStreamSize;
    modifiedItem.m_linkFlag = 'L';
    modifiedItem.m_name = RCTarHeader::s_longLink;
    modifiedItem.m_linkName.empty();

    HResult rs =WriteHeaderReal(modifiedItem);
    if (rs != RC_S_OK)
    {
        return rs;
    }

    rs = WriteBytes(item.m_name.c_str(), nameStreamSize);
    if (rs != RC_S_OK)
    {
        return rs;
    }

    rs = FillDataResidual(nameStreamSize);
    if (rs != RC_S_OK)
    {
        return rs;
    }

    modifiedItem = item;
    modifiedItem.m_name = item.m_name.substr(0, RCTarHeader::s_nameSize - 1);

    return WriteHeaderReal(modifiedItem);
}

HResult RCTarOut::FillDataResidual(uint64_t dataSize)
{
    uint32_t lastRecordSize = uint32_t(dataSize & (RCTarHeader::s_recordSize - 1));
    if (lastRecordSize == 0)
    {
        return RC_S_OK;
    }

    uint32_t residualSize = RCTarHeader::s_recordSize - lastRecordSize;
    byte_t residualBytes[RCTarHeader::s_recordSize];
    for (uint32_t i = 0; i < residualSize; i++)
    {
        residualBytes[i] = 0;
    }

    return WriteBytes(residualBytes, residualSize);
}

HResult RCTarOut::WriteFinishHeader()
{
    byte_t record[RCTarHeader::s_recordSize];
    int i;
    for (i = 0; i < RCTarHeader::s_recordSize; i++)
    {
        record[i] = 0;
    }

    for (i = 0; i < 2; i++)
    {
        HResult rs = WriteBytes(record, RCTarHeader::s_recordSize);
        if (rs != RC_S_OK)
        {
            return rs;
        }
    }

    return RC_S_OK;
}

END_NAMESPACE_RCZIP