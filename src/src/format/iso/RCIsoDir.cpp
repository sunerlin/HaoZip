/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#include "format/iso/RCIsoDir.h"

BEGIN_NAMESPACE_RCZIP

void RCIsoDir::Clear()
{
    m_parent = 0;
    m_subItems.clear();
}

int32_t RCIsoDir::GetLength(bool checkSusp, int32_t skipSize) const
{
    int32_t len = GetLengthCur(checkSusp, skipSize);
    if (m_parent != 0)
    {
        if (m_parent->m_parent != 0)
        {
            len += 1 + m_parent->GetLength(checkSusp, skipSize);
        }
    }
    return len;
}

int32_t RCIsoDir::GetLengthU() const
{
    int32_t len = (int32_t)(m_fileId.GetCapacity() / 2);
    if (m_parent != 0)
    {
        if (m_parent->m_parent != 0)
        {
            len += 1 + m_parent->GetLengthU();
        }
    }
    return len;
}

RCStringA RCIsoDir::GetPath(bool checkSusp, int32_t skipSize) const
{
    RCStringA s;
    int32_t len = GetLength(checkSusp, skipSize);
    RCStringBufferA lbuffer(&s);
    RCStringA::value_type* p = lbuffer.GetBuffer(len + 1);
    p += len;
    *p = 0;
    const RCIsoDir *cur = this;
    for (;;)
    {
        int32_t curLen = cur->GetLengthCur(checkSusp, skipSize);
        p -= curLen;
        memmove(p, (const char *)(const byte_t *)cur->GetNameCur(checkSusp, skipSize), curLen);
        cur = cur->m_parent;
        if (cur == 0)
        {
            break;
        }
        if (cur->m_parent == 0)
        {
            break;
        }
        p--;
        *p = CHAR_PATH_SEPARATOR;
    }
    lbuffer.ReleaseBuffer();
    return s;
}

RCString RCIsoDir::GetPathU() const
{
    RCString s;
    RCStringBuffer lbuffer(&s);
    int32_t len = GetLengthU();
    RCString::value_type *p = lbuffer.GetBuffer(len +  1);
    p += len;
    *p = 0;
    const RCIsoDir *cur = this;
    for (;;)
    {
        int32_t curLen = (int32_t)(cur->m_fileId.GetCapacity() / 2);
        p -= curLen;
        for (int32_t i = 0; i < curLen; i++)
        {
            byte_t b0 = cur->m_fileId[i * 2];
            byte_t b1 = cur->m_fileId[i * 2 + 1];
            p[i] = (RCString::value_type)(((RCString::value_type)b0 << 8) | b1);
        }
        cur = cur->m_parent;
        if (cur == 0)
        {
            break;
        }
        if (cur->m_parent == 0)
        {
            break;
        }
        p--;
        *p = WCHAR_PATH_SEPARATOR;
    }
    lbuffer.ReleaseBuffer();
    return s;
}

END_NAMESPACE_RCZIP
