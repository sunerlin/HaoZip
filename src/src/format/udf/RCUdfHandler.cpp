/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#include "format/udf/RCUdfHandler.h"
#include "common/RCLocalProgress.h"
#include "common/RCCreateCoder.h"
#include "filesystem/RCFileTime.h"
#include "filesystem/RCLimitedSequentialOutStream.h"
#include "filesystem/RCBufInStream.h"
#include "format/common/RCPropData.h"
#include "interface/RCArchiveType.h"

BEGIN_NAMESPACE_RCZIP

static RCPropData s_kUdfProps[] =
{
    { RCString(_T("")), RCPropID::kpidPath, RCVariantType::RC_VT_STRING},
    { RCString(_T("")), RCPropID::kpidIsDir, RCVariantType::RC_VT_BOOL},
    { RCString(_T("")), RCPropID::kpidSize, RCVariantType::RC_VT_UINT64},
    { RCString(_T("")), RCPropID::kpidPackSize, RCVariantType::RC_VT_UINT64},
    { RCString(_T("")), RCPropID::kpidMTime, RCVariantType::RC_VT_UINT64},
    { RCString(_T("")), RCPropID::kpidATime, RCVariantType::RC_VT_UINT64}
};

static RCPropData s_kUdfArcProps[] =
{
    { RCString(_T("")), RCPropID::kpidComment, RCVariantType::RC_VT_STRING},
    { RCString(_T("")), RCPropID::kpidClusterSize, RCVariantType::RC_VT_UINT64},
    { RCString(_T("")), RCPropID::kpidCTime, RCVariantType::RC_VT_UINT64},
    { RCString(_T("")), RCPropID::kpidCommented, RCVariantType::RC_VT_BOOL},
    { RCString(_T("")), RCPropID::kpidArchiveType,    RCVariantType::RC_VT_UINT64 }
};

HResult RCUdfHandler::GetNumberOfProperties(uint32_t& numProperties) 
{
    numProperties = sizeof(s_kUdfProps) / sizeof(s_kUdfProps[0]);
    return RC_S_OK;
}

HResult RCUdfHandler::GetPropertyInfo(uint32_t index, 
                                      RCString& name, 
                                      RCPropertyID& propID, 
                                      RCVariantType::RCVariantID& varType)
{
    if (index >= sizeof(s_kUdfProps) / sizeof(s_kUdfProps[0])) 
    {
        return RC_E_INVALIDARG;
    }
    const RCPropData& srcItem = s_kUdfProps[index];
    propID = srcItem.m_propID;
    varType = srcItem.m_varType;
    name = srcItem.m_propName;
    return RC_S_OK;
}

HResult RCUdfHandler::GetNumberOfArchiveProperties(uint32_t& numProperties) 
{
    numProperties = sizeof(s_kUdfArcProps) / sizeof(s_kUdfArcProps[0]);
    return RC_S_OK;
}

HResult RCUdfHandler::GetArchivePropertyInfo(uint32_t index, 
                                             RCString& name, 
                                             RCPropertyID& propID, 
                                             RCVariantType::RCVariantID& varType)
{
    if(index >= sizeof(s_kUdfArcProps) / sizeof(s_kUdfArcProps[0])) 
    {
        return RC_E_INVALIDARG;
    }
    const RCPropData& srcItem = s_kUdfArcProps[index];
    propID = srcItem.m_propID;
    varType = srcItem.m_varType;
    name = srcItem.m_propName;
    return RC_S_OK; 
}

void RCUdfHandler::UdfTimeToFileTime(const RCUdfTime &t, RCVariant& prop)
{
    uint64_t numSecs;
    const byte_t *d = t.m_data;
    if (!RCFileTime::GetSecondsSince1601(t.GetYear(), d[4], d[5], d[6], d[7], d[8], numSecs))
    {
        return;
    }
    if (t.IsLocal())
    {
        numSecs -= t.GetMinutesOffset() * 60;
    }
    uint64_t v = (((numSecs * 100 + d[9]) * 100 + d[10]) * 100 + d[11]) * 10;
    prop = v;
}

