/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "compress/ppmd/RCPPMDInfo.h"
#include "compress/ppmd/RCPPMDContextDefs.h"
#include "compress/ppmd/RCPPMDType.h"

/////////////////////////////////////////////////////////////////
//RCPPMDInfo class implementation

BEGIN_NAMESPACE_RCZIP

RCPPMDInfo::RCPPMDInfo():
    m_minContext(NULL),
    m_maxContext(NULL),
    m_foundState(NULL),
    m_numMasked(0),
    m_initEsc(0),
    m_orderFall(0),
    m_runLength(0),
    m_initRL(0),
    m_maxOrder(0),
    m_escCount(0),
    m_printCount(0),
    m_prevSuccess(0),
    m_hiBitsFlag(0)
{
}

RCPPMDInfo::~RCPPMDInfo()
{
}

uint16_t& RCPPMDInfo::GetBinSumm(const RCPPMDPPMContext::TState& rs, int32_t numStates)
{
    m_hiBitsFlag = m_HB2Flag[m_foundState->m_symbol];
    return m_binSumm[rs.m_freq - 1][ m_prevSuccess + 
                                 m_NS2BSIndx[numStates - 1] +
                                 m_hiBitsFlag + 
                                 2 * m_HB2Flag[rs.m_symbol] +
                                 ((m_runLength >> 26) & 0x20)] ;
}

RCPPMDPPMContext* RCPPMDInfo::GetContext(uint32_t offset) const
{
    return (RCPPMDPPMContext*)m_subAllocator.GetPtr(offset) ;
}

RCPPMDPPMContext* RCPPMDInfo::GetContextNoCheck(uint32_t offset) const
{
    return (RCPPMDPPMContext*)m_subAllocator.GetPtrNoCheck(offset) ;
}

RCPPMDPPMContext::TState* RCPPMDInfo::GetState(uint32_t offset) const
{
    return (RCPPMDPPMContext::TState*)m_subAllocator.GetPtr(offset) ;
}

RCPPMDPPMContext::TState* RCPPMDInfo::GetStateNoCheck(uint32_t offset) const
{
    return (RCPPMDPPMContext::TState*)m_subAllocator.GetPtr(offset) ;
}

void RCPPMDInfo::RestartModelRare()
{
    int32_t i = 0 ;
    int32_t k = 0 ;
    int32_t m = 0 ;
    memset(m_charMask,0,sizeof(m_charMask));
    m_subAllocator.InitSubAllocator();
    m_initRL = -((m_maxOrder < 12) ? m_maxOrder : 12) - 1;
    m_minContext = m_maxContext = (RCPPMDPPMContext*) m_subAllocator.AllocContext();
    m_minContext->m_suffix = 0;
    m_orderFall = m_maxOrder;
    m_minContext->m_summFreq = (uint16_t)((m_minContext->m_numStats = 256) + 1);
    m_foundState = (RCPPMDPPMContext::TState*)m_subAllocator.AllocUnits(256 / 2);
    m_minContext->m_stats = m_subAllocator.GetOffsetNoCheck(m_foundState);
    m_prevSuccess = 0;
    for (m_runLength = m_initRL, i = 0; i < 256; i++)
    {
        RCPPMDPPMContext::TState& state = m_foundState[i];
        state.m_symbol = (byte_t)i;
        state.m_freq = 1;
        state.SetSuccessor(0);
    }
    for (i = 0; i < 128; i++)
    {
        for (k = 0; k < 8; k++)
        {
            for ( m=0; m < 64; m += 8)
            {
                m_binSumm[i][k + m] = (uint16_t)(s_binScale - s_initBinEsc[k] / (i + 2));
            }
        }
    }
    for (i = 0; i < 25; i++)
    {
        for (k = 0; k < 16; k++)
        {
            m_SEE2Cont[i][k].init(5*i+10);
        }
    }
}

