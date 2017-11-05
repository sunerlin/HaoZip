/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "format/zip/RCZipAddCommon.h"
#include "format/zip/RCZipHeader.h"
#include "common/RCCreateCoder.h"
#include "interface/RCMethodDefs.h"
#include "format/common/RCSequentialInStreamWithCRC.h"
#include "format/common/RCInStreamWithCRC.h"
#include "filesystem/RCStreamUtils.h"
#include "filesystem/RCSequentialOutStreamImp.h"
#include "version/RCProductVersion.h"
#include "crypto/Zip/RCZipCipher.h"
#include "algorithm/7zCrc.h"

/////////////////////////////////////////////////////////////////
//RCZipAddCommon class implementation

BEGIN_NAMESPACE_RCZIP

static const uint32_t kLzmaPropsSize = 5;

static const uint32_t kLzmaHeaderSize = 4 + kLzmaPropsSize;

class RCZipLzmaEncoder:
    public IUnknownImpl<ICompressCoder>
{
public:

    /** 压缩/解压数据
    @param [in] inStream 数据输入流
    @param [in] outStream 数据输出流
    @param [in] inSize 输入数据大小
    @param [in] outSize 输出数据大小 
    @param [in] progress 进度回调接口
    @return 成功返回RC_S_OK，否则返回错误号 
    */
    virtual HResult Code(ISequentialInStream* inStream,
                         ISequentialOutStream* outStream, 
                         const uint64_t* inSize, 
                         const uint64_t* outSize,
                         ICompressProgressInfo* progress) ;
    
    /** 设置压缩编码属性
    @param [in] propertyArray 压缩编码属性列表
    */
    virtual HResult SetCoderProperties(const RCPropertyIDPairArray& propertyArray) ;

public:

    /** 设置编码解码器管理接口指针
    @param [in] compressCodecsInfo 编码解码器指针
    @return 成功返回RC_S_OK，否则返回错误号 
    */
    HResult SetCompressCodecsInfo(ICompressCodecsInfo* compressCodecsInfo) ;

private:

    /** 编码解码器管理接口指针
    */
    ICompressCodecsInfoPtr m_compressCodecsInfo ;

    /** 编码解码器
    */
    ICompressCoderPtr m_compressEncoder ;

    /** 头
    */
    byte_t m_header[kLzmaHeaderSize];
};

HResult RCZipLzmaEncoder::SetCoderProperties(const RCPropertyIDPairArray& propertyArray)
{
    if (!m_compressEncoder)
    {
        HResult hr = RCCreateCoder::CreateCoder(m_compressCodecsInfo.Get(),
                                                RCMethod::ID_COMPRESS_LZMA,
                                                m_compressEncoder,
                                                true);
        if (!IsSuccess(hr))
        {
            return hr ;
        }

        if (!m_compressEncoder)
        {
            return RC_E_NOTIMPL;
        }
    }

    RCSequentialOutStreamImp *outStreamSpec = new RCSequentialOutStreamImp;
    ISequentialOutStreamPtr outStream(outStreamSpec);
    outStreamSpec->Init();

    ICompressSetCoderPropertiesPtr setCoderProperties;
    m_compressEncoder.QueryInterface(IID_ICompressSetCoderProperties, setCoderProperties.GetAddress());
    if (setCoderProperties)
    {
        HResult hr = setCoderProperties->SetCoderProperties(propertyArray);
        if (!IsSuccess(hr))
        {
            return hr ;
        }
    }
    
    ICompressWriteCoderPropertiesPtr writeCoderProperties;
    m_compressEncoder.QueryInterface(IID_ICompressWriteCoderProperties, writeCoderProperties.GetAddress());
    if (writeCoderProperties)
    {
        HResult hr = writeCoderProperties->WriteCoderProperties(outStream.Get());
        if (!IsSuccess(hr))
        {
            return hr ;
        }
    }

    if (outStreamSpec->GetSize() != kLzmaPropsSize)
    {
        return RC_E_FAIL;
    }

    m_header[0] = RC_VER_MAJOR;
    m_header[1] = RC_VER_MINOR;
    m_header[2] = kLzmaPropsSize;
    m_header[3] = 0;

    memcpy(m_header + 4, outStreamSpec->GetBuffer().data(), kLzmaPropsSize);

    return RC_S_OK;
}

