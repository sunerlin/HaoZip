/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#include "format/tar/RCTarHandler.h"
#include "format/tar/RCTarIn.h"
#include "filesystem/RCFileTime.h"
#include "common/RCLocalProgress.h"
#include "common/RCStringUtil.h"
#include "filesystem/RCLimitedSequentialInStream.h"
#include "filesystem/RCLimitedSequentialOutStream.h"
#include "format/common/RCPropData.h"
#include "interface/RCArchiveType.h"

BEGIN_NAMESPACE_RCZIP

static RCPropData s_kTarProps[] =
{
  { RCString(_T("")), RCPropID::kpidPath,     RCVariantType::RC_VT_STRING},
  { RCString(_T("")), RCPropID::kpidIsDir,    RCVariantType::RC_VT_BOOL},
  { RCString(_T("")), RCPropID::kpidSize,     RCVariantType::RC_VT_UINT64},
  { RCString(_T("")), RCPropID::kpidPackSize, RCVariantType::RC_VT_UINT64},
  { RCString(_T("")), RCPropID::kpidMTime,    RCVariantType::RC_VT_UINT64},
  { RCString(_T("")), RCPropID::kpidPosixAttrib, RCVariantType::RC_VT_UINT64},
  { RCString(_T("")), RCPropID::kpidUser,     RCVariantType::RC_VT_STRING},
  { RCString(_T("")), RCPropID::kpidGroup,    RCVariantType::RC_VT_STRING},
  { RCString(_T("")), RCPropID::kpidLink,     RCVariantType::RC_VT_STRING}
};

static RCPropData s_kTarArcProps[] =
{
    { RCString(_T("")), RCPropID::kpidArchiveType,    RCVariantType::RC_VT_UINT64 }
};

HResult RCTarHandler::GetNumberOfProperties(uint32_t& numProperties)
{ 
    numProperties = sizeof(s_kTarProps) / sizeof(s_kTarProps[0]);

    return RC_S_OK; 
}

HResult RCTarHandler::GetPropertyInfo(uint32_t index, RCString& name, RCPropertyID& propID, RCVariantType::RCVariantID& varType)
{
    if (index >= sizeof(s_kTarProps) / sizeof(s_kTarProps[0])) 
    {
        return RC_E_INVALIDARG;
    }
    const RCPropData &srcItem = s_kTarProps[index];

    propID  = srcItem.m_propID ; 
    varType = srcItem.m_varType ; 
    name    = srcItem.m_propName ;

    return RC_S_OK;
}

HResult RCTarHandler::GetNumberOfArchiveProperties(uint32_t& numProperties)
{
    numProperties = sizeof(s_kTarArcProps) / sizeof(s_kTarArcProps[0]);
    return RC_S_OK;
}

HResult RCTarHandler::GetArchivePropertyInfo(uint32_t index, RCString& name, RCPropertyID& propID, RCVariantType::RCVariantID& varType)
{
    if (index >= (sizeof(s_kTarArcProps) / sizeof(s_kTarArcProps[0])))
    {
        return RC_E_INVALIDARG;
    }
    const RCPropData& srcItem = s_kTarArcProps[index];
    propID = srcItem.m_propID;
    varType = srcItem.m_varType;
    name = srcItem.m_propName;
    return RC_S_OK;
}

HResult RCTarHandler::GetArchiveProperty(RCPropertyID propID, RCVariant& value)
{
    ClearVariantValue(value) ;
    switch(propID)
    {
        case RCPropID::kpidArchiveType:
        {
            value = uint64_t(RC_ARCHIVE_TAR) ;
            break;
        }
        case RCPropID::kpidPhySize:
        {
            if (m_phySizeDefined)
            {
                value = (uint64_t)m_phySize ;
            }
            break;
        }
    }
    return RC_S_OK;
}

