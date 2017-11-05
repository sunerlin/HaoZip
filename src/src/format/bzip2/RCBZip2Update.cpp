/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "format/bzip2/RCBZip2Update.h"
#include "common/RCLocalProgress.h"
#include "common/RCCreateCoder.h"
#include "interface/RCMethodDefs.h"
#include "interface/IStream.h"

/////////////////////////////////////////////////////////////////
//RCBZip2Update class implementation

BEGIN_NAMESPACE_RCZIP

HResult RCBZip2Update::UpdateArchive(ICompressCodecsInfo* codecsInfo,
                                     uint64_t unpackSize,
                                     ISequentialOutStream* outStream,
                                     int32_t indexInClient,
                                     uint32_t dictionary,
                                     uint32_t numPasses,
                                     uint32_t numThreads,
                                     IArchiveUpdateCallback* updateCallback)
{
    HResult hr = updateCallback->SetTotal(unpackSize);
    if (hr != RC_S_OK)
    {
        return hr;
    }

    uint64_t complexity = 0;
    hr = updateCallback->SetCompleted(complexity);
    if (hr != RC_S_OK)
    {
        return hr;
    }

    ISequentialInStreamPtr fileInStream;
    hr = updateCallback->GetStream(indexInClient, fileInStream.GetAddress());
    if (hr != RC_S_OK)
    {
        return hr;
    }

    RCLocalProgress* localProgressSpec = new RCLocalProgress;
    ICompressProgressInfoPtr localProgress = localProgressSpec;
    localProgressSpec->Init(updateCallback, true);

    ICompressCoderPtr encoder;
    hr = RCCreateCoder::CreateCoder(codecsInfo,
                                    RCMethod::ID_COMPRESS_BZIP2,
                                    encoder,
                                    true);
    if (hr != RC_S_OK)
    {
        return hr;
    }

    ICompressSetCoderPropertiesPtr setCoderProperties;
    encoder.QueryInterface(IID_ICompressSetCoderProperties, setCoderProperties.GetAddress());
    if (setCoderProperties)
    {
        RCPropertyIDPairArray propertyArray;

        RCPropertyIDPair propertyIDs[] = 
        {
            RCPropertyIDPair(RCCoderPropID::kDictionarySize, (uint64_t)dictionary),
            RCPropertyIDPair(RCCoderPropID::kNumPasses,      (uint64_t)numPasses),
#ifdef COMPRESS_MT
            RCPropertyIDPair(RCCoderPropID::kNumThreads,     (uint64_t)numThreads)
#else
            RCPropertyIDPair(RCCoderPropID::kNumThreads,     (uint64_t)1)
#endif
        };

        for (uint32_t i = 0; i<sizeof(propertyIDs)/sizeof(propertyIDs[0]); i++)
        {
            propertyArray.push_back(propertyIDs[i]);
        }
                
        hr = setCoderProperties->SetCoderProperties(propertyArray);
        if (hr != RC_S_OK)
        {
            return hr;
        }
    }

    hr = encoder->Code(fileInStream.Get(), outStream, NULL, NULL, localProgress.Get());
    if (hr != RC_S_OK)
    {
        return hr;
    }

    return updateCallback->SetOperationResult(indexInClient,RC_ARCHIVE_UPDATE_RESULT_OK);
}

END_NAMESPACE_RCZIP
