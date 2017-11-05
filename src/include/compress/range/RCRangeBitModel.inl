/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

BEGIN_NAMESPACE_RCZIP

template <int32_t numMoveBits>
RCRangeBitModel<numMoveBits>::RCRangeBitModel():
    m_prob(0)
{    
}

template <int32_t numMoveBits>
uint32_t RCRangeBitModel<numMoveBits>::GetProb() const
{
    return m_prob ;
}

template <int32_t numMoveBits>
void RCRangeBitModel<numMoveBits>::SetProb(uint32_t prob)
{
    m_prob = prob ;
}

template <int32_t numMoveBits>
void RCRangeBitModel<numMoveBits>::Init()
{
    m_prob = RCRangeCoderDefs::s_kBitModelTotal / 2;
}

template <int32_t numMoveBits>
void RCRangeBitModel<numMoveBits>::UpdateModel(uint32_t symbol)
{
    if (symbol == 0)
    {
        m_prob += (RCRangeCoderDefs::kBitModelTotal - m_prob) >> numMoveBits;
    }
    else
    {
        m_prob -= (m_prob) >> numMoveBits;
    }
}

END_NAMESPACE_RCZIP
