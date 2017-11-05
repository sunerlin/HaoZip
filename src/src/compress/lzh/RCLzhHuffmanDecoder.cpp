/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "compress/lzh/RCLzhHuffmanDecoder.h"
#include "interface/RCMethodDefs.h"

/////////////////////////////////////////////////////////////////
//RCLzhHuffmanDecoder class implementation

BEGIN_NAMESPACE_RCZIP

RCLzhHuffmanDecoder::CCoderReleaser::CCoderReleaser(RCLzhHuffmanDecoder* coder):
    m_coder(coder), 
    m_needFlush(true) 
{
    
}

RCLzhHuffmanDecoder::CCoderReleaser::~CCoderReleaser()
{
    if (m_needFlush)
    {
        m_coder->m_outWindowStream.Flush() ;
    }
    m_coder->ReleaseStreams() ;
}

RCLzhHuffmanDecoder::RCLzhHuffmanDecoder():
    m_numDictBits(0)
{
}

RCLzhHuffmanDecoder::~RCLzhHuffmanDecoder()
{
}

void RCLzhHuffmanDecoder::ReleaseStreams()
{
    m_outWindowStream.ReleaseStream();
    m_inBitStream.ReleaseStream();
}

uint32_t RCLzhHuffmanDecoder::ReadBits(int32_t numBits)
{
    return m_inBitStream.ReadBits(numBits) ;
}

HResult RCLzhHuffmanDecoder::ReadLevelTable()
{
    int32_t n = ReadBits(RCLzhDecoderDefs::s_kNumLevelBits);
    if (n == 0)
    {
        m_levelHuffman.SetSymbol(ReadBits(RCLzhDecoderDefs::s_kNumLevelBits));
        if (m_levelHuffman.GetSymbol() >= RCLzhDecoderDefs::s_kNumLevelSymbols)
        {
            return RC_S_FALSE ;
        }
    }
    else
    {
        if (n > RCLzhDecoderDefs::s_kNumLevelSymbols)
        {
            return RC_S_FALSE;
        }
        m_levelHuffman.SetSymbol(-1) ;
        byte_t lens[RCLzhDecoderDefs::s_kNumLevelSymbols];
        int32_t i = 0;
        while (i < n)
        {
            int32_t c = m_inBitStream.ReadBits(3);
            if (c == 7)
            {
                while (ReadBits(1))
                {
                    if (c++ > RCLzhDecoderDefs::s_kMaxHuffmanLen)
                    {
                        return RC_S_FALSE;
                    }
                }
            }
            lens[i++] = (byte_t)c;
            if (i == RCLzhDecoderDefs::s_kNumSpecLevelSymbols)
            {
                c = ReadBits(2);
                while (--c >= 0)
                {
                    lens[i++] = 0;
                }
            }
        }
        while (i < RCLzhDecoderDefs::s_kNumLevelSymbols)
        {
            lens[i++] = 0;
        }
        m_levelHuffman.SetCodeLengths(lens);
    }
    return RC_S_OK ;
}

HResult RCLzhHuffmanDecoder::ReadPTable(int32_t numBits)
{
    int32_t n = ReadBits(numBits);
    if (n == 0)
    {
        m_pHuffmanDecoder.SetSymbol(ReadBits(numBits));
        if (m_pHuffmanDecoder.GetSymbol() >= RCLzhDecoderDefs::s_kNumDistanceSymbols)
        {
            return RC_S_FALSE;
        }
    }
    else
    {
        if (n > RCLzhDecoderDefs::s_kNumDistanceSymbols)
        {
            return RC_S_FALSE;
        }
        m_pHuffmanDecoder.SetSymbol(-1);
        byte_t lens[RCLzhDecoderDefs::s_kNumDistanceSymbols];
        int32_t i = 0;
        while (i < n)
        {
            int32_t c = m_inBitStream.ReadBits(3);
            if (c == 7)
            {
                while (ReadBits(1))
                {
                    if (c > RCLzhDecoderDefs::s_kMaxHuffmanLen)
                    {
                        return RC_S_FALSE;
                    }
                    c++;
                }
            }
            lens[i++] = (byte_t)c;
        }
        while (i < RCLzhDecoderDefs::s_kNumDistanceSymbols)
        {
            lens[i++] = 0;
        }
        m_pHuffmanDecoder.SetCodeLengths(lens);
    }
    return RC_S_OK;
}

