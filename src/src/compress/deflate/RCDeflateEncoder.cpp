/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "compress/deflate/RCDeflateEncoder.h"
#include "compress/deflate/RCDeflateDefs.h"
#include "common/RCAlloc.h"
#include "algorithm/HuffEnc.h"

/////////////////////////////////////////////////////////////////
//RCDeflateEncoder class implementation

BEGIN_NAMESPACE_RCZIP

static const int32_t   s_kDeflateNumDivPassesMax = 10; // [0, 16); ratio/speed/ram tradeoff; use big value for better compression ratio.
static const uint32_t  s_kDeflateNumTables = (1 << s_kDeflateNumDivPassesMax);

static const uint32_t s_kDeflateFixedHuffmanCodeBlockSizeMax = (1 << 8); // [0, (1 << 32)); ratio/speed tradeoff; use big value for better compression ratio.
static const uint32_t s_kDeflateDivideCodeBlockSizeMin = (1 << 7); // [1, (1 << 32)); ratio/speed tradeoff; use small value for better compression ratio.
static const uint32_t s_kDeflateDivideBlockSizeMin = (1 << 6); // [1, (1 << 32)); ratio/speed tradeoff; use small value for better compression ratio.

static const uint32_t s_kDeflateMaxUncompressedBlockSize = ((1 << 16) - 1) * 1; // [1, (1 << 32))
static const uint32_t s_kDeflateMatchArraySize = s_kDeflateMaxUncompressedBlockSize * 10; // [RCDeflateDefs::s_kMatchMaxLen * 2, (1 << 32))
static const uint32_t s_kDeflateMatchArrayLimit = s_kDeflateMatchArraySize - RCDeflateDefs::s_kMatchMaxLen * 4 * sizeof(uint16_t);
static const uint32_t s_kDeflateBlockUncompressedSizeThreshold = s_kDeflateMaxUncompressedBlockSize -
                                                        RCDeflateDefs::s_kMatchMaxLen - RCDeflateDefs::s_kNumOpts;

static const int32_t s_kDeflateMaxCodeBitLength = 11;
static const int32_t s_kDeflateMaxLevelBitLength = 7;

static const byte_t s_kDeflateNoLiteralStatPrice = 11;
static const byte_t s_kDeflateNoLenStatPrice = 11;
static const byte_t s_kDeflateNoPosStatPrice = 6;
static const uint32_t s_kDeflateIfinityPrice = 0xFFFFFFF;

static byte_t g_DeflateEncoderLenSlots[RCDeflateDefs::s_kNumLenSymbolsMax];
static byte_t g_DeflateEncoderFastPos[1 << 9];

class RCDeflateEncoderFastPosInit
{
public:
    RCDeflateEncoderFastPosInit()
    {
        int32_t i = 0 ;
        for(i = 0; i < RCDeflateDefs::s_kNumLenSlots; i++)
        {
            int32_t c = s_kDeflateLenStart32[i];
            int32_t j = 1 << s_kDeflateLenDirectBits32[i];
            for(int32_t k = 0; k < j; k++, c++)
            {
                g_DeflateEncoderLenSlots[c] = (byte_t)i;
            }
        }

        const int32_t kFastSlots = 18;
        int32_t c = 0;
        for (byte_t slotFast = 0; slotFast < kFastSlots; slotFast++)
        {
            uint32_t k = (1 << s_kDeflateDistDirectBits[slotFast]);
            for (uint32_t j = 0; j < k; j++, c++)
            {
                g_DeflateEncoderFastPos[c] = slotFast;
            }
        }
    }
};

static RCDeflateEncoderFastPosInit g_deflateEncoderFastPosInit ;


ISzAlloc RCDeflateEncoder::s_alloc = { RCDeflateEncoder::SzAlloc, RCDeflateEncoder::SzFree };

class RCDeflateEncoderReleaser
{
public:
    RCDeflateEncoderReleaser(RCDeflateEncoder* coder): 
        m_coder(coder)
    {
    }
    ~RCDeflateEncoderReleaser()
    {
        m_coder->ReleaseStreams();
    }
      
private:
    RCDeflateEncoder* m_coder;
};

RCDeflateEncoder::RCDeflateEncoder(bool deflate64Mode):
    m_deflate64Mode(deflate64Mode),
    m_numPasses(1),
    m_numDivPasses(1),
    m_numFastBytes(32),
    m_fastMode(false),
    m_btMode(true),
    m_onePosMatchesMemory(0),
    m_distanceMemory(0),
    m_created(false),
    m_values(0),
    m_tables(0),
    m_matchFinderCycles(0)
{
    m_matchMaxLen = deflate64Mode ? RCDeflateDefs::s_kMatchMaxLen64 : RCDeflateDefs::s_kMatchMaxLen32;
    m_numLenCombinations = deflate64Mode ? RCDeflateDefs::s_kNumLenSymbols64 : RCDeflateDefs::s_kNumLenSymbols32;
    m_lenStart = deflate64Mode ? s_kDeflateLenStart64 : s_kDeflateLenStart32;
    m_lenDirectBits = deflate64Mode ? s_kDeflateLenDirectBits64 : s_kDeflateLenDirectBits32;
    MatchFinder_Construct(&m_lzInWindow) ;
}

RCDeflateEncoder::~RCDeflateEncoder()
{
    Free();
    MatchFinder_Free(&m_lzInWindow, &s_alloc) ;
}

