/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

/////////////////////////////////////////////////////////////////
//RCRangeBitEncoder class implementation

BEGIN_NAMESPACE_RCZIP

template <int32_t numMoveBits>
void RCRangeBitEncoder<numMoveBits>::Encode(RCRangeEncoder* encoder, uint32_t symbol)
{
    uint32_t newBound = (encoder->GetRange() >> RCRangeCoderDefs::s_kNumBitModelTotalBits) * this->m_prob;
    if (symbol == 0)
    {
        encoder->SetRange(newBound) ;
        this->m_prob += (RCRangeCoderDefs::s_kBitModelTotal - this->m_prob) >> numMoveBits;
    }
    else
    {
        encoder->SetLow( encoder->GetLow() + newBound) ;
        encoder->SetRange( encoder->GetRange() - newBound) ;
        this->m_prob -= (this->m_prob) >> numMoveBits;
    }
    if (encoder->GetRange() < RCRangeCoderDefs::s_kTopValue)
    {
        encoder->SetRange( encoder->GetRange() << 8) ;
        encoder->ShiftLow();
    }
}

template <int32_t numMoveBits>
uint32_t RCRangeBitEncoder<numMoveBits>::GetPrice(uint32_t symbol) const
{
    return RCRangeCoderDefs::s_probPrices[(this->m_prob ^ ((-(int32_t)symbol)) & (RCRangeCoderDefs::kBitModelTotal - 1)) >> RCRangeCoderDefs::kNumMoveReducingBits];
}

template <int32_t numMoveBits>
uint32_t RCRangeBitEncoder<numMoveBits>::GetPrice0() const
{
    return RCRangeCoderDefs::s_probPrices[this->m_prob >> RCRangeCoderDefs::kNumMoveReducingBits];
}

template <int32_t numMoveBits>
uint32_t RCRangeBitEncoder<numMoveBits>::GetPrice1() const
{
    return RCRangeCoderDefs::s_probPrices[(this->m_prob ^ (RCRangeCoderDefs::kBitModelTotal - 1)) >> RCRangeCoderDefs::kNumMoveReducingBits];
}

END_NAMESPACE_RCZIP