HResult RCLzhHuffmanDecoder::ReadCTable()
{
    int32_t n = ReadBits(RCLzhDecoderDefs::s_kNumCBits);
    if (n == 0)
    {
        m_cHuffmanDecoder.SetSymbol(ReadBits(RCLzhDecoderDefs::s_kNumCBits));
        if (m_cHuffmanDecoder.GetSymbol() >= RCLzhDecoderDefs::s_kNumCSymbols)
        {
            return RC_S_FALSE;
        }
    }
    else
    {
        if (n > RCLzhDecoderDefs::s_kNumCSymbols)
        {
            return RC_S_FALSE;
        }
        m_cHuffmanDecoder.SetSymbol(-1);
        byte_t lens[RCLzhDecoderDefs::s_kNumCSymbols];
        int32_t i = 0;
        while (i < n)
        {
            int32_t c = m_levelHuffman.Decode(&m_inBitStream);
            if (c < RCLzhDecoderDefs::s_kNumSpecLevelSymbols)
            {
                if (c == 0)
                {
                    c = 1;
                }
                else if (c == 1)
                {
                    c = ReadBits(4) + 3;
                }
                else
                {
                    c = ReadBits(RCLzhDecoderDefs::s_kNumCBits) + 20;
                }
                while (--c >= 0)
                {
                    if (i > RCLzhDecoderDefs::s_kNumCSymbols)
                    {
                        return RC_S_FALSE;
                    }
                    lens[i++] = 0;
                }
            }
            else
            {
                lens[i++] = (byte_t)(c - 2);
            }
        }
        while (i < RCLzhDecoderDefs::s_kNumCSymbols)
        {
            lens[i++] = 0;
        }
        m_cHuffmanDecoder.SetCodeLengths(lens);
    }
    return RC_S_OK;
}

void RCLzhHuffmanDecoder::SetDictionary(int32_t numDictBits)
{
    m_numDictBits = numDictBits ;
}

HResult RCLzhHuffmanDecoder::Code(ISequentialInStream* inStream,
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

HResult RCLzhHuffmanDecoder::CodeReal(ISequentialInStream* inStream,
                                  ISequentialOutStream* outStream, 
                                  const uint64_t* inSize, 
                                  const uint64_t* outSize,
                                  ICompressProgressInfo* progress)
{
    if (outSize == NULL)
    {
        return RC_E_INVALIDARG;
    }
    if (!m_outWindowStream.Create(RCLzhDecoderDefs::s_kHistorySize))
    {
        return RC_E_OUTOFMEMORY;
    }
    if (!m_inBitStream.Create(1 << 20))
    {
        return RC_E_OUTOFMEMORY;
    }

    uint64_t pos = 0;
    m_outWindowStream.SetStream(outStream);
    m_outWindowStream.Init(false);
    m_inBitStream.SetStream(inStream);
    m_inBitStream.Init();
    CCoderReleaser coderReleaser(this) ;

    int32_t pbit = 0 ;
    if (m_numDictBits <= 13)
    {
        pbit = 4;
    }
    else
    {
        pbit = 5;
    }
    uint32_t blockSize = 0;
    while(pos < *outSize)
    {
        if (blockSize == 0)
        {
            if (progress != NULL)
            {
                uint64_t packSize = m_inBitStream.GetProcessedSize();
                HResult hr = progress->SetRatioInfo(packSize, pos) ;
                if(!IsSuccess(hr))
                {
                    return hr ;
                }
            }
            blockSize = ReadBits(RCLzhDecoderDefs::s_kBlockSizeBits);
            ReadLevelTable();
            ReadCTable();
            HResult hr = ReadPTable(pbit) ;
            if(!IsSuccess(hr))
            {
                return hr ;
            }
        }
        blockSize--;
        uint32_t c = m_cHuffmanDecoder.Decode(&m_inBitStream);
        if (c < 256)
        {
            m_outWindowStream.PutByte((byte_t)c);
            pos++;
        }
        else if (c >= RCLzhDecoderDefs::s_kNumCSymbols)
        {
            return RC_S_FALSE;
        }
        else
        {
            uint32_t len  = c - 256 + RCLzhDecoderDefs::s_kMinMatch;
            uint32_t distance = m_pHuffmanDecoder.Decode(&m_inBitStream);
            if (distance != 0)
            {
                distance = (1 << (distance - 1)) + ReadBits(distance - 1);
            }
            if (distance >= pos)
            {
                return RC_S_FALSE;
            }
            if (pos + len > *outSize)
            {
                len = (uint32_t)(*outSize - pos);
            }
            pos += len ;
            m_outWindowStream.CopyBlock(distance, len);
        }
    }
    coderReleaser.m_needFlush = false;
    return m_outWindowStream.Flush() ;
}

HResult RCLzhHuffmanDecoder::SetCoderProperties(const RCPropertyIDPairArray& propertyArray)
{
    RCPropertyIDPairArray::const_iterator pos = propertyArray.begin() ;
    for(; pos != propertyArray.end(); ++pos)
    {
        const RCPropertyIDPair& pair = *pos ;
        if(pair.first == RCMethodProperty::ID_SET_DICTIONARY_SIZE)
        {
            if(IsInteger64Type(pair.second))
            {
                int32_t numDictBits = static_cast<int32_t>(GetInteger64Value(pair.second)) ;
                SetDictionary(numDictBits) ;
            }
            else
            {
                return RC_E_INVALIDARG ;
            }
        }
        else
        {
            return RC_E_INVALIDARG ;
        }
    }
    return RC_S_OK ;
}

END_NAMESPACE_RCZIP
