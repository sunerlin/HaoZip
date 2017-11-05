/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "format/7z/RC7zEncoder.h"
#include "common/RCCreateCoder.h"
#include "common/RCBuffer.h"
#include "interface/RCMethodDefs.h"
#include "filesystem/RCInOutTempBuffer.h"
#include "filesystem/RCSequentialOutTempBufferImp.h"
#include "filesystem/RCSequentialOutStreamSizeCount.h"
#include "filesystem/RCSequentialOutStreamImp.h"
#include "format/common/RCMethodProps.h"
#include "format/7z/RC7zSpecStream.h"

/////////////////////////////////////////////////////////////////
//RC7zEncoder class implementation

BEGIN_NAMESPACE_RCZIP

RC7zEncoder::RC7zEncoder(const RC7zCompressionMethodMode &options):
    m_bindReverseConverter(NULL),
    m_constructed(false)
{
    if(options.IsEmpty())
    {
        _ThrowCode(RC_E_DataError) ;
    }
    m_options = options;
    m_mixerCoderSpec = NULL;
}

RC7zEncoder::~RC7zEncoder()
{
    if(m_bindReverseConverter)
    {
        delete m_bindReverseConverter;
        m_bindReverseConverter = NULL ;
    }
}

HResult RC7zEncoder::CreateMixerCoder(ICompressCodecsInfo* codecsInfo,
                                      const uint64_t* inSizeForReduce)
{
    m_mixerCoderSpec = new RCCoderMixerMT ;
    m_mixerCoder = m_mixerCoderSpec;
    HResult hr = m_mixerCoderSpec->SetBindInfo(m_bindInfo) ;
    if(!IsSuccess(hr))
    {
        return hr ;
    }
    for (int32_t i = 0; i < (int32_t)m_options.m_methods.size(); i++)
    {
        const RC7zArchiveMethodFull& methodFull = m_options.m_methods[i];
        m_codersInfo.push_back(RC7zCoderInfo());
        RC7zCoderInfo& encodingInfo = m_codersInfo.back();
        encodingInfo.m_methodID = methodFull.m_id ;
        ICompressCoderPtr encoder ;
        ICompressCoder2Ptr encoder2 ;

        hr = RCCreateCoder::CreateCoder(codecsInfo, methodFull.m_id, encoder, encoder2, true) ;
        if(!IsSuccess(hr))
        {
            return hr ;
        }
        if (!encoder && !encoder2)
        {
            return RC_E_FAIL;
        }

        IUnknownPtr encoderCommon = encoder ? (IUnknown *)encoder.Get() : (IUnknown *)encoder2.Get() ;

#ifdef COMPRESS_MT
        {
            ICompressSetCoderMtPtr setCoderMt ;
            encoderCommon.QueryInterface(IID_ICompressSetCoderMt, setCoderMt.GetAddress());
            if (setCoderMt)
            {
                hr = setCoderMt->SetNumberOfThreads(m_options.m_numThreads) ;
                if(!IsSuccess(hr))
                {
                    return hr ;
                }
            }
        }
#endif


        hr = RCMethodProps::SetMethodProperties(methodFull, inSizeForReduce, encoderCommon.Get()) ;
        if(!IsSuccess(hr))
        {
            return hr ;
        }
        ISetCompressCodecsInfoPtr setCompressCodecsInfo ;
        encoderCommon.QueryInterface(IID_ISetCompressCodecsInfo, (void **)setCompressCodecsInfo.GetAddress());
        if (setCompressCodecsInfo)
        {
            hr = setCompressCodecsInfo->SetCompressCodecsInfo(codecsInfo) ;
            if(!IsSuccess(hr))
            {
                return hr ;
            }
        }
        ICryptoSetPasswordPtr cryptoSetPassword ;
        encoderCommon.QueryInterface(IID_ICryptoSetPassword, cryptoSetPassword.GetAddress());

        if (cryptoSetPassword)
        {
            RCByteBuffer buffer ;
            const uint32_t sizeInBytes = (uint32_t)m_options.m_password.size() * 2 ;
            buffer.SetCapacity(sizeInBytes);
            for (int32_t i = 0; i < (int32_t)m_options.m_password.size(); i++)
            {
                RCString::value_type c = m_options.m_password[i];
                ((byte_t*)buffer.data())[i * 2] = (byte_t)c;
                ((byte_t*)buffer.data())[i * 2 + 1] = (byte_t)(c >> 8) ;
            }
            hr = cryptoSetPassword->CryptoSetPassword((const byte_t *)buffer.data(), sizeInBytes) ;
            if(!IsSuccess(hr))
            {
                return hr ;
            }
        }

        if (encoder)
        {
            m_mixerCoderSpec->AddCoder(encoder.Get());
        }
        else
        {
            m_mixerCoderSpec->AddCoder2(encoder2.Get());
        }
    }
    return RC_S_OK;
}

