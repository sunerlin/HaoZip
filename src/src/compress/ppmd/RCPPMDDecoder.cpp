/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "compress/ppmd/RCPPMDDecoder.h"
#include "compress/ppmd/RCPPMDSubAlloc.h"

/////////////////////////////////////////////////////////////////
//RCPPMDDecoder class implementation

BEGIN_NAMESPACE_RCZIP

static const int kLenIdFinished = -1;
static const int kLenIdNeedInit = -2;

class RCPPMDDecoderFlusher
{
public:
    bool m_needFlush;
    
public:
    RCPPMDDecoderFlusher(RCPPMDDecoder *coder):
        _coder(coder), m_needFlush(true)
    {
    }
    ~RCPPMDDecoderFlusher()
    {
        if (m_needFlush)
        {
            _coder->Flush();
        }
        _coder->ReleaseStreams() ;
  }
private:
    RCPPMDDecoder* _coder;
};

RCPPMDDecoder::RCPPMDDecoder():
    m_order(0),
    m_usedMemorySize(0),
    m_remainLen(0),
    m_outSize(0),
    m_outSizeDefined(false),
    m_processedSize(0)
{
}

RCPPMDDecoder::~RCPPMDDecoder()
{
}

HResult RCPPMDDecoder::CodeSpec(uint32_t size, byte_t* memStream)
{
    if (m_outSizeDefined)
    {
        const uint64_t rem = m_outSize - m_processedSize;
        if (size > rem)
        {
            size = (uint32_t)rem;
        }
    }
    const uint32_t startSize = size;
    if (m_remainLen == kLenIdFinished)
    {
        return RC_S_OK;
    }
    if (m_remainLen == kLenIdNeedInit)
    {
        m_rangeDecoder.Init();
        m_remainLen = 0;
        m_info.m_maxOrder = 0;
        m_info.StartModelRare(m_order);
    }
    while (size != 0)
    {
        int32_t symbol = m_info.DecodeSymbol(&m_rangeDecoder);
        if (symbol < 0)
        {
            m_remainLen = kLenIdFinished;
            break;
        }
        if (memStream != 0)
        {
            *memStream++ = (byte_t)symbol;
        }
        else
        {
            m_outStream.WriteByte((byte_t)symbol);
        }
        size--;
    }
    m_processedSize += startSize - size;
    return RC_S_OK;
}

void RCPPMDDecoder::ReleaseStreams()
{
    ReleaseInStream();
    m_outStream.ReleaseStream();
}

HResult RCPPMDDecoder::Flush()
{
    return m_outStream.Flush();
}

HResult RCPPMDDecoder::Code(ISequentialInStream* inStream,
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

HResult RCPPMDDecoder::CodeReal(ISequentialInStream* inStream,
                                ISequentialOutStream* outStream, 
                                const uint64_t* inSize, 
                                const uint64_t* outSize,
                                ICompressProgressInfo* progress)
{
    if (!m_outStream.Create(1 << 20))
    {
        return RC_E_OUTOFMEMORY;
    }

    SetInStream(inStream);
    m_outStream.SetStream(outStream);
    SetOutStreamSize(outSize);
    RCPPMDDecoderFlusher flusher(this);

    for (;;)
    {
        m_processedSize = m_outStream.GetProcessedSize();
        uint32_t curSize = (1 << 18);
        HResult hr = CodeSpec(curSize, NULL) ;
        if(!IsSuccess(hr))
        {
            return hr ;
        }
        if (m_remainLen == kLenIdFinished)
        {
            break;
        }
        if (progress != NULL)
        {
            uint64_t inSize = m_rangeDecoder.GetProcessedSize();
            HResult hr = progress->SetRatioInfo(inSize, m_processedSize) ;
            if(!IsSuccess(hr))
            {
                return hr ;
            }
        }
        if (m_outSizeDefined)
        {
            if (m_outStream.GetProcessedSize() >= m_outSize)
            {
                break;
            }
        }
    }
    flusher.m_needFlush = false;
    return Flush() ;
}

HResult RCPPMDDecoder::SetDecoderProperties2(const byte_t* data, uint32_t size)
{
    if (size < 5)
    {
        return RC_E_INVALIDARG;
    }
    m_order = data[0];
    m_usedMemorySize = 0;
    for (int i = 0; i < 4; i++)
    {
        m_usedMemorySize += ((uint32_t)(data[1 + i])) << (i * 8);
    }

    if (m_usedMemorySize >RCPPMDSubAlloc::s_kMaxMemBlockSize)
    {
        return RC_E_NOTIMPL;
    }

    if (!m_rangeDecoder.Create(1 << 20))
    {
        return RC_E_OUTOFMEMORY;
    }
    if (!m_info.m_subAllocator.StartSubAllocator(m_usedMemorySize))
    {
        return RC_E_OUTOFMEMORY ;
    }
    return RC_S_OK ;
}

HResult RCPPMDDecoder::SetInStream(ISequentialInStream* inStream)
{
    m_rangeDecoder.SetStream(inStream);
    return RC_S_OK ;
}

HResult RCPPMDDecoder::ReleaseInStream()
{
    m_rangeDecoder.ReleaseStream();
    return RC_S_OK ;
}

HResult RCPPMDDecoder::SetOutStreamSize(const uint64_t* outSize)
{
    m_outSizeDefined = (outSize != NULL);
    if (m_outSizeDefined)
    {
        m_outSize = *outSize;
    }
    m_processedSize = 0;
    m_remainLen = kLenIdNeedInit;
    m_outStream.Init();
    return RC_S_OK ;
}

HResult RCPPMDDecoder::Read(void* data, uint32_t size, uint32_t* processedSize)
{
    try
    {
        if (processedSize)
        {
            *processedSize = 0;
        }
        const uint64_t startPos = m_processedSize;
        HResult hr = CodeSpec(size, (byte_t *)data) ;
        if(!IsSuccess(hr))
        {
            return hr ;
        }
        if (processedSize)
        {
            *processedSize = (uint32_t)(m_processedSize - startPos);
        }
        return Flush();
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

END_NAMESPACE_RCZIP
