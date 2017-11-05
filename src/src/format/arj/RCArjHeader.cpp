/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#include "format/arj/RCArjHeader.h"
#include "filesystem/RCStreamUtils.h"
#include "filesystem/RCFileTime.h"
#include "common/RCBuffer.h"
#include "common/RCStringConvert.h"
#include "algorithm/CpuArch.h"

#define Get16(p) GetUi16(p)
#define Get32(p) GetUi32(p)

BEGIN_NAMESPACE_RCZIP

static const char_t *kHostOS[] =
{
    _T("MSDOS"),
    _T("PRIMOS"),
    _T("UNIX"),
    _T("AMIGA"),
    _T("MAC"),
    _T("OS/2"),
    _T("APPLE GS"),
    _T("ATARI ST"),
    _T("NEXT"),
    _T("VAX VMS"),
    _T("WIN95")
};

static const char_t *kUnknownOS = _T("Unknown");

static const int kNumHostOSes = sizeof(kHostOS) / sizeof(kHostOS[0]);

HResult RCArjDefs::ReadString(const byte_t *p, unsigned &size, RCStringA &res)
{
    RCStringA s;
    for (unsigned i = 0; i < size;)
    {
        char c = (char)p[i++];
        if (c == 0)
        {
            size = i;
            res = s;
            return RC_S_OK;
        }
        s += c;
    }
    return RC_S_FALSE;
}

HResult RCArjArchiveHeader::Parse(const byte_t *p, unsigned size)
{
    HResult result;
    if (size < RCArjDefs::kBlockSizeMin)
    {
        return RC_S_FALSE;
    }
    byte_t firstHeaderSize = p[0];
    if (firstHeaderSize > size)
    {
        return RC_S_FALSE;
    }
    m_hostOS = p[3];
    if (p[6] != RCArjDefs::kArchiveHeader)
    {
        return RC_S_FALSE;
    }
    m_cTime = Get32(p + 8);
    m_mTime = Get32(p + 12);
    m_archiveSize = Get32(p + 16);
    unsigned pos = firstHeaderSize;
    unsigned size1 = size - pos;
    result = RCArjDefs::ReadString(p + pos, size1, m_name);
    if (!IsSuccess(result))
    {
        return result;
    }
    pos += size1;
    size1 = size - pos;
    result = RCArjDefs::ReadString(p + pos, size1, m_comment);
    if (!IsSuccess(result))
    {
        return result;
    }
    pos += size1;
    return RC_S_OK;
}

inline bool RCArjDefs::TestMarkerCandidate(const byte_t *p, unsigned maxSize)
{
    if (p[0] != RCArjDefs::kSig0 || p[1] != RCArjDefs::kSig1)
    {
        return false;
    }
    uint32_t blockSize = Get16(p + 2);
    p += 4;
    if (p[6] != RCArjDefs::kArchiveHeader ||
        p[0] > blockSize ||
        maxSize < 2 + 2 + blockSize + 4 ||
        blockSize < RCArjDefs::kBlockSizeMin || blockSize > RCArjDefs::kBlockSizeMax ||
        p[28] > 8) // EncryptionVersion
    {
        return false;
    }
    // return (Get32(p + blockSize) == CrcCalc(p, blockSize));
    return true;
}

HResult RCArjDefs::FindAndReadMarker(ISequentialInStream *stream, const uint64_t *searchHeaderSizeLimit, uint64_t &position)
{
    HResult result;
    position = 0;
    const int kMarkerSizeMin = 2 + 2 + RCArjDefs::kBlockSizeMin + 4;
    const int kMarkerSizeMax = 2 + 2 + RCArjDefs::kBlockSizeMax + 4;

    RCByteBuffer byteBuffer;
    const uint32_t kBufSize = 1 << 16;
    byteBuffer.SetCapacity(kBufSize);
    byte_t *buf = byteBuffer.data();

    size_t processedSize = kMarkerSizeMax;
    result = RCStreamUtils::ReadStream(stream, buf, &processedSize);
    if (!IsSuccess(result))
    {
        return result;
    }
    if (processedSize < kMarkerSizeMin)
    {
        return RC_S_FALSE;
    }
    if (TestMarkerCandidate(buf, (unsigned)processedSize))
    {
        return RC_S_OK;
    }

    uint32_t numBytesPrev = (uint32_t)processedSize - 1;
    memmove(buf, buf + 1, numBytesPrev);
    uint64_t curTestPos = 1;
    for (;;)
    {
        if (searchHeaderSizeLimit != NULL)
        {
            if (curTestPos > *searchHeaderSizeLimit)
            {
                return RC_S_FALSE;
            }
        }
        processedSize = kBufSize - numBytesPrev;
        result = RCStreamUtils::ReadStream(stream, buf + numBytesPrev, &processedSize);
        if (!IsSuccess(result))
        {
            return result;
        }
        uint32_t numBytesInBuffer = numBytesPrev + (uint32_t)processedSize;
        if (numBytesInBuffer < kMarkerSizeMin)
        {
            return RC_S_FALSE;
        }
        uint32_t numTests = numBytesInBuffer - kMarkerSizeMin + 1;
        uint32_t pos;
        for (pos = 0; pos < numTests; pos++)
        {
            for (; buf[pos] != RCArjDefs::kSig0 && pos < numTests; pos++);
            if (pos == numTests)
            {
                break;
            }
            if (TestMarkerCandidate(buf + pos, numBytesInBuffer - pos))
            {
                position = curTestPos + pos;
                return RC_S_OK;
            }
        }
        curTestPos += pos;
        numBytesPrev = numBytesInBuffer - numTests;
        memmove(buf, buf + numTests, numBytesPrev);
    }
}

void RCArjDefs::SetTime(uint32_t dosTime, RCVariant &prop)
{
    if (dosTime == 0)
    {
        return;
    }
    RC_FILE_TIME localFileTime, utc;
    if (RCFileTime::DosTimeToFileTime(dosTime, localFileTime))
    {
        if (!RCFileTime::LocalFileTimeToFileTime(localFileTime, utc))
        {
            utc.u32HighDateTime = utc.u32LowDateTime = 0;
        }
    }
    else
    {
        utc.u32HighDateTime = utc.u32LowDateTime = 0;
    }
    uint64_t tt;
    tt = utc.u32HighDateTime;
    tt = tt << 32;
    tt += utc.u32LowDateTime;
    prop = tt;
}

void RCArjDefs::SetHostOS(byte_t hostOS, RCVariant &prop)
{
    prop = hostOS < kNumHostOSes ? RCString(kHostOS[hostOS]) : RCString(kUnknownOS);
}

void RCArjDefs::SetUnicodeString(const RCStringA &s, RCVariant &prop)
{
    if (!s.empty())
    {
        prop = RCStringConvert::MultiByteToUnicodeString(s, RC_CP_OEMCP);
    }
}


END_NAMESPACE_RCZIP