void RCPPMDInfo::StartModelRare(int maxOrder)
{
    int32_t i = 0 ;
    int32_t k = 0 ;
    int32_t m = 0 ;
    int32_t Step = 0 ;
    m_escCount = 1 ;
    m_printCount =1 ;
    if (maxOrder < 2)
    {
        memset(m_charMask,0,sizeof(m_charMask));
        m_orderFall = m_maxOrder;
        m_minContext = m_maxContext;
        while (m_minContext->m_suffix != 0)
        {
            m_minContext = GetContextNoCheck(m_minContext->m_suffix);
            m_orderFall--;
        }
        m_foundState = GetState(m_minContext->m_stats);
        m_minContext = m_maxContext;
    }
    else
    {
        m_maxOrder = maxOrder;
        RestartModelRare();
        m_NS2BSIndx[0] = 2 * 0;
        m_NS2BSIndx[1] = 2 * 1;
        memset(m_NS2BSIndx + 2, 2 * 2, 9);
        memset(m_NS2BSIndx + 11, 2 * 3, 256 - 11);
        for (i = 0; i < 3; i++)
        {
            m_NS2Indx[i] = (byte_t)i;
        }
        for (m = i, k = Step = 1; i < 256; i++)
        {
            m_NS2Indx[i] = (byte_t)m;
            if ( !--k )
            {
                k = ++Step;
                m++;
            }
        }
        memset(m_HB2Flag, 0, 0x40);
        memset(m_HB2Flag + 0x40, 0x08, 0x100 - 0x40);
        m_dummySEE2Cont.m_shift = s_periodBits;
    }
}

RCPPMDPPMContext* RCPPMDInfo::CreateSuccessors(bool skip, RCPPMDPPMContext::TState* p1)
{
    RCPPMDPPMContext::TState UpState ;
    RCPPMDPPMContext* pc = m_minContext;
    RCPPMDPPMContext* UpBranch = GetContext(m_foundState->GetSuccessor());
    RCPPMDPPMContext::TState* p = NULL ;
    RCPPMDPPMContext::TState* ps[MAX_O] ;
    RCPPMDPPMContext::TState** pps = ps ;
    if (!skip )
    {
        *pps++ = m_foundState;
        if ( !pc->m_suffix)
        {
            goto NO_LOOP;
        }
    }
    if ( p1 )
    {
        p = p1;
        pc = GetContext(pc->m_suffix);
        goto LOOP_ENTRY;
    }
    do
    {
        pc = GetContext(pc->m_suffix);
        if (pc->m_numStats != 1)
        {
            if ((p = GetStateNoCheck(pc->m_stats))->m_symbol != m_foundState->m_symbol)
            {
                do
                {
                    p++;
                }while (p->m_symbol != m_foundState->m_symbol);
            }
        }
        else
        {
            p = &(pc->OneState());
        }
LOOP_ENTRY:
        if (GetContext(p->GetSuccessor()) != UpBranch)
        {
            pc = GetContext(p->GetSuccessor());
            break;
        }
        *pps++ = p;
    }
    while ( pc->m_suffix);
NO_LOOP:
    if (pps == ps)
    {
        return pc;
    }
    UpState.m_symbol = *(byte_t*) UpBranch;
    UpState.SetSuccessor(m_subAllocator.GetOffset(UpBranch) + 1);
    if (pc->m_numStats != 1)
    {
        if ((p = GetStateNoCheck(pc->m_stats))->m_symbol != UpState.m_symbol)
        {
            do
            {
                p++;
            }while (p->m_symbol != UpState.m_symbol);
        }
        uint32_t cf = p->m_freq - 1;
        uint32_t s0 = pc->m_summFreq - pc->m_numStats - cf;
        UpState.m_freq = (byte_t)(1 + ((2 * cf <= s0) ? (5 * cf > s0) :
                                                      ((2 * cf + 3 * s0 - 1) / (2 * s0))));
    }
    else
    {
        UpState.m_freq = pc->OneState().m_freq ;
    }
    do
    {
        pc = pc->CreateChild(m_subAllocator, *--pps, UpState);
        if ( !pc )
        {
            return NULL;
        }
    }
    while (pps != ps);
    return pc;
}

