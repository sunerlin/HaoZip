/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#include "format/deb/RCDebHandler.h"
#include "format/common/RCPropData.h"
#include "format/common/RCItemNameUtils.h"
#include "common/RCStringConvert.h"
#include "common/RCLocalProgress.h"
#include "common/RCCreateCoder.h"
#include "filesystem/RCFileTime.h"
#include "filesystem/RCLimitedSequentialInStream.h"
#include "interface/RCArchiveType.h"

BEGIN_NAMESPACE_RCZIP

static RCPropData s_debProps[] =
{
    { RCString(_T("")), RCPropID::kpidPath, RCVariantType::RC_VT_STRING},
    { RCString(_T("")), RCPropID::kpidSize, RCVariantType::RC_VT_UINT64},
    { RCString(_T("")), RCPropID::kpidMTime, RCVariantType::RC_VT_UINT64}
};

static RCPropData s_debArcProps[] =
{
    { RCString(_T("")), RCPropID::kpidArchiveType,    RCVariantType::RC_VT_UINT64 }
};

HResult RCDebHandler::GetNumberOfProperties(uint32_t& numProperties) 
{
    numProperties = sizeof(s_debProps) / sizeof(s_debProps[0]);
    return RC_S_OK;
}

HResult RCDebHandler::GetPropertyInfo(uint32_t index, RCString& name, RCPropertyID& propID, RCVariantType::RCVariantID& varType)
{
    if (index >= (sizeof(s_debProps) / sizeof(s_debProps[0])))
    {
        return RC_E_INVALIDARG;
    }
    const RCPropData& srcItem = s_debProps[index];
    propID = srcItem.m_propID;
    varType = srcItem.m_varType;
    name = srcItem.m_propName;
    return RC_S_OK;
}

HResult RCDebHandler::GetArchiveProperty(RCPropertyID propID, RCVariant& value) 
{
    ClearVariantValue(value) ;
    switch(propID)
    {
        case RCPropID::kpidArchiveType:
        {
            value = uint64_t(RC_ARCHIVE_DEB) ;
            break;
        }
    }
    return RC_S_OK;
}

HResult RCDebHandler::GetNumberOfArchiveProperties(uint32_t& numProperties) 
{
    numProperties = sizeof(s_debArcProps) / sizeof(s_debArcProps[0]);
    return RC_S_OK;
}

HResult RCDebHandler::GetArchivePropertyInfo(uint32_t index, RCString& name, RCPropertyID& propID, RCVariantType::RCVariantID& varType)
{
    if (index >= (sizeof(s_debArcProps) / sizeof(s_debArcProps[0])))
    {
        return RC_E_INVALIDARG;
    }
    const RCPropData& srcItem = s_debArcProps[index];
    propID = srcItem.m_propID;
    varType = srcItem.m_varType;
    name = srcItem.m_propName;
    return RC_S_OK;
}

HResult RCDebHandler::Open(IInStream* stream, uint64_t maxCheckStartPosition, IArchiveOpenCallback* openArchiveCallback)
{
    HResult rs;
    RCDebInArchive archive;
    if(archive.Open(stream) != RC_S_OK)
    {
        return RC_S_FALSE;
    }
    m_debItems.clear();

    if (openArchiveCallback != NULL)
    {
        rs = openArchiveCallback->SetTotal(NULL, NULL);
        if (!IsSuccess(rs))
        {
            return rs;
        }
        uint64_t numFiles = m_debItems.size();
        rs = openArchiveCallback->SetCompleted(numFiles, NULL);
        if (!IsSuccess(rs))
        {
            return rs;
        }
    }

    for (;;)
    {
        RCDebItem item;
        bool filled;
        HResult result = archive.GetNextItem(filled, item);
        if (result == RC_S_FALSE)
        {
            return RC_S_FALSE;
        }
        if (result != RC_S_OK)
        {
            return RC_S_FALSE;
        }
        if (!filled)
        {
            break;
        }
        m_debItems.push_back(item);
        archive.SkipData(item.m_size);
        if (openArchiveCallback != NULL)
        {
            uint64_t numFiles = m_debItems.size();
            rs = openArchiveCallback->SetCompleted(numFiles, NULL);
            if (!IsSuccess(rs))
            {
                return rs;
            }
        }
    }
    m_inStream = stream;
    return RC_S_OK;
}

HResult RCDebHandler::Close()
{
    m_inStream.Release();
    m_debItems.clear();
    return RC_S_OK;
}

HResult RCDebHandler::GetNumberOfItems(uint32_t& numItems)
{
    numItems = static_cast<uint32_t>(m_debItems.size());
    return RC_S_OK;
}

