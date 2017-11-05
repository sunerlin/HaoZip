/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "format/lzh/RCLzhHandler.h"
#include "format/lzh/RCLzhIn.h"
#include "format/lzh/RCLzhOutStreamWithCRC.h"
#include "format/common/RCItemNameUtils.h"
#include "format/common/RCSequentialInStreamWithCRC.h"
#include "interface/RCMethodDefs.h"
#include "common/RCCreateCoder.h"
#include "common/RCLocalProgress.h"
#include "common/RCStringConvert.h"
#include "filesystem/RCFileTime.h"
#include "filesystem/RCLimitedSequentialInStream.h"
#include "compress/lzh/RCLzhHuffmanDecoder.h"
#include "format/common/RCPropData.h"
#include "interface/RCArchiveType.h"

/////////////////////////////////////////////////////////////////
//RCLzhHandler class implementation

BEGIN_NAMESPACE_RCZIP

struct RCLzhOsPair
{
    /** id
    */
    byte_t m_id;

    /** 名字
    */
    const char_t* m_name;
};

static RCLzhOsPair s_lzhOSPairs[] =
{
    {  0,  _T("MS-DOS") },
    { 'M', _T("MS-DOS") },
    { '2', _T("OS/2") },
    { '9', _T("OS9") },
    { 'K', _T("OS/68K") },
    { '3', _T("OS/386") },
    { 'H', _T("HUMAN") },
    { 'U', _T("UNIX") },
    { 'C', _T("CP/M") },
    { 'F', _T("FLEX") },
    { 'm', _T("Mac") },
    { 'R', _T("Runser") },
    { 'T', _T("TownsOS") },
    { 'X', _T("XOSK") },
    { 'w', _T("Windows95") },
    { 'W', _T("WindowsNT") },
    { 'J', _T("Java VM") }
};

static const char_t* s_lzhUnknownOS = L"Unknown";

static const char_t* GetOS(byte_t osId)
{
    for (int32_t i = 0; i < (sizeof(s_lzhOSPairs) / sizeof(s_lzhOSPairs[0])); i++)
    {
        if (s_lzhOSPairs[i].m_id == osId)
        {
            return s_lzhOSPairs[i].m_name;
        }
    }
    return s_lzhUnknownOS;
};

static RCPropData s_lzhProps[] =
{
    { RCString(_T("")), RCPropID::kpidPath, RCVariantType::RC_VT_STRING},
    { RCString(_T("")), RCPropID::kpidIsDir, RCVariantType::RC_VT_BOOL},
    { RCString(_T("")), RCPropID::kpidSize, RCVariantType::RC_VT_UINT64},
    { RCString(_T("")), RCPropID::kpidPackSize, RCVariantType::RC_VT_UINT64},
    { RCString(_T("")), RCPropID::kpidMTime, RCVariantType::RC_VT_UINT64},

    { RCString(_T("")), RCPropID::kpidCRC, RCVariantType::RC_VT_UINT64},

    { RCString(_T("")), RCPropID::kpidMethod, RCVariantType::RC_VT_STRING},
    { RCString(_T("")), RCPropID::kpidHostOS, RCVariantType::RC_VT_STRING}
};

static RCPropData s_kLZHArcProps[] =
{
    { RCString(_T("")), RCPropID::kpidArchiveType,    RCVariantType::RC_VT_UINT64 }
};

HResult RCLzhHandler::GetNumberOfProperties(uint32_t& numProperties)
{ 
    numProperties = sizeof(s_lzhProps) / sizeof(s_lzhProps[0]);

    return RC_S_OK; 
}

HResult RCLzhHandler::GetPropertyInfo(uint32_t index, RCString& name, RCPropertyID& propID, RCVariantType::RCVariantID& varType)
{
    if (index >= sizeof(s_lzhProps) / sizeof(s_lzhProps[0])) 
    {
        return RC_E_INVALIDARG;
    }
    const RCPropData& srcItem = s_lzhProps[index];

    propID  = srcItem.m_propID ; 
    varType = srcItem.m_varType ; 
    name    = srcItem.m_propName ;

    return RC_S_OK;
}

HResult RCLzhHandler::GetNumberOfArchiveProperties(uint32_t& numProperties)
{
    numProperties = sizeof(s_kLZHArcProps) / sizeof(s_kLZHArcProps[0]);
    return RC_S_OK;
}