void RCDeflateEncoder::GetMatches()
{
    if (m_isMultiPass)
    {
        m_matchDistances = m_onePosMatchesMemory + m_pos;
        if (m_secondPass)
        {
            m_pos += *m_matchDistances + 1;
            return;
        }
    }

    uint32_t distanceTmp[RCDeflateDefs::s_kMatchMaxLen * 2 + 3];

    uint32_t numPairs = (m_btMode) ?
                                    Bt3Zip_MatchFinder_GetMatches(&m_lzInWindow, distanceTmp):
                                    Hc3Zip_MatchFinder_GetMatches(&m_lzInWindow, distanceTmp);

    *m_matchDistances = (uint16_t)numPairs;

    if (numPairs > 0)
    {
        uint32_t i;
        for(i = 0; i < numPairs; i += 2)
        {
            m_matchDistances[i + 1] = (uint16_t)distanceTmp[i];
            m_matchDistances[i + 2] = (uint16_t)distanceTmp[i + 1];
        }
        uint32_t len = distanceTmp[numPairs - 2];
        if (len == m_numFastBytes && m_numFastBytes != m_matchMaxLen)
        {
            uint32_t numAvail = Inline_MatchFinder_GetNumAvailableBytes(&m_lzInWindow) + 1;
            const byte_t *pby = Inline_MatchFinder_GetPointerToCurrentPos(&m_lzInWindow) - 1;
            const byte_t *pby2 = pby - (distanceTmp[numPairs - 1] + 1);
            if (numAvail > m_matchMaxLen)
            {
                numAvail = m_matchMaxLen;
            }
            for (; len < numAvail && pby[len] == pby2[len]; len++)
            {
            }
            m_matchDistances[i - 1] = (uint16_t)len;
        }
    }
    if (m_isMultiPass)
    {
        m_pos += numPairs + 1;
    }
    if (!m_secondPass)
    {
        m_additionalOffset++;
    }
}

void RCDeflateEncoder::MovePos(uint32_t num)
{
    if (!m_secondPass && num > 0)
    {
        if (m_btMode)
        {
            Bt3Zip_MatchFinder_Skip(&m_lzInWindow, num);
        }
        else
        {
            Hc3Zip_MatchFinder_Skip(&m_lzInWindow, num);
        }
        m_additionalOffset += num;
    }
}

uint32_t RCDeflateEncoder::Backward(uint32_t& backRes, uint32_t cur)
{
    m_optimumEndIndex = cur;
    uint32_t posMem = m_optimum[cur].m_posPrev;
    uint16_t backMem = m_optimum[cur].m_backPrev;
    do
    {
        uint32_t posPrev = posMem;
        uint16_t backCur = backMem;
        backMem = m_optimum[posPrev].m_backPrev;
        posMem = m_optimum[posPrev].m_posPrev;
        m_optimum[posPrev].m_backPrev = backCur;
        m_optimum[posPrev].m_posPrev = (uint16_t)cur;
        cur = posPrev;
    } while(cur > 0);
    backRes = m_optimum[0].m_backPrev;
    m_optimumCurrentIndex = m_optimum[0].m_posPrev;
    return m_optimumCurrentIndex;
}

uint32_t RCDeflateEncoder::GetOptimal(uint32_t& backRes)
{
    if(m_optimumEndIndex != m_optimumCurrentIndex)
    {
        uint32_t len = m_optimum[m_optimumCurrentIndex].m_posPrev - m_optimumCurrentIndex;
        backRes = m_optimum[m_optimumCurrentIndex].m_backPrev;
        m_optimumCurrentIndex = m_optimum[m_optimumCurrentIndex].m_posPrev;
        return len;
    }
    m_optimumCurrentIndex = m_optimumEndIndex = 0;

    GetMatches();

    uint32_t numDistancePairs = m_matchDistances[0];
    if(numDistancePairs == 0)
    {
        return 1;
    }

    const uint16_t *matchDistances = m_matchDistances + 1;
    uint32_t lenMain = matchDistances[numDistancePairs - 2];

    if(lenMain > m_numFastBytes)
    {
        backRes = matchDistances[numDistancePairs - 1];
        MovePos(lenMain - 1);
        return lenMain;
    }
    m_optimum[1].m_price = m_literalPrices[Inline_MatchFinder_GetIndexByte(&m_lzInWindow, 0 - m_additionalOffset)];
    m_optimum[1].m_posPrev = 0;

    m_optimum[2].m_price = s_kDeflateIfinityPrice;
    m_optimum[2].m_posPrev = 1;


    uint32_t offs = 0;
    for(uint32_t i = RCDeflateDefs::s_kMatchMinLen; i <= lenMain; i++)
    {
        uint32_t distance = matchDistances[offs + 1];
        m_optimum[i].m_posPrev = 0;
        m_optimum[i].m_backPrev = (uint16_t)distance;
        m_optimum[i].m_price = m_lenPrices[i - RCDeflateDefs::s_kMatchMinLen] + m_posPrices[GetPosSlot(distance)];
        if (i == matchDistances[offs])
        {
            offs += 2;
        }
    }

    uint32_t cur = 0;
    uint32_t lenEnd = lenMain;
    for (;;)
    {
        ++cur;
        if(cur == lenEnd || cur == RCDeflateDefs::s_kNumOptsBase || m_pos >= s_kDeflateMatchArrayLimit)
        {
            return Backward(backRes, cur);
        }
        GetMatches();
        matchDistances = m_matchDistances + 1;

        uint32_t numDistancePairs = m_matchDistances[0];
        uint32_t newLen = 0;
        if(numDistancePairs != 0)
        {
            newLen = matchDistances[numDistancePairs - 2];
            if(newLen > m_numFastBytes)
            {
                uint32_t len = Backward(backRes, cur);
                m_optimum[cur].m_backPrev = matchDistances[numDistancePairs - 1];
                m_optimumEndIndex = cur + newLen;
                m_optimum[cur].m_posPrev = (uint16_t)m_optimumEndIndex;
                MovePos(newLen - 1);
                return len;
            }
        }
        uint32_t curPrice = m_optimum[cur].m_price;
        uint32_t curAnd1Price = curPrice + m_literalPrices[Inline_MatchFinder_GetIndexByte(&m_lzInWindow, cur - m_additionalOffset)];
        RCDeflateEncoderOptimal& optimum = m_optimum[cur + 1];
        if (curAnd1Price < optimum.m_price)
        {
            optimum.m_price = curAnd1Price;
            optimum.m_posPrev = (uint16_t)cur;
        }
        if(numDistancePairs == 0)
        {
            continue;
        }
        while(lenEnd < cur + newLen)
        {
            m_optimum[++lenEnd].m_price = s_kDeflateIfinityPrice;
        }
        offs = 0;
        uint32_t distance = matchDistances[offs + 1];
        curPrice += m_posPrices[GetPosSlot(distance)];
        for(uint32_t lenTest = RCDeflateDefs::s_kMatchMinLen; ; lenTest++)
        {
            uint32_t curAndLenPrice = curPrice + m_lenPrices[lenTest - RCDeflateDefs::s_kMatchMinLen];
            RCDeflateEncoderOptimal& optimum = m_optimum[cur + lenTest];
            if (curAndLenPrice < optimum.m_price)
            {
                optimum.m_price = curAndLenPrice;
                optimum.m_posPrev = (uint16_t)cur;
                optimum.m_backPrev = (uint16_t)distance;
            }
            if (lenTest == matchDistances[offs])
            {
                offs += 2;
                if (offs == numDistancePairs)
                {
                    break;
                }
                curPrice -= m_posPrices[GetPosSlot(distance)];
                distance = matchDistances[offs + 1];
                curPrice += m_posPrices[GetPosSlot(distance)];
            }
        }
    }
}

