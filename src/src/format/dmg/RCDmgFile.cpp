/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "format/dmg/RCDmgFile.h"

/////////////////////////////////////////////////////////////////
//RCDmgFile class implementation

BEGIN_NAMESPACE_RCZIP

RCDmgFile::RCDmgFile():
    m_startPos(0)
{
}

uint64_t RCDmgFile::GetUnpackSize() const
{
    uint64_t size = 0;
    size_t count = m_blocks.size() ;
    for (size_t i = 0; i < count ; ++i)
    {
        size += m_blocks[i]->m_unpSize;
    }
    return size ;
};

uint64_t RCDmgFile::GetPackSize() const
{
    uint64_t size = 0 ;
    size_t count = m_blocks.size() ;
    for (size_t i = 0; i < count; ++i)
    {
        size += m_blocks[i]->m_packSize ;
    }
    return size ;
};

END_NAMESPACE_RCZIP
