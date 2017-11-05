/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "format/common/RCCoderMixer.h"

/////////////////////////////////////////////////////////////////
//RCCoderMixer class implementation

BEGIN_NAMESPACE_RCZIP

void RCCoderMixserBindInfo::Clear()
{
    m_coders.clear();
    m_bindPairs.clear();
    m_inStreams.clear();
    m_outStreams.clear();
}

void RCCoderMixserBindInfo::GetNumStreams(uint32_t& numInStreams, uint32_t& numOutStreams) const
{
    numInStreams = 0;
    numOutStreams = 0;
    int32_t count = (int32_t)m_coders.size() ;
    for (int32_t i = 0; i < count ; i++)
    {
        const RCCoderMixserCoderStreamsInfo& coderStreamsInfo = m_coders[i];
        numInStreams += coderStreamsInfo.m_numInStreams;
        numOutStreams += coderStreamsInfo.m_numOutStreams;
    }
}

int32_t RCCoderMixserBindInfo::FindBinderForInStream(uint32_t inStream) const
{
    int32_t count = (int32_t)m_bindPairs.size() ;
    for (int32_t i = 0; i < count; i++)
    {
        if (m_bindPairs[i].m_inIndex == inStream)
        {
            return i;
        }
    }
    return -1;
}

int32_t RCCoderMixserBindInfo::FindBinderForOutStream(uint32_t outStream) const
{
    int32_t count = (int32_t)m_bindPairs.size() ;
    for (int32_t i = 0; i < count; i++)
    {
        if (m_bindPairs[i].m_outIndex == outStream)
        {
            return i;
        }
    }
    return -1;
}

uint32_t RCCoderMixserBindInfo::GetCoderInStreamIndex(uint32_t coderIndex) const
{
    uint32_t streamIndex = 0;
    for (uint32_t i = 0; i < coderIndex; i++)
    {
        streamIndex += m_coders[i].m_numInStreams;
    }
    return streamIndex;
}

uint32_t RCCoderMixserBindInfo::GetCoderOutStreamIndex(uint32_t coderIndex) const
{
    uint32_t streamIndex = 0;
    for (uint32_t i = 0; i < coderIndex; i++)
    {
        streamIndex += m_coders[i].m_numOutStreams;
    }
    return streamIndex;
}

HResult RCCoderMixserBindInfo::FindInStream(uint32_t streamIndex, 
                                            uint32_t& coderIndex,
                                            uint32_t& coderStreamIndex) const
{
    uint32_t count = (uint32_t)m_coders.size() ;
    for (coderIndex = 0; coderIndex < count; coderIndex++)
    {
        uint32_t curSize = m_coders[coderIndex].m_numInStreams;
        if (streamIndex < curSize)
        {
            coderStreamIndex = streamIndex;
            return RC_S_OK ;
        }
        streamIndex -= curSize;
    }
    return RC_E_FAIL ;
}

HResult RCCoderMixserBindInfo::FindOutStream(uint32_t streamIndex, 
                                             uint32_t& coderIndex,
                                             uint32_t& coderStreamIndex) const
{
    uint32_t count = (uint32_t)m_coders.size() ;
    for (coderIndex = 0; coderIndex < count; coderIndex++)
    {
        uint32_t curSize = m_coders[coderIndex].m_numOutStreams;
        if (streamIndex < curSize)
        {
            coderStreamIndex = streamIndex;
            return RC_S_OK;
        }
        streamIndex -= curSize;
    }
    return RC_E_FAIL ;
}