uint32_t RCDeflateEncoder::GetOptimalFast(uint32_t &backRes)
{
    GetMatches();
    uint32_t numDistancePairs = m_matchDistances[0];
    if (numDistancePairs == 0)
    {
        return 1;
    }
    uint32_t lenMain = m_matchDistances[numDistancePairs - 1];
    backRes = m_matchDistances[numDistancePairs];
    MovePos(lenMain - 1);
    return lenMain;
}

void RCDeflateEncoder::LevelTableDummy(const byte_t* levels, int32_t numLevels, uint32_t* freqs)
{
    int32_t prevLen = 0xFF;
    int32_t nextLen = levels[0];
    int32_t count = 0;
    int32_t maxCount = 7;
    int32_t minCount = 4;
    if (nextLen == 0)
    {
        maxCount = 138;
        minCount = 3;
    }
    for (int32_t n = 0; n < numLevels; n++)
    {
        int32_t curLen = nextLen;
        nextLen = (n < numLevels - 1) ? levels[n + 1] : 0xFF;
        count++;
        if (count < maxCount && curLen == nextLen)
        {
            continue;
        }

        if (count < minCount)
        {
            freqs[curLen] += (uint32_t)count;
        }
        else if (curLen != 0)
        {
            if (curLen != prevLen)
            {
                freqs[curLen]++;
                count--;
            }
            freqs[RCDeflateDefs::s_kTableLevelRepNumber]++;
        }
        else if (count <= 10)
        {
            freqs[RCDeflateDefs::s_kTableLevel0Number]++;
        }
        else
        {
            freqs[RCDeflateDefs::s_kTableLevel0Number2]++;
        }

        count = 0;
        prevLen = curLen;

        if (nextLen == 0)
        {
            maxCount = 138;
            minCount = 3;
        }
        else if (curLen == nextLen)
        {
            maxCount = 6;
            minCount = 3;
        }
        else
        {
            maxCount = 7;
            minCount = 4;
        }
    }
}

void RCDeflateEncoder::WriteBits(uint32_t value, int32_t numBits)
{
    m_outStream.WriteBits(value, numBits);
}

void RCDeflateEncoder::LevelTableCode(const byte_t* levels, int32_t numLevels, const byte_t* lens, const uint32_t* codes)
{
    int32_t prevLen = 0xFF;
    int32_t nextLen = levels[0];
    int32_t count = 0;
    int32_t maxCount = 7;
    int32_t minCount = 4;
    if (nextLen == 0)
    {
        maxCount = 138;
        minCount = 3;
    }
    for (int32_t n = 0; n < numLevels; n++)
    {
        int32_t curLen = nextLen;
        nextLen = (n < numLevels - 1) ? levels[n + 1] : 0xFF;
        count++;
        if (count < maxCount && curLen == nextLen)
        {
            continue;
        }

        if (count < minCount)
        {
            for(int32_t i = 0; i < count; i++)
            {
                WriteBits(codes[curLen], lens[curLen]) ;
            }
        }
        else if (curLen != 0)
        {
            if (curLen != prevLen)
            {
                WriteBits(codes[curLen], lens[curLen]) ;
                count--;
            }
            WriteBits(codes[RCDeflateDefs::s_kTableLevelRepNumber], lens[RCDeflateDefs::s_kTableLevelRepNumber]) ;
            WriteBits(count - 3, 2);
        }
        else if (count <= 10)
        {
            WriteBits(codes[RCDeflateDefs::s_kTableLevel0Number], lens[RCDeflateDefs::s_kTableLevel0Number]) ;
            WriteBits(count - 3, 3);
        }
        else
        {
            WriteBits(codes[RCDeflateDefs::s_kTableLevel0Number2], lens[RCDeflateDefs::s_kTableLevel0Number2]) ;
            WriteBits(count - 11, 7);
        }

        count = 0;
        prevLen = curLen;

        if (nextLen == 0)
        {
            maxCount = 138;
            minCount = 3;
        }
        else if (curLen == nextLen)
        {
            maxCount = 6;
            minCount = 3;
        }
        else
        {
            maxCount = 7;
            minCount = 4;
        }
    }
}

