/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "format/common/RCCoderMixerMT.h"

/////////////////////////////////////////////////////////////////
//RCCoderMixerMT class implementation

BEGIN_NAMESPACE_RCZIP

RCCoderMixerCoder::RCCoderMixerCoder(uint32_t numInStreams, uint32_t numOutStreams):
    RCCoderMixserCoderInfo(numInStreams, numOutStreams)
{
    m_inStreams.reserve(m_numInStreams);
    m_inStreamPointers.reserve(m_numInStreams);
    m_outStreams.reserve(m_numOutStreams);
    m_outStreamPointers.reserve(m_numOutStreams);
}

void RCCoderMixerCoder::Execute()
{
    Code(NULL) ;
}

void RCCoderMixerCoder::Code(ICompressProgressInfo *progress)
{
    m_inStreamPointers.clear();
    m_outStreamPointers.clear();
    uint32_t i;

    for (i = 0; i < m_numInStreams; i++)
    {
        if (m_inSizePointers[i] != NULL)
        {
            m_inSizePointers[i] = &m_inSizes[i];
        }
        m_inStreamPointers.push_back((ISequentialInStream *)m_inStreams[i].Get());
    }

    for (i = 0; i < m_numOutStreams; i++)
    {
        if (m_outSizePointers[i] != NULL)
        {
            m_outSizePointers[i] = &m_outSizes[i];
        }
        m_outStreamPointers.push_back((ISequentialOutStream *)m_outStreams[i].Get());
    }

    if (m_coder)
    {
        m_result = m_coder->Code(m_inStreamPointers[0], 
                                 m_outStreamPointers[0],
                                 m_inSizePointers[0], 
                                 m_outSizePointers[0], 
                                 progress);
    }
    else
    {
        RCVector<ICompressCoder2::in_stream_data> inStreamDatas;
        for (i = 0; i < m_inStreamPointers.size(); i++)
        {
            if ((i < m_inSizePointers.size()) && m_inSizePointers[i])
            {
                inStreamDatas.push_back(ICompressCoder2::in_stream_data(m_inStreamPointers[i], *(m_inSizePointers[i])));
            }
            else
            {
                inStreamDatas.push_back(ICompressCoder2::in_stream_data(m_inStreamPointers[i], 0));
            }
        }

        RCVector<ICompressCoder2::out_stream_data> outStreamDatas;
        for (i = 0; i < m_outStreamPointers.size(); i++)
        {
              if ((i < m_outSizePointers.size()) && m_outSizePointers[i])
              {
                  outStreamDatas.push_back(ICompressCoder2::out_stream_data(m_outStreamPointers[i], *(m_outSizePointers[i])));
              }
              else
              {
                  outStreamDatas.push_back(ICompressCoder2::out_stream_data(m_outStreamPointers[i], 0));
              }
        }

        m_result = m_coder2->Code(inStreamDatas,
                              outStreamDatas, 
                              progress) ;
    }
    {
        uint32_t i = 0 ;
        for (i = 0; i < m_inStreams.size(); i++)
        {
            m_inStreams[i].Release();
        }

        for (i = 0; i < m_outStreams.size(); i++)
        {
            m_outStreams[i].Release();
        }
    }
}

static void SetSizes(const uint64_t** srcSizes, 
                     RCVector<uint64_t>& sizes,
                     RCVector<const uint64_t*>& sizePointers, 
                     uint32_t numItems)
{
    sizes.clear();
    sizePointers.clear();

    for (uint32_t i = 0; i < numItems; i++)
    {
        if (srcSizes == 0 || srcSizes[i] == NULL)
        {
            sizes.push_back(0);
            sizePointers.push_back(NULL);
        }
        else
        {
            sizes.push_back(*srcSizes[i]);
            sizePointers.push_back(&sizes.back());
        }
    }
}