HResult RCTarHandler::Open2(IInStream *stream, IArchiveOpenCallback *openArchiveCallback)
{
    uint64_t endPos = 0;
    {
        HResult rs = stream->Seek(0, RC_STREAM_SEEK_END, &endPos);
        if (rs != RC_S_OK)
        {
            return rs;
        }

        rs = stream->Seek(0, RC_STREAM_SEEK_SET, NULL);
        if (rs != RC_S_OK)
        {
            return rs;
        }
    }
    m_isGood = true ;
    uint64_t pos = 0 ;
    for (;;)
    {
        RCTarItemEx item;
        bool filled;
        item.m_headerPosition = pos;

        HResult rs = RCTarIn::ReadItem(stream, filled, item);
        if (rs != RC_S_OK)
        {
            return rs;
        }

        if (!filled)
        {
            break;
        }

        m_items.push_back(item);

        rs = stream->Seek(item.GetPackSize(), RC_STREAM_SEEK_CUR, &pos);
        if (rs != RC_S_OK)
        {
            return rs;
        }

        if (pos > endPos)
        {
            return RC_S_FALSE;
        }
        if (pos == endPos)
        {
            m_isGood = false ;
            break ;
        }

        if (openArchiveCallback != NULL)
        {
            if (m_items.size() == 1)
            {
                rs = openArchiveCallback->SetTotal(static_cast<uint64_t>(-1), endPos);
                if (rs != RC_S_OK)
                {
                    return rs;
                }
            }
            if (m_items.size() % 100 == 0)
            {
                uint64_t numFiles = m_items.size();
                rs = openArchiveCallback->SetCompleted(numFiles, pos);
                if (rs != RC_S_OK)
                {
                    return rs;
                }
            }
        }
    }
    if (!m_items.empty() && (openArchiveCallback != NULL))
    {
        uint64_t numFiles = m_items.size();
        HResult rs = openArchiveCallback->SetCompleted(numFiles, endPos);
        if (rs != RC_S_OK)
        {
            return rs;
        }
    }

    if (m_items.empty())
    {
        IArchiveOpenVolumeCallbackPtr openVolumeCallback;
        if (!openArchiveCallback)
        {
            return RC_S_FALSE;
        }
       
        openArchiveCallback->QueryInterface(IID_IArchiveOpenVolumeCallback, (void **)openVolumeCallback.GetAddress());
        if (!openVolumeCallback)
        {
            return RC_S_FALSE;
        }
       
        RCVariant prop;
        if (openVolumeCallback->GetProperty(RCPropID::kpidName, prop) != RC_S_OK)
        {
            return RC_S_FALSE;
        }

        if (!IsStringType(prop))
        {
            return RC_S_FALSE;
        }
        
        RCString baseName = GetStringValue(prop);

        if(baseName.size() > 4)
        {
            baseName = baseName.substr(baseName.size() - 4);
        }

        if (RCStringUtil::CompareNoCase(baseName, RCString(_T(".tar"))) != 0)
       {
            return RC_S_FALSE;
        }
    }
    return RC_S_OK;
}

HResult RCTarHandler::Open(IInStream* stream, 
                           uint64_t /*maxCheckStartPosition*/,
                           IArchiveOpenCallback* openArchiveCallback)
    
{
    Close();

    HResult rs = Open2(stream, openArchiveCallback);
    if (rs != RC_S_OK)
    {
        return rs;
    }

    m_stream = stream;

    return RC_S_OK;
}

HResult RCTarHandler::Close()
{
    m_phySizeDefined = false;
    m_curIndex = 0;
    m_latestIsRead = false;
    m_seqStream.Release();

    m_items.clear();
    m_stream.Release();

    return RC_S_OK;
}

HResult RCTarHandler::GetNumberOfItems(uint32_t& numItems)
{
    if(m_stream != NULL)
    {
        numItems = static_cast<uint32_t>(m_items.size());
    }
    else
    {
        numItems = 0 ;
    }
    return RC_S_OK;
}

