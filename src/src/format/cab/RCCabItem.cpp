/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#include "format/cab/RCCabItem.h"

BEGIN_NAMESPACE_RCZIP

byte_t RCCabFolder::GetCompressionMethod() const 
{ 
    return static_cast<byte_t>(m_compressionTypeMajor & 0xF); 
}

uint64_t RCCabItem::GetEndOffset() const 
{ 
    return static_cast<uint64_t>(m_offset + m_size); 
}

uint32_t RCCabItem::GetWinAttributes() const 
{ 
    return (m_attributes & ~RCCabHeaderDefs::kFileNameIsUTFAttributeMask); 
}

bool RCCabItem::IsNameUTF() const 
{ 
    return (m_attributes & RCCabHeaderDefs::kFileNameIsUTFAttributeMask) != 0; 
}

bool RCCabItem::IsDir() const 
{
#ifdef RCZIP_OS_WIN
    return (m_attributes & FILE_ATTRIBUTE_DIRECTORY) != 0; 
#endif
}

bool RCCabItem::ContinuedFromPrev() const
{
    return  (m_folderIndex == RCCabHeaderDefs::kContinuedFromPrev) || 
        (m_folderIndex == RCCabHeaderDefs::kContinuedPrevAndNext);
}

bool RCCabItem::ContinuedToNext() const
{
    return  (m_folderIndex == RCCabHeaderDefs::kContinuedToNext) || 
        (m_folderIndex == RCCabHeaderDefs::kContinuedPrevAndNext);
}

int32_t RCCabItem::GetFolderIndex(int32_t numFolders) const
{
    if (ContinuedFromPrev())
    {
        return 0;
    }
    if (ContinuedToNext())
    {
        return (numFolders - 1);
    }
    return m_folderIndex;
}

END_NAMESPACE_RCZIP
