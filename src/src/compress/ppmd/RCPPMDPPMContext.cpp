/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "compress/ppmd/RCPPMDPPMContext.h"
#include "compress/ppmd/RCPPMDSubAlloc.h"

/////////////////////////////////////////////////////////////////
//RCPPMDPPMContext class implementation

BEGIN_NAMESPACE_RCZIP

RCPPMDPPMContext::RCPPMDPPMContext():
    m_numStats(0),
    m_summFreq(0),
    m_stats(0),
    m_suffix(0)
{
}

RCPPMDPPMContext::~RCPPMDPPMContext()
{
}

RCPPMDPPMContext::TState::TState():
    m_symbol(0),
    m_freq(0),
    m_successorLow(0),
    m_successorHigh(0)
{
    
}

uint32_t RCPPMDPPMContext::TState::GetSuccessor() const
{
    return m_successorLow | ((uint32_t)m_successorHigh << 16);
}

void RCPPMDPPMContext::TState::SetSuccessor(uint32_t v)
{
    m_successorLow = (uint16_t)(v & 0xFFFF) ;
    m_successorHigh = (uint16_t)((v >> 16) & 0xFFFF) ;
}

RCPPMDPPMContext* RCPPMDPPMContext::CreateChild(RCPPMDSubAlloc& subAllocator, TState* pStats, TState& FirstState)
{
    RCPPMDPPMContext* pc = (RCPPMDPPMContext*)subAllocator.AllocContext() ;
    if (pc)
    {
        pc->m_numStats = 1;
        pc->OneState() = FirstState ;
        pc->m_suffix = subAllocator.GetOffset(this);
        pStats->SetSuccessor(subAllocator.GetOffsetNoCheck(pc));
    }
    return pc;
}

RCPPMDPPMContext::TState& RCPPMDPPMContext::OneState() const
{
    return (TState&)m_summFreq ;
}

END_NAMESPACE_RCZIP