void RCDeflateEncoder::MakeTables(uint32_t maxHuffLen)
{
    Huffman_Generate(m_mainFreqs, m_mainCodes, m_newLevels.m_litLenLevels, RCDeflateDefs::s_kFixedMainTableSize, maxHuffLen);
    Huffman_Generate(m_distFreqs, m_distCodes, m_newLevels.m_distLevels, RCDeflateDefs::s_kDistTableSize64, maxHuffLen);
}

uint32_t RCDeflateEncoder::GetLzBlockPrice() const
{
    return HuffmanGetPriceSpec(m_mainFreqs, m_newLevels.m_litLenLevels, RCDeflateDefs::s_kFixedMainTableSize, m_lenDirectBits, RCDeflateDefs::s_kSymbolMatch) +
           HuffmanGetPriceSpec(m_distFreqs, m_newLevels.m_distLevels, RCDeflateDefs::s_kDistTableSize64, s_kDeflateDistDirectBits, 0);
}

void RCDeflateEncoder::TryBlock()
{
    memset(m_mainFreqs, 0, sizeof(m_mainFreqs));
    memset(m_distFreqs, 0, sizeof(m_distFreqs));

    m_valueIndex = 0;
    uint32_t blockSize = m_blockSizeRes;
    m_blockSizeRes = 0;
    for (;;)
    {
        if (m_optimumCurrentIndex == m_optimumEndIndex)
        {
            if (m_pos >= s_kDeflateMatchArrayLimit || m_blockSizeRes >= blockSize || !m_secondPass &&
                ((Inline_MatchFinder_GetNumAvailableBytes(&m_lzInWindow) == 0) || m_valueIndex >= m_valueBlockSize))
            {
                break;
            }
        }
        uint32_t pos = 0 ;
        uint32_t len = 0 ;
        if (m_fastMode)
        {
            len = GetOptimalFast(pos);
        }
        else
        {
            len = GetOptimal(pos);
        }
        RCDeflateEncoderValue& codeValue = m_values[m_valueIndex++];
        if (len >= RCDeflateDefs::s_kMatchMinLen)
        {
            uint32_t newLen = len - RCDeflateDefs::s_kMatchMinLen;
            codeValue.m_len = (uint16_t)newLen;
            m_mainFreqs[RCDeflateDefs::s_kSymbolMatch + g_DeflateEncoderLenSlots[newLen]]++;
            codeValue.m_pos = (uint16_t)pos;
            m_distFreqs[GetPosSlot(pos)]++;
        }
        else
        {
            byte_t b = Inline_MatchFinder_GetIndexByte(&m_lzInWindow, 0 - m_additionalOffset);
            m_mainFreqs[b]++;
            codeValue.SetAsLiteral();
            codeValue.m_pos = b;
        }
        m_additionalOffset -= len;
        m_blockSizeRes += len;
    }
    m_mainFreqs[RCDeflateDefs::s_kSymbolEndOfBlock]++;
    m_additionalOffset += m_blockSizeRes;
    m_secondPass = true;
}

uint32_t RCDeflateEncoder::TryDynBlock(int32_t tableIndex, uint32_t numPasses)
{
    RCDeflateEncoderTables& t = m_tables[tableIndex];
    m_blockSizeRes = t.m_blockSizeRes;
    uint32_t posTemp = t.m_pos;
    SetPrices(t);

    for (uint32_t p = 0; p < numPasses; p++)
    {
        m_pos = posTemp;
        TryBlock();
        uint32_t numHuffBits =
            (m_valueIndex > 18000 ? 12 :
            (m_valueIndex >  7000 ? 11 :
            (m_valueIndex >  2000 ? 10 : 9)));
        MakeTables(numHuffBits);
        SetPrices(m_newLevels);
    }

    (RCDeflateLevels&)t = m_newLevels;

    m_numLitLenLevels = RCDeflateDefs::s_kMainTableSize;
    while(m_numLitLenLevels > RCDeflateDefs::s_kNumLitLenCodesMin && m_newLevels.m_litLenLevels[m_numLitLenLevels - 1] == 0)
    {
        m_numLitLenLevels--;
    }

    m_numDistLevels = RCDeflateDefs::s_kDistTableSize64;
    while(m_numDistLevels > RCDeflateDefs::s_kNumDistCodesMin && m_newLevels.m_distLevels[m_numDistLevels - 1] == 0)
    {
        m_numDistLevels--;
    }

    uint32_t levelFreqs[RCDeflateDefs::s_kLevelTableSize];
    memset(levelFreqs, 0, sizeof(levelFreqs));

    LevelTableDummy(m_newLevels.m_litLenLevels, m_numLitLenLevels, levelFreqs);
    LevelTableDummy(m_newLevels.m_distLevels, m_numDistLevels, levelFreqs);

    Huffman_Generate(levelFreqs, m_levelCodes, m_levelLens, RCDeflateDefs::s_kLevelTableSize, s_kDeflateMaxLevelBitLength);

    m_numLevelCodes = RCDeflateDefs::s_kNumLevelCodesMin;
    for (uint32_t i = 0; i < RCDeflateDefs::s_kLevelTableSize; i++)
    {
        byte_t level = m_levelLens[s_kDeflateCodeLengthAlphabetOrder[i]];
        if (level > 0 && i >= m_numLevelCodes)
        {
            m_numLevelCodes = i + 1;
        }
        m_levelLevels[i] = level;
    }

    return  GetLzBlockPrice() +
            HuffmanGetPriceSpec(levelFreqs, m_levelLens, RCDeflateDefs::s_kLevelTableSize, s_kDeflateLevelDirectBits, RCDeflateDefs::s_kTableDirectLevels) +
            RCDeflateDefs::s_kNumLenCodesFieldSize + RCDeflateDefs::s_kNumDistCodesFieldSize + RCDeflateDefs::s_kNumLevelCodesFieldSize +
            m_numLevelCodes * RCDeflateDefs::s_kLevelFieldSize + RCDeflateDefs::s_kFinalBlockFieldSize + RCDeflateDefs::s_kBlockTypeFieldSize;
}