HResult RCUdfHandler::Open(IInStream* stream, uint64_t maxCheckStartPosition, IArchiveOpenCallback* callback)
{
    Close();
    RCUdfProgressImp progressImp(callback);
    HResult result = m_archive.Open(stream, &progressImp);
    if (!IsSuccess(result))
    {
        return result;
    }
    bool showVolName = (m_archive.m_logVols.size() > 1);
    for (int32_t volIndex = 0; volIndex < static_cast<int32_t>(m_archive.m_logVols.size()); volIndex++)
    {
        const RCUdfLogVol &vol = m_archive.m_logVols[volIndex];
        bool showFileSetName = (vol.m_fileSets.size() > 1);
        for (int32_t fsIndex = 0; fsIndex < static_cast<int32_t>(vol.m_fileSets.size()); fsIndex++)
        {
            const RCUdfFileSet &fs = vol.m_fileSets[fsIndex];
            for (int32_t i = ((showVolName || showFileSetName) ? 0 : 1); i < static_cast<int32_t>(fs.m_refs.size()); i++)
            {
                RCUdfRef2 ref2;
                ref2.m_vol = volIndex;
                ref2.m_fs = fsIndex;
                ref2.m_ref = i;
                m_refs2.push_back(ref2);
            }
        }
    }
    m_inStream = stream;
    return RC_S_OK;
}

HResult RCUdfHandler::Close()
{
    m_inStream.Release();
    m_archive.Clear();
    m_refs2.clear();
    return RC_S_OK;
}

HResult RCUdfHandler::GetNumberOfItems(uint32_t& numItems)
{
    numItems = static_cast<uint32_t>(m_refs2.size());
    return RC_S_OK;
}

HResult RCUdfHandler::GetProperty(uint32_t index, RCPropertyID propID, RCVariant& value) 
{
    ClearVariantValue(value) ;
    RCVariant& prop = value ;
    {
        if(index >= m_refs2.size())
        {
            return RC_E_INVALIDARG ;
        }
        const RCUdfRef2 &ref2 = m_refs2[index];
        const RCUdfLogVol &vol = m_archive.m_logVols[ref2.m_vol];
        const RCUdfRef &ref = vol.m_fileSets[ref2.m_fs].m_refs[ref2.m_ref];
        const RCUdfFile &file = m_archive.m_files[ref.m_fileIndex];
        const RCUdfItemPtr item = m_archive.m_items[file.m_itemIndex];
        switch(propID)
        {
        case RCPropID::kpidPath:
            prop = m_archive.GetItemPath(ref2.m_vol, ref2.m_fs, ref2.m_ref, m_archive.m_logVols.size() > 1, vol.m_fileSets.size() > 1); 
            break;
        case RCPropID::kpidIsDir:  
            prop = item->IsDir(); 
            break;
        case RCPropID::kpidSize:      
            if (!item->IsDir())
            {
                prop = (uint64_t)item->m_size; 
            }
            break;
        case RCPropID::kpidPackSize:  
            if (!item->IsDir()) 
            {
                prop = (uint64_t)item->m_numLogBlockRecorded * vol.m_blockSize; 
            }
            break;
        case RCPropID::kpidMTime:  
            UdfTimeToFileTime(item->m_mTime, prop); 
            break;
        case RCPropID::kpidATime:  
            UdfTimeToFileTime(item->m_aTime, prop); 
            break;
        }
    }
    return RC_S_OK;
}

