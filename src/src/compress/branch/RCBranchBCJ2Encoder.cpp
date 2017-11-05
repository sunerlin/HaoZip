/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "compress/branch/RCBranchBCJ2Encoder.h"
#include "common/RCAlloc.h"
#include "RCBranchBCJ2ImplDefs.h"

/////////////////////////////////////////////////////////////////
//RCBranchBCJ2Encoder class implementation

BEGIN_NAMESPACE_RCZIP

class RCBranchBCJ2EncoderReleaser
{
public:
    RCBranchBCJ2EncoderReleaser(RCBranchBCJ2Encoder* coder):
        _coder(coder)
    {
    }
    ~RCBranchBCJ2EncoderReleaser()
    {
        _coder->ReleaseStreams() ;
    }
private:
    RCBranchBCJ2Encoder* _coder ;
};

RCBranchBCJ2Encoder::RCBranchBCJ2Encoder():
    m_buffer(NULL)
{
}

RCBranchBCJ2Encoder::~RCBranchBCJ2Encoder()
{
    RCAlloc::Instance().MidFree(m_buffer) ;
}

bool RCBranchBCJ2Encoder::Create()
{
    if (!m_mainStream.Create(1 << 18))
    {
        return false;
    }
    if (!m_callStream.Create(1 << 18))
    {
        return false;
    }
    if (!m_jumpStream.Create(1 << 18))
    {
        return false;
    }
    if (!m_rangeEncoder.Create(1 << 20))
    {
        return false;
    }
    if (m_buffer == 0)
    {
        m_buffer = (byte_t *)RCAlloc::Instance().MidAlloc(kBufferSize);
        if (m_buffer == 0)
        {
            return false;
        }
    }
    return true;
}

HResult RCBranchBCJ2Encoder::Flush()
{
    HResult hr = m_mainStream.Flush() ;
    if(!IsSuccess(hr))
    {
        return hr ;
    }
    hr = m_callStream.Flush() ;
    if(!IsSuccess(hr))
    {
        return hr ;
    }
    hr = m_jumpStream.Flush() ;
    if(!IsSuccess(hr))
    {
        return hr ;
    }
    m_rangeEncoder.FlushData() ;
    return m_rangeEncoder.FlushStream() ;
}

void RCBranchBCJ2Encoder::ReleaseStreams()
{
    m_mainStream.ReleaseStream();
    m_callStream.ReleaseStream();
    m_jumpStream.ReleaseStream();
    m_rangeEncoder.ReleaseStream();
}

