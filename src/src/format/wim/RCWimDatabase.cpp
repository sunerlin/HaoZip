/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "format/wim/RCWimDatabase.h"

/////////////////////////////////////////////////////////////////
//RCWimDatabase class implementation

BEGIN_NAMESPACE_RCZIP

RCWimDatabase::RCWimDatabase()
{
}

RCWimDatabase::~RCWimDatabase()
{
}

uint64_t RCWimDatabase::GetUnpackSize() const
{
    uint64_t res = 0;
    for (size_t i = 0; i < m_streams.size(); ++i)
    {
        res += m_streams[i].m_resource.m_unpackSize;
    }
    return res;
}

uint64_t RCWimDatabase::GetPackSize() const
{
    uint64_t res = 0;
    for (size_t i = 0; i < m_streams.size(); ++i)
    {
        res += m_streams[i].m_resource.m_packSize;
    }
    return res;
}

void RCWimDatabase::Clear()
{
    m_streams.clear();
    m_items.clear();
}

END_NAMESPACE_RCZIP
