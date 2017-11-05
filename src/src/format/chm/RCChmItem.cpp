/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "format/chm/RCChmItem.h"
#include "format/chm/RCChmDefs.h"
#include "common/RCStringConvert.h"
#include "common/RCStringUtil.h"
#include <functional>

/////////////////////////////////////////////////////////////////
//RCChmItem class implementation

BEGIN_NAMESPACE_RCZIP

static bool AreGuidsEqual(REFGUID g1, REFGUID g2)
{
    if (g1.Data1 != g2.Data1 ||
        g1.Data2 != g2.Data2 ||
        g1.Data3 != g2.Data3)
    {
        return false;
    }

    for (int32_t i = 0; i < 8; i++)
    {
        if (g1.Data4[i] != g2.Data4[i])
        {
            return false;
        }
    }
    return true;
}

static char GetHex(byte_t value)
{
    return (char)((value < 10) ? ('0' + value) : ('A' + (value - 10)));
}

static void PrintByte(byte_t b, RCStringA& s)
{
    s += GetHex(b >> 4);
    s += GetHex(b & 0xF);
}

static void PrintUInt16(uint16_t v, RCStringA& s)
{
    PrintByte((byte_t)(v >> 8), s);
    PrintByte((byte_t)v, s);
}

static void PrintUInt32(uint32_t v, RCStringA& s)
{
    PrintUInt16((uint16_t)(v >> 16), s);
    PrintUInt16((uint16_t)v, s);
}

RCStringA RCChmMethodInfo::GetGuidString() const
{
    RCStringA s;
    s += '{';
    PrintUInt32(m_guid.Data1, s);
    s += '-';
    PrintUInt16(m_guid.Data2, s);
    s += '-';
    PrintUInt16(m_guid.Data3, s);
    s += '-';
    PrintByte(m_guid.Data4[0], s);
    PrintByte(m_guid.Data4[1], s);
    s += '-';
    for (int32_t i = 2; i < 8; i++)
    {
        PrintByte(m_guid.Data4[i], s);
    }
    s += '}';
    return s;
}

bool RCChmMethodInfo::IsLzx() const
{
    if (AreGuidsEqual(m_guid, RCChmDefs::s_chmLzxGuid))
    {
        return true;
    }
    return AreGuidsEqual(m_guid, RCChmDefs::s_help2LzxGuid);
}

bool RCChmMethodInfo::IsDes() const
{
    return AreGuidsEqual(m_guid, RCChmDefs::s_desGuid);
}

RCString RCChmMethodInfo::GetName() const
{
    RCString s;
    if (IsLzx())
    {
        s = _T("LZX:");        
        s += RCStringUtil::ConvertUInt32ToString(m_lzxInfo.GetNumDictBits());
    }
    else
    {
        RCStringA s2;
        if (IsDes())
        {
            s2 = "DES";
        }
        else
        {
            s2 = GetGuidString();
            if (m_controlData.GetCapacity() > 0)
            {
                s2 += ':';
                for (size_t i = 0; i < m_controlData.GetCapacity(); i++)
                {
                    PrintByte(m_controlData[i], s2);
                }
            }
        }
        RCStringConvert::ConvertUTF8ToUnicode(s2.c_str(), s2.length(), s);
    }
    return s;
}

bool RCChmSectionInfo::IsLzx() const
{
    if (m_methods.size() != 1)
    {
        return false;
    }
    return m_methods[0].IsLzx();
}

RCString RCChmSectionInfo::GetMethodName() const
{
    RCString s;
    if (!IsLzx())
    {
        RCString temp;
        if (RCStringConvert::ConvertUTF8ToUnicode(m_name.c_str(), m_name.length(), temp))
        {
            s += temp;
        }
        s += _T(": ");
    }

    for (int32_t i = 0; i < (int32_t)m_methods.size(); i++)
    {
        if (i != 0)
        {
            s += _T(' ');
        }
        s += m_methods[i].GetName();
    }
    return s;
}

void RCChmFilesDatabase::SetIndices()
{
    for (int32_t i = 0; i < (int32_t)m_items.size(); i++)
    {
        const RCChmItem& item = m_items[i];
        if (item.IsUserItem() && item.m_name.length() != 1)
        {
            m_indices.push_back(i);
        }
    }
}

#define RINOZ(x) { int32_t __tt = (x); if (__tt < 0) return true; else if(__tt > 0) return false;}

class RCCompareItems:
    public std::binary_function<int32_t, int32_t ,bool>
{
public:

    /** 构造函数
    */
    RCCompareItems(const RCVector<RCChmItem>* items):
        m_chmItems(items)
    {        
    }

    /** 重载()操作符
    @param [in] p1 索引一
    @param [in] p2 索引二
    @是返回ture，否则返回false
    */
    int32_t operator()(const int32_t p1, const int32_t p2)
    {
        const RCChmItem& item1 = (*m_chmItems)[p1];
        const RCChmItem& item2 = (*m_chmItems)[p2];

        bool isDir1 = item1.IsDir();
        bool isDir2 = item2.IsDir();
        if (isDir1 && !isDir2)
        {
            return true;
        }

        if (isDir2)
        {
            if (isDir1)
            {
                int32_t result = MyCompare(p1, p2);
                if (result < 0)
                {
                    return true;
                }
                else
                {
                    return false;
                }
            }
            return false;
        }

        RINOZ(MyCompare(item1.m_section, item2.m_section));
        RINOZ(MyCompare(item1.m_offset, item2.m_offset));
        RINOZ(MyCompare(item1.m_size, item2.m_size));

        int32_t result = MyCompare(p1, p2);
        if (result < 0)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

private:

    /** chm项
    */
    const RCVector<RCChmItem>* m_chmItems;
};

void RCChmFilesDatabase::Sort()
{
    std::stable_sort(m_indices.begin(), m_indices.end() ,RCCompareItems(&m_items)) ;
}

bool RCChmFilesDatabase::Check()
{
    uint64_t maxPos = 0;
    uint64_t prevSection = 0;
    for(int32_t i = 0; i < (int32_t)m_indices.size(); i++)
    {
        const RCChmItem& item = m_items[m_indices[i]];
        if (item.m_section == 0 || item.IsDir())
        {
            continue;
        }

        if (item.m_section != prevSection)
        {
            prevSection = item.m_section;
            maxPos = 0;
            continue;
        }

        if (item.m_offset < maxPos)
        {
            return false;
        }

        maxPos = item.m_offset + item.m_size;
        if (maxPos < item.m_offset)
        {
            return false;
        }
    }
    return true;
}

END_NAMESPACE_RCZIP