void RCPPMDInfo::UpdateModel()
{
    RCPPMDPPMContext::TState fs = *m_foundState ;
    RCPPMDPPMContext::TState* p = NULL;
    RCPPMDPPMContext* pc = NULL ;
    RCPPMDPPMContext* Successor = NULL ;
    uint32_t ns1 = 0 ;
    uint32_t ns = 0 ;
    uint32_t cf = 0 ;
    uint32_t sf = 0 ;
    uint32_t s0 = 0 ;
    if (fs.m_freq < s_maxFreq / 4 && m_minContext->m_suffix != 0)
    {
        pc = GetContextNoCheck(m_minContext->m_suffix);
        if (pc->m_numStats != 1)
        {
            if ((p = GetStateNoCheck(pc->m_stats))->m_symbol != fs.m_symbol)
            {
                do
                {
                    p++ ;
                }while (p->m_symbol != fs.m_symbol);
                if (p[0].m_freq >= p[-1].m_freq)
                {
                    _PPMD_SWAP(p[0],p[-1]);
                    p--;
                }
            }
            if (p->m_freq < s_maxFreq-9)
            {
                p->m_freq += 2;
                pc->m_summFreq += 2;
            }
        }
        else
        {
            p = &(pc->OneState());
            p->m_freq = (byte_t)(p->m_freq + ((p->m_freq < 32) ? 1 : 0));
        }
    }
    if (!m_orderFall )
    {
        m_maxContext = CreateSuccessors(true, p) ;
        m_minContext = m_maxContext ;
        m_foundState->SetSuccessor(m_subAllocator.GetOffset(m_minContext));
        if (m_minContext == 0)
        {
            goto RESTART_MODEL;
        }
        return;
    }
    *m_subAllocator.m_pText++ = fs.m_symbol;
    Successor = (RCPPMDPPMContext*) m_subAllocator.m_pText;
    if (m_subAllocator.m_pText >= m_subAllocator.m_unitsStart)
    {
        goto RESTART_MODEL;
    }
    if (fs.GetSuccessor() != 0)
    {
        if ((byte_t *)GetContext(fs.GetSuccessor()) <= m_subAllocator.m_pText)
        {
            RCPPMDPPMContext* cs = CreateSuccessors(false, p);
            fs.SetSuccessor(m_subAllocator.GetOffset(cs));
            if (cs == NULL)
            {
                goto RESTART_MODEL;
            }
        }
        if ( !--m_orderFall )
        {
            Successor = GetContext(fs.GetSuccessor());
            m_subAllocator.m_pText -= (m_maxContext != m_minContext);
        }
    }
    else
    {
        m_foundState->SetSuccessor(m_subAllocator.GetOffsetNoCheck(Successor));
        fs.SetSuccessor(m_subAllocator.GetOffsetNoCheck(m_minContext));
    }
    s0 = m_minContext->m_summFreq - (ns = m_minContext->m_numStats) - (fs.m_freq - 1);
    for (pc = m_maxContext; pc != m_minContext; pc = GetContext(pc->m_suffix))
    {
        if ((ns1 = pc->m_numStats) != 1)
        {
            if ((ns1 & 1) == 0)
            {
                void* ppp = m_subAllocator.ExpandUnits(GetState(pc->m_stats), ns1 >> 1);
                pc->m_stats = m_subAllocator.GetOffset(ppp);
                if (!ppp)
                {
                    goto RESTART_MODEL;
                }
            }
            pc->m_summFreq = (uint16_t)( pc->m_summFreq + 
                                       (2 * ns1 < ns) + 
                                       2 * ((4 * ns1 <= ns) & (pc->m_summFreq <= 8 * ns1)) ) ;
        }
        else
        {
            p = (RCPPMDPPMContext::TState*) m_subAllocator.AllocUnits(1);
            if ( !p )
            {
                goto RESTART_MODEL;
            }
            *p = pc->OneState();
            pc->m_stats = m_subAllocator.GetOffsetNoCheck(p);
            if (p->m_freq < s_maxFreq / 4 - 1)
            {
                p->m_freq <<= 1;
            }
            else
            {
                p->m_freq  = s_maxFreq - 4;
            }
            pc->m_summFreq = (uint16_t)(p->m_freq + m_initEsc + (ns > 3));
        }
        cf = 2 * fs.m_freq * (pc->m_summFreq+6);
        sf = s0 + pc->m_summFreq;
        if (cf < 6 * sf)
        {
            cf = 1 + (cf > sf)+(cf >= 4 * sf);
            pc->m_summFreq += 3;
        }
        else
        {
            cf = 4 + (cf >= 9 * sf) + (cf >= 12 * sf) + (cf >= 15 * sf);
            pc->m_summFreq = (uint16_t)(pc->m_summFreq + cf);
        }
        p = GetState(pc->m_stats) + ns1;
        p->SetSuccessor(m_subAllocator.GetOffset(Successor));
        p->m_symbol = fs.m_symbol;
        p->m_freq = (byte_t)cf;
        pc->m_numStats = (uint16_t)++ns1;
    }
    m_minContext = GetContext(fs.GetSuccessor());
    m_maxContext = m_minContext ;
    return;
RESTART_MODEL:
    RestartModelRare();
    m_escCount = 0;
    m_printCount = 0xFF;
}

void RCPPMDInfo::ClearMask()
{
    m_escCount = 1;
    memset(m_charMask, 0, sizeof(m_charMask));
}