HResult RCZipLzmaEncoder::Code(ISequentialInStream* inStream,
                               ISequentialOutStream* outStream, 
                               const uint64_t* inSize, 
                               const uint64_t* outSize,
                               ICompressProgressInfo* progress) 
{
    HResult hr = RCStreamUtils::WriteStream(outStream, m_header, kLzmaHeaderSize);
    if(!IsSuccess(hr))
    {
        return hr ;
    }

    return m_compressEncoder->Code(inStream, outStream, inSize, outSize, progress);
}

HResult RCZipLzmaEncoder::SetCompressCodecsInfo(ICompressCodecsInfo* compressCodecsInfo)
{
    if (!compressCodecsInfo)
    {
        return RC_S_FALSE;
    }

    m_compressCodecsInfo = compressCodecsInfo;

    return RC_S_OK;
}

RCZipAddCommon::RCZipAddCommon(const RCZipCompressionMethodMode& options):
    m_options(options),
    m_copyCoderSpec(NULL),
    m_cryptoStreamSpec(0),
    m_zipLzmaEncoder(NULL),
    m_shrinkEncoder(NULL),
    m_implodedEncoder(NULL),
    m_deflatedComEncoder(NULL),
    m_deflated64Encoder(NULL),
    m_bzip2Encoder(NULL)
{
}

static HResult GetStreamCRC(ISequentialInStream* inStream, uint32_t& resultCRC)
{
    uint32_t crc = CRC_INIT_VAL;
    const uint32_t kBufferSize = (1 << 14);
    byte_t buffer[kBufferSize];

    for (;;)
    {
        uint32_t realProcessedSize;
        HResult hr = inStream->Read(buffer, kBufferSize, &realProcessedSize);
        if (hr != RC_S_OK)
        {
            return hr;
        }

        if(realProcessedSize == 0)
        {
            resultCRC = CRC_GET_DIGEST(crc);
            return RC_S_OK;
        }

        crc = CrcUpdate(crc, buffer, (size_t)realProcessedSize);
    }
}

