/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "compress/branch/RCBranchBCJ2Decoder.h"
#include "RCBranchBCJ2ImplDefs.h"

/////////////////////////////////////////////////////////////////
//RCBranchBCJ2Decoder class implementation

BEGIN_NAMESPACE_RCZIP

class RCBranchBCJ2CoderReleaser
{
public:
    RCBranchBCJ2CoderReleaser(RCBranchBCJ2Decoder* coder):
        _coder(coder)
    {
    }
    ~RCBranchBCJ2CoderReleaser()
    {
        _coder->ReleaseStreams() ;
    }
private:
    RCBranchBCJ2Decoder* _coder ;
};

RCBranchBCJ2Decoder::RCBranchBCJ2Decoder()
{
}

RCBranchBCJ2Decoder::~RCBranchBCJ2Decoder()
{
}

void RCBranchBCJ2Decoder::ReleaseStreams()
{
    m_mainInStream.ReleaseStream();
    m_callStream.ReleaseStream();
    m_jumpStream.ReleaseStream();
    m_rangeDecoder.ReleaseStream();
    m_outStream.ReleaseStream();
}

HResult RCBranchBCJ2Decoder::Flush()
{
    return m_outStream.Flush() ;
}

HResult RCBranchBCJ2Decoder::Code(const std::vector<in_stream_data>& inStreams,
                                  const std::vector<out_stream_data>& outStreams, 
                                  ICompressProgressInfo* progress)
{
    try
    {
        return CodeReal(inStreams, outStreams, progress);
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

HResult RCBranchBCJ2Decoder::CodeReal(const std::vector<in_stream_data>& inStreams,
                                      const std::vector<out_stream_data>& outStreams, 
                                      ICompressProgressInfo* progress) 
{
    if (inStreams.size() != 4 || outStreams.size() != 1)
    {
        return RC_E_INVALIDARG;
    }

    if (!m_mainInStream.Create(1 << 16))
    {
        return RC_E_OUTOFMEMORY;
    }
    if (!m_callStream.Create(1 << 20))
    {
        return RC_E_OUTOFMEMORY;
    }
    if (!m_jumpStream.Create(1 << 16))
    {
        return RC_E_OUTOFMEMORY;
    }
    if (!m_rangeDecoder.Create(1 << 20))
    {
        return RC_E_OUTOFMEMORY;
    }
    if (!m_outStream.Create(1 << 16))
    {
        return RC_E_OUTOFMEMORY;
    }

    m_mainInStream.SetStream(inStreams[0].first);
    m_callStream.SetStream(inStreams[1].first);
    m_jumpStream.SetStream(inStreams[2].first);
    m_rangeDecoder.SetStream(inStreams[3].first);
    m_outStream.SetStream(outStreams[0].first);

    m_mainInStream.Init();
    m_callStream.Init();
    m_jumpStream.Init();
    m_rangeDecoder.Init();
    m_outStream.Init();

    for (int32_t i = 0; i < 256 + 2; i++)
    {
        m_statusDecoder[i].Init();
    }

    RCBranchBCJ2CoderReleaser releaser(this);

    byte_t prevByte = 0;
    uint32_t processedBytes = 0;
    for (;;)
    {
        if (processedBytes >= (1 << 20) && progress != NULL)
        {
            const uint64_t nowPos64 = m_outStream.GetProcessedSize();
            HResult hr = progress->SetRatioInfo(0, nowPos64) ;
            if(!IsSuccess(hr))
            {
                return hr ;
            }
            processedBytes = 0;
        }
        uint32_t i = 0 ;
        byte_t b = 0 ;
        const uint32_t kBurstSize = (1 << 18);
        for (i = 0; i < kBurstSize; i++)
        {
            if (!m_mainInStream.ReadByte(b))
            {
                return Flush();
            }
            m_outStream.WriteByte(b);
            if (IsJ(prevByte, b))
            {
                break;
            }
            prevByte = b;
        }
        processedBytes += i;
        if (i == kBurstSize)
        {
            continue;
        }
        uint32_t index = GetIndex(prevByte, b);
        if (m_statusDecoder[index].Decode(&m_rangeDecoder) == 1)
        {
            uint32_t src = 0;
            RCInBuffer& s = (b == 0xE8) ? m_callStream : m_jumpStream;
            for (int32_t i = 0; i < 4; i++)
            {
                byte_t b0;
                if(!s.ReadByte(b0))
                {
                    return RC_S_FALSE;
                }
                src <<= 8;
                src |= ((uint32_t)b0);
            }
            uint32_t dest = src - (uint32_t(m_outStream.GetProcessedSize()) + 4) ;
            m_outStream.WriteByte((byte_t)(dest));
            m_outStream.WriteByte((byte_t)(dest >> 8));
            m_outStream.WriteByte((byte_t)(dest >> 16));
            m_outStream.WriteByte((byte_t)(dest >> 24));
            prevByte = (byte_t)(dest >> 24);
            processedBytes += 4;
        }
        else
        {
            prevByte = b;
        }
    }
}

END_NAMESPACE_RCZIP
