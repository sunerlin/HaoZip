/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "compress/shrink/RCShrinkDecoder.h"
#include "common/RCInBuffer.h"
#include "common/RCOutBuffer.h"
#include "common/RCBitlDecoder.h"
#include "common/RCAlloc.h"

/////////////////////////////////////////////////////////////////
//RCShrinkDecoder class implementation

BEGIN_NAMESPACE_RCZIP

RCShrinkDecoder::RCShrinkDecoder()
{
}

RCShrinkDecoder::~RCShrinkDecoder()
{
}

HResult RCShrinkDecoder::Code(ISequentialInStream* inStream,
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

HResult RCShrinkDecoder::CodeReal(ISequentialInStream* inStream,
                                  ISequentialOutStream* outStream, 
                                  const uint64_t* inSize, 
                                  const uint64_t* outSize,
                                  ICompressProgressInfo* progress)
{
    RCBitlDecoder<RCInBuffer> inBuffer;
    RCOutBuffer outBuffer;

    if (!inBuffer.Create(s_kBufferSize))
    {
        return RC_E_OUTOFMEMORY;
    }
    inBuffer.SetStream(inStream);
    inBuffer.Init();

    if (!outBuffer.Create(s_kBufferSize))
    {
        return RC_E_OUTOFMEMORY;
    }
    outBuffer.SetStream(outStream);
    outBuffer.Init();

    uint64_t prevPos = 0;
    int32_t numBits = s_kNumMinBits;
    uint32_t head = 257;
    bool needPrev = false;
    uint32_t lastSymbol = 0;

    int32_t i = 0 ;
    for (i = 0; i < s_kNumItems; i++)
    {
        m_parents[i] = 0;
    }
    for (i = 0; i < s_kNumItems; i++)
    {
        m_suffixes[i] = 0;
    }
    for (i = 0; i < 257; i++)
    {
        m_isFree[i] = false;
    }
    for (; i < s_kNumItems; i++)
    {
        m_isFree[i] = true;
    }

    for (;;)
    {
        uint32_t symbol = inBuffer.ReadBits(numBits);
        if (inBuffer.ExtraBitsWereRead())
        {
            break;
        }
        if (m_isFree[symbol])
        {
            return RC_S_FALSE;
        }
        if (symbol == 256)
        {
            uint32_t symbol = inBuffer.ReadBits(numBits);
            if (symbol == 1)
            {
                if (numBits < s_kNumMaxBits)
                {
                    numBits++;
                }
            }
            else if (symbol == 2)
            {
                if (needPrev)
                {
                    m_isFree[head - 1] = true;
                }
                for (i = 257; i < s_kNumItems; i++)
                {
                    m_isParent[i] = false;
                }
                for (i = 257; i < s_kNumItems; i++)
                {
                    if (!m_isFree[i])
                    {
                        m_isParent[m_parents[i]] = true;
                    }
                }
                for (i = 257; i < s_kNumItems; i++)
                {
                    if (!m_isParent[i])
                    {
                        m_isFree[i] = true;
                    }
                }
                head = 257;
                while (head < s_kNumItems && !m_isFree[head])
                {
                    head++;
                }
                if (head < s_kNumItems)
                {
                    needPrev = true;
                    m_isFree[head] = false;
                    m_parents[head] = (uint16_t)lastSymbol;
                    head++;
                }
            }
            else
            {
                return RC_S_FALSE;
            }
            continue;
        }
        uint32_t cur = symbol;
        i = 0;
        int32_t corectionIndex = -1;
        while (cur >= 256)
        {
            if (cur == head - 1)
            {
                corectionIndex = i;
            }
            m_stack[i++] = m_suffixes[cur];
            cur = m_parents[cur];
        }
        m_stack[i++] = (byte_t)cur;
        if (needPrev)
        {
            m_suffixes[head - 1] = (byte_t)cur;
            if (corectionIndex >= 0)
            {
                m_stack[corectionIndex] = (byte_t)cur;
            }
        }
        while (i > 0)
        {
            outBuffer.WriteByte((m_stack[--i]));
        }
        while (head < s_kNumItems && !m_isFree[head])
        {
            head++;
        }
        if (head < s_kNumItems)
        {
            needPrev = true;
            m_isFree[head] = false;
            m_parents[head] = (uint16_t)symbol;
            head++;
        }
        else
        {
            needPrev = false;
        }
        lastSymbol = symbol;

        uint64_t nowPos = outBuffer.GetProcessedSize();
        if (progress != NULL && nowPos - prevPos > (1 << 18))
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
    return outBuffer.Flush();
}

END_NAMESPACE_RCZIP
