/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "format/wim/RCWimHeader.h"
#include "RCWimInDefs.h"
#include "algorithm/CpuArch.h"

#define Get32(p) GetUi32(p)
#define Get16(p) GetUi16(p)

#define GetResource(p, res) res.Parse(p)

/////////////////////////////////////////////////////////////////
//RCWimHeader class implementation

BEGIN_NAMESPACE_RCZIP

RCWimHeader::RCWimHeader():
    m_flags(0),
    m_version(0),
    m_partNumber(0),
    m_numParts(0),
    m_numImages(0)
{
}

RCWimHeader::~RCWimHeader()
{
}

HResult RCWimHeader::Parse(const byte_t* p)
{
    uint32_t haderSize = Get32(p + 8);
    if (haderSize < 0x74)
    {
        return RC_S_FALSE;
    }
    m_version = Get32(p + 0x0C);
    m_flags = Get32(p + 0x10);
    if (!IsSupported())
    {
        return RC_S_FALSE;
    }
    uint32_t chunkSize = Get32(p + 0x14);
    if (chunkSize != s_kWimChunkSize && chunkSize != 0)
    {
        return RC_S_FALSE;
    }
    memcpy(m_guid, p + 0x18, 16);
    m_partNumber = Get16(p + 0x28);
    m_numParts = Get16(p + 0x2A);
    int offset = 0x2C;
    if (IsNewVersion())
    {
        m_numImages = Get32(p + offset);
        offset += 4;
    }
    GetResource(p + offset, m_offsetResource);
    GetResource(p + offset + 0x18, m_xmlResource);
    GetResource(p + offset + 0x30, m_metadataResource);
    return RC_S_OK;
}

bool RCWimHeader::IsCompressed() const
{
    return (m_flags & NWimHeaderFlags::s_compression) != 0;
}

bool RCWimHeader::IsSupported() const
{
    return (!IsCompressed() || (m_flags & NWimHeaderFlags::s_lzx) != 0 || (m_flags & NWimHeaderFlags::s_xpress) != 0 ) ;
}

bool RCWimHeader::IsLzxMode() const
{
    return (m_flags & NWimHeaderFlags::s_lzx) != 0;
}

bool RCWimHeader::IsSpanned() const
{
    return (!IsCompressed() || (m_flags & NWimHeaderFlags::s_spanned) != 0);
}

bool RCWimHeader::IsNewVersion() const
{
    return (m_version > 0x010C00);
}

bool RCWimHeader::AreFromOnArchive(const RCWimHeader& h)
{
    return (memcmp(m_guid, h.m_guid, sizeof(m_guid)) == 0) && (h.m_numParts == m_numParts);
}

END_NAMESPACE_RCZIP
