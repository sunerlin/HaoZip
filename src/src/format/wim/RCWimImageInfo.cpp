/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "format/wim/RCWimImageInfo.h"
#include "common/RCXml.h"
#include "common/RCStringUtilA.h"
#include "common/RCStringConvert.h"

/////////////////////////////////////////////////////////////////
//RCWimImageInfo class implementation

BEGIN_NAMESPACE_RCZIP

RCWimImageInfo::RCWimImageInfo():
    m_cTimeDefined(false), 
    m_mTimeDefined(false), 
    m_nameDefined(false)
{
    
}

RCWimImageInfo::~RCWimImageInfo()
{
}

bool RCWimImageInfo::ParseNumber64(const RCStringA& s, uint64_t& res)
{
    const char* end = NULL ;
    if(RCStringUtilA::Left(s,2) == "0x")
    {
        if (s.size() == 2)
        {
            return false;
        }
        res = RCStringUtilA::ConvertHexStringToUInt64((const char *)s.c_str() + 2, &end);
    }
    else
    {
        if (s.empty())
        {
            return false;
        }
        res = RCStringUtilA::ConvertStringToUInt64(s.c_str(), &end);
    }
    if(end)
    {
        return *end == 0 ;
    }
    else
    {
        return false ;
    }
}

bool RCWimImageInfo::ParseNumber32(const RCStringA& s, uint32_t& res)
{
    uint64_t res64 = 0 ;
    if (!ParseNumber64(s, res64) || res64 >= ((uint64_t)1 << 32))
    {
        return false;
    }
    res = (uint32_t)res64 ;
    return true;
}

void RCWimImageInfo::ParseTime(const RCXmlItem& item, bool& defined, RC_FILE_TIME& ft, const RCStringA& s)
{
    defined = false;
    int32_t cTimeIndex = item.FindSubTag(s);
    if (cTimeIndex >= 0)
    {
        const RCXmlItem& timeItem = item.GetSubItems()[cTimeIndex];
        uint32_t high = 0, low = 0;
        if (ParseNumber32(timeItem.GetSubStringForTag("HIGHPART"), high) &&
            ParseNumber32(timeItem.GetSubStringForTag("LOWPART"), low))
        {
            defined = true;
            ft.u32HighDateTime = high ;
            ft.u32LowDateTime = low ;
        }
    }
}

void RCWimImageInfo::Parse(const RCXmlItem& item)
{
    ParseTime(item, m_cTimeDefined, m_cTime, "CREATIONTIME");
    ParseTime(item, m_mTimeDefined, m_mTime, "LASTMODIFICATIONTIME");
    RCStringA subString = item.GetSubStringForTag("NAME") ;
    m_nameDefined = RCStringConvert::ConvertUTF8ToUnicode(subString.c_str(),subString.size(), m_name);
}

END_NAMESPACE_RCZIP