HResult RC7zEncoder::EncoderConstr()
{
    if (m_constructed)
    {
        return RC_S_OK;
    }
    if (m_options.m_methods.empty())
    {
        // it has only password method;
        if (!m_options.m_passwordIsDefined)
        {
            _ThrowCode(RC_E_DataError) ;
        }
        if (!m_options.m_binds.empty())
        {
            _ThrowCode(RC_E_DataError) ;
        }
        RCCoderMixserCoderStreamsInfo coderStreamsInfo;
        RC7zArchiveMethodFull method ;

        method.m_numInStreams = 1;
        method.m_numOutStreams = 1;
        coderStreamsInfo.m_numInStreams = 1;
        coderStreamsInfo.m_numOutStreams = 1;
        method.m_id = RCMethod::ID_CRYPTO_7Z_AES ;

        m_options.m_methods.push_back(method);
        m_bindInfo.m_coders.push_back(coderStreamsInfo);

        m_bindInfo.m_inStreams.push_back(0);
        m_bindInfo.m_outStreams.push_back(0);
    }
    else
    {
        uint32_t numInStreams = 0 ;
        uint32_t numOutStreams = 0 ;
        int32_t i = 0 ;
        for (i = 0; i < (int32_t)m_options.m_methods.size(); i++)
        {
            const RC7zArchiveMethodFull& methodFull = m_options.m_methods[i];
            RCCoderMixserCoderStreamsInfo coderStreamsInfo ;
            coderStreamsInfo.m_numInStreams = methodFull.m_numOutStreams;
            coderStreamsInfo.m_numOutStreams = methodFull.m_numInStreams;
            if (m_options.m_binds.empty())
            {
                if (i < (int32_t)m_options.m_methods.size() - 1)
                {
                    RCCoderMixerBindPair bindPair;
                    bindPair.m_inIndex = numInStreams + coderStreamsInfo.m_numInStreams;
                    bindPair.m_outIndex = numOutStreams;
                    m_bindInfo.m_bindPairs.push_back(bindPair);
                }
                else
                {
                    m_bindInfo.m_outStreams.insert(m_bindInfo.m_outStreams.begin(), numOutStreams);
                }
                for (uint32_t j = 1; j < coderStreamsInfo.m_numOutStreams; j++)
                {
                    m_bindInfo.m_outStreams.push_back(numOutStreams + j);
                }
            }

            numInStreams += coderStreamsInfo.m_numInStreams;
            numOutStreams += coderStreamsInfo.m_numOutStreams;

            m_bindInfo.m_coders.push_back(coderStreamsInfo);
        }

        if (!m_options.m_binds.empty())
        {
            for (i = 0; i < (int32_t)m_options.m_binds.size(); i++)
            {
                RCCoderMixerBindPair bindPair;
                const RC7zBind& bind = m_options.m_binds[i];
                bindPair.m_inIndex = m_bindInfo.GetCoderInStreamIndex(bind.m_inCoder) + bind.m_inStream;
                bindPair.m_outIndex = m_bindInfo.GetCoderOutStreamIndex(bind.m_outCoder) + bind.m_outStream;
                m_bindInfo.m_bindPairs.push_back(bindPair);
            }
            for (i = 0; i < (int32_t)numOutStreams; i++)
            {
                if (m_bindInfo.FindBinderForOutStream(i) == -1)
                {
                    m_bindInfo.m_outStreams.push_back(i);
                }
            }
        }

        for (i = 0; i < (int32_t)numInStreams; i++)
        {
            if (m_bindInfo.FindBinderForInStream(i) == -1)
            {
                m_bindInfo.m_inStreams.push_back(i);
            }
        }

        if (m_bindInfo.m_inStreams.empty())
        {
            _ThrowCode(RC_E_DataError) ;
        }

        // Make main stream first in list
        int32_t inIndex = m_bindInfo.m_inStreams[0];
        for (;;)
        {
            uint32_t coderIndex = 0 ;
            uint32_t coderStreamIndex = 0 ;
            HResult hr = m_bindInfo.FindInStream(inIndex, coderIndex, coderStreamIndex);
            if(!IsSuccess(hr))
            {
                return hr ;
            }
            uint32_t outIndex = m_bindInfo.GetCoderOutStreamIndex(coderIndex);
            int32_t binder = m_bindInfo.FindBinderForOutStream(outIndex);
            if (binder >= 0)
            {
                inIndex = m_bindInfo.m_bindPairs[binder].m_inIndex;
                continue;
            }
            for (i = 0; i < (int32_t)m_bindInfo.m_outStreams.size(); i++)
            {
                if (m_bindInfo.m_outStreams[i] == outIndex)
                {
                    RCVectorUtils::Delete(m_bindInfo.m_outStreams,i) ;
                    m_bindInfo.m_outStreams.insert(m_bindInfo.m_outStreams.begin(), outIndex);
                    break;
                }
            }
            break;
        }

        if (m_options.m_passwordIsDefined)
        {
            int32_t numCryptoStreams = (int32_t)m_bindInfo.m_outStreams.size();

            for (i = 0; i < numCryptoStreams; i++)
            {
                RCCoderMixerBindPair bindPair ;
                bindPair.m_inIndex = numInStreams + i;
                bindPair.m_outIndex = m_bindInfo.m_outStreams[i];
                m_bindInfo.m_bindPairs.push_back(bindPair);
            }
            m_bindInfo.m_outStreams.clear();

            for (i = 0; i < numCryptoStreams; i++)
            {
                RCCoderMixserCoderStreamsInfo coderStreamsInfo ;
                RC7zArchiveMethodFull method;
                method.m_numInStreams = 1;
                method.m_numOutStreams = 1;
                coderStreamsInfo.m_numInStreams = method.m_numOutStreams;
                coderStreamsInfo.m_numOutStreams = method.m_numInStreams;
                method.m_id = RCMethod::ID_CRYPTO_7Z_AES ;

                m_options.m_methods.push_back(method);
                m_bindInfo.m_coders.push_back(coderStreamsInfo);
                m_bindInfo.m_outStreams.push_back(numOutStreams + i);
            }
        }

    }

    for (int32_t i = (int32_t)m_options.m_methods.size() - 1; i >= 0; i--)
    {
        const RC7zArchiveMethodFull& methodFull = m_options.m_methods[i] ;
        m_decompressionMethods.push_back(methodFull.m_id);
    }

    m_bindReverseConverter = new RCCoderMixserBindReverseConverter(m_bindInfo);
    m_bindReverseConverter->CreateReverseBindInfo(m_decompressBindInfo);
    m_constructed = true;
    return RC_S_OK;
}