HResult RCDebHandler::GetProperty(uint32_t index, RCPropertyID propID, RCVariant& value) 
{
    ClearVariantValue(value) ;
    RCVariant& prop = value ;
    const RCDebItem &item = m_debItems[index];

    switch(propID)
    {
    case RCPropID::kpidPath: 
        prop = RCItemNameUtils::GetOSName2(RCStringConvert::MultiByteToUnicodeString(item.m_name, RC_CP_OEMCP)); 
        break;
    case RCPropID::kpidSize:
    case RCPropID::kpidPackSize:
        prop = item.m_size;
        break;
    case RCPropID::kpidMTime:
        {
            if (item.m_mTime != 0)
            {
                RC_FILE_TIME fileTime;
                RCFileTime::UnixTimeToFileTime(item.m_mTime, fileTime);
                uint64_t tt;
                tt = fileTime.u32HighDateTime;
                tt = tt << 32;
                tt += fileTime.u32LowDateTime;
                //prop = fileTime;
                prop = tt;
            }
            break;
        }
    }
    return RC_S_OK;
    
}

HResult RCDebHandler::Extract(const RCVector<uint32_t>& indices, int32_t testMode, IArchiveExtractCallback* extractCallback) 
{
    bool allFilesMode = (indices.empty());
    uint32_t numItems;
    if (allFilesMode)
    {
        numItems = static_cast<uint32_t>(m_debItems.size());
    }
    else
    {
        numItems = static_cast<uint32_t>(indices.size());
    }
    if (numItems == 0)
    {
        return RC_S_OK;
    }
    uint64_t totalSize = 0;
    uint32_t i;
    for (i = 0; i < numItems; i++)
    {
        totalSize += m_debItems[allFilesMode ? i : indices[i]].m_size;
    }
    extractCallback->SetTotal(totalSize);
    uint64_t currentTotalSize = 0;

    ICompressCoderPtr copyCoder;
    HResult rs = RCCreateCoder::CreateCoder(m_compressCodecsInfo.Get(),RCMethod::ID_COMPRESS_COPY, copyCoder, false);
    if (!IsSuccess(rs))
    {
        return rs;
    }

    RCLocalProgress *lps = new RCLocalProgress;
    ICompressProgressInfoPtr progress = lps;
    lps->Init(extractCallback, false);

    RCLimitedSequentialInStream *streamSpec = new RCLimitedSequentialInStream;
    ISequentialInStreamPtr inStream(streamSpec);
    streamSpec->SetStream(m_inStream.Get());

    for (i = 0; i < numItems; i++)
    {
        lps->SetInSize(currentTotalSize);
        lps->SetOutSize(currentTotalSize);
        rs = lps->SetCur();
        if (!IsSuccess(rs))
        {
            return rs;
        }
        ISequentialOutStreamPtr realOutStream;
        int32_t askMode = testMode ? RC_ARCHIVE_EXTRACT_ASK_MODE_TEST : RC_ARCHIVE_EXTRACT_ASK_MODE_EXTRACT;
        int32_t index = allFilesMode ? i : indices[i];
        const RCDebItem &item = m_debItems[index];
        rs = extractCallback->GetStream(index, realOutStream.GetAddress(), askMode);
        if (!IsSuccess(rs))
        {
            return rs;
        }
        currentTotalSize += item.m_size ;

        if (!testMode && !realOutStream)
        {
            continue;
        }
        rs = extractCallback->PrepareOperation(index,askMode);
        if (!IsSuccess(rs))
        {
            return rs;
        }
        if (testMode)
        {
            rs = extractCallback->SetOperationResult(index,RC_ARCHIVE_EXTRACT_RESULT_OK);
            if (!IsSuccess(rs))
            {
                return rs;
            }
            continue;
        }
        rs = m_inStream->Seek(item.GetDataPos(), RC_STREAM_SEEK_SET, NULL);
        if (!IsSuccess(rs))
        {
            return rs;
        }
        streamSpec->Init(item.m_size);
        rs = copyCoder->Code(inStream.Get(), realOutStream.Get(), NULL, NULL, progress.Get());
        if (!IsSuccess(rs))
        {
            return rs;
        }
        realOutStream.Release();
        ICompressGetInStreamProcessedSizePtr lptr;
        uint64_t proc_size;
        rs = copyCoder.QueryInterface(IID_ICompressGetInStreamProcessedSize, lptr.GetAddress());
        if (!IsSuccess(rs))
        {
            return rs;
        }
        rs = lptr->GetInStreamProcessedSize(proc_size);
        if (!IsSuccess(rs))
        {
            return rs;
        }
        rs = extractCallback->SetOperationResult(index,(proc_size == item.m_size) ?
                                                        RC_ARCHIVE_EXTRACT_RESULT_OK : 
                                                        RC_ARCHIVE_EXTRACT_RESULT_DATA_ERROR);
        if (!IsSuccess(rs))
        {
            return rs;
        }
    }
    return RC_S_OK;
}

HResult RCDebHandler::SetCompressCodecsInfo(ICompressCodecsInfo* compressCodecsInfo) 
{
    if (compressCodecsInfo)
    {
        m_compressCodecsInfo = compressCodecsInfo;
        return RC_S_OK;
    }
    return RC_S_OK;
}

END_NAMESPACE_RCZIP
