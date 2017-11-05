/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "compress/deflate/RCZlibDecoder.h"
#include "compress/deflate/RCZlibOutStreamWithAdler.h"
#include "compress/deflate/RCDeflateCOMDecoder.h"
#include "filesystem/RCStreamUtils.h"

/////////////////////////////////////////////////////////////////
//RCZlibDecoder class implementation

BEGIN_NAMESPACE_RCZIP

RCZlibDecoder::RCZlibDecoder():
    m_adlerSpec(NULL),
    m_deflateDecoderSpec(NULL)    
{
}

RCZlibDecoder::~RCZlibDecoder()
{
}

HResult RCZlibDecoder::Code(ISequentialInStream* inStream,
                            ISequentialOutStream* outStream, 
                            const uint64_t* inSize, 
                            const uint64_t* outSize,
                            ICompressProgressInfo* progress)
{
    if (!m_spAdlerStream)
    {
        m_adlerSpec = new RCZlibOutStreamWithAdler ;
        m_spAdlerStream = m_adlerSpec;
    }
    if (!m_spDeflateDecoder)
    {
        m_deflateDecoderSpec = new RCDeflateCOMDecoder ;
        m_deflateDecoderSpec->m_zlibMode = true;
        m_spDeflateDecoder = m_deflateDecoderSpec;
    }

    byte_t buf[2];
    HResult hr = RCStreamUtils::ReadStream_FALSE(inStream, buf, 2) ;
    if(!IsSuccess(hr))
    {
        return hr ;
    }
    int32_t method = buf[0] & 0xF;
    if (method != 8)
    {
        return RC_S_FALSE;
    }
    // int dicSize = buf[0] >> 4;
    if ((((uint32_t)buf[0] << 8) + buf[1]) % 31 != 0)
    {
        return RC_S_FALSE;
    }
    if ((buf[1] & 0x20) != 0) // dictPresent
    {
        return RC_S_FALSE;
    }
    // int level = (buf[1] >> 6);

    m_adlerSpec->SetStream(outStream);
    m_adlerSpec->Init();
    HResult res = m_spDeflateDecoder->Code(inStream, m_spAdlerStream.Get(), inSize, outSize, progress);
    m_adlerSpec->ReleaseStream();

    if (res == RC_S_OK)
    {
        const byte_t *p = m_deflateDecoderSpec->m_zlibFooter;
        uint32_t adler = ((uint32_t)p[0] << 24) | ((uint32_t)p[1] << 16) | ((uint32_t)p[2] << 8) | p[3];
        if (adler != m_adlerSpec->GetAdler())
        {
            return RC_S_FALSE;
        }
    }
    return res;
}

uint64_t RCZlibDecoder::GetInputProcessedSize() const
{
    if(m_deflateDecoderSpec)
    {
        return m_deflateDecoderSpec->GetInputProcessedSize() + 2 ;
    }
    else
    {
        return 0 ;
    }
}

END_NAMESPACE_RCZIP
