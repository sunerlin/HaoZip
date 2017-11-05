/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#include "format/arj/RCArjItem.h"
#include "algorithm/CpuArch.h"

#define Get16(p) GetUi16(p)
#define Get32(p) GetUi32(p)

BEGIN_NAMESPACE_RCZIP

bool RCArjItem::IsEncrypted() const 
{ 
    return (m_flags & RCArjDefs::kGarbled) != 0; 
}

bool RCArjItem::IsDir() const 
{ 
    return (m_fileType == RCArjDefs::kDirectory); 
}

bool RCArjItem::IsSplitAfter() const
{
    return (m_flags & RCArjDefs::kVolume) != 0 ;
}

bool RCArjItem::IsSplitBefore() const
{
    return (m_flags & RCArjDefs::kExtFile) != 0 ;
}

uint32_t RCArjItem::GetWinAttributes() const
{
    uint32_t winAtrributes;
    switch(m_hostOS)
    {
    case RCArjDefs::kMSDOS:
    case RCArjDefs::kWIN95:
        winAtrributes = m_fileAccessMode;
        break;

    default:
        winAtrributes = 0;
    }

    if (IsDir())
    {
        winAtrributes |= FILE_ATTRIBUTE_DIRECTORY;
    }
    return winAtrributes;
}

HResult RCArjItem::Parse(const byte_t *p, unsigned size)
{
    HResult result;
    if (size < RCArjDefs::kBlockSizeMin)
    {
        return RC_S_FALSE;
    }
    byte_t firstHeaderSize = p[0];
    m_version = p[1];
    m_extractVersion = p[2];
    m_hostOS = p[3];
    m_flags = p[4];
    m_method = p[5];
    m_fileType = p[6];
    // Reserved = p[7];
    m_mTime = Get32(p + 8);
    m_packSize = Get32(p + 12);
    m_size = Get32(p + 16);
    m_fileCRC = Get32(p + 20);
    // FilespecPositionInFilename = Get16(p + 24);
    m_fileAccessMode = Get16(p + 26);
    // FirstChapter = p[28];
    // FirstChapter = p[29];
    
    m_splitPos = 0;
    if (IsSplitBefore() && firstHeaderSize >= 34)
    {
        m_splitPos = Get32(p + 30);
    }
    
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

END_NAMESPACE_RCZIP