void RCPPMDInfo::update1(RCPPMDPPMContext::TState* p)
{
    (m_foundState = p)->m_freq += 4;
    m_minContext->m_summFreq += 4;
    if (p[0].m_freq > p[-1].m_freq)
    {
        _PPMD_SWAP(p[0],p[-1]);
        m_foundState = --p;
        if (p->m_freq > s_maxFreq)
        {
            rescale();
        }
    }
}

void RCPPMDInfo::update2(RCPPMDPPMContext::TState* p)
{
    (m_foundState = p)->m_freq += 4;
    m_minContext->m_summFreq += 4;
    if (p->m_freq > s_maxFreq)
    {
        rescale();
    }
    m_escCount++;
    m_runLength = m_initRL;
}

RCPPMDSEE2Context* RCPPMDInfo::makeEscFreq2(int32_t Diff, uint32_t &scale)
{
    RCPPMDSEE2Context* psee2c = NULL ;
    if (m_minContext->m_numStats != 256)
    {
        psee2c = m_SEE2Cont[m_NS2Indx[Diff-1]] +
                (Diff < (GetContext(m_minContext->m_suffix))->m_numStats - m_minContext->m_numStats) +
                2 * (m_minContext->m_summFreq < 11 * m_minContext->m_numStats) +
                4 * (m_numMasked > Diff) +
                m_hiBitsFlag ;
        scale = psee2c->getMean();
    }
    else
    {
        psee2c = &m_dummySEE2Cont;
        scale = 1;
    }
    return psee2c ;
}

void RCPPMDInfo::rescale()
{
    int32_t OldNS = m_minContext->m_numStats ;
    int32_t i = m_minContext->m_numStats - 1 ;
    int32_t Adder = 0 ;
    int32_t EscFreq = 0 ;
    RCPPMDPPMContext::TState* p1 = NULL ;
    RCPPMDPPMContext::TState* p = NULL ;
    RCPPMDPPMContext::TState* stats = GetStateNoCheck(m_minContext->m_stats);
    for (p = m_foundState; p != stats; p--)
    {
        _PPMD_SWAP(p[0], p[-1]);
    }
    stats->m_freq += 4;
    m_minContext->m_summFreq += 4;
    EscFreq = m_minContext->m_summFreq - p->m_freq;
    Adder = (m_orderFall != 0);
    p->m_freq = (byte_t)((p->m_freq + Adder) >> 1);
    m_minContext->m_summFreq = p->m_freq;
    do
    {
        EscFreq -= (++p)->m_freq;
        p->m_freq = (byte_t)((p->m_freq + Adder) >> 1);
        m_minContext->m_summFreq = (uint16_t)(m_minContext->m_summFreq + p->m_freq);
        if (p[0].m_freq > p[-1].m_freq)
        {
            RCPPMDPPMContext::TState tmp = *(p1 = p);
            do
            {
                p1[0] = p1[-1];
            }
            while (--p1 != stats && tmp.m_freq > p1[-1].m_freq);
            *p1 = tmp;
        }
    }
    while ( --i );
    if (p->m_freq == 0)
    {
        do
        {
            i++;
        }while ((--p)->m_freq == 0);
        EscFreq += i;
        m_minContext->m_numStats = (uint16_t)(m_minContext->m_numStats - i);
        if (m_minContext->m_numStats == 1)
        {
            RCPPMDPPMContext::TState tmp = *stats;
            do
            {
                tmp.m_freq = (byte_t)(tmp.m_freq - (tmp.m_freq >> 1));
                EscFreq >>= 1;
            } while (EscFreq > 1);
            m_subAllocator.FreeUnits(stats, (OldNS+1) >> 1);
            *(m_foundState = &m_minContext->OneState()) = tmp;  
            return;
        }
    }
    EscFreq -= (EscFreq >> 1);
    m_minContext->m_summFreq = (uint16_t)(m_minContext->m_summFreq + EscFreq);
    int32_t n0 = (OldNS+1) >> 1, n1 = (m_minContext->m_numStats + 1) >> 1;
    if (n0 != n1)
    {
        m_minContext->m_stats = m_subAllocator.GetOffset(m_subAllocator.ShrinkUnits(stats, n0, n1));
    }
    m_foundState = GetState(m_minContext->m_stats) ;
}

void RCPPMDInfo::NextContext()
{
    RCPPMDPPMContext* c = GetContext(m_foundState->GetSuccessor());
    if (!m_orderFall && (byte_t *)c > m_subAllocator.m_pText)
    {
        m_minContext = m_maxContext = c;
    }
    else
    {
        UpdateModel();
        if (m_escCount == 0)
        {
            ClearMask();
        }
    }
}

END_NAMESPACE_RCZIP
