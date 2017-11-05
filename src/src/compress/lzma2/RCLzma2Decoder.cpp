/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "compress/lzma2/RCLzma2Decoder.h"
#include "common/RCAlloc.h"
#include "filesystem/RCStreamUtils.h"

/////////////////////////////////////////////////////////////////
//RCLzmaEncoder class implementation

BEGIN_NAMESPACE_RCZIP

#define RC_LZMA2_IN_BUF_SIZE ( (uint32_t)(1 << 20) )

ISzAlloc RCLzma2Decoder::s_alloc = { &RCLzma2Decoder::SzAlloc, &RCLzma2Decoder::SzFree } ;

RCLzma2Decoder::RCLzma2Decoder(): m_inBuf(0), m_outSizeDefined(false)
{
    Lzma2Dec_Construct(&m_state);
}

RCLzma2Decoder::~RCLzma2Decoder()
{
    Lzma2Dec_Free(&m_state, &s_alloc);
    RCAlloc::Instance().MyFree(m_inBuf);
}

void* RCLzma2Decoder::SzAlloc(void *p, size_t size)
{
    p = p ; 
    return RCAlloc::Instance().MyAlloc(size);
}

void RCLzma2Decoder::SzFree(void *p, void *address)
{
    p = p ; 
    RCAlloc::Instance().MyFree(address);
}

HResult RCLzma2Decoder::SResToHRESULT(result_t res)
{
    switch(res)
    {
        case SZ_OK: 
            return RC_S_OK;
        case SZ_ERROR_MEM: 
            return RC_E_OUTOFMEMORY;
        case SZ_ERROR_PARAM: 
            return RC_E_INVALIDARG;
        case SZ_ERROR_DATA: 
            return RC_S_FALSE;
        default:
            break ;
    }
    return RC_E_FAIL ;
}


HResult RCLzma2Decoder::SetDecoderProperties2(const byte_t* data, uint32_t size)
{
    if (size != 1)
    {
        return RC_E_VersionNotSupported;
    }

    HResult hr = SResToHRESULT(Lzma2Dec_Allocate(&m_state, *data, &s_alloc));
    if(!IsSuccess(hr))
    {
        return hr ;
    }

    if (m_inBuf == 0)
    {
        m_inBuf = (byte_t *)RCAlloc::Instance().MyAlloc(RC_LZMA2_IN_BUF_SIZE);;
        if (m_inBuf == 0)
        {
            return RC_E_OUTOFMEMORY;
        }
    }

    return RC_S_OK;
}

HResult RCLzma2Decoder::GetInStreamProcessedSize(uint64_t& size)
{ 
    size = m_inSizeProcessed; 
    return RC_S_OK;
}

HResult RCLzma2Decoder::SetInStream(ISequentialInStream* inStream)
{ 
    m_inStream = inStream;
    return RC_S_OK;
}

HResult RCLzma2Decoder::ReleaseInStream()
{
    m_inStream.Release();
    return RC_S_OK;
}

HResult RCLzma2Decoder::SetOutStreamSize(const uint64_t* outSize)
{
    m_outSizeDefined = (outSize != NULL);
    if (m_outSizeDefined)
    {
        m_outSize = *outSize;
    }

    Lzma2Dec_Init(&m_state);

    m_inPos = m_inSize = 0;
    m_inSizeProcessed = m_outSizeProcessed = 0;

    return RC_S_OK;
}

