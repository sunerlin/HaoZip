/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#include "format/lzma/RCLzmaHeader.h"
#include "algorithm/CpuArch.h"

BEGIN_NAMESPACE_RCZIP

RCLzmaHeader::RCLzmaHeader():
    m_size(0),
    m_filterID(0)
{
    
}

uint32_t RCLzmaHeader::GetDicSize() const
{
    return GetUi32(m_lzmaProps + 1);
}

bool RCLzmaHeader::HasSize() const
{
    return (m_size != (uint64_t)(int64_t)-1) ;
}

bool RCLzmaHeader::CheckDicSize(const byte_t* p)
{
    uint32_t dicSize = GetUi32(p) ;
    for (int32_t i = 1; i <= 30; i++)
    {
        if (dicSize == ((uint32_t)2 << i) || dicSize == ((uint32_t)3 << i))
        {
            return true ;
        }
    }
    return (dicSize == 0xFFFFFFFF) ;
}

bool RCLzmaHeader::Parse(const byte_t* buf, bool isThereFilter)
{
    m_filterID = 0;
    if (isThereFilter)
    {
        m_filterID = buf[0];
    }
    const byte_t *sig = buf + (isThereFilter ? 1 : 0);
    for (int32_t i = 0; i < 5; i++)
    {
        m_lzmaProps[i] = sig[i];
    }
    m_size = GetUi64(sig + 5);
    return  m_lzmaProps[0] < 5 * 5 * 9 &&
            m_filterID < 2 &&
            (!HasSize() || m_size < ((uint64_t)1 << 56)) &&
            CheckDicSize(m_lzmaProps + 1) ;
}

END_NAMESPACE_RCZIP
