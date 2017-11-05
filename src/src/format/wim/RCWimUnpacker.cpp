/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "format/wim/RCWimUnpacker.h"
#include "format/common/RCOutStreamWithSha1.h"
#include "filesystem/RCLimitedSequentialInStream.h"
#include "filesystem/RCStreamUtils.h"
#include "common/RCCreateCoder.h"
#include "interface/RCMethodDefs.h"
#include "algorithm/CpuArch.h"
#include "RCWimInDefs.h"

#define Get32(p) GetUi32(p)
#define Get64(p) GetUi64(p)

/////////////////////////////////////////////////////////////////
//RCWimUnpacker class implementation

BEGIN_NAMESPACE_RCZIP

RCWimUnpacker::RCWimUnpacker(ICompressCodecsInfo* compressCodecsInfo):
    m_compressCodecsInfo(compressCodecsInfo)
{
}

RCWimUnpacker::~RCWimUnpacker()
{
}

HResult RCWimUnpacker::SetCompressCodecsInfo(ICompressCodecsInfo* compressCodecsInfo)
{
    m_compressCodecsInfo = compressCodecsInfo ;
    return RC_S_OK ;
}

HResult RCWimUnpacker::Unpack(IInStream* inStream, 
                              const RCWimResource& resource, 
                              bool lzxMode,
                              ISequentialOutStream* outStream, 
                              ICompressProgressInfo* progress)
{
    HResult hr = inStream->Seek(resource.m_offset, RC_STREAM_SEEK_SET, NULL) ;
    if(!IsSuccess(hr))
    {
        return hr ;
    }

    RCLimitedSequentialInStream* limitedStreamSpec = new RCLimitedSequentialInStream();
    ISequentialInStreamPtr limitedStream = limitedStreamSpec;
    limitedStreamSpec->SetStream(inStream);

    if (!m_copyCoder)
    {
        HResult res = RCCreateCoder::CreateCoder(m_compressCodecsInfo.Get(),
                                                 RCMethod::ID_COMPRESS_COPY,
                                                 m_copyCoder,
                                                 false);
        if(!IsSuccess(res) || !m_copyCoder)
        {
            return RC_S_FALSE ;
        }
    }
    if(!resource.IsCompressed())
    {
        if (resource.m_packSize != resource.m_unpackSize)
        {
            return RC_S_FALSE ;
        }
        limitedStreamSpec->Init(resource.m_packSize);
        return m_copyCoder->Code(limitedStreamSpec, outStream, NULL, NULL, progress);
    }
    if (resource.m_unpackSize == 0)
    {
        return RC_S_OK;
    }
    uint64_t numChunks = (resource.m_unpackSize + s_kWimChunkSize - 1) >> s_kWimChunkSizeBits;
    uint32_t entrySize = ((resource.m_unpackSize > (uint64_t)1 << 32) ? 8 : 4);
    uint64_t sizesBufSize64 = entrySize * (numChunks - 1);
    size_t sizesBufSize = (size_t)sizesBufSize64;
    if (sizesBufSize != sizesBufSize64)
    {
        return RC_E_OUTOFMEMORY ;
    }
    if (sizesBufSize > m_sizesBuf.GetCapacity())
    {
        m_sizesBuf.Free();
        m_sizesBuf.SetCapacity(sizesBufSize);
    }
    hr = RCStreamUtils::ReadStream_FALSE(inStream, (byte_t*)m_sizesBuf.data(), sizesBufSize) ;
    if(!IsSuccess(hr))
    {
        return hr ;
    }
    const byte_t* p = (const byte_t*)m_sizesBuf.data();

    if (lzxMode && !m_lzxDecoder)
    {
        HResult res = RCCreateCoder::CreateCoder(m_compressCodecsInfo.Get(),
                                                 RCMethod::ID_COMPRESS_LZX,
                                                 m_lzxDecoder,
                                                 false);
        if(!IsSuccess(res) || !m_lzxDecoder)
        {
            return RC_S_FALSE ;
        }
        ICompressSetCoderPropertiesPtr propset;
        RCPropertyIDPairArray proparray;
        res = m_lzxDecoder.QueryInterface(IID_ICompressSetCoderProperties, propset.GetAddress());
        if (!IsSuccess(res))
        {
            return res;
        }
        proparray.push_back(RCPropertyIDPairArray::value_type(RCMethodProperty::ID_SET_PARAMS, static_cast<uint64_t>(s_kWimChunkSizeBits)));
        proparray.push_back(RCPropertyIDPairArray::value_type(RCMethodProperty::ID_SET_LZX_WIM_MODE, true));
        res = propset->SetCoderProperties(proparray);
        if (!IsSuccess(res))
        {
            return res;
        }
    }

    uint64_t baseOffset = resource.m_offset + sizesBufSize64;
    uint64_t outProcessed = 0;
    for (uint32_t i = 0; i < (uint32_t)numChunks; ++i)
    {
        uint64_t offset = 0 ;
        if (i > 0)
        {
            offset = (entrySize == 4) ? Get32(p): Get64(p);
            p += entrySize;
        }
        uint64_t nextOffset = resource.m_packSize - sizesBufSize64;
        if (i + 1 < (uint32_t)numChunks)
        {
            nextOffset = (entrySize == 4) ? Get32(p): Get64(p);
        }
        if (nextOffset < offset)
        {
            return RC_S_FALSE;
        }

        hr = inStream->Seek(baseOffset + offset, RC_STREAM_SEEK_SET, NULL) ;
        if(!IsSuccess(hr))
        {
            return hr ;
        }
        uint64_t inSize = nextOffset - offset;
        limitedStreamSpec->Init(inSize) ;

        if (progress)
        {
            hr = progress->SetRatioInfo(offset, outProcessed) ;
            if(!IsSuccess(hr))
            {
                return hr ;
            }
        }

        uint32_t outSize = s_kWimChunkSize;
        if (outProcessed + outSize > resource.m_unpackSize)
        {
            outSize = (uint32_t)(resource.m_unpackSize - outProcessed);
        }
        uint64_t outSize64 = outSize;
        if (inSize == outSize)
        {
            hr = m_copyCoder->Code(limitedStreamSpec, outStream, NULL, &outSize64, NULL) ;
            if(!IsSuccess(hr))
            {
                return hr ;
            }
        }
        else
        {
            if (lzxMode)
            {
                ICompressSetCoderPropertiesPtr propset;
                RCPropertyIDPairArray proparray;
                HResult res = m_lzxDecoder.QueryInterface(IID_ICompressSetCoderProperties, propset.GetAddress());
                if (!IsSuccess(res))
                {
                    return res;
                }
                proparray.push_back(RCPropertyIDPairArray::value_type(RCMethodProperty::ID_SET_KEEP_HISTORY, false));
                res = propset->SetCoderProperties(proparray);
                if (!IsSuccess(res))
                {
                    return res;
                }
                hr = m_lzxDecoder->Code(limitedStreamSpec, outStream, NULL, &outSize64, NULL) ;
                if(!IsSuccess(hr))
                {
                    return hr ;
                }
            }
            else
            {
                hr = m_xpressDecoder.Code(limitedStreamSpec, outStream, outSize) ;
                if(!IsSuccess(hr))
                {
                    return hr ;
                }
            }
        }
        outProcessed += outSize;
    }
    return RC_S_OK ;
}

HResult RCWimUnpacker::Unpack( IInStream* inStream, 
                               const RCWimResource& resource,
                               bool lzxMode,
                               ISequentialOutStream* outStream, 
                               ICompressProgressInfo* progress,
                               byte_t* digest)
{
    RCOutStreamWithSha1* shaStreamSpec = new RCOutStreamWithSha1();
    ISequentialOutStreamPtr shaStream = shaStreamSpec;
    shaStreamSpec->SetStream(outStream) ;
    shaStreamSpec->Init(digest != NULL) ;
    HResult result = Unpack(inStream, resource, lzxMode, shaStream.Get(), progress);
    if (digest)
    {
        shaStreamSpec->Final(digest);
    }
    return result ;
}
END_NAMESPACE_RCZIP