void RCCoderMixerCoder::SetCoderInfo(const uint64_t **inSizes, const uint64_t **outSizes)
{
    SetSizes(inSizes, m_inSizes, m_inSizePointers, m_numInStreams);
    SetSizes(outSizes, m_outSizes, m_outSizePointers, m_numOutStreams);
}

RCCoderMixerMT::RCCoderMixerMT()
{
}

RCCoderMixerMT::~RCCoderMixerMT()
{
}

void RCCoderMixerMT::AddCoderCommon()
{
    const RCCoderMixserCoderStreamsInfo& c = m_bindInfo.m_coders[m_coders.size()];
    RCCoderMixerCoder* threadCoderInfo = new RCCoderMixerCoder(c.m_numInStreams, c.m_numOutStreams);
    RCCoderMixerCoderPtr spCoderInfo(threadCoderInfo) ;
    m_coders.push_back(spCoderInfo);
}

HResult RCCoderMixerMT::Init(const std::vector<in_stream_data>& inStreams,
                             const std::vector<out_stream_data>& outStreams)
{
    int32_t i = 0 ;
    for (i = 0; i < (int32_t)m_coders.size(); i++)
    {
        RCCoderMixerCoderPtr& coderInfo = m_coders[i];
        const RCCoderMixserCoderStreamsInfo& coderStreamsInfo = m_bindInfo.m_coders[i];
        coderInfo->m_inStreams.clear();

        uint32_t j = 0 ;
        for (j = 0; j < coderStreamsInfo.m_numInStreams; j++)
        {
            coderInfo->m_inStreams.push_back(NULL);
        }

        coderInfo->m_outStreams.clear();

        for (j = 0; j < coderStreamsInfo.m_numOutStreams; j++)
        {
            coderInfo->m_outStreams.push_back(NULL);
        }
    }

    for (i = 0; i < (int32_t)m_bindInfo.m_bindPairs.size(); i++)
    {
        const RCCoderMixerBindPair& bindPair = m_bindInfo.m_bindPairs[i];
        uint32_t inCoderIndex, inCoderStreamIndex;
        uint32_t outCoderIndex, outCoderStreamIndex;

        HResult hr = m_bindInfo.FindInStream(bindPair.m_inIndex, inCoderIndex, inCoderStreamIndex);
        if (!IsSuccess(hr))
        {
            return hr ;
        }

        hr = m_bindInfo.FindOutStream(bindPair.m_outIndex, outCoderIndex, outCoderStreamIndex);
        if (!IsSuccess(hr))
        {
            return hr ;
        }

        m_streamBinders[i]->CreateStreams(
            (m_coders[inCoderIndex]->m_inStreams[inCoderStreamIndex]).GetAddress(),
            (m_coders[outCoderIndex]->m_outStreams[outCoderStreamIndex]).GetAddress());
    }

    for (i = 0; i < (int32_t)m_bindInfo.m_inStreams.size(); i++)
    {
        uint32_t inCoderIndex, inCoderStreamIndex;
        m_bindInfo.FindInStream(m_bindInfo.m_inStreams[i], inCoderIndex, inCoderStreamIndex);
        m_coders[inCoderIndex]->m_inStreams[inCoderStreamIndex] = inStreams[i].first;
    }

    for (i = 0; i < (int32_t)m_bindInfo.m_outStreams.size(); i++)
    {
        uint32_t outCoderIndex, outCoderStreamIndex;
        HResult hr = m_bindInfo.FindOutStream(m_bindInfo.m_outStreams[i], outCoderIndex, outCoderStreamIndex);
        if (!IsSuccess(hr))
        {
            return hr ;
        }
        m_coders[outCoderIndex]->m_outStreams[outCoderStreamIndex] = outStreams[i].first;
    }
    return RC_S_OK;
}

HResult RCCoderMixerMT::ReturnIfError(HResult code)
{
    for (int32_t i = 0; i < (int32_t)m_coders.size(); i++)
    {
        if (m_coders[i]->m_result == code)
        {
            return code;
        }
    }
    return RC_S_OK;
}

