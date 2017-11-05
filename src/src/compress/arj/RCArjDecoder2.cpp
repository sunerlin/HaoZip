/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "compress/arj/RCArjDecoder2.h"

/////////////////////////////////////////////////////////////////
//RCArjDecoder2 class implementation

BEGIN_NAMESPACE_RCZIP

#define HISTORY_SIZE 26624
#define MATCH_MIN_LEN 3

RCArjDecoder2::CCoderReleaser::CCoderReleaser(RCArjDecoder2* coder):
    m_coder(coder), 
    m_needFlush(true) 
{    
}

RCArjDecoder2::CCoderReleaser::~CCoderReleaser()
{
    if (m_needFlush)
    {
        m_coder->m_outWindowStream.Flush() ;
    }
    m_coder->ReleaseStreams() ;
}

RCArjDecoder2::RCArjDecoder2()
{
}

RCArjDecoder2::~RCArjDecoder2()
{
}

void RCArjDecoder2::ReleaseStreams()
{
    m_outWindowStream.ReleaseStream();
    m_inBitStream.ReleaseStream();
}

HResult RCArjDecoder2::Code(ISequentialInStream* inStream,
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

HResult RCArjDecoder2::CodeReal(ISequentialInStream* inStream,
                                ISequentialOutStream* outStream, 
                                const uint64_t* inSize, 
                                const uint64_t* outSize,
                                ICompressProgressInfo* progress)
{
    if (outSize == NULL)
    {
        return RC_E_INVALIDARG;
    }
    if (!m_outWindowStream.Create(HISTORY_SIZE))
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
    CCoderReleaser coderReleaser(this);

    while(pos < *outSize)
    {
        const uint32_t kStartWidth = 0;
        const uint32_t kStopWidth = 7;
        uint32_t power = 1 << kStartWidth;
        uint32_t width;
        uint32_t len = 0;
        for (width = kStartWidth; width < kStopWidth; width++)
        {
            if (m_inBitStream.ReadBits(1) == 0)
            {
                break;
            }
            len += power;
            power <<= 1;
        }
        if (width != 0)
            len += m_inBitStream.ReadBits(width);
        if (len == 0)
        {
            m_outWindowStream.PutByte((byte_t)m_inBitStream.ReadBits(8));
            pos++;
            continue;
        }
        else
        {
            len = len - 1 + MATCH_MIN_LEN;
            const uint32_t kStartWidth = 9;
            const uint32_t kStopWidth = 13;
            uint32_t power = 1 << kStartWidth;
            uint32_t width;
            uint32_t distance = 0;
            for (width = kStartWidth; width < kStopWidth; width++)
            {
                if (m_inBitStream.ReadBits(1) == 0)
                {
                    break;
                }
                distance += power;
                power <<= 1;
            }
            if (width != 0)
                distance += m_inBitStream.ReadBits(width);
            if (distance >= pos)
            {
                return RC_S_FALSE;
            }
            m_outWindowStream.CopyBlock(distance, len);
            pos += len;
        }
    }
    coderReleaser.m_needFlush = false;
    return m_outWindowStream.Flush();
}

END_NAMESPACE_RCZIP
