/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#include "format/udf/RCUdfFile.h"
#include "algorithm/CpuArch.h"

#define Get16(p) GetUi16(p)
#define Get32(p) GetUi32(p)
#define Get64(p) GetUi64(p)

BEGIN_NAMESPACE_RCZIP

RCUdfFile::RCUdfFile(): 
    m_itemIndex(-1) 
{
}

RCString RCUdfFile::GetName() const 
{ 
    return m_id.GetString(); 
}

uint32_t RCUdfMyExtent::GetLen() const 
{ 
    return m_len & 0x3FFFFFFF; 
}

uint32_t RCUdfMyExtent::GetType() const 
{ 
    return m_len >> 30; 
}

bool RCUdfMyExtent::IsRecAndAlloc() const 
{ 
    return GetType() == RCUdfDefs::SHORT_ALLOC_DESC_TYPE_RECORDED_AND_ALLOCATED; 
}

void RCUdfItem::Parse(const byte_t *buf)
{
    m_size = Get64(buf + 56);
    m_numLogBlockRecorded = Get64(buf + 64);
    m_aTime.Parse(buf + 72);
    m_mTime.Parse(buf + 84);
}

bool RCUdfItem::IsRecAndAlloc() const
{
    for (int32_t i = 0; i < static_cast<int32_t>(m_extents.size()); i++)
    {
        if (!m_extents[i].IsRecAndAlloc())
        {
            return false;
        }
    }
    return true;
}

uint64_t RCUdfItem::GetChunksSumSize() const
{
    if (m_isInline)
    {
        return m_inlineData.GetCapacity();
    }
    uint64_t size = 0;
    for (int32_t i = 0; i < static_cast<int32_t>(m_extents.size()); i++)
    {
        size += m_extents[i].GetLen();
    }
    return size;
}

bool RCUdfItem::CheckChunkSizes() const  
{  
    return GetChunksSumSize() == m_size; 
}

bool RCUdfItem::IsDir() const 
{ 
    return m_icbTag.IsDir(); 
}

HResult RCUdfTag::Parse(const byte_t *buf, size_t size)
{
    if (size < 16)
    {
        return RC_S_FALSE;
    }
    byte_t sum = 0;
    int32_t i;
    for (i = 0; i <  4; i++) 
    {
        sum = sum + buf[i];
    }
    for (i = 5; i < 16; i++) 
    {
        sum = sum + buf[i];
    }
    if (sum != buf[4] || buf[5] != 0) 
    {
        return RC_S_FALSE;
    }
    m_id = Get16(buf);
    m_version = Get16(buf + 2);
    uint16_t crc = Get16(buf + 8);
    uint16_t crcLen = Get16(buf + 10);
    if (size >= 16 + (size_t)crcLen)
    {
        if (crc == RCUdfCrc16::Crc16Calc(buf + 16, crcLen))
        {
            return RC_S_OK;
        }
    }
    return RC_S_FALSE;
}

bool RCUdfFileId::IsItLinkParent() const 
{ 
    return (m_fileCharacteristics & RCUdfDefs::s_fileidCharacsParent) != 0; 
}

HResult RCUdfFileId::Parse(const byte_t *p, size_t size, size_t &processed)
{
    HResult result;
    processed = 0;
    if (size < 38)
    {
        return RC_S_FALSE;
    }
    RCUdfTag tag;
    result = tag.Parse(p, size);
    if (!IsSuccess(result))
    {
        return result;
    }
    if (tag.m_id != RCUdfDefs::DESC_TYPE_FILEID)
    {
        return RC_S_FALSE;
    }
    m_fileCharacteristics = p[18];
    uint32_t idLen = p[19];
    m_icb.Parse(p + 20);
    uint32_t  impLen = Get16(p + 36);
    if (size < 38 + idLen + impLen)
    {
        return RC_S_FALSE;
    }
    processed = 38;
    processed += impLen;
    m_id.Parse(p + processed, idLen);
    processed += idLen;
    for (;(processed & 3) != 0; processed++)
    {
        if (p[processed] != 0)
        {
            return RC_S_FALSE;
        }
    }
    return (processed <= size) ? RC_S_OK : RC_S_FALSE;
}

void RCUdfExtent::Parse(const byte_t *buf)
{
    m_len = Get32(buf);
    m_pos = Get32(buf + 4);
}

END_NAMESPACE_RCZIP
