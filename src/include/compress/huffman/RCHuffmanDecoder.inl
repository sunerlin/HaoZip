/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

/////////////////////////////////////////////////////////////////
//RCHuffmanDecoder class implementation

BEGIN_NAMESPACE_RCZIP

template <int32_t kNumBitsMax, uint32_t m_NumSymbols>
bool RCHuffmanDecoder<kNumBitsMax,m_NumSymbols>::SetCodeLengths(const byte_t* codeLengths)
{
    int32_t lenCounts[kNumBitsMax + 1];
    uint32_t tmpPositions[kNumBitsMax + 1];
    int32_t i = 0 ;
    for(i = 1; i <= kNumBitsMax; i++)
    {
        lenCounts[i] = 0;
    }
    uint32_t symbol = 0 ;
    for (symbol = 0; symbol < m_NumSymbols; ++symbol)
    {
        int32_t len = codeLengths[symbol];
        if (len > kNumBitsMax)
        {
            return false;
        }
        lenCounts[len]++;
        m_symbols[symbol] = 0xFFFFFFFF ;
    }
    lenCounts[0] = 0;
    m_positions[0] = 0;
    m_limits[0] = 0;
    uint32_t startPos = 0;
    uint32_t index = 0;
    const uint32_t kMaxValue = (1 << kNumBitsMax);
    for (i = 1; i <= kNumBitsMax; ++i)
    {
        startPos += lenCounts[i] << (kNumBitsMax - i);
        if (startPos > kMaxValue)
        {
            return false;
        }
        m_limits[i] = (i == kNumBitsMax) ? kMaxValue : startPos;
        m_positions[i] = m_positions[i - 1] + lenCounts[i - 1];
        tmpPositions[i] = m_positions[i];
        if(i <= s_kNumTableBits)
        {
            uint32_t limit = (m_limits[i] >> (kNumBitsMax - s_kNumTableBits));
            for (; index < limit; index++)
            {
                m_lengths[index] = (byte_t)i;
            }
        }
    }
    for (symbol = 0; symbol < m_NumSymbols; ++symbol)
    {
        int32_t len = codeLengths[symbol];
        if (len != 0)
        {
            m_symbols[tmpPositions[len]++] = symbol;
        }
    }
    return true ;
}

END_NAMESPACE_RCZIP