HResult RC7zEncoder::Encode(ICompressCodecsInfo* codecsInfo,
                            ISequentialInStream *inStream,
                            const uint64_t *inStreamSize, 
                            const uint64_t *inSizeForReduce,
                            RC7zFolder &folderItem,
                            ISequentialOutStream *outStream,
                            RCVector<uint64_t> &packSizes,
                            ICompressProgressInfo *compressProgress)
{
    HResult hr = EncoderConstr() ;
    if(!IsSuccess(hr))
    {
        return hr ;
    }

    if (m_mixerCoderSpec == NULL)
    {
        hr = CreateMixerCoder(codecsInfo, inSizeForReduce) ;
        if(!IsSuccess(hr))
        {
            return hr ;
        }
    }
    m_mixerCoderSpec->ReInit();

    RCVector< RCSharedPtr<RCInOutTempBuffer> > inOutTempBuffers;
    RCVector<RCSequentialOutTempBufferImp*> tempBufferSpecs;
    RCVector<ISequentialOutStreamPtr> tempBuffers;
    int32_t numMethods = (int32_t)m_bindInfo.m_coders.size() ;
    int32_t i = 0 ;
    for (i = 1; i < (int32_t)m_bindInfo.m_outStreams.size(); i++)
    {
        RCSharedPtr<RCInOutTempBuffer> spTempBuffer(new RCInOutTempBuffer()) ;
        inOutTempBuffers.push_back(spTempBuffer);
        spTempBuffer->Create();
        spTempBuffer->InitWriting();
    }
    for (i = 1; i < (int32_t)m_bindInfo.m_outStreams.size(); i++)
    {
        RCSequentialOutTempBufferImp *tempBufferSpec = new RCSequentialOutTempBufferImp;
        ISequentialOutStreamPtr tempBuffer = tempBufferSpec;
        tempBufferSpec->Init(inOutTempBuffers[i - 1].get());
        tempBuffers.push_back(tempBuffer);
        tempBufferSpecs.push_back(tempBufferSpec);
    }

    for (i = 0; i < numMethods; i++)
    {
        m_mixerCoderSpec->SetCoderInfo(i, NULL, NULL);
    }
    if (m_bindInfo.m_inStreams.empty())
    {
        return RC_E_FAIL;
    }
    uint32_t mainCoderIndex = 0 ;
    uint32_t mainStreamIndex = 0 ;
    hr = m_bindInfo.FindInStream(m_bindInfo.m_inStreams[0], mainCoderIndex, mainStreamIndex);
    if(!IsSuccess(hr))
    {
        return hr ;
    }
    if (inStreamSize != NULL)
    {
        RCVector<const uint64_t *> sizePointers;
        for (uint32_t i = 0; i < m_bindInfo.m_coders[mainCoderIndex].m_numInStreams; i++)
        {
            if (i == mainStreamIndex)
            {
                sizePointers.push_back(inStreamSize);
            }
            else
            {
                sizePointers.push_back(NULL);
            }
        }
        m_mixerCoderSpec->SetCoderInfo(mainCoderIndex, &sizePointers.front(), NULL);
    }

    RC7zSpecStream *inStreamSizeCountSpec = new RC7zSpecStream;
    ISequentialInStreamPtr inStreamSizeCount = inStreamSizeCountSpec;
    RCSequentialOutStreamSizeCount *outStreamSizeCountSpec = new RCSequentialOutStreamSizeCount;
    ISequentialOutStreamPtr outStreamSizeCount = outStreamSizeCountSpec;

    inStreamSizeCountSpec->Init(inStream);
    outStreamSizeCountSpec->SetStream(outStream);
    outStreamSizeCountSpec->Init();

    RCVector<ISequentialInStream*> inStreamPointers;
    RCVector<ISequentialOutStream*> outStreamPointers;
    inStreamPointers.push_back(inStreamSizeCount.Get());
    outStreamPointers.push_back(outStreamSizeCount.Get());
    for (i = 1; i < (int32_t)m_bindInfo.m_outStreams.size(); i++)
    {
        outStreamPointers.push_back(tempBuffers[i - 1].Get());
    }

    for (i = 0; i < (int32_t)m_codersInfo.size(); i++)
    {
        RC7zCoderInfo &encodingInfo = m_codersInfo[i];

        ICryptoResetInitVectorPtr resetInitVector;
        m_mixerCoderSpec->m_coders[i]->QueryInterface(IID_ICryptoResetInitVector, (void **)resetInitVector.GetAddress());
        if (resetInitVector != NULL)
        {
            resetInitVector->ResetInitVector();
        }

        ICompressWriteCoderPropertiesPtr writeCoderProperties;
        m_mixerCoderSpec->m_coders[i]->QueryInterface(IID_ICompressWriteCoderProperties, (void **)writeCoderProperties.GetAddress());
        if (writeCoderProperties != NULL)
        {
            RCSequentialOutStreamImp* outStreamSpec = new RCSequentialOutStreamImp;
            ISequentialOutStreamPtr outStream(outStreamSpec);
            outStreamSpec->Init();
            writeCoderProperties->WriteCoderProperties(outStream.Get());
            size_t size = outStreamSpec->GetSize();
            encodingInfo.m_properties.SetCapacity(size);
            memmove(encodingInfo.m_properties.data(), outStreamSpec->GetBuffer().data(), size);
        }
    }

    uint32_t progressIndex = mainCoderIndex;

    for (i = 0; (i + 1) < (int32_t)m_codersInfo.size(); ++i)
    {
        RCMethodID methodID = m_codersInfo[i].m_methodID ;
        if ( (methodID == RCMethod::ID_COMPRESS_DELTA) || 
             (methodID == RCMethod::ID_COMPRESS_BRANCH_BCJ) ||
             (methodID == RCMethod::ID_COMPRESS_BRANCH_BCJ_2) )
        {
            progressIndex = i + 1 ;
        }
    }

    m_mixerCoderSpec->SetProgressCoderIndex(progressIndex);

    RCVector<ICompressCoder2::in_stream_data> inStreamDatas;
    inStreamDatas.push_back(ICompressCoder2::in_stream_data(inStreamPointers[0], 0));

    RCVector<ICompressCoder2::out_stream_data> outStreamDatas;
    for (i = 0; i < (int32_t)outStreamPointers.size(); i++)
    {
        outStreamDatas.push_back(ICompressCoder2::out_stream_data(outStreamPointers[i], 0));
    }

    hr = m_mixerCoder->Code(inStreamDatas,
                            outStreamDatas, 
                            compressProgress) ;
    if(!IsSuccess(hr))
    {
        return hr ;
    }
    ConvertBindInfoToFolderItemInfo(m_decompressBindInfo, m_decompressionMethods, folderItem);

    packSizes.push_back(outStreamSizeCountSpec->GetSize());

    for (i = 1; i < (int32_t)m_bindInfo.m_outStreams.size(); i++)
    {
        RCInOutTempBuffer* inOutTempBuffer = inOutTempBuffers[i - 1].get() ;
        HResult writeHr = inOutTempBuffer->WriteToStream(outStream) ;
        if(!IsSuccess(writeHr))
        {
            return writeHr ;
        }
        packSizes.push_back(inOutTempBuffer->GetDataSize());
    }

    for (i = 0; i < (int32_t)m_bindReverseConverter->m_numSrcInStreams; i++)
    {
        int32_t binder = m_bindInfo.FindBinderForInStream(m_bindReverseConverter->m_destOutToSrcInMap[i]);
        uint64_t streamSize = 0 ;
        if (binder < 0)
        {
            streamSize = inStreamSizeCountSpec->GetSize();
        }
        else
        {
            streamSize = m_mixerCoderSpec->GetWriteProcessedSize(binder);
        }
        folderItem.m_unpackSizes.push_back(streamSize);
    }
    for (i = numMethods - 1; i >= 0; i--)
    {
        folderItem.m_coders[numMethods - 1 - i].m_properties = m_codersInfo[i].m_properties;
    }
    return RC_S_OK;
}

