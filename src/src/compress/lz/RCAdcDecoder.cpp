/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#include "compress/lz/RCAdcDecoder.h"

BEGIN_NAMESPACE_RCZIP

class RCAdcDecoderReleaser
{
public:
    RCAdcDecoderReleaser(RCAdcDecoder* coder): 
        m_Coder(coder), 
        m_needFlush(true)
    {
    }
    ~RCAdcDecoderReleaser()
    {
        if (m_needFlush)
        {
            m_Coder->Flush();
        }
        m_Coder->ReleaseStreams() ;
    }
public:
    bool m_needFlush ;
    
private:
    RCAdcDecoder* m_Coder ;
};

RCAdcDecoder::RCAdcDecoder()
{
    
}

RCAdcDecoder::~RCAdcDecoder()
{
    
}

void RCAdcDecoder::Flush() 
{
    m_outWindowStream.Flush() ;
}

void RCAdcDecoder::ReleaseStreams()
{
    m_outWindowStream.ReleaseStream();
    m_inStream.ReleaseStream();
}

HResult RCAdcDecoder::CodeReal(ISequentialInStream* inStream,
                               ISequentialOutStream* outStream, 
                               const uint64_t* inSize, 
                               const uint64_t* outSize,
                               ICompressProgressInfo* progress)

{
    if (!m_outWindowStream.Create(1 << 18))
    {
        return RC_E_OUTOFMEMORY;
    }
    if (!m_inStream.Create(1 << 18))
    {
        return RC_E_OUTOFMEMORY;
    }

    m_outWindowStream.SetStream(outStream);
    m_outWindowStream.Init(false);
    m_inStream.SetStream(inStream);
    m_inStream.Init();

    RCAdcDecoderReleaser coderReleaser(this);

    const uint32_t kStep = (1 << 20);
    uint64_t nextLimit = kStep;

    uint64_t pos = 0;
    while (pos < *outSize)
    {
        if (pos > nextLimit && progress)
        {
            uint64_t packSize = m_inStream.GetProcessedSize();
            HResult hr = progress->SetRatioInfo(packSize, pos) ;
            if(!IsSuccess(hr))
            {
                return hr ;
            }
            nextLimit += kStep;
        }
        byte_t b = 0x0 ;
        if (!m_inStream.ReadByte(b))
        {
            return RC_S_FALSE ;
        }
        uint64_t rem = *outSize - pos;
        if (b & 0x80)
        {
            unsigned num = (b & 0x7F) + 1;
            if (num > rem)
            {
                return RC_S_FALSE;
            }
            for (unsigned i = 0; i < num; i++)
            {
                if (!m_inStream.ReadByte(b))
                {
                    return RC_S_FALSE ;
                }
                m_outWindowStream.PutByte(b);
            }
            pos += num;
            continue;
        }
        byte_t b1;
        if (!m_inStream.ReadByte(b1))
        {
            return RC_S_FALSE;
        }

        uint32_t len = 0 ;
        uint32_t distance = 0 ;

        if (b & 0x40)
        {
            len = ((uint32_t)b & 0x3F) + 4;
            byte_t b2 = 0 ;
            if (!m_inStream.ReadByte(b2))
            {
                return RC_S_FALSE;
            }
            distance = ((uint32_t)b1 << 8) + b2;
        }
        else
        {
            b &= 0x3F;
            len = ((uint32_t)b >> 2) + 3;
            distance = (((uint32_t)b & 3) << 8) + b1;
        }

        if (distance >= pos || len > rem)
        {
            return RC_S_FALSE;
        }
        m_outWindowStream.CopyBlock(distance, len);
        pos += len;
    }
    if (*inSize != m_inStream.GetProcessedSize())
    {
        return RC_S_FALSE;
    }
    coderReleaser.m_needFlush = false;
    return m_outWindowStream.Flush() ;
}

HResult RCAdcDecoder::Code(ISequentialInStream* inStream,
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
        return RC_S_FALSE;
    }
}

END_NAMESPACE_RCZIP