HResult RCLzma2Decoder::Code(ISequentialInStream* inStream,
                             ISequentialOutStream* outStream, 
                             const uint64_t* inSize, 
                             const uint64_t* outSize,
                             ICompressProgressInfo* progress)
{
    if (m_inBuf == 0)
    {
        return RC_S_FALSE;
    }
    SetOutStreamSize(outSize);

    for (;;)
    {
        if (m_inPos == m_inSize)
        {
            m_inPos = m_inSize = 0;
            HResult hr = inStream->Read(m_inBuf, RC_LZMA2_IN_BUF_SIZE, &m_inSize);
            if(!IsSuccess(hr))
            {
                return hr ;
            }
        }

        size_t dicPos = m_state.decoder.dicPos;
        size_t curSize = m_state.decoder.dicBufSize - dicPos;
        const uint32_t kStepSize = ((uint32_t)1 << 22);
        if (curSize > kStepSize)
        {
            curSize = (size_t)kStepSize;
        }

        ELzmaFinishMode finishMode = LZMA_FINISH_ANY;
        if (m_outSizeDefined)
        {
            const uint64_t rem = m_outSize - m_outSizeProcessed;
            if (rem < curSize)
            {
                curSize = (size_t)rem;
                /*
                // finishMode = LZMA_FINISH_END;
                we can't use LZMA_FINISH_END here to allow partial decoding
                */
            }
        }

        size_t inSizeProcessed = m_inSize - m_inPos;
        ELzmaStatus status;
        HResult res = Lzma2Dec_DecodeToDic(&m_state,
                                            dicPos + curSize,
                                            m_inBuf + m_inPos,
                                            &inSizeProcessed,
                                            finishMode,
                                            &status);

        m_inPos += (uint32_t)inSizeProcessed;
        m_inSizeProcessed += inSizeProcessed;
        size_t outSizeProcessed = m_state.decoder.dicPos - dicPos;
        m_outSizeProcessed += outSizeProcessed;

        bool finished = (inSizeProcessed == 0 && outSizeProcessed == 0);
        bool stopDecoding = (m_outSizeDefined && m_outSizeProcessed >= m_outSize);

        if (res != 0 || m_state.decoder.dicPos == m_state.decoder.dicBufSize || finished || stopDecoding)
        {
            HResult res2 = RCStreamUtils::WriteStream(outStream, m_state.decoder.dic, m_state.decoder.dicPos);
            if (res != 0)
            {
                return RC_S_FALSE;
            }
            if(!IsSuccess(res2))
            {
                return res2 ;
            }
            if (stopDecoding)
            {
                return RC_S_OK;
            }
            if (finished)
            {
                return (status == LZMA_STATUS_FINISHED_WITH_MARK ? RC_S_OK : RC_S_FALSE);
            }
        }

        if (m_state.decoder.dicPos == m_state.decoder.dicBufSize)
        {
            m_state.decoder.dicPos = 0;
        }

        if (progress != NULL)
        {
            HResult hr = progress->SetRatioInfo(m_inSizeProcessed, m_outSizeProcessed);
            if(!IsSuccess(hr))
            {
                return hr ;
            }
        }
    }
}

HResult RCLzma2Decoder::Read(void* data, uint32_t size, uint32_t* processedSize)
{
    if (processedSize)
    {
        *processedSize = 0;
    }

    do
    {
        if (m_inPos == m_inSize)
        {
            m_inPos = m_inSize = 0;

            HResult hr = m_inStream->Read(m_inBuf, RC_LZMA2_IN_BUF_SIZE, &m_inSize);
            if(!IsSuccess(hr))
            {
                return hr ;
            }
        }
        {
            size_t inProcessed = m_inSize - m_inPos;

            if (m_outSizeDefined)
            {
                const uint64_t rem = m_outSize - m_outSizeProcessed;
                if (rem < size)
                {
                    size = (uint32_t)rem;
                }
            }

            size_t outProcessed = size;
            ELzmaStatus status;
            result_t res = Lzma2Dec_DecodeToBuf(&m_state, 
                                                (byte_t *)data,
                                                &outProcessed,
                                                m_inBuf + m_inPos,
                                                &inProcessed,
                                                LZMA_FINISH_ANY,
                                                &status);
            m_inPos += (uint32_t)inProcessed;
            m_inSizeProcessed += inProcessed;
            m_outSizeProcessed += outProcessed;
            size -= (uint32_t)outProcessed;
            data = (byte_t *)data + outProcessed;
            if (processedSize)
            {
                *processedSize += (uint32_t)outProcessed;
            }

            HResult hr = SResToHRESULT(res);
            if(!IsSuccess(hr))
            {
                return hr ;
            }

            if (inProcessed == 0 && outProcessed == 0)
            {
                return RC_S_OK;
            }
        }

    }while (size != 0);

    return RC_S_OK;
}

END_NAMESPACE_RCZIP