void RC7zEncoder::ConvertBindInfoToFolderItemInfo(RCCoderMixserBindInfo& bindInfo,
                                                  const RCVector<RCMethodID>& decompressionMethods,
                                                  RC7zFolder& folder)
{
    folder.m_coders.clear();
    folder.m_packStreams.clear();
    folder.m_bindPairs.clear();
    int32_t i = 0 ;
    for (i = 0; i < (int32_t)bindInfo.m_bindPairs.size(); i++)
    {
        RC7zBindPair bindPair;
        bindPair.m_inIndex = bindInfo.m_bindPairs[i].m_inIndex;
        bindPair.m_outIndex = bindInfo.m_bindPairs[i].m_outIndex;
        folder.m_bindPairs.push_back(bindPair);
    }
    for (i = 0; i < (int32_t)bindInfo.m_coders.size(); i++)
    {
        RC7zCoderInfo coderInfo;
        RCCoderMixserCoderStreamsInfo& coderStreamsInfo = bindInfo.m_coders[i];
        coderInfo.m_numInStreams = coderStreamsInfo.m_numInStreams;
        coderInfo.m_numOutStreams = coderStreamsInfo.m_numOutStreams;
        coderInfo.m_methodID = decompressionMethods[i];
        folder.m_coders.push_back(coderInfo);
    }
    for (i = 0; i < (int32_t)bindInfo.m_inStreams.size(); i++)
    {
        folder.m_packStreams.push_back(bindInfo.m_inStreams[i]);
    }
}

END_NAMESPACE_RCZIP
