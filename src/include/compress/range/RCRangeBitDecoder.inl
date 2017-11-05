/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

/////////////////////////////////////////////////////////////////
//RCRangeBitDecoder class implementation

BEGIN_NAMESPACE_RCZIP

template <int32_t numMoveBits>
uint32_t RCRangeBitDecoder<numMoveBits>::Decode(RCRangeDecoder *decoder)
{
    uint32_t newBound = (decoder->GetRange() >> RCRangeCoderDefs::s_kNumBitModelTotalBits) * this->m_prob ;
    if (decoder->GetCode() < newBound)
    {
        decoder->SetRange(newBound) ;
        this->m_prob += (RCRangeCoderDefs::s_kBitModelTotal - this->m_prob) >> numMoveBits;
        if (decoder->GetRange() < RCRangeCoderDefs::s_kTopValue)
        {
            decoder->SetCode((decoder->GetCode() << 8) | decoder->GetStream().ReadByte());
            decoder->SetRange( decoder->GetRange() << 8) ;
        }
        return 0;
    }
    else
    {
        decoder->SetRange(decoder->GetRange() - newBound) ;
        decoder->SetCode( decoder->GetCode()- newBound) ;
        this->m_prob -= (this->m_prob) >> numMoveBits;
        if (decoder->GetRange() < RCRangeCoderDefs::s_kTopValue)
        {
            decoder->SetCode( (decoder->GetCode() << 8) | decoder->GetStream().ReadByte() );
            decoder->SetRange( decoder->GetRange() << 8) ;
        }
        return 1 ;
    }
}

END_NAMESPACE_RCZIP
