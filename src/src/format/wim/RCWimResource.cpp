/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "format/wim/RCWimResource.h"
#include "algorithm/CpuArch.h"

#define Get64(p) GetUi64(p)

/////////////////////////////////////////////////////////////////
//RCWimResource class implementation

BEGIN_NAMESPACE_RCZIP

RCWimResource::RCWimResource():
    m_packSize(0),
    m_offset(0),
    m_unpackSize(0),
    m_flags(0)
{
}

RCWimResource::~RCWimResource()
{
}

void RCWimResource::Parse(const byte_t* p) 
{
    m_flags = p[7];
    m_packSize = Get64(p) & (((uint64_t)1 << 56) - 1);
    m_offset = Get64(p + 8);
    m_unpackSize = Get64(p + 16);
}

bool RCWimResource::IsCompressed() const
{
    return (m_flags & NWimResourceFlags::s_compressed) != 0;
}

bool RCWimResource::IsMetadata() const
{
    return (m_flags & NWimResourceFlags::s_metadata) != 0;
}

bool RCWimResource::IsEmpty() const
{
    return (m_unpackSize == 0) ;
}

END_NAMESPACE_RCZIP