HResult RCTarHandler::SkipTo(uint32_t index)
{
    while (m_curIndex < index || !m_latestIsRead)
    {
        if (m_latestIsRead)
        {
            uint64_t packSize = m_latestItem.GetPackSize();            
            HResult hr = m_spCopyCoder->Code(m_seqStream.Get(), NULL, &packSize, &packSize, NULL) ;
            if(!IsSuccess(hr))
            {
                return hr ;
            }
            m_latestIsRead = false;
            m_curIndex++;
        }
        else
        {
            bool filled = false ;
            HResult hr = RCTarIn::ReadItem(m_seqStream.Get(), filled, m_latestItem) ;
            if(!IsSuccess(hr))
            {
                return hr ;
            }
            if (!filled)
            {
                return RC_E_INVALIDARG ;
            }
            m_latestIsRead = true ;
        }
    }
    return RC_S_OK ;
}

static RCString TarStringToUnicode(const RCStringA& s)
{
    return RCStringConvert::MultiByteToUnicodeString(s, RC_CP_OEMCP);
}

HResult RCTarHandler::GetProperty(uint32_t index, RCPropertyID propID, RCVariant& value)
{
    ClearVariantValue(value) ;    
    RCVariant& prop = value;
    const RCTarItemEx* item = NULL ;
    if (m_stream)
    {
        if(index >= m_items.size())
        {
            return RC_E_INVALIDARG;
        }
        item = &m_items[index] ;
    }
    else
    {
        if (index < m_curIndex)
        {
            return RC_E_INVALIDARG;
        }
        else
        {
            HResult hr = SkipTo(index) ;
            if(!IsSuccess(hr))
            {
                return hr ;
            }
            item = &m_latestItem;
        }
    }
    switch(propID)
    {
    case RCPropID::kpidPath:  
        {
            prop = RCItemNameUtils::GetOSName2(TarStringToUnicode(item->m_name));
        }            
        break;

    case RCPropID::kpidIsDir:  
        {
            prop = item->IsDir();
        }
        break;

    case RCPropID::kpidSize:
        {
            prop = item->m_size; 
        }
        break;

    case RCPropID::kpidPackSize:
        {
            prop = item->GetPackSize();
        }
        break;

    case RCPropID::kpidMTime:
        if (item->m_mTime != 0)
        {
            RC_FILE_TIME ft ;
            RCFileTime::UnixTimeToFileTime(item->m_mTime, ft) ;
            prop = RCFileTime::ConvertFromFileTime(ft) ;
        }
        break;
        
    case RCPropID::kpidPosixAttrib:
        prop = (uint64_t)item->m_mode ;
        break;
        
    case RCPropID::kpidUser:
        {
            prop = TarStringToUnicode(item->m_user); 
        }
        break;

    case RCPropID::kpidGroup:
        {
            prop = TarStringToUnicode(item->m_group); 
        }
        break;
        
    case RCPropID::kpidLink:
        prop = TarStringToUnicode(item->m_linkName);
        break ;
    }

    return RC_S_OK;
}

 HResult RCTarHandler::Extract(const RCVector<uint32_t>& indices,
                               int32_t testMode,
                               IArchiveExtractCallback* extractCallback)