HResult RCBranchBCJ2Encoder::Code(const std::vector<in_stream_data>& inStreams,
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

HResult RCBranchBCJ2Encoder::CodeReal(const std::vector<in_stream_data>& inStreams,
                                      const std::vector<out_stream_data>& outStreams, 
                                      ICompressProgressInfo* progress)
{
    if (inStreams.size() != 1 || outStreams.size() != 4)
    {
        return RC_E_INVALIDARG;
    }

    if (!Create())
    {
        return RC_E_OUTOFMEMORY;
    }

    bool sizeIsDefined = false;
    uint64_t inSize = inStreams[0].second ;
    if (inSize <= kDefaultLimit)
    {
        sizeIsDefined = true ;
    }

    ISequentialInStream* inStream = inStreams[0].first ;

    m_mainStream.SetStream(outStreams[0].first);
    m_mainStream.Init();
    m_callStream.SetStream(outStreams[1].first);
    m_callStream.Init();
    m_jumpStream.SetStream(outStreams[2].first);
    m_jumpStream.Init();
    m_rangeEncoder.SetStream(outStreams[3].first);
    m_rangeEncoder.Init();
    for (int i = 0; i < 256 + 2; i++)
    {
        m_statusEncoder[i].Init();
    }
    RCBranchBCJ2EncoderReleaser releaser(this);

    ICompressGetSubStreamSizePtr getSubStreamSize;
    {
        inStream->QueryInterface(IID_ICompressGetSubStreamSize, (void **)getSubStreamSize.GetAddress());
    }

    uint32_t nowPos = 0;
    uint64_t nowPos64 = 0;
    uint32_t bufferPos = 0;

    byte_t prevByte = 0;

    uint64_t subStreamIndex = 0;
    uint64_t subStreamStartPos  = 0;
    uint64_t subStreamEndPos = 0;

    for (;;)
    {
        uint32_t processedSize = 0;
        for (;;)
        {
            uint32_t size = kBufferSize - (bufferPos + processedSize);
            uint32_t processedSizeLoc;
            if (size == 0)
            {
                break;
            }
            HResult hr = inStream->Read(m_buffer + bufferPos + processedSize, size, &processedSizeLoc) ;
            if(!IsSuccess(hr))
            {
                return hr ;
            }
            if (processedSizeLoc == 0)
            {
                break;
            }
            processedSize += processedSizeLoc;
        }
        uint32_t endPos = bufferPos + processedSize;

        if (endPos < 5)
        {
            // change it
            for (bufferPos = 0; bufferPos < endPos; bufferPos++)
            {
                byte_t b = m_buffer[bufferPos];
                m_mainStream.WriteByte(b);
                uint32_t index;
                if (b == 0xE8)
                {
                    index = prevByte;
                }
                else if (b == 0xE9)
                {
                    index = 256;
                }
                else if (IsJcc(prevByte, b))
                {
                    index = 257;
                }
                else
                {
                    prevByte = b;
                    continue;
                }
                m_statusEncoder[index].Encode(&m_rangeEncoder, 0);
                prevByte = b;
            }
            return Flush();
        }

        bufferPos = 0;
        uint32_t limit = endPos - 5;
        while(bufferPos <= limit)
        {
            byte_t b = m_buffer[bufferPos];
            m_mainStream.WriteByte(b);
            if (!IsJ(prevByte, b))
            {
                bufferPos++;
                prevByte = b;
                continue;
            }
            byte_t nextByte = m_buffer[bufferPos + 4];
            uint32_t src = (uint32_t(nextByte) << 24) |
                           (uint32_t(m_buffer[bufferPos + 3]) << 16) |
                           (uint32_t(m_buffer[bufferPos + 2]) << 8) |
                           (m_buffer[bufferPos + 1]);
            uint32_t dest = (nowPos + bufferPos + 5) + src;
            // if (Test86MSByte(nextByte))
            bool convert;
            if (getSubStreamSize != NULL)
            {
                uint64_t currentPos = (nowPos64 + bufferPos);
                while (subStreamEndPos < currentPos)
                {
                    uint64_t subStreamSize;
                    HResult result = getSubStreamSize->GetSubStreamSize(subStreamIndex, subStreamSize);
                    if (result == RC_S_OK)
                    {
                        subStreamStartPos = subStreamEndPos;
                        subStreamEndPos += subStreamSize;
                        subStreamIndex++;
                    }
                    else if (result == RC_S_FALSE || result == RC_E_NOTIMPL)
                    {
                        getSubStreamSize.Release();
                        subStreamStartPos = 0;
                        subStreamEndPos = subStreamStartPos - 1;
                    }
                    else
                    {
                        return result;
                    }
                }
                if (getSubStreamSize == NULL)
                {
                    if (sizeIsDefined)
                    {
                        convert = (dest < inSize);
                    }
                    else
                    {
                        convert = Test86MSByte(nextByte);
                    }
                }
                else if (subStreamEndPos - subStreamStartPos > kDefaultLimit)
                {
                    convert = Test86MSByte(nextByte);
                }
                else
                {
                    uint64_t dest64 = (currentPos + 5) + int64_t(int32_t(src));
                    convert = (dest64 >= subStreamStartPos && dest64 < subStreamEndPos);
                }
            }
            else if (sizeIsDefined)
            {
                convert = (dest < inSize);
            }
            else
            {
                convert = Test86MSByte(nextByte);
            }
            uint32_t index = GetIndex(prevByte, b);
            if (convert)
            {
                m_statusEncoder[index].Encode(&m_rangeEncoder, 1);
                bufferPos += 5;
                RCOutBuffer& s = (b == 0xE8) ? m_callStream : m_jumpStream;
                for (int32_t i = 24; i >= 0; i -= 8)
                {
                    s.WriteByte((byte_t)(dest >> i));
                }
                prevByte = nextByte;
            }
            else
            {
                m_statusEncoder[index].Encode(&m_rangeEncoder, 0);
                bufferPos++;
                prevByte = b;
            }
        }
        nowPos += bufferPos;
        nowPos64 += bufferPos;

        if (progress != NULL)
        {
            HResult hr = progress->SetRatioInfo(nowPos64, 0) ;
        }

        uint32_t i = 0;
        while(bufferPos < endPos)
        {
            m_buffer[i++] = m_buffer[bufferPos++];
        }
        bufferPos = i ;
    }
}

END_NAMESPACE_RCZIP
