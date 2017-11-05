/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#include "format/cab/RCCabCheckSum2.h"

BEGIN_NAMESPACE_RCZIP

RCCabCheckSum2::RCCabCheckSum2(): 
    m_value(0)
{
}

void RCCabCheckSum2::Init() 
{ 
    m_value = 0;  
    m_pos = 0; 
}

void RCCabCheckSum2::FinishDataUpdate()
{
    for (int32_t i = 0; i < m_pos; i++)
    {
        m_value ^= ((uint32_t)(m_hist[i])) << (8 * (m_pos - i - 1));
    }
}

void RCCabCheckSum2::UpdateUInt32(uint32_t v) 
{ 
    m_value ^= v; 
}

uint32_t RCCabCheckSum2::GetResult() const 
{  
    return m_value; 
}

void RCCabCheckSum2::Update(const void *data, uint32_t size)
{
    uint32_t checkSum = m_value;
    const byte_t *dataPointer = static_cast<const byte_t *>(data);

    while (size != 0 && m_pos != 0)
    {
        m_hist[m_pos] = *dataPointer++;
        m_pos = (m_pos + 1) & 3;
        size--;
        if (m_pos == 0)
        {
            for (int i = 0; i < 4; i++)
            {
                checkSum ^= ((uint32_t)m_hist[i]) << (8 * i);
            }
        }
    }
    int numWords = size / 4;
    while (numWords-- != 0)
    {
        uint32_t temp = *dataPointer++;
        temp |= ((uint32_t)(*dataPointer++)) <<  8;
        temp |= ((uint32_t)(*dataPointer++)) << 16;
        temp |= ((uint32_t)(*dataPointer++)) << 24;
        checkSum ^= temp;
    }
    m_value = checkSum;
    size &= 3;
    while (size != 0)
    {
        m_hist[m_pos] = *dataPointer++;
        m_pos = (m_pos + 1) & 3;
        size--;
    }
}

END_NAMESPACE_RCZIP