uint32_t RCDeflateEncoder::TryFixedBlock(int32_t tableIndex)
{
    RCDeflateEncoderTables& t = m_tables[tableIndex];
    m_blockSizeRes = t.m_blockSizeRes;
    m_pos = t.m_pos;
    m_newLevels.SetFixedLevels();
    SetPrices(m_newLevels);
    TryBlock();
    return RCDeflateDefs::s_kFinalBlockFieldSize + RCDeflateDefs::s_kBlockTypeFieldSize + GetLzBlockPrice();
}

void RCDeflateEncoder::SetPrices(const RCDeflateLevels& levels)
{
    if (m_fastMode)
    {
        return;
    }
    uint32_t i = 0 ;
    for(i = 0; i < 256; i++)
    {
        byte_t price = levels.m_litLenLevels[i];
        m_literalPrices[i] = ((price != 0) ? price : s_kDeflateNoLiteralStatPrice);
    }

    for(i = 0; i < m_numLenCombinations; i++)
    {
        uint32_t slot = g_DeflateEncoderLenSlots[i];
        byte_t price = levels.m_litLenLevels[RCDeflateDefs::s_kSymbolMatch + slot];
        m_lenPrices[i] = (byte_t)(((price != 0) ? price : s_kDeflateNoLenStatPrice) + m_lenDirectBits[slot]);
    }

    for(i = 0; i < RCDeflateDefs::s_kDistTableSize64; i++)
    {
        byte_t price = levels.m_distLevels[i];
        m_posPrices[i] = (byte_t)(((price != 0) ? price: s_kDeflateNoPosStatPrice) + s_kDeflateDistDirectBits[i]);
    }
}

void RCDeflateEncoder::WriteBlock()
{
    HuffmanReverseBits(m_mainCodes, m_newLevels.m_litLenLevels, RCDeflateDefs::s_kFixedMainTableSize);
    HuffmanReverseBits(m_distCodes, m_newLevels.m_distLevels, RCDeflateDefs::s_kDistTableSize64);

    for (uint32_t i = 0; i < m_valueIndex; i++)
    {
        const RCDeflateEncoderValue& codeValue = m_values[i];
        if (codeValue.IsLiteral())
        {
            m_outStream.WriteBits(m_mainCodes[codeValue.m_pos], m_newLevels.m_litLenLevels[codeValue.m_pos]) ;
        }
        else
        {
            uint32_t len = codeValue.m_len;
            uint32_t lenSlot = g_DeflateEncoderLenSlots[len];
            m_outStream.WriteBits(m_mainCodes[RCDeflateDefs::s_kSymbolMatch + lenSlot], 
                                  m_newLevels.m_litLenLevels[RCDeflateDefs::s_kSymbolMatch + lenSlot]) ;
            m_outStream.WriteBits(len - m_lenStart[lenSlot], m_lenDirectBits[lenSlot]);
            uint32_t dist = codeValue.m_pos;
            uint32_t posSlot = GetPosSlot(dist);
            m_outStream.WriteBits(m_distCodes[posSlot], m_newLevels.m_distLevels[posSlot]) ;
            m_outStream.WriteBits(dist - s_kDeflateDistStart[posSlot], s_kDeflateDistDirectBits[posSlot]);
        }
    }
    m_outStream.WriteBits(m_mainCodes[RCDeflateDefs::s_kSymbolEndOfBlock], m_newLevels.m_litLenLevels[RCDeflateDefs::s_kSymbolEndOfBlock]) ;
}

HResult RCDeflateEncoder::Create()
{
    try {
        if (m_values == 0)
        {
            m_values = (RCDeflateEncoderValue*)RCAlloc::Instance().MyAlloc((s_kDeflateMaxUncompressedBlockSize) * sizeof(RCDeflateEncoderValue));
            if (m_values == 0)
            {
                return RC_E_OUTOFMEMORY;
            }
        }
        if (m_tables == 0)
        {
            m_tables = (RCDeflateEncoderTables *)RCAlloc::Instance().MyAlloc((s_kDeflateNumTables) * sizeof(RCDeflateEncoderTables));
            if (m_tables == 0)
            {
                return RC_E_OUTOFMEMORY;
            }
        }

        if (m_isMultiPass)
        {
            if (m_onePosMatchesMemory == 0)
            {
                m_onePosMatchesMemory = (uint16_t *)RCAlloc::Instance().MidAlloc(s_kDeflateMatchArraySize * sizeof(uint16_t));
                if (m_onePosMatchesMemory == 0)
                {
                    return RC_E_OUTOFMEMORY;
                }
            }
        }
        else
        {
            if (m_distanceMemory == 0)
            {
                m_distanceMemory = (uint16_t *)RCAlloc::Instance().MyAlloc((RCDeflateDefs::s_kMatchMaxLen + 2) * 2 * sizeof(uint16_t));
                if (m_distanceMemory == 0)
                {
                    return RC_E_OUTOFMEMORY;
                }
                m_matchDistances = m_distanceMemory;
            }
        }

        if (!m_created)
        {
            m_lzInWindow.btMode = m_btMode ? 1 : 0;
            m_lzInWindow.numHashBytes = 3;
            if (!MatchFinder_Create(&m_lzInWindow,
                m_deflate64Mode ? RCDeflateDefs::s_kHistorySize64 : RCDeflateDefs::s_kHistorySize32,
                RCDeflateDefs::s_kNumOpts + s_kDeflateMaxUncompressedBlockSize,
                m_numFastBytes, m_matchMaxLen - m_numFastBytes, &s_alloc))
            {
                return RC_E_OUTOFMEMORY;
            }
            if (!m_outStream.Create(1 << 20))
            {
                return RC_E_OUTOFMEMORY;
            }
        }
        if (m_matchFinderCycles != 0)
        {
            m_lzInWindow.cutValue = m_matchFinderCycles;
        }
        m_created = true;
        return RC_S_OK;
    }
    catch(...)
    {
        return RC_E_OUTOFMEMORY;
    }
}

