/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "compress/ppmd/RCPPMDSEE2Context.h"
#include "compress/ppmd/RCPPMDContextDefs.h"

/////////////////////////////////////////////////////////////////
//RCPPMDSEE2Context class implementation

BEGIN_NAMESPACE_RCZIP

RCPPMDSEE2Context::RCPPMDSEE2Context():
    m_summ(0),
    m_shift(0),
    m_count(0)
{
}

RCPPMDSEE2Context::~RCPPMDSEE2Context()
{
}

void RCPPMDSEE2Context::init(int32_t InitVal)
{
    m_shift = s_periodBits - 4 ;
    m_summ = (uint16_t)(InitVal << m_shift ) ; 
    m_count = 4 ; 
}

uint32_t RCPPMDSEE2Context::getMean()
{
    uint32_t RetVal = (m_summ >> m_shift);
    m_summ = (uint16_t)(m_summ - RetVal) ;
    return RetVal + (RetVal == 0) ;
}

void RCPPMDSEE2Context::update()
{
    if (m_shift < s_periodBits && --m_count == 0)
    {
        m_summ <<= 1;
        m_count = (byte_t)(3 << m_shift++);
    }
}

END_NAMESPACE_RCZIP