RCCoderMixserBindReverseConverter::RCCoderMixserBindReverseConverter(const RCCoderMixserBindInfo& srcBindInfo):
    m_srcBindInfo(srcBindInfo)
{
    srcBindInfo.GetNumStreams(m_numSrcInStreams, m_numSrcOutStreams);

    uint32_t  j = 0 ;
    for (j = 0; j < m_numSrcInStreams; j++)
    {
        m_srcInToDestOutMap.push_back(0);
        m_destOutToSrcInMap.push_back(0);
    }

    for (j = 0; j < m_numSrcOutStreams; j++)
    {
        m_srcOutToDestInMap.push_back(0);
        m_destInToSrcOutMap.push_back(0);
    }

    uint32_t destInOffset = 0;
    uint32_t destOutOffset = 0;
    uint32_t srcInOffset = m_numSrcInStreams;
    uint32_t srcOutOffset = m_numSrcOutStreams;

    for (int32_t i = (int32_t)srcBindInfo.m_coders.size() - 1; i >= 0; i--)
    {
        const RCCoderMixserCoderStreamsInfo &srcCoderInfo = srcBindInfo.m_coders[i];

        srcInOffset -= srcCoderInfo.m_numInStreams;
        srcOutOffset -= srcCoderInfo.m_numOutStreams;

        uint32_t j = 0 ;
        for (j = 0; j < srcCoderInfo.m_numInStreams; j++, destOutOffset++)
        {
            uint32_t index = srcInOffset + j;
            m_srcInToDestOutMap[index] = destOutOffset;
            m_destOutToSrcInMap[destOutOffset] = index;
        }

        for (j = 0; j < srcCoderInfo.m_numOutStreams; j++, destInOffset++)
        {
            uint32_t index = srcOutOffset + j;
            m_srcOutToDestInMap[index] = destInOffset;
            m_destInToSrcOutMap[destInOffset] = index;
        }
    }
}

void RCCoderMixserBindReverseConverter::CreateReverseBindInfo(RCCoderMixserBindInfo &destBindInfo)
{
    destBindInfo.m_coders.clear();
    destBindInfo.m_bindPairs.clear();
    destBindInfo.m_inStreams.clear();
    destBindInfo.m_outStreams.clear();

    int32_t i = 0 ;
    for (i = (int32_t)m_srcBindInfo.m_coders.size() - 1; i >= 0; i--)
    {
        const RCCoderMixserCoderStreamsInfo& srcCoderInfo = m_srcBindInfo.m_coders[i];
        RCCoderMixserCoderStreamsInfo destCoderInfo;
        destCoderInfo.m_numInStreams = srcCoderInfo.m_numOutStreams;
        destCoderInfo.m_numOutStreams = srcCoderInfo.m_numInStreams;
        destBindInfo.m_coders.push_back(destCoderInfo);
    }

    for (i = (int32_t)m_srcBindInfo.m_bindPairs.size() - 1; i >= 0; i--)
    {
        const RCCoderMixerBindPair& srcBindPair = m_srcBindInfo.m_bindPairs[i];
        RCCoderMixerBindPair destBindPair;
        destBindPair.m_inIndex = m_srcOutToDestInMap[srcBindPair.m_outIndex];
        destBindPair.m_outIndex = m_srcInToDestOutMap[srcBindPair.m_inIndex];
        destBindInfo.m_bindPairs.push_back(destBindPair);
    }

    for (i = 0; i < (int32_t)m_srcBindInfo.m_inStreams.size(); i++)
    {
        destBindInfo.m_outStreams.push_back(m_srcInToDestOutMap[m_srcBindInfo.m_inStreams[i]]);
    }

    for (i = 0; i < (int32_t)m_srcBindInfo.m_outStreams.size(); i++)
    {
        destBindInfo.m_inStreams.push_back(m_srcOutToDestInMap[m_srcBindInfo.m_outStreams[i]]);
    }
}

RCCoderMixserCoderInfo::RCCoderMixserCoderInfo(uint32_t numInStreams, uint32_t numOutStreams):
    m_numInStreams(numInStreams),
    m_numOutStreams(numOutStreams)
{
    m_inSizes.reserve(m_numInStreams);
    m_inSizePointers.reserve(m_numInStreams);
    m_outSizePointers.reserve(m_numOutStreams);
    m_outSizePointers.reserve(m_numOutStreams);
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

void RCCoderMixserCoderInfo::SetCoderInfo(const uint64_t** inSizes,
                                          const uint64_t** outSizes)
{
    SetSizes(inSizes, m_inSizes, m_inSizePointers, m_numInStreams);
    SetSizes(outSizes, m_outSizes, m_outSizePointers, m_numOutStreams);
}

HResult RCCoderMixserCoderInfo::QueryInterface(RC_IID iid, void** outObject) const
{
    IUnknown* p = m_coder ? (IUnknown*)m_coder.Get() : (IUnknown*)m_coder2.Get() ;
    if (p)
    {
        return p->QueryInterface(iid, outObject);
    }
    else
    {
        return RC_S_FALSE ;
    }
}

END_NAMESPACE_RCZIP