void RCDeflateEncoder::Free()
{
    RCAlloc::Instance().MidFree(m_onePosMatchesMemory);
    m_onePosMatchesMemory = 0;
    RCAlloc::Instance().MyFree(m_distanceMemory);
    m_distanceMemory = 0;
    RCAlloc::Instance().MyFree(m_values);
    m_values = 0;
    RCAlloc::Instance().MyFree(m_tables);
    m_tables = 0;
}

void RCDeflateEncoder::WriteStoreBlock(uint32_t blockSize, uint32_t additionalOffset, bool finalBlock)
{
    do
    {
        uint32_t curBlockSize = (blockSize < (1 << 16)) ? blockSize : (1 << 16) - 1;
        blockSize -= curBlockSize;
        WriteBits((finalBlock && (blockSize == 0) ? RCDeflateDefs::s_kFinalBlock: RCDeflateDefs::s_kNotFinalBlock), RCDeflateDefs::s_kFinalBlockFieldSize);
        WriteBits(RCDeflateDefs::s_kStored, RCDeflateDefs::s_kBlockTypeFieldSize);
        m_outStream.FlushByte();
        WriteBits((uint16_t)curBlockSize, RCDeflateDefs::s_kStoredBlockLengthFieldSize);
        WriteBits((uint16_t)~curBlockSize, RCDeflateDefs::s_kStoredBlockLengthFieldSize);
        const byte_t *data = Inline_MatchFinder_GetPointerToCurrentPos(&m_lzInWindow)- additionalOffset;
        for(uint32_t i = 0; i < curBlockSize; i++)
        {
            m_outStream.WriteByte(data[i]);
        }
        additionalOffset -= curBlockSize;        
    } while(blockSize != 0);
}

void RCDeflateEncoder::ReleaseStreams()
{
    m_seqInStream.RealStream.Release();
    m_outStream.ReleaseStream();
}

uint32_t RCDeflateEncoder::GetBlockPrice(int32_t tableIndex, int32_t numDivPasses)
{
    RCDeflateEncoderTables& t = m_tables[tableIndex];
    t.m_staticMode = false;
    uint32_t price = TryDynBlock(tableIndex, m_numPasses);
    t.m_blockSizeRes = m_blockSizeRes;
    uint32_t numValues = m_valueIndex;
    uint32_t posTemp = m_pos;
    uint32_t additionalOffsetEnd = m_additionalOffset;

    if (m_checkStatic && m_valueIndex <= s_kDeflateFixedHuffmanCodeBlockSizeMax)
    {
        const uint32_t fixedPrice = TryFixedBlock(tableIndex);
        t.m_staticMode = (fixedPrice < price);
        if (t.m_staticMode)
        {
            price = fixedPrice;
        }
    }

    const uint32_t storePrice = GetStorePrice(m_blockSizeRes, 0); // bitPosition
    t.m_storeMode = (storePrice <= price);
    if (t.m_storeMode)
    {
        price = storePrice;
    }

    t.m_useSubBlocks = false;

    if (numDivPasses > 1 && numValues >= s_kDeflateDivideCodeBlockSizeMin)
    {
        RCDeflateEncoderTables& t0 = m_tables[(tableIndex << 1)];
        (RCDeflateLevels &)t0 = t;
        t0.m_blockSizeRes = t.m_blockSizeRes >> 1;
        t0.m_pos = t.m_pos;
        uint32_t subPrice = GetBlockPrice((tableIndex << 1), numDivPasses - 1);

        uint32_t blockSize2 = t.m_blockSizeRes - t0.m_blockSizeRes;
        if (t0.m_blockSizeRes >= s_kDeflateDivideBlockSizeMin && blockSize2 >= s_kDeflateDivideBlockSizeMin)
        {
            RCDeflateEncoderTables &t1 = m_tables[(tableIndex << 1) + 1];
            (RCDeflateLevels &)t1 = t;
            t1.m_blockSizeRes = blockSize2;
            t1.m_pos = m_pos;
            m_additionalOffset -= t0.m_blockSizeRes;
            subPrice += GetBlockPrice((tableIndex << 1) + 1, numDivPasses - 1);
            t.m_useSubBlocks = (subPrice < price);
            if (t.m_useSubBlocks)
            {
                price = subPrice;
            }
        }
    }
    m_additionalOffset = additionalOffsetEnd;
    m_pos = posTemp;
    return price;
}

