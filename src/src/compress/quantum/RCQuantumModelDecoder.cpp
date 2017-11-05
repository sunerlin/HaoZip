/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "compress/quantum/RCQuantumModelDecoder.h"
#include "compress/quantum/RCQuantumRangeDecoder.h"

/////////////////////////////////////////////////////////////////
//RCQuantumModelDecoder class implementation

BEGIN_NAMESPACE_RCZIP

RCQuantumModelDecoder::RCQuantumModelDecoder()
{
}

RCQuantumModelDecoder::~RCQuantumModelDecoder()
{
}

void RCQuantumModelDecoder::Init(uint32_t numItems)
{
    m_numItems = numItems;
    m_reorderCount = RCQuantumDefs::s_kReorderCountStart;
    for(uint32_t i = 0; i < numItems; i++)
    {
        m_freqs[i] = (uint16_t)(numItems - i);
        m_values[i] = (byte_t)i;
    }
    m_freqs[numItems] = 0;
}

uint32_t RCQuantumModelDecoder::Decode(RCQuantumRangeDecoder* rangeDecoder)
{
    uint32_t threshold = rangeDecoder->GetThreshold(m_freqs[0]) ;
    uint32_t i = 0 ;
    for (i = 1; m_freqs[i] > threshold; i++)
    {
    }
    rangeDecoder->Decode(m_freqs[i], m_freqs[i - 1], m_freqs[0]);
    uint32_t res = m_values[--i];
    do
    {
        m_freqs[i] += RCQuantumDefs::s_kUpdateStep;
    }
    while(i-- != 0);

    if (m_freqs[0] > RCQuantumDefs::s_kFreqSumMax)
    {
        if (--m_reorderCount == 0)
        {
            m_reorderCount = RCQuantumDefs::s_kReorderCount;
            for(i = 0; i < m_numItems; i++)
            {
                m_freqs[i] = (uint16_t)(((m_freqs[i] - m_freqs[i + 1]) + 1) >> 1);
            }
            for(i = 0; i < m_numItems - 1; i++)
            {
                for(uint32_t j = i + 1; j < m_numItems; j++)
                {
                    if (m_freqs[i] < m_freqs[j])
                    {
                        uint16_t tmpFreq = m_freqs[i];
                        byte_t tmpVal = m_values[i];
                        m_freqs[i] = m_freqs[j];
                        m_values[i] = m_values[j];
                        m_freqs[j] = tmpFreq;
                        m_values[j] = tmpVal;
                    }
                    do
                    {
                        m_freqs[i] = (uint16_t)(m_freqs[i] + m_freqs[i + 1]);
                    }
                    while(i-- != 0);
                }
            }
        }
        else
        {
            i = m_numItems - 1;
            do
            {
                m_freqs[i] >>= 1;
                if (m_freqs[i] <= m_freqs[i + 1])
                {
                    m_freqs[i] = (uint16_t)(m_freqs[i + 1] + 1);
                }
            }
            while(i-- != 0);
        }
    }
    return res;
}
END_NAMESPACE_RCZIP
