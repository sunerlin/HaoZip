/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "format/gzip/RCGZipHandler.h"
#include "format/gzip/RCGZipDefs.h"
#include "format/gzip/RCGZipUpdate.h"
#include "filesystem/RCFileTime.h"
#include "common/RCStringConvert.h"
#include "common/RCCreateCoder.h"
#include "common/RCStringUtil.h"
#include "interface/RCMethodDefs.h"
#include "format/gzip/RCGZipItem.h"
#include "filesystem/RCFileName.h"

/////////////////////////////////////////////////////////////////
//RCGZipHandler class implementation

BEGIN_NAMESPACE_RCZIP

HResult RCGZipHandler::GetFileTimeType(uint32_t& type)
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

HResult RCGZipHandler::UpdateItems(ISequentialOutStream* outStream, 
                                   uint32_t numItems,
                                   IArchiveUpdateCallback* updateCallback)
{
    if (numItems != 1)
    {
        return RC_E_INVALIDARG;
    }

    int32_t newData = 0 ;
    int32_t newProps = 0 ;
    uint32_t indexInArchive = 0 ;
    if (!updateCallback)
    {
        return RC_E_FAIL;
    }
    HResult hr = updateCallback->GetUpdateItemInfo(0, newData, newProps, indexInArchive) ;
    if(!IsSuccess(hr))
    {
        return hr ;
    }

    RCGZipItem newItem = m_item;
    newItem.m_extraFlags = 0;
    newItem.m_flags = 0 ;
    if (IntToBool(newProps))
    {
        {
            RC_FILE_TIME utcTime ;
            RCVariant prop ;
            hr = updateCallback->GetProperty(0, RCPropID::kpidMTime, prop) ;
            if (hr != RC_S_OK)
            {
                return hr;
            }

            if (!IsUInt64Type(prop))
            {
                return RC_E_INVALIDARG;
            }
               
            RCFileTime::ConvertToFileTime(GetUInt64Value(prop), utcTime);
            if (!RCFileTime::FileTimeToUnixTime(utcTime, newItem.m_time))
            {
                return RC_E_INVALIDARG ;
            }
        }
        {
            RCVariant prop ;
            hr = updateCallback->GetProperty(0, RCPropID::kpidPath, prop) ;
            if (hr != RC_S_OK)
            {
                return hr;
            }
            if (IsStringType(prop))
            {
                RCString name = GetStringValue(prop) ;
                RCString::size_type dirDelimiterPos = name.rfind(RCFileName::GetDirDelimiter()) ;
                if (dirDelimiterPos != RCString::npos)
                {
                    name = RCStringUtil::Mid(name, (int32_t)dirDelimiterPos + 1) ;
                }
                newItem.m_name = RCStringConvert::UnicodeStringToMultiByte(name, RC_CP_ACP);
                if (!newItem.m_name.empty())
                {
                    newItem.m_flags |= RCGZipHeader::NFlags::kName ;
                }
            }
            else if (!IsEmptyType(prop))
            {
                return RC_E_INVALIDARG ;
            }
        }
        {
            RCVariant prop;
            hr = updateCallback->GetProperty(0, RCPropID::kpidIsDir, prop);
            if (hr != RC_S_OK)
            {
                return hr;
            }

            if (IsBooleanType(prop))
            {
                if (GetBooleanValue(prop))
                {
                    return RC_E_INVALIDARG;
                }
            }
            else if (!IsEmptyType(prop))
            {
                return RC_E_INVALIDARG;
            }
        }
    }

    if (IntToBool(newData))
    {
        uint64_t size = 0 ;
        {
            RCVariant prop ;
            hr = updateCallback->GetProperty(0, RCPropID::kpidSize, prop) ;
            if(!IsUInt64Type(prop))
            {
                return RC_E_INVALIDARG ;
            }
            size = prop.GetUInt64Value() ;
        }

        return RCGZipUpdate::UpdateArchive(m_compressCodecsInfo.Get(), outStream, size, newItem, m_method, updateCallback);
    }

    if (indexInArchive != 0)
    {
        return RC_E_INVALIDARG;
    }

    if (!m_inStream)
    {
        return RC_E_NOTIMPL ;
    }

    uint64_t offset = m_startPosition;
    if (IntToBool(newProps))
    {
        newItem.WriteHeader(outStream);
        offset += m_headerSize;
    }
    hr = m_inStream->Seek(offset, RC_STREAM_SEEK_SET, NULL) ;
    if(!IsSuccess(hr))
    {
        return hr ;
    }
    return CopyStreams(m_compressCodecsInfo.Get(), m_inStream.Get(), outStream) ;
}

END_NAMESPACE_RCZIP
