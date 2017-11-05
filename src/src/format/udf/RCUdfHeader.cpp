/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#include "format/udf/RCUdfHeader.h"
#include "algorithm/CpuArch.h"

#define Get16(p) GetUi16(p)
#define Get32(p) GetUi32(p)
#define Get64(p) GetUi64(p)

BEGIN_NAMESPACE_RCZIP

unsigned RCUdfTime::GetType() const 
{ 
    return m_data[1] >> 4; 
}

bool RCUdfTime::IsLocal() const 
{ 
    return GetType() == 1; 
}

int32_t RCUdfTime::GetMinutesOffset() const
{
    int32_t t = (m_data[0] | ((uint16_t)m_data[1] << 8)) & 0xFFF;
    if ((t >> 11) != 0)
    {
        t -= (1 << 12);
    }
    return (t > (60 * 24) || t < -(60 * 24)) ? 0 : t;
}

unsigned RCUdfTime::GetYear() const 
{ 
    return (m_data[2] | ((uint16_t)m_data[3] << 8)); 
}

void RCUdfTime::Parse(const byte_t *buf)
{
    memcpy(m_data, buf, sizeof(m_data));
}

RCUdfPartition::RCUdfPartition(): 
    m_volIndex(-1),
    m_map(new RCUdfMap32)
{
}

RCUdfPartition::RCUdfPartition(const RCUdfPartition& val)
{
    m_number = val.m_number;
    m_pos = val.m_pos;
    m_len = val.m_len;
    m_volIndex = val.m_volIndex;
    m_map = val.m_map;
}

RCUdfPartition& RCUdfPartition::operator=(const RCUdfPartition& val)
{
    if (this != &val)
    {
        m_number = val.m_number;
        m_pos = val.m_pos;
        m_len = val.m_len;
        m_volIndex = val.m_volIndex;
        m_map = val.m_map;
    }
    return *this;
}

void RCUdfLogBlockAddr::Parse(const byte_t *buf)
{
    m_pos = Get32(buf);
    m_partitionRef = Get16(buf + 4);
}

void RCUdfShortAllocDesc::Parse(const byte_t *buf)
{
    m_len = Get32(buf);
    m_pos = Get32(buf + 4);
}

uint32_t RCUdfLongAllocDesc::GetLen() const 
{ 
    return m_len & 0x3FFFFFFF; 
}

uint32_t RCUdfLongAllocDesc::GetType() const 
{ 
    return m_len >> 30; 
}

bool RCUdfLongAllocDesc::IsRecAndAlloc() const 
{ 
    return GetType() == RCUdfDefs::SHORT_ALLOC_DESC_TYPE_RECORDED_AND_ALLOCATED; 
}

void RCUdfLongAllocDesc::Parse(const byte_t *buf)
{
    m_len = Get32(buf);
    m_location.Parse(buf + 4);
}

bool RCUdfIcbTag::IsDir() const 
{ 
    return m_fileType == RCUdfDefs::ICB_FILE_TYPE_DIR; 
}

int32_t RCUdfIcbTag::GetDescriptorType() const 
{ 
    return m_flags & 3; 
}

void RCUdfIcbTag::Parse(const byte_t *p)
{
    m_fileType = p[11];
    m_flags = Get16(p + 18);
}

END_NAMESPACE_RCZIP
