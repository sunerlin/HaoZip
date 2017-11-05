/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "compress/z/RCZDecoder.h"
#include "common/RCAlloc.h"
#include "common/RCInBuffer.h"
#include "common/RCOutBuffer.h"

/////////////////////////////////////////////////////////////////
//RCZDecoder class implementation

BEGIN_NAMESPACE_RCZIP

class RCZDecoderDefs
{
public:
    static const uint32_t   kBufferSize = (1 << 20);
    static const byte_t     kNumBitsMask = 0x1F;
    static const byte_t     kBlockModeMask = 0x80;
    static const int32_t    kNumMinBits = 9;
    static const int32_t    kNumMaxBits = 16;
};

RCZDecoder::RCZDecoder():
    m_properties(0),
    m_numMaxBits(0),
    m_parents(NULL),
    m_suffixes(NULL),
    m_stack(NULL)
{
}

RCZDecoder::~RCZDecoder()
{
    Free();
}

void RCZDecoder::Free()
{
    RCAlloc::Instance().MyFree(m_parents); 
    m_parents = 0;
    RCAlloc::Instance().MyFree(m_suffixes);
    m_suffixes = 0;
    RCAlloc::Instance().MyFree(m_stack);
    m_stack = 0;
}

HResult RCZDecoder::Code(ISequentialInStream* inStream,
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

HResult RCZDecoder::CodeReal(ISequentialInStream* inStream,
                             ISequentialOutStream* outStream, 
                             const uint64_t* inSize, 
                             const uint64_t* outSize,
                             ICompressProgressInfo* progress)
{
    RCInBuffer inBuffer;
    RCOutBuffer outBuffer;
    if (!inBuffer.Create(RCZDecoderDefs::kBufferSize))
    {
        return RC_E_OUTOFMEMORY;
    }
    inBuffer.SetStream(inStream);
    inBuffer.Init();

    if (!outBuffer.Create(RCZDecoderDefs::kBufferSize))
    {
        return RC_E_OUTOFMEMORY;
    }
    outBuffer.SetStream(outStream);
    outBuffer.Init();

    int maxbits = m_properties & RCZDecoderDefs::kNumBitsMask;
    if (maxbits < RCZDecoderDefs::kNumMinBits || maxbits > RCZDecoderDefs::kNumMaxBits)
    {
        return RC_S_FALSE;
    }
    uint32_t numItems = 1 << maxbits;
    bool blockMode = ((m_properties & RCZDecoderDefs::kBlockModeMask) != 0);

    if (maxbits != m_numMaxBits || m_parents == 0 || m_suffixes == 0 || m_stack == 0)
    {
        Free();
        m_parents = (uint16_t *)RCAlloc::Instance().MyAlloc(numItems * sizeof(uint16_t)); 
        if (m_parents == 0)
        {
            return RC_E_OUTOFMEMORY;
        }
        m_suffixes = (byte_t *)RCAlloc::Instance().MyAlloc(numItems * sizeof(byte_t)); 
        if (m_suffixes == 0)
        {
            return RC_E_OUTOFMEMORY;
        }
        m_stack = (byte_t *)RCAlloc::Instance().MyAlloc(numItems * sizeof(byte_t)); 
        if (m_stack == 0)
        {
            return RC_E_OUTOFMEMORY;
        }
        m_numMaxBits = maxbits;
    }

    uint64_t prevPos = 0;
    int numBits = RCZDecoderDefs::kNumMinBits;
    uint32_t head = blockMode ? 257 : 256;

    bool needPrev = false;

    unsigned bitPos = 0;
    unsigned numBufBits = 0;

    byte_t buf[RCZDecoderDefs::kNumMaxBits + 4];

    m_parents[256] = 0; // virus protection
    m_suffixes[256] = 0;

    for (;;)
    {
        if (numBufBits == bitPos)
        {
            numBufBits = (unsigned)inBuffer.ReadBytes(buf, numBits) * 8;
            bitPos = 0;
            uint64_t nowPos = outBuffer.GetProcessedSize();
            if (progress != NULL && nowPos - prevPos >= (1 << 18))
            {
                prevPos = nowPos;
                uint64_t packSize = inBuffer.GetProcessedSize();
                HResult hr = progress->SetRatioInfo(packSize, nowPos) ;
                if(!IsSuccess(hr))
                {
                    return hr ;
                }
            }
        }
        unsigned bytePos = bitPos >> 3;
        uint32_t symbol = buf[bytePos] | ((uint32_t)buf[bytePos + 1] << 8) | ((uint32_t)buf[bytePos + 2] << 16);
        symbol >>= (bitPos & 7);
        symbol &= (1 << numBits) - 1;
        bitPos += numBits;
        if (bitPos > numBufBits)
        {
            break;
        }
        if (symbol >= head)
        {
            return RC_S_FALSE;
        }
        if (blockMode && symbol == 256)
        {
            numBufBits = bitPos = 0;
            numBits = RCZDecoderDefs::kNumMinBits;
            head = 257;
            needPrev = false;
            continue;
        }
        uint32_t cur = symbol;
        int i = 0;
        while (cur >= 256)
        {
            m_stack[i++] = m_suffixes[cur];
            cur = m_parents[cur];
        }
        m_stack[i++] = (byte_t)cur;
        if (needPrev)
        {
            m_suffixes[head - 1] = (byte_t)cur;
            if (symbol == head - 1)
            {
                m_stack[0] = (byte_t)cur;
            }
        }
        do
        {
            outBuffer.WriteByte((m_stack[--i]));
        }
        while (i > 0);
        if (head < numItems)
        {
            needPrev = true;
            m_parents[head++] = (uint16_t)symbol;
            if (head > ((uint32_t)1 << numBits))
            {
                if (numBits < maxbits)
                {
                    numBufBits = bitPos = 0;
                    numBits++;
                }
            }
        }
        else
        {
            needPrev = false;
        }
    }
    return outBuffer.Flush() ;
}

HResult RCZDecoder::SetDecoderProperties2(const byte_t* data, uint32_t size)
{
    if (size < 1)
    {
        return RC_E_INVALIDARG;
    }
    m_properties = data[0];
    return RC_S_OK ;
}

END_NAMESPACE_RCZIP
