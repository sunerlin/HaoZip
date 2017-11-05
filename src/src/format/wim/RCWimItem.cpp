/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "format/wim/RCWimItem.h"

/////////////////////////////////////////////////////////////////
//RCWimItem class implementation

BEGIN_NAMESPACE_RCZIP

RCWimItem::RCWimItem():
    m_hasMetadata(true), 
    m_streamIndex(-1) 
{
}

RCWimItem::~RCWimItem()
{
}

bool RCWimItem::isDir() const
{
    return m_hasMetadata && ((m_attrib & 0x10) != 0);
}

bool RCWimItem::HasStream() const
{
    for (int32_t i = 0; i < s_kWimHashSize; ++i)
    {
        if (m_hash[i] != 0)
        {
            return true;
        }
    }
    return false;
}

END_NAMESPACE_RCZIP