HResult RCLzhHandler::GetArchivePropertyInfo(uint32_t index, RCString& name, RCPropertyID& propID, RCVariantType::RCVariantID& varType)
{
    if (index >= (sizeof(s_kLZHArcProps) / sizeof(s_kLZHArcProps[0])))
    {
        return RC_E_INVALIDARG;
    }
    const RCPropData& srcItem = s_kLZHArcProps[index];
    propID = srcItem.m_propID;
    varType = srcItem.m_varType;
    name = srcItem.m_propName;
    return RC_S_OK;
}

HResult RCLzhHandler::GetArchiveProperty(RCPropertyID propID, RCVariant& value)
{
    ClearVariantValue(value) ;
    switch(propID)
    {
        case RCPropID::kpidArchiveType:
        {
            value = uint64_t(RC_ARCHIVE_LZH) ;
            break;
        }
    }
    return RC_S_OK;
}

HResult RCLzhHandler::GetNumberOfItems(uint32_t& numItems)
{
    numItems = (uint32_t)m_lzhItems.size();

    return RC_S_OK;
}

HResult RCLzhHandler::GetProperty(uint32_t index, RCPropertyID propID, RCVariant& value)
{
    ClearVariantValue(value) ;
    if(index >= m_lzhItems.size())
    {
        return RC_E_INVALIDARG ;
    }
    RCVariant& prop = value;
    const RCLzhItemEx& item = m_lzhItems[index];
    switch(propID)
    {
    case RCPropID::kpidPath:
        {
            RCString s = RCItemNameUtils::WinNameToOSName(RCStringConvert::MultiByteToUnicodeString(item.GetName(), RC_CP_OEMCP));
            if (!s.empty())
            {
                if (s[s.length() - 1] == WCHAR_PATH_SEPARATOR)
                {
                    s.erase(s.length() - 1);
                }
                prop = s;
            }
        }
        break;

    case RCPropID::kpidIsDir:
        {
            prop = item.IsDir();
        }
        break;

    case RCPropID::kpidSize:
        {
            prop = (uint64_t)item.m_size;
        }
        break;

    case RCPropID::kpidPackSize:
        {
            prop = (uint64_t)item.m_packSize;
        }
        break;

    case RCPropID::kpidCRC:
        {
            prop = (uint64_t)item.m_crc;
        }
        break;

    case RCPropID::kpidHostOS:
        {
            prop = RCString(GetOS(item.m_osId));
        }
        break;

    case RCPropID::kpidMTime:
        {
            RC_FILE_TIME utc ;
            uint32_t unixTime = 0 ;
            if (item.GetUnixTime(unixTime))
            {
                RCFileTime::UnixTimeToFileTime(unixTime, utc);
            }
            else
            {
                RC_FILE_TIME localFileTime;
                if (RCFileTime::DosTimeToFileTime(item.m_modifiedTime, localFileTime))
                {
                    if (!RCFileTime::LocalFileTimeToFileTime(localFileTime, utc))
                    {
                        utc.u32HighDateTime = 0;
                        utc.u32LowDateTime  = 0;
                    }
                }
                else
                {
                    utc.u32HighDateTime = 0;
                    utc.u32LowDateTime  = 0;
                }
            }            
            prop = RCFileTime::ConvertFromFileTime(utc) ;
        }
        break;

    case RCPropID::kpidMethod:
        {
            RCString methodName ;
            for(size_t index = 0; index < RCLzhHeader::s_methodIdSize; ++index)
            {
                RCString::value_type ch = item.m_method[index] ;
                methodName += ch ;
            }
            prop = methodName ;
        }
        break;
    }

    return RC_S_OK;
}

