/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "format/nsis/RCNsisDecode.h"
#include "interface/RCMethodDefs.h"
#include "common/RCCreateCoder.h"
#include "filesystem/RCStreamUtils.h"

/////////////////////////////////////////////////////////////////
//RCNsisDecode class implementation

BEGIN_NAMESPACE_RCZIP

HResult RCNsisDecode::Init(ICompressCodecsInfo* codecsInfo,
                           IInStream* inStream,
                           RCNsisMethodType::EEnum method,
                           bool thereIsFilterFlag,
                           bool& useFilter)
{
    HResult hr;
    useFilter = false;

    if (m_decoderInStream)
    {
        if (method != m_method)
        {
            Release();
        }
    }

    m_method = method;
    if (!m_codecInStream)
    {
        RCMethodID methodID;
        switch (method)
        {
        case RCNsisMethodType::kCopy:
            {
                methodID = RCMethod::ID_COMPRESS_COPY;
            }
            break;

        case RCNsisMethodType::kDeflate:
            {
                methodID = RCMethod::ID_COMPRESS_DEFLATE_COM;
            }
            break;

        case RCNsisMethodType::kBZip2:
            {
                methodID = RCMethod::ID_COMPRESS_BZIP2;
            }
            break;

        case RCNsisMethodType::kLZMA:
            {
                methodID = RCMethod::ID_COMPRESS_LZMA;
            }
            break;

        default:
            {
                return RC_E_NOTIMPL;
            }
        }

        ICompressCoderPtr decoder;
        hr = RCCreateCoder::CreateCoder(codecsInfo,
                                        methodID,
                                        decoder,
                                        false);
        if (hr != RC_S_OK)
        {
            return hr;
        }

        if (!decoder)
        {
            return RC_E_NOTIMPL;
        }

        if (methodID == RCMethod::ID_COMPRESS_DEFLATE_COM)
        {
            ICompressSetCoderPropertiesPtr setCoderProperties;
            decoder->QueryInterface(IID_ICompressSetCoderProperties, (void**)setCoderProperties.GetAddress());
            if (setCoderProperties)
            {
                RCPropertyIDPairArray propertyArray;
                propertyArray.push_back(RCPropertyIDPair(RCMethodProperty::ID_SET_NSIS_DEFLATE,true));
                hr = setCoderProperties->SetCoderProperties(propertyArray);
                if (hr != RC_S_OK)
                {
                    return hr;
                }
            }
        }

        decoder.QueryInterface(IID_ISequentialInStream, m_codecInStream.GetAddress());
        if (!m_codecInStream)
        {
            return RC_E_NOTIMPL;
        }
    }

    if (thereIsFilterFlag)
    {
        uint32_t processedSize;
        byte_t flag;
        hr = inStream->Read(&flag, 1, &processedSize);
        if (hr != RC_S_OK)
        {
            return hr;
        }

        if (processedSize != 1)
        {
            return RC_E_FAIL;
        }

        if (flag > 1)
        {
            return RC_E_NOTIMPL;
        }

        useFilter = (flag != 0);
    }

    if (useFilter)
    {
        if (!m_filterInStream)
        {
            ICompressFilterPtr filter;
            hr = RCCreateCoder::CreateFilter(codecsInfo,
                                             RCMethod::ID_COMPRESS_BRANCH_BCJ,
                                             filter,
                                             false);
       
            if (hr != RC_S_OK)
            {
                return hr;
            }

            if (!filter)
            {
                return RC_E_NOTIMPL;
            }

            filter.QueryInterface(IID_ISequentialInStream, m_filterInStream.GetAddress());
            if (!m_filterInStream)
            {
                return RC_E_NOTIMPL;
            }
        }

        ICompressSetInStreamPtr setInStream;
        m_filterInStream.QueryInterface(IID_ICompressSetInStream, setInStream.GetAddress());
        if (!setInStream)
        {
            return RC_E_NOTIMPL;
        }

        hr = setInStream->SetInStream(m_codecInStream.Get());
        if (hr != RC_S_OK)
        {
            return hr;
        }

        m_decoderInStream = m_filterInStream;
    }
    else
    {
        m_decoderInStream = m_codecInStream;
    }

    if (method == RCNsisMethodType::kLZMA)
    {
        ICompressSetDecoderProperties2Ptr setDecoderProperties;
        m_codecInStream.QueryInterface(IID_ICompressSetDecoderProperties2, setDecoderProperties.GetAddress());
        if (setDecoderProperties)
        {
            static const uint32_t kPropertiesSize = 5;
            byte_t properties[kPropertiesSize];
            uint32_t processedSize;
            hr = inStream->Read(properties, kPropertiesSize, &processedSize);
            if (hr != RC_S_OK)
            {
                return hr;
            }

            if (processedSize != kPropertiesSize)
            {
                return RC_E_FAIL;
            }

            hr = setDecoderProperties->SetDecoderProperties2((const byte_t *)properties, kPropertiesSize);
            if (hr != RC_S_OK)
            {
                return hr;
            }
        }
    }

    {
        ICompressSetInStreamPtr setInStream;
        m_codecInStream.QueryInterface(IID_ICompressSetInStream, setInStream.GetAddress());
        if (!setInStream)
        {
            return RC_E_NOTIMPL;
        }

        hr = setInStream->SetInStream(inStream);
        if (hr != RC_S_OK)
        {
            return hr;
        }
    }

    {
        ICompressSetOutStreamSizePtr setOutStreamSize;
        m_codecInStream.QueryInterface(IID_ICompressSetOutStreamSize, setOutStreamSize.GetAddress());
        if (!setOutStreamSize)
        {
            return RC_E_NOTIMPL;
        }

        hr = setOutStreamSize->SetOutStreamSize(NULL);
        if (hr != RC_S_OK)
        {
            return hr;
        }
    }

    if (useFilter)
    {
        /*
        ICompressSetOutStreamSizePtr setOutStreamSize;
        m_filterInStream.QueryInterface(IID_ICompressSetOutStreamSize, &setOutStreamSize);
        if (!setOutStreamSize)
        return RC_E_NOTIMPL;
        hr = setOutStreamSize->SetOutStreamSize(NULL);
        if (hr != RC_S_OK)
        {
        return hr;
        }
        */
    }

    return RC_S_OK;
}

HResult RCNsisDecode::Read(void* data, size_t* processedSize)
{
    return RCStreamUtils::ReadStream(m_decoderInStream.Get(), data, processedSize);
}

END_NAMESPACE_RCZIP
