/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "compress/rar/RCRar2Decoder.h"

/////////////////////////////////////////////////////////////////
//RCRar2Decoder class implementation

BEGIN_NAMESPACE_RCZIP

static const byte_t s_kRar2LenStart[RCRar2Defs::s_kLenTableSize]            = {0,1,2,3,4,5,6,7,8,10,12,14,16,20,24,28,32,40,48,56,64,80,96,112,128,160,192,224};
static const byte_t s_kRar2LenDirectBits[RCRar2Defs::s_kLenTableSize]       = {0,0,0,0,0,0,0,0,1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4,  4,  5,  5,  5,  5};

static const uint32_t s_kRar2DistStart[RCRar2Defs::s_kDistTableSize]        = {0,1,2,3,4,6,8,12,16,24,32,48,64,96,128,192,256,384,512,768,1024,1536,2048,3072,4096,6144,8192,12288,16384,24576,32768U,49152U,65536,98304,131072,196608,262144,327680,393216,458752,524288,589824,655360,720896,786432,851968,917504,983040};
static const byte_t s_kRar2DistDirectBits[RCRar2Defs::s_kDistTableSize]     = {0,0,0,0,1,1,2, 2, 3, 3, 4, 4, 5, 5,  6,  6,  7,  7,  8,  8,   9,   9,  10,  10,  11,  11,  12,   12,   13,   13,    14,    14,   15,   15,    16,    16,    16,    16,    16,    16,    16,    16,    16,    16,    16,    16,    16,    16};

static const byte_t s_kRar2LevelDirectBits[RCRar2Defs::s_kLevelTableSize]   = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 3, 7};

static const byte_t s_kRar2Len2DistStarts[RCRar2Defs::s_kLen2NumNumbers]    = {0,4,8,16,32,64,128,192};
static const byte_t s_kRar2Len2DistDirectBits[RCRar2Defs::s_kLen2NumNumbers]= {2,2,3, 4, 5, 6,  6,  6};

static const uint32_t s_kRar2HistorySize = 1 << 20;

class RCRar2DecoderReleaser
{
public:
    RCRar2DecoderReleaser(RCRar2Decoder* coder):
        m_coder(coder)
    {
    }
    ~RCRar2DecoderReleaser()
    {
        m_coder->ReleaseStreams();
    }
private:
    RCRar2Decoder* m_coder;
};

RCRar2Decoder::RCRar2Decoder():
    m_audioMode(false),
    m_numChannels(0),
    m_repDistPtr(0),
    m_lastLength(0),
    m_packSize(0),
    m_isSolid(0)
{
}

RCRar2Decoder::~RCRar2Decoder()
{
}

void RCRar2Decoder::InitStructures()
{
    m_mmFilter.Init();
    for(int32_t i = 0; i < RCRar2Defs::s_kNumRepDists; i++)
    {
        m_repDists[i] = 0;
    }
    m_repDistPtr = 0;
    m_lastLength = 0;
    memset(m_lastLevels, 0, RCRar2Defs::s_kMaxTableSize);
}

uint32_t RCRar2Decoder::ReadBits(int32_t numBits)
{
    return m_inBitStream.ReadBits(numBits);
}

bool RCRar2Decoder::ReadTables(void)
{
    byte_t levelLevels[RCRar2Defs::s_kLevelTableSize];
    byte_t newLevels[RCRar2Defs::s_kMaxTableSize];
    m_audioMode = (ReadBits(1) == 1);

    if (ReadBits(1) == 0)
    {
        memset(m_lastLevels, 0, RCRar2Defs::s_kMaxTableSize);
    }
    int32_t numLevels = 0 ;
    if (m_audioMode)
    {
        m_numChannels = ReadBits(2) + 1;
        if (m_mmFilter.m_currentChannel >= m_numChannels)
        {
            m_mmFilter.m_currentChannel = 0;
        }
        numLevels = m_numChannels * RCRar2Defs::s_kMMTableSize;
    }
    else
    {
        numLevels = RCRar2Defs::s_kHeapTablesSizesSum;
    }

    int32_t i = 0 ;
    for (i = 0; i < RCRar2Defs::s_kLevelTableSize; i++)
    {
        levelLevels[i] = (byte_t)ReadBits(4);
    }
    if(!m_levelDecoder.SetCodeLengths(levelLevels))
    {
        return false ;
    }
    i = 0;
    while (i < numLevels)
    {
        uint32_t number = m_levelDecoder.DecodeSymbol(&m_inBitStream);
        if (number < RCRar2Defs::s_kTableDirectLevels)
        {
            newLevels[i] = (byte_t)((number + m_lastLevels[i]) & RCRar2Defs::s_kLevelMask);
            i++;
        }
        else
        {
            if (number == RCRar2Defs::s_kTableLevelRepNumber)
            {
                int32_t t = ReadBits(2) + 3;
                for (int32_t reps = t; reps > 0 && i < numLevels ; reps--, i++)
                {
                    newLevels[i] = newLevels[i - 1];
                }
            }
            else
            {
                int32_t num = 0 ;
                if (number == RCRar2Defs::s_kTableLevel0Number)
                {
                    num = ReadBits(3) + 3;
                }
                else if (number == RCRar2Defs::s_kTableLevel0Number2)
                {
                    num = ReadBits(7) + 11;
                }
                else
                {
                    return false;
                }
                for (;num > 0 && i < numLevels; num--)
                {
                    newLevels[i++] = 0;
                }
            }
        }
    }
    if (m_audioMode)
    {
        for (i = 0; i < m_numChannels; i++)
        {
            if(!m_mmDecoders[i].SetCodeLengths(&newLevels[i * RCRar2Defs::s_kMMTableSize]))
            {
                return false ;
            }
        }
    }
    else
    {
        if(!m_mainDecoder.SetCodeLengths(&newLevels[0]))
        {
            return false ;
        }
        if(!m_distDecoder.SetCodeLengths(&newLevels[RCRar2Defs::s_kMainTableSize]))
        {
            return false ;
        }
        if(!m_lenDecoder.SetCodeLengths(&newLevels[RCRar2Defs::s_kMainTableSize + RCRar2Defs::s_kDistTableSize]))
        {
            return false ;
        }
    }
    memcpy(m_lastLevels, newLevels, RCRar2Defs::s_kMaxTableSize);
    return true;
}

