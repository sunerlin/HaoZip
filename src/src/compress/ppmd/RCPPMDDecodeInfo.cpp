/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "compress/ppmd/RCPPMDDecodeInfo.h"
#include "compress/ppmd/RCPPMDPPMContext.h"
#include "compress/ppmd/RCPPMDContextDefs.h"
#include "compress/ppmd/RCPPMDSEE2Context.h"

/////////////////////////////////////////////////////////////////
//RCPPMDDecodeInfo class implementation

BEGIN_NAMESPACE_RCZIP

void RCPPMDDecodeInfo::DecodeBinSymbol(RCPPMDRangeDecoderVirt *rangeDecoder)
{
    RCPPMDPPMContext::TState& rs = m_minContext->OneState();
    uint16_t& bs = GetBinSumm(rs, GetContextNoCheck(m_minContext->m_suffix)->m_numStats);
    if (rangeDecoder->DecodeBit(bs, s_totBits) == 0)
    {
        m_foundState = &rs;
        rs.m_freq = (byte_t)(rs.m_freq + (rs.m_freq < 128 ? 1: 0));
        bs = (uint16_t)(bs + s_interval - GET_MEAN(bs, s_periodBits, 2));
        m_prevSuccess = 1;
        m_runLength++;
    }
    else
    {
        bs = (uint16_t)(bs - GET_MEAN(bs, s_periodBits, 2));
        m_initEsc = s_expEscape[bs >> 10];
        m_numMasked = 1;
        m_charMask[rs.m_symbol] = m_escCount;
        m_prevSuccess = 0;
        m_foundState = NULL;
    }
}

void RCPPMDDecodeInfo::DecodeSymbol1(RCPPMDRangeDecoderVirt *rangeDecoder)
{
    RCPPMDPPMContext::TState* p = GetStateNoCheck(m_minContext->m_stats);
    int32_t i = 0 ;
    int32_t count = 0 ;
    int32_t hiCnt = 0 ;
    if ((count = rangeDecoder->GetThreshold(m_minContext->m_summFreq)) < (hiCnt = p->m_freq))
    {
        m_prevSuccess = (2 * hiCnt > m_minContext->m_summFreq);
        m_runLength += m_prevSuccess;
        rangeDecoder->Decode(0, p->m_freq); // m_minContext->m_summFreq);
        (m_foundState = p)->m_freq = (byte_t)(hiCnt += 4);
        m_minContext->m_summFreq += 4;
        if (hiCnt > s_maxFreq)
        {
            rescale();
        }
        return;
    }
    m_prevSuccess = 0;
    i = m_minContext->m_numStats - 1;
    while ((hiCnt += (++p)->m_freq) <= count)
    {
        if (--i == 0)
        {
            m_hiBitsFlag = m_HB2Flag[m_foundState->m_symbol];
            rangeDecoder->Decode(hiCnt, m_minContext->m_summFreq - hiCnt); // , m_minContext->m_summFreq);
            m_charMask[p->m_symbol] = m_escCount;
            i = (m_numMasked = m_minContext->m_numStats)-1;
            m_foundState = NULL;
            do
            {
                m_charMask[(--p)->m_symbol] = m_escCount; 
            } while ( --i );
            return ;
        }
    }
    rangeDecoder->Decode(hiCnt - p->m_freq, p->m_freq); // , m_minContext->m_summFreq);
    update1(p);
}

void RCPPMDDecodeInfo::DecodeSymbol2(RCPPMDRangeDecoderVirt* rangeDecoder)
{
    int32_t count = 0 ;
    int32_t hiCnt = 0 ;
    int32_t i = m_minContext->m_numStats - m_numMasked ;
    uint32_t freqSum = 0 ;
    RCPPMDSEE2Context* psee2c = makeEscFreq2(i, freqSum);
    RCPPMDPPMContext::TState* ps[256] ;
    RCPPMDPPMContext::TState** pps = ps ;
    RCPPMDPPMContext::TState* p = GetStateNoCheck(m_minContext->m_stats)-1 ;
    hiCnt = 0;
    do
    {
        do
        {
            p++;
        }while (m_charMask[p->m_symbol] == m_escCount);
        hiCnt += p->m_freq;
        *pps++ = p;
    }
    while ( --i );

    freqSum += hiCnt;
    count = rangeDecoder->GetThreshold(freqSum);

    p = *(pps = ps);
    if (count < hiCnt)
    {
        hiCnt = 0;
        while ((hiCnt += p->m_freq) <= count)
        {
            p=*++pps;
        }
        rangeDecoder->Decode(hiCnt - p->m_freq, p->m_freq);

        psee2c->update();
        update2(p);
    }
    else
    {
        rangeDecoder->Decode(hiCnt, freqSum - hiCnt); // , freqSum);

        i = m_minContext->m_numStats - m_numMasked;
        pps--;
        do
        {
            m_charMask[(*++pps)->m_symbol] = m_escCount;
        }while ( --i );
        psee2c->m_summ = (uint16_t)(psee2c->m_summ + freqSum);
        m_numMasked = m_minContext->m_numStats;
    }
}

int32_t RCPPMDDecodeInfo::DecodeSymbol(RCPPMDRangeDecoderVirt* rangeDecoder)
{
    if (m_minContext == 0)
    {
        return -1;
    }

    if (m_minContext->m_numStats != 1)
    {
        DecodeSymbol1(rangeDecoder);
    }
    else
    {
        DecodeBinSymbol(rangeDecoder);
    }
    while ( !m_foundState )
    {
        do
        {
            m_orderFall++;
            m_minContext = GetContext(m_minContext->m_suffix);
            if (m_minContext == 0)
            {
                return -1;
            }
        }
        while (m_minContext->m_numStats == m_numMasked);
        DecodeSymbol2(rangeDecoder);
    }
    byte_t symbol = m_foundState->m_symbol;
    NextContext();
    return symbol;
 }

END_NAMESPACE_RCZIP