{
    ISequentialInStream* stream = m_seqStream.Get() ;
    bool seqMode = (m_stream == NULL) ;
    if (!seqMode)
    {
        stream = m_stream.Get() ;
    }
    uint32_t numItems = static_cast<uint32_t>(indices.size());
    bool allFilesMode = (numItems == uint32_t(0));

    if (allFilesMode)
    {
        numItems = static_cast<uint32_t>(m_items.size());
    }

    if ( (numItems == 0) && (m_stream != NULL) )
    {
        return RC_S_OK ;
    }

    uint64_t totalSize = 0;
    uint32_t i = 0 ;
    for (i = 0; i < numItems; i++)
    {
        totalSize += m_items[allFilesMode ? i : indices[i]].m_size;
    }
    extractCallback->SetTotal(totalSize);

    uint64_t totalPackSize = 0 ;
    totalSize = 0 ;
    totalPackSize = 0 ;

    RCLocalProgress* lps = new RCLocalProgress;
    ICompressProgressInfoPtr progress = lps;
    lps->Init(extractCallback, false);
     
    RCLimitedSequentialInStream* streamSpec = new RCLimitedSequentialInStream ;
    ISequentialInStreamPtr inStream(streamSpec) ;
    streamSpec->SetStream(stream) ;

    RCLimitedSequentialOutStream* outStreamSpec = new RCLimitedSequentialOutStream ;
    ISequentialOutStreamPtr outStream(outStreamSpec);

    for (i = 0; i < numItems || seqMode; i++)
    {
        lps->SetInSize(totalPackSize);
        lps->SetOutSize(totalSize);
         
        HResult rs = lps->SetCur();
        if (rs != RC_S_OK)
        {
            return rs;
        }

        ISequentialOutStreamPtr realOutStream ;         
        int32_t askMode = testMode ? RC_ARCHIVE_EXTRACT_ASK_MODE_TEST : RC_ARCHIVE_EXTRACT_ASK_MODE_EXTRACT;
        int32_t index = allFilesMode ? i : indices[i];

        const RCTarItemEx* item = NULL ;
        if (seqMode)
        {
            HResult res = SkipTo(index) ;
            if (res == RC_E_INVALIDARG)
            {
                break ;
            }
            if(!IsSuccess(res))
            {
                return res ;
            }
            item = &m_latestItem ;
        }
        else
        {
            if(index >= (int32_t)m_items.size())
            {
                return RC_E_INVALIDARG ;
            }
            item = &m_items[index] ;
        }
        
        lps->SetIndex(index) ;
        rs = extractCallback->GetStream(index, realOutStream.GetAddress(), askMode);
        if (rs != RC_S_OK)
        {
            return rs;
        }

        totalSize += item->m_size;
        totalPackSize += item->GetPackSize();
         
        if (item->IsDir())
        {
            rs = extractCallback->PrepareOperation(index,askMode);
            if (rs != RC_S_OK)
            {
                return rs;
            }
            rs = extractCallback->SetOperationResult(index,RC_ARCHIVE_EXTRACT_RESULT_OK);
            if (rs != RC_S_OK)
            {
                return rs;
            }
            continue;
        }
        bool skipMode = false ;
        if (!testMode && !realOutStream)
        {
            if (!seqMode)
            {
                continue;
            }
            skipMode = true;
            askMode = RC_ARCHIVE_EXTRACT_ASK_MODE_SKIP ;
        }

        rs = extractCallback->PrepareOperation(index, askMode);
        if (rs != RC_S_OK)
        {
            return rs;
        }

        outStreamSpec->SetStream(realOutStream.Get());
        realOutStream.Release();
        outStreamSpec->Init(skipMode ? 0 : item->m_size, true);

        if (!seqMode)
        {
            rs = m_stream->Seek(item->GetDataPosition(), RC_STREAM_SEEK_SET, NULL);
            if (rs != RC_S_OK)
            {
                return rs;
            }
        }

        streamSpec->Init(item->GetPackSize()) ;
        rs = m_spCopyCoder->Code(inStream.Get(), outStream.Get(), NULL, NULL, progress.Get());
        if (rs != RC_S_OK)
        {
            return rs;
        }
        if (seqMode)
        {
            m_latestIsRead = false;
            m_curIndex++;
        }
        outStreamSpec->ReleaseStream();
        rs = extractCallback->SetOperationResult( index,
                                                  (outStreamSpec->GetRem() == 0) ?
                                                  RC_ARCHIVE_EXTRACT_RESULT_OK:
                                                  RC_ARCHIVE_EXTRACT_RESULT_DATA_ERROR);

        if (rs != RC_S_OK)
        {
            return rs;
        }
    }
    return RC_S_OK;
 }

END_NAMESPACE_RCZIP