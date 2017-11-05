/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "compress/rar/RCRar2Filter.h"

/////////////////////////////////////////////////////////////////
//RCRar2Filter class implementation

BEGIN_NAMESPACE_RCZIP

RCRar2Filter::RCRar2Filter()
{
}

RCRar2Filter::~RCRar2Filter()
{
}

void RCRar2Filter::Init()
{
    memset(this, 0, sizeof(*this)) ;
}

byte_t RCRar2Filter::Decode(int32_t& channelDelta, byte_t deltaByte)
{
    m_d4 = m_d3;
    m_d3 = m_d2;
    m_d2 = m_lastDelta - m_d1;
    m_d1 = m_lastDelta;
    int32_t predictedValue = ((8 * m_lastChar + m_k1 * m_d1 + m_k2 * m_d2 + m_k3 * m_d3 + m_k4 * m_d4 + m_k5 * channelDelta) >> 3);

    byte_t realValue = (byte_t)(predictedValue - deltaByte);
    int32_t i = ((int32_t)(signed char)deltaByte) << 3;

    m_dif[0] += abs(i);
    m_dif[1] += abs(i - m_d1);
    m_dif[2] += abs(i + m_d1);
    m_dif[3] += abs(i - m_d2);
    m_dif[4] += abs(i + m_d2);
    m_dif[5] += abs(i - m_d3);
    m_dif[6] += abs(i + m_d3);
    m_dif[7] += abs(i - m_d4);
    m_dif[8] += abs(i + m_d4);
    m_dif[9] += abs(i - channelDelta);
    m_dif[10] += abs(i + channelDelta);

    channelDelta = m_lastDelta = (signed char)(realValue - m_lastChar);
    m_lastChar = realValue;

    if (((++m_byteCount) & 0x1F) == 0)
    {
        uint32_t minDif = m_dif[0];
        uint32_t numMinDif = 0;
        m_dif[0] = 0;
        for (i = 1; i < sizeof(m_dif) / sizeof(m_dif[0]); i++)
        {
            if (m_dif[i] < minDif)
            {
                minDif = m_dif[i];
                numMinDif = i;
            }
            m_dif[i] = 0;
        }
        switch(numMinDif)
        {
            case 1: if (m_k1 >= -16) m_k1--; break;
            case 2: if (m_k1 <   16) m_k1++; break;
            case 3: if (m_k2 >= -16) m_k2--; break;
            case 4: if (m_k2 <   16) m_k2++; break;
            case 5: if (m_k3 >= -16) m_k3--; break;
            case 6: if (m_k3 <   16) m_k3++; break;
            case 7: if (m_k4 >= -16) m_k4--; break;
            case 8: if (m_k4 <   16) m_k4++; break;
            case 9: if (m_k5 >= -16) m_k5--; break;
            case 10:if (m_k5 <   16) m_k5++; break;
            default:
                break ;
        }
    }
    return realValue;
}

END_NAMESPACE_RCZIP