HResult RCLzhHandler::Open(IInStream* stream,
                           uint64_t maxCheckStartPosition,
                           IArchiveOpenCallback* openArchiveCallback)
{
    m_lzhItems.clear();
    
    HResult hr;
    uint64_t endPos = 0;
    bool needSetTotal = true;

    if (openArchiveCallback != NULL)
    {
        hr = stream->Seek(0, RC_STREAM_SEEK_END, &endPos);
        if (hr != RC_S_OK)
        {
            return hr;
        }

        hr = stream->Seek(0, RC_STREAM_SEEK_SET, NULL);
        if (hr != RC_S_OK)
        {
            return hr;
        }
    }
    RCLzhIn inArchive ;
    hr = inArchive.Open(stream);
    if (hr != RC_S_OK)
    {
        return hr;
    }

    for (;;)
    {
        RCLzhItemEx item;
        bool filled = false ;
        hr = inArchive.GetNextItem(filled, item) ;
        if(hr == RC_E_UnexpectedEndOfArchive)
        {
            //已经读取到文件尾部, 特殊处理
            if(!m_lzhItems.empty())
            {
                hr = RC_S_OK ;
            }
        }
        if (hr != RC_S_OK)
        {
            return RC_S_FALSE;
        }

        if (!filled)
        {
            break;
        }

        m_lzhItems.push_back(item);
        inArchive.Skip(item.m_packSize);

        if (openArchiveCallback != NULL)
        {
            if (needSetTotal)
            {
                hr = openArchiveCallback->SetTotal(0, endPos);
                if (hr != RC_S_OK)
                {
                    return hr;
                }
                needSetTotal = false;
            }

            if (m_lzhItems.size() % 100 == 0)
            {
                uint64_t numFiles = m_lzhItems.size();
                uint64_t numBytes = item.m_dataPosition;
                hr = openArchiveCallback->SetCompleted(numFiles, numBytes);
                if (hr != RC_S_OK)
                {
                    return hr;
                }
            }
        }
    }

    if (m_lzhItems.empty())
    {
        return RC_S_FALSE;
    }

    m_stream = stream;

    return RC_S_OK;
}

HResult RCLzhHandler::Close()
{
    m_lzhItems.clear();
    m_stream.Release();

    return RC_S_OK;
}

