/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#include "format/deb/RCDebHeader.h"
#include "common/RCStringUtilA.h"
#include "filesystem/RCStreamUtils.h"

BEGIN_NAMESPACE_RCZIP

const char* RCDebDefs::kSignature  = "!<arch>\n";

void RCDebDefs::MyStrNCpy(char *dest, const char *src, int32_t size)
{
    for (int32_t i = 0; i < size; i++)
    {
        char c = src[i];
        dest[i] = c;
        if (c == 0)
        {
            break;
        }
    }
}

bool RCDebDefs::OctalToNumber(const char *s, int32_t size, uint64_t &res)
{
    char sz[32];
    MyStrNCpy(sz, s, size);
    sz[size] = 0;
    const char *end;
    int32_t i;
    for (i = 0; sz[i] == ' '; i++);
    res = RCStringUtilA::ConvertOctStringToUInt64(sz + i, &end);
    return (*end == ' ' || *end == 0);
}

bool RCDebDefs::OctalToNumber32(const char *s, int32_t size, uint32_t &res)
{
    uint64_t res64;
    if (!OctalToNumber(s, size, res64))
    {
        return false;
    }
    res = (uint32_t)res64;
    return (res64 <= 0xFFFFFFFF);
}

bool RCDebDefs::DecimalToNumber(const char *s, int32_t size, uint64_t &res)
{
    char sz[32];
    MyStrNCpy(sz, s, size);
    sz[size] = 0;
    const char *end;
    int32_t i;
    for (i = 0; sz[i] == ' '; i++);
    res = RCStringUtilA::ConvertStringToUInt64(sz + i, &end);
    return (*end == ' ' || *end == 0);
}

bool RCDebDefs::DecimalToNumber32(const char *s, int32_t size, uint32_t &res)
{
    uint64_t res64;
    if (!DecimalToNumber(s, size, res64))
    {
        return false;
    }
    res = (uint32_t)res64;
    return (res64 <= 0xFFFFFFFF);
}

#define RIF(x) { if (!(x)) return RC_S_FALSE; }

HResult RCDebInArchive::Open(IInStream *inStream)
{
    HResult result;
    result = inStream->Seek(0, RC_STREAM_SEEK_CUR, &m_position);
    if (!IsSuccess(result))
    {
        return result;
    }
    char signature[RCDebDefs::kSignatureLen];
    result = RCStreamUtils::ReadStream_FALSE(inStream, signature, RCDebDefs::kSignatureLen);
    if (!IsSuccess(result))
    {
        return result;
    }
    m_position += RCDebDefs::kSignatureLen;
    if (memcmp(signature, RCDebDefs::kSignature, RCDebDefs::kSignatureLen) != 0)
    {
        return RC_S_FALSE;
    }
    m_stream = inStream;
    return RC_S_OK;
}

HResult RCDebInArchive::GetNextItemReal(bool &filled, RCDebItem &item)
{
    HResult result;
    filled = false;

    char header[RCDebDefs::kHeaderSize];
    const char *cur = header;

    size_t processedSize = sizeof(header);
    item.m_headerPos = m_position;
    result = RCStreamUtils::ReadStream(m_stream.Get(), header, &processedSize);
    if (!IsSuccess(result))
    {
        return result;
    }
    m_position += processedSize;
    if (processedSize != sizeof(header))
    {
        return RC_S_OK;
    }

    char tempString[RCDebDefs::kNameSize + 1];
    RCDebDefs::MyStrNCpy(tempString, cur, RCDebDefs::kNameSize);
    cur += RCDebDefs::kNameSize;
    tempString[RCDebDefs::kNameSize] = '\0';
    item.m_name = tempString;
    RCStringUtilA::Trim(item.m_name);

    for (int32_t i = 0; i < static_cast<int32_t>(item.m_name.length()); i++)
    {
        if (((byte_t)item.m_name[i]) < 0x20)
        {
            return RC_S_FALSE;
        }
    }

    RIF(RCDebDefs::DecimalToNumber32(cur, RCDebDefs::kTimeSize, item.m_mTime));
    cur += RCDebDefs::kTimeSize;

    cur += 6 + 6;

    RIF(RCDebDefs::OctalToNumber32(cur, RCDebDefs::kModeSize, item.m_mode));
    cur += RCDebDefs::kModeSize;

    RIF(RCDebDefs::DecimalToNumber(cur, RCDebDefs::kSizeSize, item.m_size));
    cur += RCDebDefs::kSizeSize;

    filled = true;
    return RC_S_OK;
}

HResult RCDebInArchive::GetNextItem(bool &filled, RCDebItem &item)
{
    for (;;)
    {
        HResult result;
        result = GetNextItemReal(filled, item);
        if (!IsSuccess(result))
        {
            return result;
        }
        if (!filled)
        {
            return RC_S_OK;
        }
        if (RCStringUtilA::Compare(item.m_name,"debian-binary") != 0)
        {
            return RC_S_OK;
        }
        if (item.m_size != 4)
        {
            return RC_S_OK;
        }
        SkipData(item.m_size);
    }
}

HResult RCDebInArchive::SkipData(uint64_t dataSize)
{
    return m_stream->Seek((dataSize + 1) & (~((uint64_t)0x1)), RC_STREAM_SEEK_CUR, &m_position);
}

END_NAMESPACE_RCZIP
