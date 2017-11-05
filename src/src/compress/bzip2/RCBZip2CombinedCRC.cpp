/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "compress/bzip2/RCBZip2CombinedCRC.h"

/////////////////////////////////////////////////////////////////
//RCBZip2CombinedCRC class implementation

BEGIN_NAMESPACE_RCZIP

RCBZip2CombinedCRC::RCBZip2CombinedCRC():
    m_value(0)
{
}

RCBZip2CombinedCRC::~RCBZip2CombinedCRC()
{
}

void RCBZip2CombinedCRC::Init()
{
    m_value = 0;
}

void RCBZip2CombinedCRC::Update(uint32_t v)
{
    m_value = ((m_value << 1) | (m_value >> 31)) ^ v;
}

uint32_t RCBZip2CombinedCRC::GetDigest() const
{
    return m_value ;
}

END_NAMESPACE_RCZIP