HResult RCLzhHandler::Extract(const std::vector<uint32_t>& indices,
                              int32_t testMode,
                              IArchiveExtractCallback* extractCallback)
{
    uint32_t numItems = (uint32_t)indices.size();
    bool btestMode = (testMode != 0);
    uint64_t totalUnPacked = 0;
    uint64_t totalPacked  = 0;

    bool allFilesMode = (numItems == uint32_t(0));
    if (allFilesMode)
    {
        numItems = (uint32_t)m_lzhItems.size();
    }

    if (numItems == 0)
    {
        return RC_S_OK;
    }

    HResult hr;
    uint32_t i;
    for (i = 0; i < numItems; i++)
    {
        const RCLzhItemEx& item = m_lzhItems[allFilesMode ? i : indices[i]];
        totalUnPacked += item.m_size;
        totalPacked   += item.m_packSize;
    }
    extractCallback->SetTotal(totalUnPacked);

    uint64_t currentTotalUnPacked = 0;
    uint64_t currentTotalPacked   = 0;
    uint64_t currentItemUnPacked;
    uint64_t currentItemPacked;

    RCLzhHuffmanDecoder* lzhDecoderSpec = 0;
    ICompressCoderPtr lzhDecoder;
    ICompressCoderPtr lzh1Decoder;
     
    ICompressCoderPtr copyCoder;

    hr = RCCreateCoder::CreateCoder(m_compressCodecsInfo.Get(),
                                    RCMethod::ID_COMPRESS_COPY,
                                    copyCoder,
                                    false);
    if (hr != RC_S_OK)
    {
        return hr;
    }

    ICompressGetInStreamProcessedSizePtr copyCoderSize;
    hr = copyCoder.QueryInterface(ICompressGetInStreamProcessedSize::IID, copyCoderSize.GetAddress());
    if (hr != RC_S_OK)
    {
        return hr;
    }

    RCLocalProgress* lps = new RCLocalProgress;
    ICompressProgressInfoPtr progress = lps;
    lps->Init(extractCallback, false);

    RCLimitedSequentialInStream* streamSpec = new RCLimitedSequentialInStream;
    ISequentialInStreamPtr inStream(streamSpec);
    streamSpec->SetStream(m_stream.Get());

    for (i = 0; i < numItems; i++, currentTotalUnPacked += currentItemUnPacked,
        currentTotalPacked += currentItemPacked)
    {
        currentItemUnPacked = 0;
        currentItemPacked = 0;

        lps->SetInSize(currentTotalPacked);
        lps->SetOutSize(currentTotalUnPacked);

        hr = lps->SetCur();
        if (hr != RC_S_OK)
        {
            return hr;
        }

        ISequentialOutStreamPtr realOutStream;

        int32_t askMode;
        askMode = btestMode ? 
            RC_ARCHIVE_EXTRACT_ASK_MODE_TEST :
        RC_ARCHIVE_EXTRACT_ASK_MODE_EXTRACT;

        int32_t index = allFilesMode ? i : indices[i];

        const RCLzhItemEx& item = m_lzhItems[index];
        hr = extractCallback->GetStream(index, realOutStream.GetAddress(), askMode);
        if (hr != RC_S_OK)
        {
            return hr;
        }

        if (item.IsDir())
        {
            // if (!btestMode)
            {
                hr = extractCallback->PrepareOperation(index,askMode);
                if (hr != RC_S_OK)
                {
                    return hr;
                }

                hr = extractCallback->SetOperationResult(index,RC_ARCHIVE_EXTRACT_RESULT_OK);
                if (hr != RC_S_OK)
                {
                    return hr;
                }
            }
            continue;
        }

        if (!btestMode && (!realOutStream))
        {
            continue;
        }

        hr = extractCallback->PrepareOperation(index,askMode);
        if (hr != RC_S_OK)
        {
            return hr;
        }

        currentItemUnPacked = item.m_size;
        currentItemPacked = item.m_packSize;

        {
            RCLzhOutStreamWithCRC* outStreamSpec = new RCLzhOutStreamWithCRC;
            ISequentialOutStreamPtr outStream(outStreamSpec);
            outStreamSpec->Init(realOutStream.Get());
            realOutStream.Release();

            uint64_t pos;
            m_stream->Seek(item.m_dataPosition, RC_STREAM_SEEK_SET, &pos);

            streamSpec->Init(item.m_packSize);

            HResult result = RC_S_OK;
            int32_t opRes = RC_ARCHIVE_EXTRACT_RESULT_OK;

            if (item.IsCopyMethod())
            {
                result = copyCoder->Code(inStream.Get(), outStream.Get(), NULL, NULL, progress.Get());
                
                uint64_t u64size;
                copyCoderSize.Get()->GetInStreamProcessedSize(u64size);

                if (result == RC_S_OK && u64size != item.m_packSize)
                {
                    result = RC_S_FALSE;
                }
            }
            else if (item.IsLh4GroupMethod())
            {
                if (!lzhDecoder)
                {
                    hr = RCCreateCoder::CreateCoder(m_compressCodecsInfo.Get(),
                                                    RCMethod::ID_COMPRESS_LZH,
                                                    lzhDecoder,
                                                    false);
                    if (hr != RC_S_OK)
                    {
                        return hr;
                    }
                    lzhDecoderSpec = dynamic_cast<RCLzhHuffmanDecoder*>(lzhDecoder.Get());
                }

                //SetDictionary
                RCPropertyIDPairArray proparray;
                ICompressSetCoderPropertiesPtr propset;

                HResult hr = lzhDecoderSpec->QueryInterface(IID_ICompressSetCoderProperties, (void**)propset.GetAddress());
                if (hr != RC_S_OK)
                {
                    return hr;
                }

                proparray.push_back(RCPropertyIDPairArray::value_type(RCMethodProperty::ID_SET_DICTIONARY_SIZE, (uint64_t)item.GetNumDictBits()));

                hr = propset->SetCoderProperties(proparray);
                if (hr != RC_S_OK)
                {
                    return hr;
                }

                result = lzhDecoder->Code(inStream.Get(), outStream.Get(), NULL, &currentItemUnPacked, progress.Get());
            }
            else
            {
                opRes = RC_ARCHIVE_EXTRACT_RESULT_UNSUPPORTED_METHOD;
            }

            if (opRes == RC_ARCHIVE_EXTRACT_RESULT_OK)
            {
                if (result == RC_S_FALSE)
                {
                    opRes = RC_ARCHIVE_EXTRACT_RESULT_DATA_ERROR;
                }
                else
                {
                    if (result != RC_S_OK)
                    {
                        return result;
                    }

                    if (outStreamSpec->GetCRC() != item.m_crc)
                    {
                        opRes = RC_ARCHIVE_EXTRACT_RESULT_CRC_ERROR;
                    }
                }
            }
            outStream.Release();

            hr = extractCallback->SetOperationResult(index,opRes);
            if (hr != RC_S_OK)
            {
                return hr;
            }
        }
    }
    return RC_S_OK;
}

END_NAMESPACE_RCZIP