HResult RCUdfHandler::Extract(const std::vector<uint32_t>& indices, 
                              int32_t testMode, 
                              IArchiveExtractCallback* extractCallback) 
{
    HResult result;
    bool allFilesMode = indices.empty() ;
    uint32_t numItems;
    if (allFilesMode)
    {
        numItems = static_cast<uint32_t>(m_refs2.size());
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
        uint32_t index = (allFilesMode ? i : indices[i]);
        const RCUdfRef2 &ref2 = m_refs2[index];
        const RCUdfRef &ref = m_archive.m_logVols[ref2.m_vol].m_fileSets[ref2.m_fs].m_refs[ref2.m_ref];
        const RCUdfFile &file = m_archive.m_files[ref.m_fileIndex];
        const RCUdfItemPtr item = m_archive.m_items[file.m_itemIndex];
        if (!item->IsDir())
        {
            totalSize += item->m_size;
        }
    }
    extractCallback->SetTotal(totalSize);

    uint64_t currentTotalSize = 0;

    ICompressCoderPtr copyCoder;
    result = RCCreateCoder::CreateCoder(m_compressCodecsInfo.Get(),RCMethod::ID_COMPRESS_COPY, copyCoder, false);
    if (!IsSuccess(result))
    {
        return result;
    }

    RCLocalProgress *lps = new RCLocalProgress;
    ICompressProgressInfoPtr progress = lps;
    lps->Init(extractCallback, false);
    
    RCLimitedSequentialOutStream* outStreamSpec = new RCLimitedSequentialOutStream ;
    ISequentialOutStreamPtr outStream(outStreamSpec) ;

    for (i = 0; i < numItems; ++i)
    {
        lps->SetInSize(currentTotalSize);
        lps->SetOutSize(currentTotalSize);
        result = lps->SetCur();
        if (!IsSuccess(result))
        {
            return result;
        }

        ISequentialOutStreamPtr realOutStream;
        int32_t askMode = testMode ? RC_ARCHIVE_EXTRACT_ASK_MODE_TEST : RC_ARCHIVE_EXTRACT_ASK_MODE_EXTRACT;
        uint32_t index = allFilesMode ? i : indices[i];

        result = extractCallback->GetStream(index, realOutStream.GetAddress(), askMode);
        if (!IsSuccess(result))
        {
            return result;
        }

        const RCUdfRef2& ref2 = m_refs2[index];
        const RCUdfRef& ref = m_archive.m_logVols[ref2.m_vol].m_fileSets[ref2.m_fs].m_refs[ref2.m_ref];
        const RCUdfFile& file = m_archive.m_files[ref.m_fileIndex];
        const RCUdfItem& item = *m_archive.m_items[file.m_itemIndex];

        if (item.IsDir())
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
        currentTotalSize += item.m_size ;
        
        if (!testMode && !realOutStream)
        {
            continue;
        }
        result = extractCallback->PrepareOperation(index,askMode);
        if (!IsSuccess(result))
        {
            return result;
        }
        outStreamSpec->SetStream(realOutStream.Get());
        realOutStream.Release();
        outStreamSpec->Init(item.m_size);

        int32_t opRes = RC_ARCHIVE_EXTRACT_RESULT_UNSUPPORTED_METHOD ;
        ISequentialInStreamPtr udfInStream ;
        HResult res = GetStream(index, udfInStream.GetAddress());
        if (res == RC_E_NOTIMPL)
        {
            opRes = RC_ARCHIVE_EXTRACT_RESULT_UNSUPPORTED_METHOD ;
        }
        else if (res != RC_S_OK)
        {
            opRes = RC_ARCHIVE_EXTRACT_RESULT_DATA_ERROR ;
        }
        else
        {
            result = copyCoder->Code(udfInStream.Get(), outStream.Get(), NULL, NULL, progress.Get());
            if (!IsSuccess(result))
            {
                return result;
            }
            opRes = outStreamSpec->IsFinishedOK() ? RC_ARCHIVE_EXTRACT_RESULT_OK :
                                                    RC_ARCHIVE_EXTRACT_RESULT_DATA_ERROR ;
        }
        outStreamSpec->ReleaseStream();
        result = extractCallback->SetOperationResult(index, opRes) ;
        if (!IsSuccess(result))
        {
            return result;
        }
    }
    return RC_S_OK ;
}

HResult RCUdfHandler::GetArchiveProperty(RCPropertyID propID, RCVariant& value) 
{
    ClearVariantValue(value) ;
    RCVariant& prop = value ;
    switch(propID)
    {
    case RCPropID::kpidComment:
        {
            prop = m_archive.GetComment();
            break;
        }
    case RCPropID::kpidCommented:
        {
            prop = m_archive.m_logVols.empty() ? false : true ;
            break;
        }
    case RCPropID::kpidClusterSize:
        if (m_archive.m_logVols.size() > 0)
        {
            uint32_t blockSize = m_archive.m_logVols[0].m_blockSize;
            int32_t i;
            for (i = 1; i < static_cast<int32_t>(m_archive.m_logVols.size()); i++)
            {
                if (m_archive.m_logVols[i].m_blockSize != blockSize)
                {
                    break;
                }
            }
            if (i == (int32_t)m_archive.m_logVols.size())
            {
                prop = static_cast<uint64_t>(blockSize);
            }
        }
        break;

    case RCPropID::kpidCTime:
        if (m_archive.m_logVols.size() == 1)
        {
            const RCUdfLogVol &vol = m_archive.m_logVols[0];
            if (vol.m_fileSets.size() >= 1)
            {
                UdfTimeToFileTime(vol.m_fileSets[0].m_recodringTime, prop);
            }
        }
        break;
    case RCPropID::kpidArchiveType:
        {
            prop = uint64_t(RC_ARCHIVE_UDF) ;
            break;
        }
    }
    return RC_S_OK;
}