bool RCRar2Decoder::ReadLastTables()
{
    // it differs a little from pure RAR sources;
    // uint64_t ttt = m_inBitStream.GetProcessedSize() + 2;
    // + 2 works for: return 0xFF; in RCInBuffer::ReadByte.
    if (m_inBitStream.GetProcessedSize() + 7 <= m_packSize) // test it: probably incorrect;
    {
        // if (m_inBitStream.GetProcessedSize() + 2 <= m_packSize) // test it: probably incorrect;
        if (m_audioMode)
        {
            uint32_t symbol = m_mmDecoders[m_mmFilter.m_currentChannel].DecodeSymbol(&m_inBitStream);
            if (symbol == 256)
            {
                return ReadTables();
            }
            if (symbol >= RCRar2Defs::s_kMMTableSize)
            {
                return false;
            }
        }
        else
        {
            uint32_t number = m_mainDecoder.DecodeSymbol(&m_inBitStream);
            if (number == RCRar2Defs::s_kReadTableNumber)
            {
                return ReadTables();
            }
            if (number >= RCRar2Defs::s_kMainTableSize)
            {
                return false;
            }
        }
    }
    return true;
}

bool RCRar2Decoder::DecodeMm(uint32_t pos)
{
    while (pos-- > 0)
    {
        uint32_t symbol = m_mmDecoders[m_mmFilter.m_currentChannel].DecodeSymbol(&m_inBitStream);
        if (symbol == 256)
        {
            return true;
        }
        if (symbol >= RCRar2Defs::s_kMMTableSize)
        {
            return false;
        }
        /*
        byte_t byPredict = m_Predictor.Predict();
        byte_t byReal = (byte_t)(byPredict - (byte_t)symbol);
        m_Predictor.Update(byReal, byPredict);
        */
        byte_t byReal = m_mmFilter.Decode((byte_t)symbol);
        m_outWindowStream.PutByte(byReal);
        if (++m_mmFilter.m_currentChannel == m_numChannels)
        {
            m_mmFilter.m_currentChannel = 0;
        }
    }
    return true;
}

