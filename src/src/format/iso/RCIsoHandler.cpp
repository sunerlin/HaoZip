/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#include "format/iso/RCIsoHandler.h"
#include "format/common/RCItemNameUtils.h"
#include "format/common/RCPropData.h"
#include "common/RCStringConvert.h"
#include "common/RCStringUtil.h"
#include "common/RCLocalProgress.h"
#include "common/RCCreateCoder.h"
#include "filesystem/RCLimitedSequentialInStream.h"
#include "filesystem/RCLimitedSequentialOutStream.h"
#include "interface/RCArchiveType.h"

BEGIN_NAMESPACE_RCZIP

static RCPropData s_isoProps[] =
{
    { RCString(_T("")), RCPropID::kpidPath, RCVariantType::RC_VT_STRING},
    { RCString(_T("")), RCPropID::kpidIsDir, RCVariantType::RC_VT_BOOL},
    { RCString(_T("")), RCPropID::kpidSize, RCVariantType::RC_VT_UINT64},
    { RCString(_T("")), RCPropID::kpidPackSize, RCVariantType::RC_VT_UINT64},
    { RCString(_T("")), RCPropID::kpidMTime, RCVariantType::RC_VT_UINT64}
};

static RCPropData s_isoArcProps[] =
{
    { RCString(_T("")), RCPropID::kpidArchiveType,    RCVariantType::RC_VT_UINT64 }
};

HResult RCIsoHandler::GetNumberOfProperties(uint32_t& numProperties) 
{
    numProperties = sizeof(s_isoProps) / sizeof(s_isoProps[0]);
    return RC_S_OK;
}

HResult RCIsoHandler::GetPropertyInfo(uint32_t index, 
                                      RCString& name, 
                                      RCPropertyID& propID, 
                                      RCVariantType::RCVariantID& varType)
{
    if (index >= (sizeof(s_isoProps) / sizeof(s_isoProps[0])))
    {
        return RC_E_INVALIDARG;
    }
    const RCPropData& srcItem = s_isoProps[index];
    propID = srcItem.m_propID;
    varType = srcItem.m_varType;
    name = srcItem.m_propName;
    return RC_S_OK;
}

HResult RCIsoHandler::GetNumberOfArchiveProperties(uint32_t& numProperties) 
{
    numProperties = sizeof(s_isoArcProps) / sizeof(s_isoArcProps[0]);
    return RC_S_OK;
}

HResult RCIsoHandler::GetArchivePropertyInfo(uint32_t index, RCString& name, 
                                             RCPropertyID& propID, 
                                             RCVariantType::RCVariantID& varType)
{
    if (index >= (sizeof(s_isoArcProps) / sizeof(s_isoArcProps[0])))
    {
        return RC_E_INVALIDARG;
    }
    const RCPropData& srcItem = s_isoArcProps[index];
    propID = srcItem.m_propID;
    varType = srcItem.m_varType;
    name = srcItem.m_propName;
    return RC_S_OK;
}

HResult RCIsoHandler::GetArchiveProperty(RCPropertyID propID, RCVariant& value) 
{
    ClearVariantValue(value) ;
    switch(propID)
    {
        case RCPropID::kpidArchiveType:
        {
            value = uint64_t(RC_ARCHIVE_ISO) ;
            break;
        }
    }
    return RC_S_OK;
}

HResult RCIsoHandler::Open(IInStream* stream, 
                           uint64_t maxCheckStartPosition, 
                           IArchiveOpenCallback* openArchiveCallback)
{
    Close();
    if(m_archive.Open(stream, openArchiveCallback) != RC_S_OK)
    {
        m_archive.Reset();
        return RC_S_FALSE;
    }
    m_archive.Reset();
    m_inStream = stream;
    return RC_S_OK;
}

HResult RCIsoHandler::Close()
{
    m_archive.Clear();
    m_inStream.Release();
    return RC_S_OK;
}

HResult RCIsoHandler::GetNumberOfItems(uint32_t& numItems)
{
    numItems = static_cast<uint32_t>(m_archive.m_refs.size() + m_archive.m_bootEntries.size());
    return RC_S_OK;
}