HResult RCUdfHandler::SetCompressCodecsInfo(ICompressCodecsInfo* compressCodecsInfo) 
{
    if (compressCodecsInfo)
    {
        m_compressCodecsInfo = compressCodecsInfo;
        return RC_S_OK;
    }
    return RC_S_OK;
}

HResult RCUdfHandler::GetStream(uint32_t index, ISequentialInStream** stream)
{
    if( (stream == NULL) || (index >= m_refs2.size()) )
    {
        return RC_E_INVALIDARG ;
    }
    *stream = 0 ;

    const RCUdfRef2& ref2 = m_refs2[index] ;
    const RCUdfLogVol& vol = m_archive.m_logVols[ref2.m_vol] ;
    const RCUdfRef& ref = vol.m_fileSets[ref2.m_fs].m_refs[ref2.m_ref];
    const RCUdfFile& file = m_archive.m_files[ref.m_fileIndex];
    const RCUdfItem& item = *m_archive.m_items[file.m_itemIndex];
    uint64_t size = item.m_size ;

    if (!item.IsRecAndAlloc() || !item.CheckChunkSizes() || ! m_archive.CheckItemExtents(ref2.m_vol, item))
    {
        return RC_E_NOTIMPL ;
    }

    if (item.m_isInline)
    {
        RCBufInStream* inStreamSpec = new RCBufInStream ;
        ISequentialInStreamPtr inStream = inStreamSpec ;
        RCReferenceBuf* referenceBuf = new RCReferenceBuf ;
        IUnknownPtr ref = referenceBuf ;
        referenceBuf->m_buf = item.m_inlineData ;
        inStreamSpec->Init(referenceBuf) ;
        *stream = inStream.Detach() ;
        return RC_S_OK;
    }

    RCUdfExtentsStream* extentStreamSpec = new RCUdfExtentsStream() ;
    ISequentialInStreamPtr extentStream = extentStreamSpec ;

    extentStreamSpec->m_stream = m_inStream ;

    uint64_t virtOffset = 0;
    for (int32_t extentIndex = 0; extentIndex < (int32_t)item.m_extents.size(); extentIndex++)
    {
        const RCUdfMyExtent& extent = item.m_extents[extentIndex] ;
        uint32_t len = extent.GetLen() ;
        if (len == 0)
        {
            continue ;
        }
        if (size < len)
        {
            return RC_S_FALSE ;
        }
        int32_t partitionIndex = vol.m_partitionMaps[extent.m_partitionRef].m_partitionIndex ;
        uint32_t logBlockNumber = extent.m_pos ;
        const RCUdfPartition& partition = m_archive.m_partitions[partitionIndex];
        uint64_t offset = ((uint64_t)partition.m_pos << m_archive.m_secLogSize) + (uint64_t)logBlockNumber * vol.m_blockSize ;

        RCUdfSeekExtent se ;
        se.m_phy = offset ;
        se.m_virt = virtOffset;
        virtOffset += len;
        extentStreamSpec->m_extents.push_back(se) ;
        size -= len ;
    }
    if (size != 0)
    {
        return RC_S_FALSE ;
    }
    RCUdfSeekExtent se ;
    se.m_phy = 0 ;
    se.m_virt = virtOffset ;
    extentStreamSpec->m_extents.push_back(se) ;
    extentStreamSpec->Init() ;
    *stream = extentStream.Detach() ;
    return RC_S_OK ;
}

END_NAMESPACE_RCZIP
