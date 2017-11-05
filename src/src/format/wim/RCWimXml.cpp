/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "format/wim/RCWimXml.h"
#include "common/RCStringBuffer.h"
#include "common/RCStringConvert.h"
#include "common/RCXml.h"
#include "algorithm/CpuArch.h"

#define Get16(p) GetUi16(p)
#define Get32(p) GetUi32(p)
#define Get64(p) GetUi64(p)

/////////////////////////////////////////////////////////////////
//RCWimXml class implementation

BEGIN_NAMESPACE_RCZIP

RCWimXml::RCWimXml():
    m_volIndex(0)
{
}

RCWimXml::~RCWimXml()
{
}

void RCWimXml::Parse()
{
    size_t size = m_data.GetCapacity() ;
    if (size < 2 || (size & 1) != 0 || (size > 1 << 24))
    {
        return;
    }
    const byte_t* p = m_data.data() ;
    if (Get16(p) != 0xFEFF)
    {
        return;
    }
    RCString s;
    {
        RCStringBuffer sBuffer(&s) ;
        RCString::value_type* chars = sBuffer.GetBuffer((int)size / 2 + 1);
        for (size_t i = 2; i < size; i += 2)
        {
            *chars++ = (RCString::value_type)Get16(p + i);
        }
        *chars = 0 ;
        sBuffer.ReleaseBuffer();
    }

    RCStringA utf;
    if (!RCStringConvert::ConvertUnicodeToUTF8(s.c_str(),s.size(), utf))
    {
        return;
    }
    RCXml xml;
    if (!xml.Parse(utf))
    {
        return;
    }
    if (xml.GetRoot().GetName() != "WIM")
    {
        return;
    }

    for (int32_t i = 0; i < (int32_t)xml.GetRoot().GetSubItems().size(); ++i)
    {
        const RCXmlItem& item = xml.GetRoot().GetSubItems()[i];
        if (item.IsTagged("IMAGE"))
        {
            RCWimImageInfo imageInfo;
            imageInfo.Parse(item);
            m_images.push_back(imageInfo);
        }
    }
}

END_NAMESPACE_RCZIP
