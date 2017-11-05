/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "compress/ppmd/RCPPMDEncodeInfo.h"
#include "compress/ppmd/RCPPMDPPMContext.h"
#include "compress/ppmd/RCPPMDContextDefs.h"
#include "compress/ppmd/RCPPMDSEE2Context.h"

/////////////////////////////////////////////////////////////////
//RCPPMDEncodeInfo class implementation

BEGIN_NAMESPACE_RCZIP

void RCPPMDEncodeInfo::EncodeBinSymbol(int32_t symbol, RCRangeEncoder* rangeEncoder) 
{
    RCPPMDPPMContext::TState& rs = m_minContext->OneState();
    uint16_t& bs = GetBinSumm(rs, GetContextNoCheck(m_minContext->m_suffix)->m_numStats);
    if (rs.m_symbol == symbol)
    {
        m_foundState = &rs;
        rs.m_freq = (byte_t)(rs.m_freq + (rs.m_freq < 128 ? 1: 0));
        rangeEncoder->EncodeBit(bs, s_totBits, 0);
        bs = (uint16_t)(bs + s_interval - GET_MEAN(bs, s_periodBits, 2));
        m_prevSuccess = 1;
        m_runLength++;
    }
    else
    {
        rangeEncoder->EncodeBit(bs, s_totBits, 1);
        bs = (uint16_t)(bs - GET_MEAN(bs, s_periodBits, 2));
        m_initEsc = s_expEscape[bs >> 10];
        m_numMasked = 1;
        m_charMask[rs.m_symbol] = m_escCount;
        m_prevSuccess = 0;
        m_foundState = NULL;
    }
}

void RCPPMDEncodeInfo::EncodeSymbol1(int32_t symbol, RCRangeEncoder* rangeEncoder)
{
    RCPPMDPPMContext::TState* p = GetStateNoCheck(m_minContext->m_stats);
    if (p->m_symbol == symbol)
    {
        m_prevSuccess = (2 * (p->m_freq) > m_minContext->m_summFreq);
        m_runLength += m_prevSuccess;
        rangeEncoder->Encode(0, p->m_freq, m_minContext->m_summFreq);
        (m_foundState = p)->m_freq += 4;
        m_minContext->m_summFreq += 4;
        if (p->m_freq > s_maxFreq)
        {
            rescale();
        }
        return;
    }
    m_prevSuccess = 0;
    int LoCnt = p->m_freq, i = m_minContext->m_numStats - 1;
    while ((++p)->m_symbol != symbol)
    {
        LoCnt += p->m_freq;
        if (--i == 0)
        {
            m_hiBitsFlag = m_HB2Flag[m_foundState->m_symbol];
            m_charMask[p->m_symbol] = m_escCount;
            i=(m_numMasked = m_minContext->m_numStats)-1;
            m_foundState = NULL;
            do
            {
                m_charMask[(--p)->m_symbol] = m_escCount;
            } while ( --i );
            rangeEncoder->Encode(LoCnt, m_minContext->m_summFreq - LoCnt, m_minContext->m_summFreq);
            return;
        }
    }
    rangeEncoder->Encode(LoCnt, p->m_freq, m_minContext->m_summFreq);
    update1(p);
}

void RCPPMDEncodeInfo::EncodeSymbol2(int32_t symbol, RCRangeEncoder* rangeEncoder)
{
    int hiCnt, i = m_minContext->m_numStats - m_numMasked;
    uint32_t scale;
    RCPPMDSEE2Context* psee2c = makeEscFreq2(i, scale);
    RCPPMDPPMContext::TState* p = GetStateNoCheck(m_minContext->m_stats) - 1;
    hiCnt = 0;
    do
    {
        do
        {
            p++;
        } while (m_charMask[p->m_symbol] == m_escCount);
        hiCnt += p->m_freq;
        if (p->m_symbol == symbol)
        {
            goto SYMBOL_FOUND;
        }
        m_charMask[p->m_symbol] = m_escCount;
    }
    while ( --i );

    rangeEncoder->Encode(hiCnt, scale, hiCnt + scale);
    scale += hiCnt;

    psee2c->m_summ = (uint16_t)(psee2c->m_summ + scale);
    m_numMasked = m_minContext->m_numStats;
    return;
SYMBOL_FOUND:

    uint32_t highCount = hiCnt;
    uint32_t lowCount = highCount - p->m_freq;
    if ( --i )
    {
        RCPPMDPPMContext::TState* p1 = p;
        do
        {
            do
            {
                p1++;
            } while (m_charMask[p1->m_symbol] == m_escCount);
            hiCnt += p1->m_freq;
        }
        while ( --i );
    }
    // SubRange.scale += hiCnt;
    scale += hiCnt;
    rangeEncoder->Encode(lowCount, highCount - lowCount, scale);
    psee2c->update();
    update2(p);
}

void RCPPMDEncodeInfo::EncodeSymbol(int32_t c, RCRangeEncoder* rangeEncoder) 
{
    if (m_minContext->m_numStats != 1)
    {
        EncodeSymbol1(c, rangeEncoder);
    }
    else
    {
        EncodeBinSymbol(c, rangeEncoder);
    }
    while ( !m_foundState )
    {
        do
        {
            m_orderFall++;
            m_minContext = GetContext(m_minContext->m_suffix);
            if (m_minContext == 0)
            {
                return; //  S_OK;
            }
        }
        while (m_minContext->m_numStats == m_numMasked);
        EncodeSymbol2(c, rangeEncoder);
    }
    NextContext();
}

END_NAMESPACE_RCZIP
