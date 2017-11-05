/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "format/zip/RCZipItem.h"
#include "algorithm/CpuArch.h"
#include "format/common/RCItemNameUtils.h"
#include "base/RCWindowsDefs.h" 

/////////////////////////////////////////////////////////////////
//RCZipItem class implementation

BEGIN_NAMESPACE_RCZIP

bool operator==(const RCZipVersion &v1, const RCZipVersion &v2)
{
    return (v1.m_version == v2.m_version) && (v1.m_hostOS == v2.m_hostOS);
}

bool operator!=(const RCZipVersion &v1, const RCZipVersion &v2)
{
    return !(v1 == v2);
}

bool RCZipExtraSubBlock::ExtractNtfsTime(int index, RC_FILE_TIME& ft) const
{
    ft.u32HighDateTime = 0;
    ft.u32LowDateTime  = 0;
    uint32_t size = (uint32_t)m_data.GetCapacity();

    if (m_id != RCZipHeader::NExtraID::kNTFS || size < 32)
    {
        return false;
    }

    const byte_t *p = m_data.data();
    p    += 4; // for reserved
    size -= 4;

    while (size > 4)
    {
        uint16_t tag      = GetUi16(p);
        uint32_t attrSize = GetUi16(p + 2);

        p    += 4;
        size -= 4;

        if (attrSize > size)
        {
            attrSize = size;
        }

        if (tag == RCZipHeader::NNtfsExtra::kTagTime && attrSize >= 24)
        {
            p += 8 * index;
            ft.u32LowDateTime = GetUi32(p);
            ft.u32HighDateTime = GetUi32(p + 4);

            return true;
        }

        p    += attrSize;
        size -= attrSize;
    }

    return false;
}

bool RCZipExtraSubBlock::ExtractUnixTime(int index, uint32_t& res) const
{
    res = 0;
    uint32_t size = (uint32_t)m_data.GetCapacity();
    if (m_id != RCZipHeader::NExtraID::kUnixTime || size < 5)
    {
        return false;
    }
    const byte_t *p = (const byte_t *)m_data.data() ;
    byte_t flags = *p++;
    size--;
    for (int32_t i = 0; i < 3; i++)
    {
        if ((flags & (1 << i)) != 0)
        {
            if (size < 4)
            {
                return false;
            }
            if (index == i)
            {
                res = GetUi32(p);
                return true;
            }
            p += 4;
            size -= 4;
        }
    }
    return false;
}

bool RCZipLocalItem::IsDir() const
{
    return RCItemNameUtils::HasTailSlash(m_name, GetCodePage());
}

bool RCZipItem::IsDir() const
{
    if (RCItemNameUtils::HasTailSlash(m_name, GetCodePage()))
    {
        return true;
    }

    if (!m_fromCentral)
    {
        return false;
    }

    uint16_t highAttributes = uint16_t((m_externalAttributes >> 16 ) & 0xFFFF);

    switch(m_madeByVersion.m_hostOS)
    {
    case RCZipHeader::NHostOS::kAMIGA:
        switch (highAttributes & RCZipHeader::NAmigaAttribute::kIFMT)
        {
        case RCZipHeader::NAmigaAttribute::kIFDIR: 
            {
                return true;
            }

        case RCZipHeader::NAmigaAttribute::kIFREG: 
            {
                return false;
            }

        default: 
            {
                return false; 
            }
        }

    case RCZipHeader::NHostOS::kFAT:
    case RCZipHeader::NHostOS::kNTFS:
    case RCZipHeader::NHostOS::kHPFS:
    case RCZipHeader::NHostOS::kVFAT:
        {
            return ((m_externalAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0);
        }

    case RCZipHeader::NHostOS::kAtari:
    case RCZipHeader::NHostOS::kMac:
    case RCZipHeader::NHostOS::kVMS:
    case RCZipHeader::NHostOS::kVM_CMS:
    case RCZipHeader::NHostOS::kAcorn:
    case RCZipHeader::NHostOS::kMVS:
        {
            return false; 
        }

    default:
        {
            /*
            switch (highAttributes & RCZipHeader::NUnixAttribute::kIFMT)
            {
            case RCZipHeader::NUnixAttribute::kIFDIR:
            return true;
            default:
            return false;
            }
            */
            return false;
        }
    }
}

uint32_t RCZipLocalItem::GetWinAttributes() const
{
    uint32_t winAttributes = 0;
    if (IsDir())
    {
        winAttributes |= FILE_ATTRIBUTE_DIRECTORY;
    }

    return winAttributes;
}

uint32_t RCZipItem::GetWinAttributes() const
{
    uint32_t winAttributes = 0;
    switch(m_madeByVersion.m_hostOS)
    {
    case RCZipHeader::NHostOS::kFAT:
    case RCZipHeader::NHostOS::kNTFS:
        if (m_fromCentral)
        {
            winAttributes = m_externalAttributes;
        }
        break;
    default:
        winAttributes = 0; // must be converted from unix value;
    }

    if (IsDir())       // test it;
    {
        winAttributes |= FILE_ATTRIBUTE_DIRECTORY;
    }

    return winAttributes;
}

void RCZipLocalItem::SetFlagBits(int startBitNumber, int numBits, int value)
{
    uint16_t mask = (uint16_t)(((1 << numBits) - 1) << startBitNumber);
    m_flags &= ~mask;
    m_flags |= value << startBitNumber;
}

void RCZipLocalItem::SetBitMask(int bitMask, bool enable)
{
    if(enable)
    {
        m_flags |= bitMask;
    }
    else
    {
        m_flags &= ~bitMask;
    }
}

void RCZipLocalItem::SetEncrypted(bool encrypted)
{ 
    SetBitMask(RCZipHeader::NFlags::kEncrypted, encrypted); 
}

void RCZipLocalItem::SetUtf8(bool isUtf8) 
{ 
    SetBitMask(RCZipHeader::NFlags::kUtf8, isUtf8);
}

END_NAMESPACE_RCZIP
