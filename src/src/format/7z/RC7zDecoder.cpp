/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "format/7z/RC7zDecoder.h"
#include "locked/RCLockedInStream.h"
#include "locked/RCLockedSequentialInStreamImp.h"
#include "filesystem/RCLimitedSequentialInStream.h"
#include "common/RCCreateCoder.h"

/////////////////////////////////////////////////////////////////
//RC7zDecoder class implementation

BEGIN_NAMESPACE_RCZIP

RC7zDecoder::RC7zDecoder():
    m_bindInfoExPrevIsDefined(false),
    m_mixerCoderMTSpec(NULL),
    m_mixerCoderCommon(NULL)
{
}

RC7zDecoder::~RC7zDecoder()
{
}

void RC7zDecoder::ConvertFolderItemInfoToBindInfo(const RC7zFolder& folder,
                                                  RC7zBindInfoEx& bindInfo)  
{
    bindInfo.Clear();
    int32_t i = 0 ;
    for (i = 0; i < (int32_t)folder.m_bindPairs.size(); i++)
    {
        RCCoderMixerBindPair bindPair;
        bindPair.m_inIndex = (uint32_t)folder.m_bindPairs[i].m_inIndex;
        bindPair.m_outIndex = (uint32_t)folder.m_bindPairs[i].m_outIndex;
        bindInfo.m_bindPairs.push_back(bindPair);
    }
    uint32_t outStreamIndex = 0;
    for (i = 0; i < (int32_t)folder.m_coders.size(); i++)
    {
        RCCoderMixserCoderStreamsInfo coderStreamsInfo;
        const RC7zCoderInfo& coderInfo = folder.m_coders[i];
        coderStreamsInfo.m_numInStreams = (uint32_t)coderInfo.m_numInStreams;
        coderStreamsInfo.m_numOutStreams = (uint32_t)coderInfo.m_numOutStreams;
        bindInfo.m_coders.push_back(coderStreamsInfo);
        bindInfo.m_coderMethodIDs.push_back(coderInfo.m_methodID);
        for (uint32_t j = 0; j < coderStreamsInfo.m_numOutStreams; j++, outStreamIndex++)
        {
            if (folder.FindBindPairForOutStream(outStreamIndex) < 0)
            {
                bindInfo.m_outStreams.push_back(outStreamIndex);
            }
        }
    }
    for (i = 0; i < (int32_t)folder.m_packStreams.size(); i++)
    {
        bindInfo.m_inStreams.push_back((uint32_t)folder.m_packStreams[i]);
    }
}

bool RC7zDecoder::AreCodersEqual(const RCCoderMixserCoderStreamsInfo& a1,
                                 const RCCoderMixserCoderStreamsInfo& a2) 
{
    return (a1.m_numInStreams == a2.m_numInStreams) &&
           (a1.m_numOutStreams == a2.m_numOutStreams) ;
}

bool RC7zDecoder::AreBindPairsEqual(const RCCoderMixerBindPair& a1, 
                                    const RCCoderMixerBindPair& a2)
{
    return (a1.m_inIndex == a2.m_inIndex) &&
           (a1.m_outIndex == a2.m_outIndex) ;
}

bool RC7zDecoder::AreBindInfoExEqual(const RC7zBindInfoEx& a1, 
                                     const RC7zBindInfoEx& a2)
{
    if (a1.m_coders.size() != a2.m_coders.size())
    {
        return false;
    }
    int32_t i = 0 ;
    for (i = 0; i < (int32_t)a1.m_coders.size(); i++)
    {
        if (!AreCodersEqual(a1.m_coders[i], a2.m_coders[i]))
        {
            return false;
        }
    }
    if (a1.m_bindPairs.size() != a2.m_bindPairs.size())
    {
        return false;
    }
    for (i = 0; i < (int32_t)a1.m_bindPairs.size(); i++)
    {
        if (!AreBindPairsEqual(a1.m_bindPairs[i], a2.m_bindPairs[i]))
        {
            return false;
        }
    }
    for (i = 0; i < (int32_t)a1.m_coderMethodIDs.size(); i++)
    {
        if (a1.m_coderMethodIDs[i] != a2.m_coderMethodIDs[i])
        {
            return false;
        }
    }
    if (a1.m_inStreams.size() != a2.m_inStreams.size())
    {
        return false;
    }
    if (a1.m_outStreams.size() != a2.m_outStreams.size())
    {
        return false;
    }
    return true;
}

