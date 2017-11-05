/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "format/lzma/RCLzmaFiltersDecoder.h"
#include "interface/RCMethodDefs.h"
#include "common/RCCreateCoder.h"

/////////////////////////////////////////////////////////////////
//RCLzmaFiltersDecoder class implementation

BEGIN_NAMESPACE_RCZIP

uint64_t RCLzmaFiltersDecoder::GetInputProcessedSize() const
{
    uint64_t processedSize = 0 ;
    ICompressGetInStreamProcessedSizePtr inStreamProcessedSize ;
    HResult hr = m_lzmaDecoder.QueryInterface(ICompressGetInStreamProcessedSize::IID, (void**)inStreamProcessedSize.GetAddress()) ;
    if(inStreamProcessedSize != NULL )
    {        
        inStreamProcessedSize->GetInStreamProcessedSize(processedSize) ;
    }    
    return processedSize ;
}

void RCLzmaFiltersDecoder::ReleaseInStream()
{
    ICompressSetInStreamPtr setInStream ;
    HResult hr = m_lzmaDecoder.QueryInterface(ICompressSetInStream::IID, (void**)setInStream.GetAddress()) ;
    if(setInStream != NULL )
    {
        setInStream->ReleaseInStream() ;
    }
}

HResult RCLzmaFiltersDecoder::ReadInput(byte_t* data, uint32_t size, uint32_t* processedSize)
{
    RCZIP_ASSERT(m_lzmaDecoderSpec != NULL) ;
    return m_lzmaDecoderSpec->ReadFromInputStream(data, size, processedSize) ;
}

HResult RCLzmaFiltersDecoder::Create(ICompressCodecsInfo* compressCodecsInfo, 
                                     bool filteredMode, 
                                     ISequentialInStream* inStream)
{
    if (!m_lzmaDecoder)
    {
        HResult hr = RCCreateCoder::CreateCoder(compressCodecsInfo,
                                                RCMethod::ID_COMPRESS_LZMA,
                                                m_lzmaDecoder,
                                                false) ;
                                        
        if(!IsSuccess(hr) || (m_lzmaDecoder == NULL))
        {
            return RC_E_NOTIMPL ;
        }
        m_lzmaDecoderSpec = NULL ;
        hr = m_lzmaDecoder.QueryInterface(ILzmaResumeDecoder::IID, (void**)m_lzmaDecoderSpec.GetAddress()) ;
        if(!IsSuccess(hr) || (m_lzmaDecoderSpec == NULL))
        {
            return RC_E_NOTIMPL ;
        }
    }
    if (filteredMode && !m_bcjStream)
    {
        ICompressCoderPtr coder;
        HResult hr = RCCreateCoder::CreateCoder(compressCodecsInfo,
                                                RCMethod::ID_COMPRESS_BRANCH_BCJ,
                                                coder,
                                                false);
        if(!IsSuccess(hr) || (coder == NULL))
        {
            return RC_E_NOTIMPL ;
        }

        coder.QueryInterface(IID_ISequentialOutStream, m_bcjStream.GetAddress());
        if (!m_bcjStream)
        {
            return RC_E_NOTIMPL;
        }
    }
    
    ICompressSetInStreamPtr setInStream ;
    HResult hr = m_lzmaDecoder.QueryInterface(ICompressSetInStream::IID, (void**)setInStream.GetAddress()) ;
    if(!IsSuccess(hr) || (setInStream == NULL) )
    {
        return RC_E_NOTIMPL ;
    }
    return setInStream->SetInStream(inStream) ;
}

HResult RCLzmaFiltersDecoder::Code(const RCLzmaHeader& header,
                                   ISequentialOutStream* outStream, 
                                   ICompressProgressInfo* progress)
{
    if (header.m_filterID > 1)
    {
        return RC_E_NOTIMPL ;
    }

    {
        ICompressSetDecoderProperties2Ptr setDecoderProperties;
        m_lzmaDecoder.QueryInterface(ICompressSetDecoderProperties2::IID, (void**)setDecoderProperties.GetAddress());
        if (!setDecoderProperties)
        {
            return RC_E_NOTIMPL ;
        }
        HResult hr = setDecoderProperties->SetDecoderProperties2(header.m_lzmaProps, 5) ;
        if(!IsSuccess(hr))
        {
            return hr ;
        }
    }

    ICompressSetOutStreamPtr setOutStream ;
    bool filteredMode = (header.m_filterID == 1) ;
    if (filteredMode)
    {
        m_bcjStream.QueryInterface(ICompressSetOutStream::IID, (void**)setOutStream.GetAddress());
        if (!setOutStream)
        {
            return RC_E_NOTIMPL ;
        }
        HResult hr = setOutStream->SetOutStream(outStream) ;
        outStream = m_bcjStream.Get() ;
    }

    const uint64_t* Size = header.HasSize() ? &header.m_size : NULL ;
    HResult res = m_lzmaDecoderSpec->CodeResume(outStream, Size, progress) ;

    if (filteredMode)
    {
        IOutStreamFlushPtr flush ;
        m_bcjStream.QueryInterface(IOutStreamFlush::IID, (void**)flush.GetAddress() );
        if (flush)
        {
            HResult res2 = flush->Flush() ;
            if (res == RC_S_OK)
            {
                res = res2 ;
            }
        }
        HResult res2 = setOutStream->ReleaseOutStream() ;
        if (res == RC_S_OK)
        {
            res = res2 ;
        }
    }
    if(!IsSuccess(res))
    {
        return res ;
    }
    return RC_S_OK ;
}

END_NAMESPACE_RCZIP