HResult RCIsoHandler::GetProperty(uint32_t index, RCPropertyID propID, RCVariant& value) 
{
    ClearVariantValue(value) ;
    RCVariant& prop = value ;
    if (index >= static_cast<uint32_t>(m_archive.m_refs.size()))
    {
        index -= static_cast<uint32_t>(m_archive.m_refs.size());
        const RCIsoBootInitialEntry &be = m_archive.m_bootEntries[index];
        switch(propID)
        {
            case RCPropID::kpidPath:
                {
                    RCString s = _T("[BOOT]") WSTRING_PATH_SEPARATOR;
                    s += be.GetName();
                    prop = s;
                    break;
                }
            case RCPropID::kpidIsDir:
                prop = false;
                break;
            case RCPropID::kpidSize:
            case RCPropID::kpidPackSize:
                prop = static_cast<uint64_t>(m_archive.GetBootItemSize(index));
                break;
        }
    }
    else
    {
        const RCIsoRef &ref = m_archive.m_refs[index];
        const RCIsoDir &item = ref.m_dir->m_subItems[ref.m_index];
        switch(propID)
        {
            case RCPropID::kpidPath:
                {
                    RCString s;
                    if (m_archive.IsJoliet())
                    {
                        s = item.GetPathU();
                    }
                    else
                    {
                        s = RCStringConvert::MultiByteToUnicodeString(item.GetPath(m_archive.m_isSusp, m_archive.m_suspSkipSize), RC_CP_OEMCP);
                    }
                    int32_t pos = (int32_t)s.rfind(_T(';'));
                    if (pos >= 0 && pos == (int32_t)s.size() - 2)
                    {
                        if (s[s.size() - 1] == _T('1'))
                        {
                            s = RCStringUtil::Left(s, pos);
                        }
                    }
                    if (!s.empty())
                    {
                        if (s[s.size() - 1] == _T('.'))
                        {
                            s = RCStringUtil::Left(s, static_cast<int32_t>(s.size() - 1));
                        }
                    }
                    prop = RCItemNameUtils::GetOSName2(s);
                }
                break;
            case RCPropID::kpidIsDir:
                prop = item.IsDir();
                break;
            case RCPropID::kpidSize:
            case RCPropID::kpidPackSize:
                if (!item.IsDir())
                {
                    prop = static_cast<uint64_t>(item.m_dataLength);
                }
                break;
            case RCPropID::kpidMTime:
                {
                    RC_FILE_TIME utcFileTime;
                    uint64_t ltime;
                    if (item.m_dateTime.GetFileTime(utcFileTime))
                    {
                        ltime = utcFileTime.u32HighDateTime;
                        ltime = ltime << 32;
                        ltime += utcFileTime.u32LowDateTime;
                        prop = ltime;
                    }
                    break;
                }
        }
    }
    return RC_S_OK;
}