HResult RC7zDecoder::Decode(ICompressCodecsInfo* codecsInfo,
                            IInStream* inStream,
                            uint64_t startPos,
                            const uint64_t* packSizes,
                            const RC7zFolder& folderInfo,
                            ISequentialOutStream* outStream,
                            ICompressProgressInfo* compressProgress,
                            ICryptoGetTextPassword* getTextPassword, 
                            bool& passwordIsDefined,
                            uint32_t numThreads )
{
    if (!folderInfo.CheckStructure())
    {
        return RC_E_NOTIMPL ;
    }

    passwordIsDefined = false;
    RCVector< ISequentialInStreamPtr > inStreams;

    RCLockedInStream lockedInStream ;
    lockedInStream.Init(inStream) ;

    for (int32_t j = 0; j < (int32_t)folderInfo.m_packStreams.size(); j++)
    {
        RCLockedSequentialInStreamImp *lockedStreamImpSpec = new RCLockedSequentialInStreamImp;
        ISequentialInStreamPtr lockedStreamImp = lockedStreamImpSpec;
        lockedStreamImpSpec->Init(&lockedInStream, startPos);
        startPos += packSizes[j];

        RCLimitedSequentialInStream *streamSpec = new RCLimitedSequentialInStream;
        ISequentialInStreamPtr inStream = streamSpec;
        streamSpec->SetStream(lockedStreamImp.Get());
        streamSpec->Init(packSizes[j]);
        inStreams.push_back(inStream);
    }

    int32_t numCoders = (int32_t)folderInfo.m_coders.size();

    RC7zBindInfoEx bindInfo;
    ConvertFolderItemInfoToBindInfo(folderInfo, bindInfo);
    bool createNewCoders = false ;
    if (!m_bindInfoExPrevIsDefined)
    {
        createNewCoders = true;
    }
    else
    {
        createNewCoders = !AreBindInfoExEqual(bindInfo, m_bindInfoExPrev);
    }
    if (createNewCoders)
    {
        int32_t i = 0 ;
        m_decoders.clear();
        m_mixerCoder.Release();

        m_mixerCoderMTSpec = new RCCoderMixerMT ;
        m_mixerCoder = m_mixerCoderMTSpec ;
        m_mixerCoderCommon = m_mixerCoderMTSpec ;
        HResult hr = m_mixerCoderCommon->SetBindInfo(bindInfo) ;
        if(!IsSuccess(hr))
        {
            return hr ;
        }
        for (i = 0; i < numCoders; i++)
        {
            const RC7zCoderInfo &coderInfo = folderInfo.m_coders[i];
            ICompressCoderPtr decoder;
            ICompressCoder2Ptr decoder2;
            hr = RCCreateCoder::CreateCoder(codecsInfo,
                                            coderInfo.m_methodID, 
                                            decoder, 
                                            decoder2, 
                                            false) ;
            if(!IsSuccess(hr))
            {
                return hr ;
            }
            IUnknownPtr decoderUnknown ;
            if (coderInfo.IsSimpleCoder())
            {
                if (decoder == 0)
                {
                    return RC_E_NOTIMPL ;
                }
                decoderUnknown = (IUnknown *)decoder.Get() ;
                m_mixerCoderMTSpec->AddCoder(decoder.Get()) ;
            }
            else
            {
                if (decoder2 == 0)
                {
                    return RC_E_NOTIMPL;
                }
                decoderUnknown = (IUnknown *)decoder2.Get() ;
                m_mixerCoderMTSpec->AddCoder2(decoder2.Get());
            }
            m_decoders.push_back(decoderUnknown);

            ISetCompressCodecsInfoPtr setCompressCodecsInfo;
            decoderUnknown.QueryInterface(IID_ISetCompressCodecsInfo, (void **)setCompressCodecsInfo.GetAddress());
            if (setCompressCodecsInfo)
            {
                HResult hr = setCompressCodecsInfo->SetCompressCodecsInfo(codecsInfo) ;
                if(!IsSuccess(hr))
                {
                    return hr ;
                }
            }
        }
        m_bindInfoExPrev = bindInfo ;
        m_bindInfoExPrevIsDefined = true ;
    }
    int32_t i = 0 ;
    m_mixerCoderCommon->ReInit();

    uint32_t packStreamIndex = 0 ;
    uint32_t unpackStreamIndex = 0;
    uint32_t coderIndex = 0;

    for (i = 0; i < numCoders; i++)
    {
        const RC7zCoderInfo& coderInfo = folderInfo.m_coders[i];
        IUnknownPtr& decoder = m_decoders[coderIndex];

        {
            ICompressSetDecoderProperties2Ptr setDecoderProperties;
            decoder.QueryInterface(IID_ICompressSetDecoderProperties2, setDecoderProperties.GetAddress());
            if (setDecoderProperties)
            {
                const RCByteBuffer& properties = coderInfo.m_properties;
                size_t size = properties.GetCapacity();
                if (size > 0xFFFFFFFF)
                {
                    return RC_E_NOTIMPL;
                }
                if (size > 0)
                {
                    HResult hr = setDecoderProperties->SetDecoderProperties2((const byte_t *)properties.data(), (uint32_t)size) ;
                    if(!IsSuccess(hr))
                    {
                        return hr ;
                    }
                }
            }
        }

        ICompressSetCoderMtPtr setCoderMt;
        decoder.QueryInterface(IID_ICompressSetCoderMt, setCoderMt.GetAddress());
        if(setCoderMt)
        {
            HResult hr = setCoderMt->SetNumberOfThreads(numThreads) ;
            if(!IsSuccess(hr))
            {
                return hr ;
            }
        }

        {
            ICryptoSetPasswordPtr cryptoSetPassword;
            decoder.QueryInterface(IID_ICryptoSetPassword, cryptoSetPassword.GetAddress());
            if (cryptoSetPassword)
            {
                if (getTextPassword == 0)
                {
                    return RC_E_FAIL;
                }
                RCString passwordValue;
                HResult hr = getTextPassword->CryptoGetTextPassword(passwordValue) ;
                if(!IsSuccess(hr))
                {
                    return hr ;
                }
                RCByteBuffer buffer;
                passwordIsDefined = true ;
                const RCString password(passwordValue);
                const uint32_t sizeInBytes = (uint32_t)password.size() * 2;
                buffer.SetCapacity(sizeInBytes);
                for (int32_t i = 0; i < (int32_t)password.size(); i++)
                {
                    RCString::value_type c = password[i];
                    ((byte_t *)buffer.data())[i * 2] = (byte_t)c;
                    ((byte_t *)buffer.data())[i * 2 + 1] = (byte_t)(c >> 8);
                }
                if(sizeInBytes > 0)
                {
                    hr = cryptoSetPassword->CryptoSetPassword((const byte_t *)buffer.data(), sizeInBytes) ;
                }
                if(!IsSuccess(hr))
                {
                    return hr ;
                }
            }
        }

        coderIndex++;

        uint32_t numInStreams = (uint32_t)coderInfo.m_numInStreams;
        uint32_t numOutStreams = (uint32_t)coderInfo.m_numOutStreams;
        RCVector<const uint64_t *> packSizesPointers;
        RCVector<const uint64_t *> unpackSizesPointers;
        packSizesPointers.reserve(numInStreams);
        unpackSizesPointers.reserve(numOutStreams);
        uint32_t j = 0 ;
        for (j = 0; j < numOutStreams; j++, unpackStreamIndex++)
        {
            unpackSizesPointers.push_back(&folderInfo.m_unpackSizes[unpackStreamIndex]);
        }

        for (j = 0; j < numInStreams; j++, packStreamIndex++)
        {
            int32_t bindPairIndex = folderInfo.FindBindPairForInStream(packStreamIndex);
            if (bindPairIndex >= 0)
            {
                packSizesPointers.push_back(&folderInfo.m_unpackSizes[(uint32_t)folderInfo.m_bindPairs[bindPairIndex].m_outIndex]);
            }
            else
            {
                int32_t index = folderInfo.FindPackStreamArrayIndex(packStreamIndex);
                if (index < 0)
                {
                    return RC_E_FAIL;
                }
                packSizesPointers.push_back(&packSizes[index]);
            }
        }

        m_mixerCoderCommon->SetCoderInfo(i, &packSizesPointers.front(), &unpackSizesPointers.front());
    }
    uint32_t mainCoder = 0 ;
    uint32_t temp = 0 ;
    HResult hr = bindInfo.FindOutStream(bindInfo.m_outStreams[0], mainCoder, temp);
    if(!IsSuccess(hr))
    {
        return hr ;
    }
    m_mixerCoderMTSpec->SetProgressCoderIndex(mainCoder);
 
    if (numCoders == 0)
    {
        return RC_S_OK ;
    }

    /*
    RCVector<ISequentialInStream *> inStreamPointers;
    inStreamPointers.reserve(inStreams.size());
    for (i = 0; i < inStreams.size(); i++)
    {
        inStreamPointers.push_back(inStreams[i].Get());
    }
    ISequentialOutStream *outStreamPointer = outStream;
    */

    RCVector<ICompressCoder2::in_stream_data> inStreamDatas;
    for (i = 0; i < (int32_t)inStreams.size(); i++)
    {
        inStreamDatas.push_back(ICompressCoder2::in_stream_data(inStreams[i].Get(), 0));
    }

    RCVector<ICompressCoder2::out_stream_data> outStreamDatas;
    outStreamDatas.push_back(ICompressCoder2::out_stream_data(outStream, 0));
  
    return m_mixerCoder->Code(inStreamDatas,
                              outStreamDatas, 
                              compressProgress) ;
}

END_NAMESPACE_RCZIP