HResult RCCoderMixerMT::SetBindInfo(const RCCoderMixserBindInfo &bindInfo)
{
    m_bindInfo = bindInfo;
    m_streamBinders.clear();
    int32_t count = (int32_t)m_bindInfo.m_bindPairs.size() ;
    for (int32_t i = 0; i < count; i++)
    {
        RCStreamBinderPtr spStreamBinder(new RCStreamBinder()) ;
        m_streamBinders.push_back(spStreamBinder);

        HResult hr = spStreamBinder->CreateEvents() ;
        if (!IsSuccess(hr))
        {
            return hr ;
        }
    }
    return RC_S_OK;
}

void RCCoderMixerMT::AddCoder(ICompressCoder *coder)
{
    AddCoderCommon();
    m_coders.back()->m_coder = coder ;
}

void RCCoderMixerMT::AddCoder2(ICompressCoder2 *coder)
{
    AddCoderCommon();
    m_coders.back()->m_coder2 = coder;
}

void RCCoderMixerMT::SetProgressCoderIndex(int32_t coderIndex)
{
    m_progressCoderIndex = coderIndex;
}

void RCCoderMixerMT::ReInit()
{
    for (int32_t i = 0; i < (int32_t)m_streamBinders.size(); i++)
    {
        m_streamBinders[i]->ReInit();
    }
}

void RCCoderMixerMT::SetCoderInfo(uint32_t coderIndex, const uint64_t **inSizes, const uint64_t **outSizes)
{
    m_coders[coderIndex]->SetCoderInfo(inSizes, outSizes);
}

uint64_t RCCoderMixerMT::GetWriteProcessedSize(uint32_t binderIndex) const
{
    return m_streamBinders[binderIndex]->GetProcessedSize() ;
}

HResult RCCoderMixerMT::Code(const std::vector<in_stream_data>& inStreams,
                             const std::vector<out_stream_data>& outStreams, 
                             ICompressProgressInfo* progress)
{
    if (inStreams.size() != (uint32_t)m_bindInfo.m_inStreams.size() ||
        outStreams.size() != (uint32_t)m_bindInfo.m_outStreams.size())
    {
        return RC_E_INVALIDARG;
    }

    Init(inStreams, outStreams);

    int32_t i = 0 ;
 
    for (i = 0; i < (int32_t)m_coders.size(); i++)
    {
        if (i != m_progressCoderIndex)
        {
            m_coders[i]->Start();
        }
    }

    m_coders[m_progressCoderIndex]->Code(progress);

    for (i = 0; i < (int32_t)m_coders.size(); i++)
    {
        if (i != m_progressCoderIndex)
        {
            m_coders[i]->WaitFinish();
        }
    }
    
/*
    for (i = 0; i < (int32_t)m_coders.size(); i++)
    {
        m_coders[i]->Code(progress);
    }
*/
    HResult hr = ReturnIfError(RC_E_ABORT) ;
    if (!IsSuccess(hr))
    {
        return hr ;
    }

    hr = ReturnIfError(RC_E_OUTOFMEMORY) ;
    if (!IsSuccess(hr))
    {
        return hr ;
    }

    for (i = 0; i < (int32_t)m_coders.size(); i++)
    {
        HResult result = m_coders[i]->m_result;
        if (result != RC_S_OK && result != RC_E_FAIL && result != RC_S_FALSE)
        {
            return result;
        }
    }

    hr = ReturnIfError(RC_S_FALSE) ;
    if (!IsSuccess(hr))
    {
        return hr ;
    }

    for (i = 0; i < (int32_t)m_coders.size(); i++)
    {
        HResult result = m_coders[i]->m_result;
        if (result != RC_S_OK)
        {
            return result;
        }
    }
    return RC_S_OK;
}

END_NAMESPACE_RCZIP