void RCDeflateEncoder::CodeBlock(int32_t tableIndex, bool finalBlock)
{
    RCDeflateEncoderTables& t = m_tables[tableIndex];
    if (t.m_useSubBlocks)
    {
        CodeBlock((tableIndex << 1), false);
        CodeBlock((tableIndex << 1) + 1, finalBlock);
    }
    else
    {
        if (t.m_storeMode)
        {
            WriteStoreBlock(t.m_blockSizeRes, m_additionalOffset, finalBlock);
        }
        else
        {
            WriteBits((finalBlock ? RCDeflateDefs::s_kFinalBlock: RCDeflateDefs::s_kNotFinalBlock), RCDeflateDefs::s_kFinalBlockFieldSize);
            if (t.m_staticMode)
            {
                WriteBits(RCDeflateDefs::s_kFixedHuffman, RCDeflateDefs::s_kBlockTypeFieldSize);
                TryFixedBlock(tableIndex);
                int32_t i = 0 ;
                const int32_t kMaxStaticHuffLen = 9 ;
                for (i = 0; i < RCDeflateDefs::s_kFixedMainTableSize; i++)
                {
                    m_mainFreqs[i] = (uint32_t)1 << (kMaxStaticHuffLen - m_newLevels.m_litLenLevels[i]);
                }
                for (i = 0; i < RCDeflateDefs::s_kFixedDistTableSize; i++)
                {
                    m_distFreqs[i] = (uint32_t)1 << (kMaxStaticHuffLen - m_newLevels.m_distLevels[i]);
                }
                MakeTables(kMaxStaticHuffLen);
            }
            else
            {
                if (m_numDivPasses > 1 || m_checkStatic)
                {
                    TryDynBlock(tableIndex, 1);
                }
                WriteBits(RCDeflateDefs::s_kDynamicHuffman, RCDeflateDefs::s_kBlockTypeFieldSize);
                WriteBits(m_numLitLenLevels - RCDeflateDefs::s_kNumLitLenCodesMin, RCDeflateDefs::s_kNumLenCodesFieldSize);
                WriteBits(m_numDistLevels - RCDeflateDefs::s_kNumDistCodesMin, RCDeflateDefs::s_kNumDistCodesFieldSize);
                WriteBits(m_numLevelCodes - RCDeflateDefs::s_kNumLevelCodesMin, RCDeflateDefs::s_kNumLevelCodesFieldSize);

                for (uint32_t i = 0; i < m_numLevelCodes; i++)
                {
                    WriteBits(m_levelLevels[i], RCDeflateDefs::s_kLevelFieldSize);
                }

                HuffmanReverseBits(m_levelCodes, m_levelLens, RCDeflateDefs::s_kLevelTableSize);
                LevelTableCode(m_newLevels.m_litLenLevels, m_numLitLenLevels, m_levelLens, m_levelCodes);
                LevelTableCode(m_newLevels.m_distLevels, m_numDistLevels, m_levelLens, m_levelCodes);
            }
            WriteBlock();
        }
        m_additionalOffset -= t.m_blockSizeRes;
    }
}

HResult RCDeflateEncoder::CodeReal( ISequentialInStream* inStream,
                                    ISequentialOutStream* outStream, 
                                     const uint64_t* inSize, 
                                     const uint64_t* outSize,
                                     ICompressProgressInfo* progress)
{
    m_checkStatic = (m_numPasses != 1 || m_numDivPasses != 1);
    m_isMultiPass = (m_checkStatic || (m_numPasses != 1 || m_numDivPasses != 1));

    HResult hr = Create() ;
    if(!IsSuccess(hr))
    {
        return hr ;
    }

    m_valueBlockSize = (7 << 10) + (1 << 12) * m_numDivPasses;

    uint64_t nowPos = 0;

    m_seqInStream.RealStream = inStream;
    m_seqInStream.SeqInStream.Read = Read;
    m_lzInWindow.stream = &m_seqInStream.SeqInStream;

    MatchFinder_Init(&m_lzInWindow);
    m_outStream.SetStream(outStream);
    m_outStream.Init();

    RCDeflateEncoderReleaser coderReleaser(this);

    m_optimumEndIndex = m_optimumCurrentIndex = 0;

    RCDeflateEncoderTables &t = m_tables[1];
    t.m_pos = 0;
    t.InitStructures();

    m_additionalOffset = 0;
    do
    {
        t.m_blockSizeRes = s_kDeflateBlockUncompressedSizeThreshold;
        m_secondPass = false;
        GetBlockPrice(1, m_numDivPasses);
        CodeBlock(1, Inline_MatchFinder_GetNumAvailableBytes(&m_lzInWindow) == 0);
        nowPos += m_tables[1].m_blockSizeRes;
        if (progress != NULL)
        {
            uint64_t packSize = m_outStream.GetProcessedSize();
            HResult hr = progress->SetRatioInfo(nowPos, packSize) ;
            if(!IsSuccess(hr))
            {
                return hr ;
            }
        }
    }
    while (Inline_MatchFinder_GetNumAvailableBytes(&m_lzInWindow) != 0);
    if (m_lzInWindow.result != SZ_OK)
    {
        return m_lzInWindow.result;
    }
    return m_outStream.Flush();
}

HResult RCDeflateEncoder::BaseCode( ISequentialInStream* inStream,
                                    ISequentialOutStream* outStream, 
                                    const uint64_t* inSize, 
                                    const uint64_t* outSize,
                                    ICompressProgressInfo *progress)
{
    try
    {
        return CodeReal(inStream, outStream, inSize, outSize, progress);
    }
    catch(HResult errorCode)
    {
        return errorCode ;
    }
    catch(...)
    {
        return RC_E_FAIL ;
    }
}

