/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#include "format/iso/RCIsoItem.h"

BEGIN_NAMESPACE_RCZIP

bool RCIsoRecordingDateTime::GetFileTime(RC_FILE_TIME &ft) const
{
    uint64_t value;
    bool res = RCFileTime::GetSecondsSince1601(m_year + 1900, m_month, m_day, m_hour, m_minute, m_second, value);
    if (res)
    {
        value -= (uint64_t)((int64_t)m_gmtOffset * 15 * 60);
        value *= 10000000;
    }
    ft.u32LowDateTime = static_cast<uint32_t>(value);
    ft.u32HighDateTime = static_cast<uint32_t>(value >> 32);
    return res;
}

bool RCIsoDirRecord::IsDir() const 
{ 
    return  (m_fileFlags & RCIsoHeaderDefs::kDirectory) != 0; 
}

bool RCIsoDirRecord::IsSystemItem() const
{
    if (m_fileId.GetCapacity() != 1)
    {
        return false;
    }
    byte_t b = *m_fileId.data();
    return (b == 0 || b == 1);
}

const byte_t* RCIsoDirRecord::FindSuspName(int32_t skipSize, int32_t &lenRes) const
{
    lenRes = 0;
    const byte_t *p = m_systemUse.data() + skipSize;
    int32_t length = (int32_t)(m_systemUse.GetCapacity() - skipSize);
    while (length >= 5)
    {
        int32_t len = p[2];
        if (p[0] == 'N' && p[1] == 'M' && p[3] == 1)
        {
            lenRes = len - 5;
            return p + 5;
        }
        p += len;
        length -= len;
    }
    return 0;
}

int32_t RCIsoDirRecord::GetLengthCur(bool checkSusp, int32_t skipSize) const
{
    if (checkSusp)
    {
        int32_t len;
        const byte_t *res = FindSuspName(skipSize, len);
        if (res != 0)
        {
            return len;
        }
    }
    return (int32_t)m_fileId.GetCapacity();
}

const byte_t* RCIsoDirRecord::GetNameCur(bool checkSusp, int32_t skipSize) const
{
    if (checkSusp)
    {
        int32_t len;
        const byte_t *res = FindSuspName(skipSize, len);
        if (res != 0)
        {
            return res;
        }
    }
    return m_fileId.data();
}

bool RCIsoDirRecord::CheckSusp(const byte_t *p, int32_t &startPos) const
{
    if (p[0] == 'S' && p[1] == 'P' && p[2] == 0x7 && p[3] == 0x1 &&
            p[4] == 0xBE && p[5] == 0xEF)
    {
        startPos = p[6];
        return true;
    }
    return false;
}

bool RCIsoDirRecord::CheckSusp(int32_t &startPos) const
{
    const byte_t *p = m_systemUse.data();
    int32_t length = (int32_t)m_systemUse.GetCapacity();
    const int32_t kMinLen = 7;
    if (length < kMinLen)
    {
        return false;
    }
    if (CheckSusp(p, startPos))
    {
        return true;
    }
    const int32_t kOffset2 = 14;
    if (length < kOffset2 + kMinLen)
    {
        return false;
    }
    return CheckSusp(p + kOffset2, startPos);
}

END_NAMESPACE_RCZIP
