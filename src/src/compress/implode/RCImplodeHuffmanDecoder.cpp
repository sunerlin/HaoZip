/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "compress/implode/RCImplodeHuffmanDecoder.h"

/////////////////////////////////////////////////////////////////
//RCImplodeHuffmanDecoder class implementation

BEGIN_NAMESPACE_RCZIP

RCImplodeHuffmanDecoder::RCImplodeHuffmanDecoder(uint32_t numSymbols):
    m_numSymbols(numSymbols)
{
    m_symbols = new uint32_t[m_numSymbols] ;
}

RCImplodeHuffmanDecoder::~RCImplodeHuffmanDecoder()
{
    delete [] m_symbols ;
}

bool RCImplodeHuffmanDecoder::SetCodeLengths(const byte_t* codeLengths)
{
    int32_t lenCounts[s_kNumBitsInLongestCode + 2] ;
    int32_t tmpPositions[s_kNumBitsInLongestCode + 1];
    int32_t i = 0 ;
    for(i = 0; i <= s_kNumBitsInLongestCode; i++)
    {
        lenCounts[i] = 0;
    }
    uint32_t symbolIndex = 0 ;
    for (symbolIndex = 0; symbolIndex < m_numSymbols; symbolIndex++)
    {
        lenCounts[codeLengths[symbolIndex]]++;
    }
    m_limitits[s_kNumBitsInLongestCode + 1] = 0;
    m_positions[s_kNumBitsInLongestCode + 1] = 0;
    lenCounts[s_kNumBitsInLongestCode + 1] =  0;

    uint32_t startPos = 0;
    const uint32_t kMaxValue = (1 << s_kNumBitsInLongestCode);

    for (i = s_kNumBitsInLongestCode; i > 0; i--)
    {
        startPos += lenCounts[i] << (s_kNumBitsInLongestCode - i);
        if (startPos > kMaxValue)
        {
            return false;
        }
        m_limitits[i] = startPos;
        m_positions[i] = m_positions[i + 1] + lenCounts[i + 1];
        tmpPositions[i] = m_positions[i] + lenCounts[i];
    }

    if (startPos != kMaxValue)
    {
        return false;
    }

    for (symbolIndex = 0; symbolIndex < m_numSymbols; symbolIndex++)
    {
        if (codeLengths[symbolIndex] != 0)
        {
            m_symbols[--tmpPositions[codeLengths[symbolIndex]]] = symbolIndex;
        }
    }
    return true;
}

uint32_t RCImplodeHuffmanDecoder::DecodeSymbol(RCInBit* inStream)
{
    uint32_t numBits = 0;
    uint32_t value = inStream->GetValue(s_kNumBitsInLongestCode);
    int32_t i = 0 ;
    for(i = s_kNumBitsInLongestCode; i > 0; i--)
    {
        if (value < m_limitits[i])
        {
            numBits = i;
            break;
        }
    }
    if (i == 0)
    {
        return 0xFFFFFFFF;
    }
    inStream->MovePos(numBits);
    uint32_t index = m_positions[numBits] +
                     ((value - m_limitits[numBits + 1]) >> (s_kNumBitsInLongestCode - numBits));
    if (index >= m_numSymbols)
    {
        return 0xFFFFFFFF;
    }
    return m_symbols[index];
}

END_NAMESPACE_RCZIP