HResult RCDeflateEncoder::BaseSetEncoderProperties2(const RCPropertyIDPairArray& propertyArray)
{
    RCPropertyIDPairArray::const_iterator pos = propertyArray.begin() ;
    for(; pos != propertyArray.end(); ++pos)
    {
        const RCPropertyIDPair& idPair = *pos ;
        RCPropertyID propID = idPair.first ;
        const RCVariant& propVariant = idPair.second ;
        switch (propID)
        {
            case RCCoderPropID::kNumPasses:
                {
                    if (!IsInteger64Type(propVariant))
                    {
                        return RC_E_INVALIDARG ;
                    }
                    m_numDivPasses = static_cast<int32_t>( GetInteger64Value(propVariant) ) ;                    
                    if (m_numDivPasses == 0)
                    {
                        m_numDivPasses = 1;
                    }
                    if (m_numDivPasses == 1)
                    {
                        m_numPasses = 1;
                    }
                    else if (m_numDivPasses <= s_kDeflateNumDivPassesMax)
                    {
                        m_numPasses = 2;
                    }
                    else
                    {
                        m_numPasses = 2 + (m_numDivPasses - s_kDeflateNumDivPassesMax);
                        m_numDivPasses = s_kDeflateNumDivPassesMax;
                    }
                }
                break ;
            case RCCoderPropID::kNumFastBytes:
                {
                    if (!IsInteger64Type(propVariant))
                    {
                        return RC_E_INVALIDARG ;
                    }
                    m_numFastBytes = static_cast<uint32_t>( GetInteger64Value(propVariant) ) ; 
                    if(m_numFastBytes < RCDeflateDefs::s_kMatchMinLen || m_numFastBytes > m_matchMaxLen)
                    {
                        return RC_E_INVALIDARG; 
                    }
                }
                break ;
            case RCCoderPropID::kMatchFinderCycles:
                {
                    if (!IsInteger64Type(propVariant))
                    {
                        return RC_E_INVALIDARG ;
                    }
                    m_matchFinderCycles = static_cast<uint32_t>( GetInteger64Value(propVariant) ) ; 
                }
                break ;
            case RCCoderPropID::kAlgorithm:
                {
                    if (!IsInteger64Type(propVariant))
                    {
                        return RC_E_INVALIDARG ;
                    }
                    uint32_t maximize = static_cast<uint32_t>( GetInteger64Value(propVariant) ) ; 
                    m_fastMode = (maximize == 0) ;
                    m_btMode = !m_fastMode ;
                }
                break ;
            default:
                return RC_E_INVALIDARG ;
        }
    }
    return RC_S_OK;
}

result_t RCDeflateEncoder::Read(void* object, void* data, size_t* size)
{
    const uint32_t kStepSize = (uint32_t)1 << 31;
    uint32_t curSize = ((*size < kStepSize) ? (uint32_t)*size : kStepSize);
    HResult res = ((CSeqInStream *)object)->RealStream->Read(data, curSize, &curSize);
    *size = curSize;
    return (result_t)res;
}

uint32_t RCDeflateEncoder::GetPosSlot(uint32_t pos)
{
    if (pos < 0x200)
    {
        return g_DeflateEncoderFastPos[pos];
    }
    return g_DeflateEncoderFastPos[pos >> 8] + 16 ;
}

void* RCDeflateEncoder::SzAlloc(void* p, size_t size)
{
    p = p; 
    return RCAlloc::Instance().MyAlloc(size);
}
void RCDeflateEncoder::SzFree(void* p, void* address)
{
    p = p; 
    RCAlloc::Instance().MyFree(address);
}

uint32_t RCDeflateEncoder::HuffmanGetPrice(const uint32_t *freqs, const byte_t *lens, uint32_t num)
{
    uint32_t price = 0;
    uint32_t i = 0 ;
    for (i = 0; i < num; i++)
    {
        price += lens[i] * freqs[i];
    }
    return price ;
};

uint32_t RCDeflateEncoder::HuffmanGetPriceSpec(const uint32_t *freqs, const byte_t *lens, uint32_t num, const byte_t *extraBits, uint32_t extraBase)
{
    return HuffmanGetPrice(freqs, lens, num) +
           HuffmanGetPrice(freqs + extraBase, extraBits, num - extraBase) ;
}

void RCDeflateEncoder::HuffmanReverseBits(uint32_t *codes, const byte_t *lens, uint32_t num)
{
    for (uint32_t i = 0; i < num; i++)
    {
        uint32_t x = codes[i];
        x = ((x & 0x5555) << 1) | ((x & 0xAAAA) >> 1);
        x = ((x & 0x3333) << 2) | ((x & 0xCCCC) >> 2);
        x = ((x & 0x0F0F) << 4) | ((x & 0xF0F0) >> 4);
        codes[i] = (((x & 0x00FF) << 8) | ((x & 0xFF00) >> 8)) >> (16 - lens[i]);
    }
}

uint32_t RCDeflateEncoder::GetStorePrice(uint32_t blockSize, int32_t bitPosition)
{
    uint32_t price = 0;
    do
    {
        uint32_t nextBitPosition = (bitPosition + RCDeflateDefs::s_kFinalBlockFieldSize + RCDeflateDefs::s_kBlockTypeFieldSize) & 7;
        int32_t numBitsForAlign = nextBitPosition > 0 ? (8 - nextBitPosition): 0;
        uint32_t curBlockSize = (blockSize < (1 << 16)) ? blockSize : (1 << 16) - 1;
        price += RCDeflateDefs::s_kFinalBlockFieldSize + RCDeflateDefs::s_kBlockTypeFieldSize + numBitsForAlign + (2 + 2) * 8 + curBlockSize * 8;
        bitPosition = 0;
        blockSize -= curBlockSize;
    } while(blockSize != 0);
    return price ;
}

END_NAMESPACE_RCZIP
