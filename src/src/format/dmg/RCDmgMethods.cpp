/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "format/dmg/RCDmgMethods.h"
#include "RCDmgDefs.h"
#include "format/dmg/RCDmgFile.h"
#include "common/RCStringUtil.h"

/////////////////////////////////////////////////////////////////
//RCDmgMethods class implementation

BEGIN_NAMESPACE_RCZIP

RCDmgMethods::RCDmgMethods()
{
}

void RCDmgMethods::Update(const RCDmgFile& file)
{
    size_t count = file.m_blocks.size() ;
    for (size_t i = 0; i < count ; ++i)
    {
        const RCDmgBlock& b = *file.m_blocks[i] ;
        int32_t index = RCVectorUtils::FindInSorted(m_types, b.m_type);
        if (index < 0)
        {
            index = RCVectorUtils::AddToUniqueSorted(m_types, b.m_type);
            RCDmgMethodStatPtr spStat(new RCDmgMethodStat) ;
            RCVectorUtils::Insert(m_stats, index, spStat);
        }
        RCDmgMethodStatPtr& m = m_stats[index] ;
        m->m_packSize += b.m_packSize;
        m->m_unpSize += b.m_unpSize;
        ++(m->m_numBlocks) ;
    }
}

RCString RCDmgMethods::GetString() const
{
    RCString res ;
    size_t count = m_types.size() ;
    for (size_t i = 0; i < count ; ++i)
    {
        if (i != 0)
        {
            res += _T(' ') ;
        }
        RCString::value_type buf[64] ;
        const RCString::value_type* s = NULL ;
        const RCDmgMethodStat& m = *m_stats[i] ;
        bool showPack = true ;
        uint32_t type = m_types[i] ;
        switch(type)
        {
            case RCDmgDefs::METHOD_ZERO_0: 
                s = _T("zero0") ; 
                showPack = (m.m_packSize != 0); 
                break;
            case RCDmgDefs::METHOD_ZERO_2: 
                s = _T("zero2"); 
                showPack = (m.m_packSize != 0); 
                break;
            case RCDmgDefs::METHOD_COPY:
                s = _T("copy") ;
                showPack = (m.m_unpSize != m.m_packSize); 
                break;
            case RCDmgDefs::METHOD_ADC:
                s = _T("adc") ; 
                break;
            case RCDmgDefs::METHOD_ZLIB:
                s = _T("zlib") ; 
                break;
            case RCDmgDefs::METHOD_BZIP2:
                s = _T("bzip2") ; 
                break;
            default: 
                RCStringUtil::ConvertUInt64ToString(type, buf); 
                s = buf ;
        }
        res += s;
        if (m.m_numBlocks != 1)
        {
            res += _T('[') ;
            RCStringUtil::ConvertUInt64ToString(m.m_numBlocks, buf);
            res += buf;
            res += _T(']') ;
        }
        res += _T('-') ;
        res += GetSizeString(m.m_unpSize);
        if (showPack)
        {
            res += _T('-');
            res += GetSizeString(m.m_packSize);
        }
    }
    return res;
}

RCString RCDmgMethods::GetSizeString(uint64_t value) const
{
    RCString::value_type s[64];
    RCString::value_type c;
    if (value < (uint64_t)20000)
    {
        c = 0;
    }
    else if (value < ((uint64_t)20000 << 10))
    {
        value >>= 10; 
        c = _T('K');
    }
    else if (value < ((uint64_t)20000 << 20))
    {
        value >>= 20; 
        c = _T('M');
    }
    else
    {
        value >>= 30;
        c = _T('G') ;
    }
    RCStringUtil::ConvertUInt64ToString(value, s);
    size_t p = RCStringUtil::StringLen(s);
    s[p++] = c ;
    s[p++] = _T('\0') ;
    return s ;
}

END_NAMESPACE_RCZIP