HResult RCIsoHandler::Extract(const std::vector<uint32_t>& indices, 
                              int32_t testMode, 
                              IArchiveExtractCallback* extractCallback) 
{
    HResult result = RC_S_OK ;
    bool allFilesMode = indices.empty();
    uint32_t numItems = 0 ;
    if (allFilesMode)
    {
        numItems = static_cast<uint32_t>(m_archive.m_refs.size());
    }
    else
    {
        numItems = static_cast<uint32_t>(indices.size());
    }
    if(numItems == 0)
    {
        return RC_S_OK;
    }
    uint64_t totalSize = 0;
    uint32_t i;
    for(i = 0; i < numItems; i++)
    {
        uint32_t index = (allFilesMode ? i : indices[i]);
        if (index < static_cast<uint32_t>(m_archive.m_refs.size()))
        {
            const RCIsoRef &ref = m_archive.m_refs[index];
            const RCIsoDir &item = ref.m_dir->m_subItems[ref.m_index];
            totalSize += item.m_dataLength;
        }
        else
        {
            totalSize += m_archive.GetBootItemSize(index - static_cast<uint32_t>(m_archive.m_refs.size()));
        }
    }
    extractCallback->SetTotal(totalSize);
    uint64_t currentTotalSize = 0;
    uint64_t currentItemSize;

    //NCompress::CCopyCoder *copyCoderSpec = new NCompress::CCopyCoder();
    //CMyComPtr<ICompressCoder> copyCoder = copyCoderSpec;
    ICompressCoderPtr copyCoder;
    result = RCCreateCoder::CreateCoder(m_compressCodecsInfo.Get(),RCMethod::ID_COMPRESS_COPY, copyCoder, false);
    if (!IsSuccess(result))
    {
        return result;
    }

    RCLocalProgress* lps = new RCLocalProgress;
    ICompressProgressInfoPtr progress = lps;
    lps->Init(extractCallback, false);
    RCLimitedSequentialInStream *streamSpec = new RCLimitedSequentialInStream;
    ISequentialInStreamPtr inStream(streamSpec);
    streamSpec->SetStream(m_inStream.Get());
    
    RCLimitedSequentialOutStream* outStreamSpec = new RCLimitedSequentialOutStream ;
    ISequentialOutStreamPtr outStream(outStreamSpec);

    for (i = 0; i < numItems; i++, currentTotalSize += currentItemSize)
    {
        lps->SetInSize(currentTotalSize);
        lps->SetOutSize(currentTotalSize);
        result = lps->SetCur();
        if (!IsSuccess(result))
        {
            return result;
        }

        currentItemSize = 0;
        ISequentialOutStreamPtr realOutStream;
        int32_t askMode = testMode ? RC_ARCHIVE_EXTRACT_ASK_MODE_TEST : RC_ARCHIVE_EXTRACT_ASK_MODE_EXTRACT;
        uint32_t index = allFilesMode ? i : indices[i];

        result = extractCallback->GetStream(index, realOutStream.GetAddress(), askMode);
        if (!IsSuccess(result))
        {
            return result;
        }

        uint64_t blockIndex = 0 ;
        if (index < static_cast<uint32_t>(m_archive.m_refs.size()))
        {
            const RCIsoRef &ref = m_archive.m_refs[index];
            const RCIsoDir &item = ref.m_dir->m_subItems[ref.m_index];
            if(item.IsDir())
            {
                result = extractCallback->PrepareOperation(index,askMode);
                if (!IsSuccess(result))
                {
                    return result;
                }
                result = extractCallback->SetOperationResult(index,RC_ARCHIVE_EXTRACT_RESULT_OK);
                if (!IsSuccess(result))
                {
                    return result;
                }
                continue;
            }
            currentItemSize = item.m_dataLength;
            blockIndex = item.m_extentLocation;
        }
        else
        {
            int32_t bootIndex = index - static_cast<int32_t>(m_archive.m_refs.size());
            const RCIsoBootInitialEntry &be = m_archive.m_bootEntries[bootIndex];
            currentItemSize = m_archive.GetBootItemSize(bootIndex);
            blockIndex = be.m_loadRBA;
        }

        if (!testMode && !realOutStream)
        {
            continue;
        }
        result = extractCallback->PrepareOperation(index,askMode);
        if (!IsSuccess(result))
        {
            return result;
        }
        outStreamSpec->SetStream(realOutStream.Get()) ;
        realOutStream.Release() ;
        outStreamSpec->Init(currentItemSize) ;
    
        result = m_inStream->Seek(blockIndex * m_archive.m_blockSize, RC_STREAM_SEEK_SET, NULL);
        if (!IsSuccess(result))
        {
            return result;
        }
        streamSpec->Init(currentItemSize);
        result = copyCoder->Code(inStream.Get(), outStream.Get(), NULL, NULL, progress.Get());
        if (!IsSuccess(result))
        {
            return result;
        }
        outStreamSpec->ReleaseStream() ;
        result = extractCallback->SetOperationResult(index, outStreamSpec->IsFinishedOK() ?
                                                            RC_ARCHIVE_EXTRACT_RESULT_OK:
                                                            RC_ARCHIVE_EXTRACT_RESULT_DATA_ERROR);
        if (!IsSuccess(result))
        {
            return result;
        }
    }
    return RC_S_OK;
}

HResult RCIsoHandler::SetCompressCodecsInfo(ICompressCodecsInfo* compressCodecsInfo) 
{
    if (compressCodecsInfo)
    {
        m_compressCodecsInfo = compressCodecsInfo;
        return RC_S_OK;
    }
    return RC_S_OK;
}

END_NAMESPACE_RCZIP