bool RCRar2Decoder::DecodeLz(int32_t pos)
{
    while (pos > 0)
    {
        uint32_t number = m_mainDecoder.DecodeSymbol(&m_inBitStream);
        uint32_t length, distance;
        if (number < 256)
        {
            m_outWindowStream.PutByte(byte_t(number));
            pos--;
            continue;
        }
        else if (number >= RCRar2Defs::s_kMatchNumber)
        {
            number -= RCRar2Defs::s_kMatchNumber;
            length = RCRar2Defs::s_kNormalMatchMinLen + 
                     uint32_t(s_kRar2LenStart[number]) +
                     m_inBitStream.ReadBits(s_kRar2LenDirectBits[number]);
            number = m_distDecoder.DecodeSymbol(&m_inBitStream);
            if (number >= RCRar2Defs::s_kDistTableSize)
            {
                return false;
            }
            distance = s_kRar2DistStart[number] + m_inBitStream.ReadBits(s_kRar2DistDirectBits[number]);
            if (distance >= RCRar2Defs::s_kDistLimit3)
            {
                length += 2 - ((distance - RCRar2Defs::s_kDistLimit4) >> 31);
            }
        }
        else if (number == RCRar2Defs::s_kRepBothNumber)
        {
            length = m_lastLength;
            distance = m_repDists[(m_repDistPtr + 4 - 1) & 3];
        }
        else if (number < RCRar2Defs::s_kLen2Number)
        {
            distance = m_repDists[(m_repDistPtr - (number - RCRar2Defs::s_kRepNumber + 1)) & 3];
            number = m_lenDecoder.DecodeSymbol(&m_inBitStream);
            if (number >= RCRar2Defs::s_kLenTableSize)
            {
                return false;
            }
            length = 2 + s_kRar2LenStart[number] + m_inBitStream.ReadBits(s_kRar2LenDirectBits[number]);
            if (distance >= RCRar2Defs::s_kDistLimit2)
            {
                length++;
                if (distance >= RCRar2Defs::s_kDistLimit3)
                {
                    length += 2 - ((distance - RCRar2Defs::s_kDistLimit4) >> 31);
                }
            }
        }
        else if (number < RCRar2Defs::s_kReadTableNumber)
        {
            number -= RCRar2Defs::s_kLen2Number;
            distance = s_kRar2Len2DistStarts[number] +
                       m_inBitStream.ReadBits(s_kRar2Len2DistDirectBits[number]);
            length = 2 ;
        }
        else if (number == RCRar2Defs::s_kReadTableNumber)
        {
            return true;
        }
        else
        {
            return false;
        }
        m_repDists[m_repDistPtr++ & 3] = distance;
        m_lastLength = length;
        if (!m_outWindowStream.CopyBlock(distance, length))
        {
            return false;
        }
        pos -= length;
    }
    return true;
}

void RCRar2Decoder::ReleaseStreams()
{
    m_outWindowStream.ReleaseStream();
    m_inBitStream.ReleaseStream();
}

HResult RCRar2Decoder::Code(ISequentialInStream* inStream,
                            ISequentialOutStream* outStream, 
                            const uint64_t* inSize, 
                            const uint64_t* outSize,
                            ICompressProgressInfo* progress)
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

HResult RCRar2Decoder::CodeReal(ISequentialInStream* inStream,
                                ISequentialOutStream* outStream, 
                                const uint64_t* inSize, 
                                const uint64_t* outSize,
                                ICompressProgressInfo* progress)
{
    if (inSize == NULL || outSize == NULL)
    {
        return RC_E_INVALIDARG;
    }

    if (!m_outWindowStream.Create(s_kRar2HistorySize))
    {
        return RC_E_OUTOFMEMORY;
    }
    if (!m_inBitStream.Create(1 << 20))
    {
        return RC_E_OUTOFMEMORY;
    }

    m_packSize = *inSize;

    uint64_t pos = 0 ;
    uint64_t unPackSize = *outSize ;

    m_outWindowStream.SetStream(outStream);
    m_outWindowStream.Init(m_isSolid);
    m_inBitStream.SetStream(inStream);
    m_inBitStream.Init();

    RCRar2DecoderReleaser coderReleaser(this);
    if (!m_isSolid)
    {
        InitStructures();
        if (unPackSize == 0)
        {
            if (m_inBitStream.GetProcessedSize() + 2 <= m_packSize) // test it: probably incorrect;
            {
                if (!ReadTables())
                {
                    return RC_S_FALSE;
                }
            }
            return RC_S_OK;
        }
        if (!ReadTables())
        {
            return RC_S_FALSE;
        }
    }

    uint64_t startPos = m_outWindowStream.GetProcessedSize();
    while(pos < unPackSize)
    {
        uint32_t blockSize = 1 << 20;
        if (blockSize > unPackSize - pos)
        {
            blockSize = (uint32_t)(unPackSize - pos);
        }
        uint64_t blockStartPos = m_outWindowStream.GetProcessedSize();
        if (m_audioMode)
        {
            if (!DecodeMm(blockSize))
            {
                return RC_S_FALSE;
            }
        }
        else
        {
            if (!DecodeLz((int32_t)blockSize))
            {
                return RC_S_FALSE;
            }
        }
        uint64_t globalPos = m_outWindowStream.GetProcessedSize();
        pos = globalPos - blockStartPos;
        if (pos < blockSize)
        {
            if (!ReadTables())
            {
                return RC_S_FALSE;
            }
        }
        pos = globalPos - startPos;
        if (progress != 0)
        {
            uint64_t packSize = m_inBitStream.GetProcessedSize();
            HResult hr = progress->SetRatioInfo(packSize, pos) ;
            if(!IsSuccess(hr))
            {
                return hr ;
            }
        }
    }
    if (pos > unPackSize)
    {
        return RC_S_FALSE;
    }

    if (!ReadLastTables())
    {
        return RC_S_FALSE;
    }
    return m_outWindowStream.Flush();
}

HResult RCRar2Decoder::SetDecoderProperties2(const byte_t* data, uint32_t size)
{
    if (size < 1)
    {
        return RC_E_INVALIDARG ;
    }
    m_isSolid = (data[0] != 0) ;
    return RC_S_OK ;
}

END_NAMESPACE_RCZIP