HResult RCZipAddCommon::Compress(ICompressCodecsInfo* codecsInfo,
                                 ISequentialInStream* inStream,
                                 IOutStream* outStream,
                                 ICompressProgressInfo* progress,
                                 RCZipCompressingResult& operationResult,
                                 RCZipExtFilterPtr& zipExtFilter,
                                 RCZipFileFilter& zipFileFilter,
                                 bool isFileFilter)
{
    RCSequentialInStreamWithCRC* inSecCrcStreamSpec = 0;
    RCInStreamWithCRC* inCrcStreamSpec = 0;
    ISequentialInStreamPtr inCrcStream;

    {
        IInStreamPtr inStream2;
        // we don't support stdin, since stream from stdin can require 64-bit size header
        HResult hr = inStream->QueryInterface(IID_IInStream, (void **)inStream2.GetAddress());
        if (hr != RC_S_OK)
        {
            return hr;
        }

        if (inStream2)
        {
            inCrcStreamSpec = new RCInStreamWithCRC;
            inCrcStream = inCrcStreamSpec;
            inCrcStreamSpec->SetStream(inStream2.Get());
            inCrcStreamSpec->Init();
        }
        else
        {
            inSecCrcStreamSpec = new RCSequentialInStreamWithCRC;
            inCrcStream = inSecCrcStreamSpec;
            inSecCrcStreamSpec->SetStream(inStream);
            inSecCrcStreamSpec->Init();
        }
    }

    int32_t numTestMethods = (int32_t)m_options.m_methodSequence.size();
    if (numTestMethods > 1 || m_options.m_passwordIsDefined)
    {
        if (inCrcStreamSpec == 0)
        {
            if (m_options.m_passwordIsDefined)
            {
                return RC_E_NOTIMPL;
            }
            numTestMethods = 1;
        }
    }

    byte_t method = 0;
    RCOutStreamReleaser outStreamReleaser;

    for (int32_t i = 0; i < numTestMethods; i++)
    {
        if (inCrcStreamSpec != 0)
        {
            HResult hr = inCrcStreamSpec->Seek(0, RC_STREAM_SEEK_SET, NULL);
            if (hr != RC_S_OK)
            {
                return hr;
            }
        }

        HResult hr = outStream->Seek(0, RC_STREAM_SEEK_SET, NULL);
        if (hr != RC_S_OK)
        {
            return hr;
        }

        if (m_options.m_passwordIsDefined)
        {
            if (!m_cryptoStream)
            {
                m_cryptoStreamSpec = new RCFilterCoder;
                m_cryptoStream     = m_cryptoStreamSpec;
            }

            if (m_options.m_isAesMode)
            {
                ICompressFilterPtr filter;
                hr = RCCreateCoder::CreateFilter(codecsInfo,
                                                 RCMethod::ID_CRYPTO_WZ_AES,
                                                 filter,
                                                 true);
                if (hr != RC_S_OK)
                {
                    return hr;
                }

                m_cryptoStreamSpec->SetFilter(filter.Get());
                m_aesFilter     = filter;
                m_filterAesSpec = dynamic_cast<RCWzAESEncoder*>(filter.Get());

                m_filterAesSpec->SetKeyMode(m_options.m_aesKeyMode);

                ICryptoSetPasswordPtr setPassword;
                hr = m_filterAesSpec->QueryInterface(IID_ICryptoSetPassword, (void **)setPassword.GetAddress());
                if (hr != RC_S_OK)
                {
                    return hr;
                }

                hr = setPassword->CryptoSetPassword((const byte_t*)m_options.m_password.c_str(),
                                                    (uint32_t)m_options.m_password.length());
                if (hr != RC_S_OK)
                {
                    return hr;
                }

                hr = m_filterAesSpec->WriteHeader(outStream);
                if (hr != RC_S_OK)
                {
                    return hr;
                }
            }
            else
            {
                ICompressFilterPtr filter;
                hr = RCCreateCoder::CreateFilter(codecsInfo,
                                                 RCMethod::ID_CRYPTO_ZIP,
                                                 filter,
                                                 true);
                if (hr != RC_S_OK)
                {
                    return hr;
                }

                m_cryptoStreamSpec->SetFilter(filter.Get());
                m_zipCryptoFilter = filter;
                m_filterSpec      = dynamic_cast<RCZipEncoder*>(filter.Get());

                ICryptoSetPasswordPtr setPassword;
                hr = m_zipCryptoFilter->QueryInterface(IID_ICryptoSetPassword, (void **)setPassword.GetAddress());
                if (hr != RC_S_OK)
                {
                    return hr;
                }

                hr = setPassword->CryptoSetPassword((const byte_t*)m_options.m_password.c_str(),
                                                    (uint32_t)m_options.m_password.length());
                if (hr != RC_S_OK)
                {
                    return hr;
                }

                uint32_t crc = 0;

                hr = GetStreamCRC(inStream, crc);
                if (hr != RC_S_OK)
                {
                    return hr;
                }

                hr = inCrcStreamSpec->Seek(0, RC_STREAM_SEEK_SET, NULL);
                if (hr != RC_S_OK)
                {
                    return hr;
                }

                hr = m_filterSpec->CryptoSetCRC(crc);
                if (hr != RC_S_OK)
                {
                    return hr;
                }

                hr = m_filterSpec->WriteHeader(outStream);
                if (hr != RC_S_OK)
                {
                    return hr;
                }
           }

           hr = (m_cryptoStreamSpec->SetOutStream(outStream));
           if (hr != RC_S_OK)
           {
               return hr;
           }

           outStreamReleaser.SetFilterCoder(m_cryptoStreamSpec);
        }

        RCZipCompressionMethodMode options = m_options;
        method = m_options.m_methodSequence[i];
        if (isFileFilter)
        {
            method = (byte_t)zipFileFilter.m_methodID;

            if (zipFileFilter.m_methodID == RCZipHeader::NCompressionMethod::kDeflated ||
                zipFileFilter.m_methodID == RCZipHeader::NCompressionMethod::kDeflated64)
            {
                options.m_numPasses = zipFileFilter.m_deflate.m_numPasses;
                options.m_numFastBytes = zipFileFilter.m_deflate.m_numFastBytes;
                options.m_algo = zipFileFilter.m_deflate.m_algo;
            }
            else if (zipFileFilter.m_methodID == RCZipHeader::NCompressionMethod::kBZip2)
            {
                options.m_numPasses = zipFileFilter.m_bzip2.m_numPasses;
                options.m_dicSize = zipFileFilter.m_bzip2.m_dicSize;
            }
            else if (zipFileFilter.m_methodID == RCZipHeader::NCompressionMethod::kLZMA)
            {
                options.m_dicSize = zipFileFilter.m_lzma.m_dicSize;
                options.m_numFastBytes = zipFileFilter.m_lzma.m_numFastBytes;
                options.m_matchFinder = zipFileFilter.m_lzma.m_matchFinder;
                options.m_algo = zipFileFilter.m_lzma.m_algo;
            }
            else if (zipFileFilter.m_methodID != RCZipHeader::NCompressionMethod::kStored)
            {
                return RC_E_FAIL;
            }
        }
        else if (zipExtFilter != 0)
        {
            method = (byte_t)zipExtFilter->m_methodID;

            if (zipExtFilter->m_methodID == RCZipHeader::NCompressionMethod::kDeflated ||
                zipExtFilter->m_methodID == RCZipHeader::NCompressionMethod::kDeflated64)
            {
                options.m_numPasses = zipExtFilter->m_deflate.m_numPasses;
                options.m_numFastBytes = zipExtFilter->m_deflate.m_numFastBytes;
                options.m_algo = zipExtFilter->m_deflate.m_algo;
            }
            else if (zipExtFilter->m_methodID == RCZipHeader::NCompressionMethod::kBZip2)
            {
                options.m_numPasses = zipExtFilter->m_bzip2.m_numPasses;
                options.m_dicSize = zipExtFilter->m_bzip2.m_dicSize;
            }
            else if (zipExtFilter->m_methodID == RCZipHeader::NCompressionMethod::kLZMA)
            {
                options.m_dicSize = zipExtFilter->m_lzma.m_dicSize;
                options.m_numFastBytes = zipExtFilter->m_lzma.m_numFastBytes;
                options.m_matchFinder = zipExtFilter->m_lzma.m_matchFinder;
                options.m_algo = zipExtFilter->m_lzma.m_algo;
            }
            else if (zipExtFilter->m_methodID != RCZipHeader::NCompressionMethod::kStored)
            {
                return RC_E_FAIL;
            }
        }

        ICompressCoderPtr* compressEncoder;

        switch(method)
        {
        case RCZipHeader::NCompressionMethod::kStored:
            {
                if(m_copyCoderSpec == NULL)
                {
                    ICompressCoderPtr copy;
                    hr = RCCreateCoder::CreateCoder(codecsInfo,
                                                    RCMethod::ID_COMPRESS_COPY,
                                                    copy,
                                                    true);
                    if (hr != RC_S_OK)
                    {
                        return hr;
                    }

                    m_copyCoderSpec = dynamic_cast<RCCopyCoder*>(copy.Get());
                    m_copyCoder     = copy;
                }

                ISequentialOutStreamPtr outStreamNew;
                if (m_options.m_passwordIsDefined)
                {
                    outStreamNew = m_cryptoStream;
                }
                else
                {
                    outStreamNew = outStream;
                }

                hr = m_copyCoder->Code(inCrcStream.Get(), outStreamNew.Get(), NULL, NULL, progress);
                if (hr != RC_S_OK)
                {
                    return hr;
                }

                operationResult.m_extractVersion = RCZipHeader::NCompressionMethod::kStoreExtractVersion;
                
                break;
            }

        default:
            {
                switch (method)
                {
                case RCZipHeader::NCompressionMethod::kLZMA:
                    {
                        compressEncoder = &m_zipLzmaEncoder;
                    }
                    break;

                case RCZipHeader::NCompressionMethod::kShrunk:
                    {
                        compressEncoder = &m_shrinkEncoder;
                    }
                    break;

                case RCZipHeader::NCompressionMethod::kImploded:
                    {
                        compressEncoder = &m_implodedEncoder;
                    }
                    break;

                case RCZipHeader::NCompressionMethod::kDeflated:
                    {
                        compressEncoder = &m_deflatedComEncoder;
                    }
                    break;

                case RCZipHeader::NCompressionMethod::kDeflated64:
                    {
                        compressEncoder = &m_deflated64Encoder;
                    }
                    break;

                case RCZipHeader::NCompressionMethod::kBZip2:
                    {
                        compressEncoder = &m_bzip2Encoder;
                    }
                    break;

                default:
                    {
                        return RC_S_FALSE;
                    }
                }

                if(!*compressEncoder)
                {         
                    if (method == RCZipHeader::NCompressionMethod::kLZMA)
                    {
                        RCZipLzmaEncoder* zipLzmaEncoder = new RCZipLzmaEncoder();
                        *compressEncoder = zipLzmaEncoder;

                        RCPropertyIDPairArray propertyArray; 

                        RCPropertyIDPair propertyIDs[] =
                        {
#ifdef COMPRESS_MT
                            RCPropertyIDPair(RCCoderPropID::kNumThreads,        (uint64_t)options.m_numThreads),
#endif
                            RCPropertyIDPair(RCCoderPropID::kAlgorithm,         (uint64_t)options.m_algo),
                            RCPropertyIDPair(RCCoderPropID::kDictionarySize,    (uint64_t)options.m_dicSize),
                            RCPropertyIDPair(RCCoderPropID::kNumFastBytes,      (uint64_t)options.m_numFastBytes),
                            RCPropertyIDPair(RCCoderPropID::kMatchFinder,       options.m_matchFinder),
                            RCPropertyIDPair(RCCoderPropID::kMatchFinderCycles, (uint64_t)options.m_numMatchFinderCycles)
                        };

                        for (uint32_t i = 0; i<sizeof(propertyIDs)/sizeof(propertyIDs[0]); i++)
                        {
                            propertyArray.push_back(propertyIDs[i]);
                        }

                        if (!options.m_numMatchFinderCyclesDefined)
                        {
                             propertyArray.pop_back();
                        }

                        hr = zipLzmaEncoder->SetCompressCodecsInfo(codecsInfo);
                        if (!IsSuccess(hr))
                        {
                            return hr ;
                        }

                        hr = zipLzmaEncoder->SetCoderProperties(propertyArray);
                        if (!IsSuccess(hr))
                        {
                            return hr ;
                        }
                    }
                    else
                    {

                        RCMethodID methodId;
                        switch(method)
                        {
                        case RCZipHeader::NCompressionMethod::kShrunk:
                            {
                                methodId = RCMethod::ID_COMPRESS_SHRINK;
                            }
                            break;

                        case RCZipHeader::NCompressionMethod::kImploded:
                            {
                                methodId = RCMethod::ID_COMPRESS_IMPLODE;
                            }
                            break;

                        case RCZipHeader::NCompressionMethod::kDeflated:
                            {
                                methodId = RCMethod::ID_COMPRESS_DEFLATE_COM;
                            }
                            break;

                        case RCZipHeader::NCompressionMethod::kDeflated64:
                            {
                                methodId = RCMethod::ID_COMPRESS_DEFLATE_COM_64;
                            }
                            break;

                        case RCZipHeader::NCompressionMethod::kBZip2:
                            {
                                methodId = RCMethod::ID_COMPRESS_BZIP2;
                            }
                            break;

                            /*
                            case RCZipHeader::NCompressionMethod::kPPMd:
                            {
                            methodId = RCMethod::ID_COMPRESS_PPMD;
                            }
                            break;
                            */

                            // 没找到对应的编码器
                        default:
                            {
                                return RC_S_FALSE;
                            }

                        }

                        hr = RCCreateCoder::CreateCoder(codecsInfo,
                                                        methodId,
                                                        *compressEncoder,
                                                        true);
                        if (hr != RC_S_OK)
                        {
                            return hr;
                        }

                        if (!*compressEncoder)
                        {
                            return RC_E_NOTIMPL;
                        }

                        if (method == RCZipHeader::NCompressionMethod::kDeflated ||
                            method == RCZipHeader::NCompressionMethod::kDeflated64)
                        {
                            RCPropertyIDPairArray propertyArray;

                            RCPropertyIDPair propertyIDs[] = 
                            {
                                RCPropertyIDPair(RCCoderPropID::kAlgorithm,         (uint64_t)options.m_algo),
                                RCPropertyIDPair(RCCoderPropID::kNumPasses,         (uint64_t)options.m_numPasses),
                                RCPropertyIDPair(RCCoderPropID::kNumFastBytes,      (uint64_t)options.m_numFastBytes),
                                RCPropertyIDPair(RCCoderPropID::kMatchFinderCycles, (uint64_t)options.m_numMatchFinderCycles)
                            };

                            for (uint32_t i = 0; i<sizeof(propertyIDs)/sizeof(propertyIDs[0]); i++)
                            {
                                propertyArray.push_back(propertyIDs[i]);
                            }

                            if (!options.m_numMatchFinderCyclesDefined)
                            {
                                propertyArray.pop_back();
                            }

                            ICompressSetCoderPropertiesPtr setCoderProperties;
                            (*compressEncoder).QueryInterface(IID_ICompressSetCoderProperties, setCoderProperties.GetAddress());
                            if (setCoderProperties)
                            {
                                hr = setCoderProperties->SetCoderProperties(propertyArray);
                                if (hr != RC_S_OK)
                                {
                                    return hr;
                                }
                            }
                        }
                        else if (method == RCZipHeader::NCompressionMethod::kBZip2)
                        {
                            RCPropertyIDPairArray propertyArray;

                            RCPropertyIDPair propertyIDs[] = 
                            {
                                RCPropertyIDPair(RCCoderPropID::kDictionarySize, (uint64_t)options.m_dicSize),
                                RCPropertyIDPair(RCCoderPropID::kNumPasses,      (uint64_t)options.m_numPasses),
#ifdef COMPRESS_MT
                                RCPropertyIDPair(RCCoderPropID::kNumThreads,     (uint64_t)options.m_numThreads)
#endif
                            };

                            for (uint32_t i = 0; i<sizeof(propertyIDs)/sizeof(propertyIDs[0]); i++)
                            {
                                propertyArray.push_back(propertyIDs[i]);
                            }

                            ICompressSetCoderPropertiesPtr setCoderProperties;
                            (*compressEncoder).QueryInterface(IID_ICompressSetCoderProperties, setCoderProperties.GetAddress());
                            if (setCoderProperties)
                            {
                                hr = setCoderProperties->SetCoderProperties(propertyArray);
                                if (hr != RC_S_OK)
                                {
                                    return hr;
                                }
                            }
                        }
                    }
                }

                ISequentialOutStreamPtr outStreamNew;
                if (m_options.m_passwordIsDefined)
                {
                    outStreamNew = m_cryptoStream;
                }
                else
                {
                    outStreamNew = outStream;
                }

                hr = (*compressEncoder)->Code(inCrcStream.Get(), outStreamNew.Get(), NULL, NULL, progress);
                if (hr != RC_S_OK)
                {
                    return hr;
                }

                operationResult.m_extractVersion = RCZipHeader::NCompressionMethod::kDeflateExtractVersion;
                
                break;
            }
        }

        hr = outStream->Seek(0, RC_STREAM_SEEK_CUR, &operationResult.m_packSize);
        if (hr != RC_S_OK)
        {
            return hr;
        }

        if (inCrcStreamSpec != 0)
        {
            operationResult.m_crc = inCrcStreamSpec->GetCRC();
            operationResult.m_unpackSize = inCrcStreamSpec->GetSize();
        }
        else
        {
            operationResult.m_crc = inSecCrcStreamSpec->GetCRC();
            operationResult.m_unpackSize = inSecCrcStreamSpec->GetSize();
        }

        if (m_options.m_passwordIsDefined)
        {
            if (operationResult.m_packSize < operationResult.m_unpackSize +
                (m_options.m_isAesMode ? m_filterAesSpec->GetHeaderSize() : RCZipCipher::s_headerSize))
            {
                break;
            }
        }
        else if (operationResult.m_packSize < operationResult.m_unpackSize)
        {
            break;
        }
    }

    if (m_options.m_isAesMode)
    {
        HResult hr = m_filterAesSpec->WriteFooter(outStream);
        if (hr != RC_S_OK)
        {
            return hr;
        }

        hr = outStream->Seek(0, RC_STREAM_SEEK_CUR, &operationResult.m_packSize);
        if (hr != RC_S_OK)
        {
            return hr;
        }
    }

    operationResult.m_method = method;

    return outStream->SetSize(operationResult.m_packSize);
}

END_NAMESPACE_RCZIP
