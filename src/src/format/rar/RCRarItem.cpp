/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "format/rar/RCRarItem.h"
#include "base/RCWindowsDefs.h"

/////////////////////////////////////////////////////////////////
//RCRarItem class implementation

BEGIN_NAMESPACE_RCZIP

bool RCRarItem::IgnoreItem() const
{
    switch(m_hostOS)
    {
    case RCRarHeader::NFile::kHostMSDOS:
    case RCRarHeader::NFile::kHostOS2:
    case RCRarHeader::NFile::kHostWin32:
        {
            return ((m_attrib & RCRarHeader::NFile::kLabelFileAttribute) != 0);
        }
    }
    return false;
}

bool RCRarItem::IsDir() const
{
    if (GetDictSize() == RCRarHeader::NFile::kDictDirectoryValue)
    {
        return true;
    }

    switch(m_hostOS)
    {
    case RCRarHeader::NFile::kHostMSDOS:
    case RCRarHeader::NFile::kHostOS2:
    case RCRarHeader::NFile::kHostWin32:
        if ((m_attrib & FILE_ATTRIBUTE_DIRECTORY) != 0)
        {
            return true;
        }
    }
    return false;
}

uint32_t RCRarItem::GetWinAttributes() const
{
    uint32_t winAttributes;
    switch(m_hostOS)
    {
    case RCRarHeader::NFile::kHostMSDOS:
    case RCRarHeader::NFile::kHostOS2:
    case RCRarHeader::NFile::kHostWin32:
        {
            winAttributes = m_attrib;
        }
        break;

    default:
        {
            winAttributes = 0; // must be converted from unix value;
        }
    }

    if (IsDir())
    {
        winAttributes |= RCRarHeader::NFile::kWinFileDirectoryAttributeMask;
    }

    return winAttributes;
}

END_NAMESPACE_RCZIP
