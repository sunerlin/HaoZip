/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "format/bzip2/RCBZip2Handler.h"
#include "common/RCCreateCoder.h"
#include "interface/RCMethodDefs.h"
#include "format/bzip2/RCBZip2ConstDefs.h"
#include "format/bzip2/RCBZip2Update.h"

/////////////////////////////////////////////////////////////////
//RCBZip2Handler class implementation

BEGIN_NAMESPACE_RCZIP

HResult RCBZip2Handler::GetFileTimeType(uint32_t& type)
{
    type = RC_ARCHIVE_FILE_TIME_TYPE_UNIX;
    return RC_S_OK;
}

static HResult CopyStreams(ICompressCodecsInfo* codecsInfo,
                           ISequentialInStream* inStream,
                           ISequentialOutStream* outStream)
{
    ICompressCoderPtr copyCoder;
    HResult hr = RCCreateCoder::CreateCoder(codecsInfo,
                                            RCMethod::ID_COMPRESS_COPY,
                                            copyCoder,
                                            true);
    if (hr != RC_S_OK)
    {
        return hr;
    }

    return copyCoder->Code(inStream, outStream, NULL, NULL, NULL);
}

HResult RCBZip2Handler::UpdateItems(ISequentialOutStream* outStream, 
                                    uint32_t numItems,
                                    IArchiveUpdateCallback* updateCallback)
{
    if (numItems != 1)
    {
        return RC_E_INVALIDARG;
    }

    int32_t newData;
    int32_t newProps;
    uint32_t indexInArchive;
    if (!updateCallback)
    {
        return RC_E_FAIL;
    }

    HResult hr = updateCallback->GetUpdateItemInfo(0, newData, newProps, indexInArchive);
    if (hr != RC_S_OK)
    {
        return hr;
    }

    if (IntToBool(newProps))
    {
        RCVariant prop;
        hr = updateCallback->GetProperty(0, RCPropID::kpidIsDir, prop);

        if (IsBooleanType(prop))
        {
            if (GetBooleanValue(prop) != false)
            {
                return RC_E_INVALIDARG;
            }
        }
        else if (!IsEmptyType(prop))
        {
            return RC_E_INVALIDARG;
        }
    }

    if (IntToBool(newData))
    {
        uint64_t size;
        {
            RCVariant prop;
            hr = updateCallback->GetProperty(0, RCPropID::kpidSize, prop);
            if (hr != RC_S_OK)
            {
                return hr;
            }

            if (!IsUInt64Type(prop))
            {
                return RC_E_INVALIDARG;
            }

            size = GetUInt64Value(prop);;
        }

        uint32_t dicSize = m_dicSize;
        if (dicSize == 0xFFFFFFFF)
        {
            dicSize = (m_level >= 5 ? RCBZip2ConstDefs::s_bzip2DicSizeX5 :
            (m_level >= 3 ? RCBZip2ConstDefs::s_bzip2DicSizeX3 :
            RCBZip2ConstDefs::s_bzip2DicSizeX1));
        }

        uint32_t numPasses = m_numPasses;
        if (numPasses == 0xFFFFFFFF)
        {
            numPasses = (m_level >= 9 ? RCBZip2ConstDefs::s_bzip2NumPassesX9 :
            (m_level >= 7 ? RCBZip2ConstDefs::s_bzip2NumPassesX7 :
            RCBZip2ConstDefs::s_bzip2NumPassesX1));
        }

        return RCBZip2Update::UpdateArchive(m_compressCodecsInfo.Get(),
                                            size,
                                            outStream,
                                            0,
                                            dicSize,
                                            numPasses,
                                            m_numThreads,
                                            updateCallback);
    }

    if (indexInArchive != 0)
    {
        return RC_E_INVALIDARG;
    }
    if(m_stream != NULL)
    {
        hr = m_stream->Seek(m_startPosition, RC_STREAM_SEEK_SET, NULL);
        if (hr != RC_S_OK)
        {
            return hr;
        }
    }
    return CopyStreams(m_compressCodecsInfo.Get(), m_stream.Get(), outStream);
}

END_NAMESPACE_RCZIP
