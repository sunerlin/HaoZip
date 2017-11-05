/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#include "format/com/RCComHandler.h"
#include "format/common/RCPropData.h"
#include "filesystem/RCStreamUtils.h"
#include "interface/RCArchiveType.h"
#include "interface/RCMethodDefs.h"
#include "common/RCCreateCoder.h"
#include "common/RCLocalProgress.h"
#include "filesystem/RCClusterInStream.h"

BEGIN_NAMESPACE_RCZIP

static RCPropData s_kComProps[] =
{
    { RCString(_T("")), RCPropID::kpidPath, RCVariantType::RC_VT_STRING},
    { RCString(_T("")), RCPropID::kpidIsDir, RCVariantType::RC_VT_BOOL},
    { RCString(_T("")), RCPropID::kpidSize, RCVariantType::RC_VT_UINT64},
    { RCString(_T("")), RCPropID::kpidPackSize, RCVariantType::RC_VT_UINT64},
    { RCString(_T("")), RCPropID::kpidCTime, RCVariantType::RC_VT_UINT64},
    { RCString(_T("")), RCPropID::kpidMTime, RCVariantType::RC_VT_UINT64}
};

static RCPropData s_kComArcProps[] =
{
    { RCString(_T("")), RCPropID::kpidClusterSize, RCVariantType::RC_VT_UINT64},
    { RCString(_T("")), RCPropID::kpidSectorSize,  RCVariantType::RC_VT_UINT64},
    { RCString(_T("")), RCPropID::kpidArchiveType,    RCVariantType::RC_VT_UINT64 }
};

HResult RCComHandler::GetArchiveProperty(RCPropertyID propID, RCVariant& value) 
{
    ClearVariantValue(value) ;
    switch(propID)
    {
    case RCPropID::kpidClusterSize: 
        value = (uint64_t)1 << m_db.m_sectorSizeBits; 
        break;
    case RCPropID::kpidSectorSize: 
        value = (uint64_t)1 << m_db.m_miniSectorSizeBits; 
        break;
    case RCPropID::kpidArchiveType:
        value = uint64_t(RC_ARCHIVE_COM) ;
        break;
    default:
        return RC_S_OK ;
    }
    return RC_S_OK;
}

HResult RCComHandler::GetNumberOfProperties(uint32_t& numProperties) 
{
    numProperties = sizeof(s_kComProps) / sizeof(s_kComProps[0]);
    return RC_S_OK;
}

HResult RCComHandler::GetPropertyInfo(uint32_t index, RCString& name, RCPropertyID& propID, RCVariantType::RCVariantID& varType)
{
    if (index >= (sizeof(s_kComProps) / sizeof(s_kComProps[0])))
    {
        return RC_E_INVALIDARG;
    }
    const RCPropData& srcItem = s_kComProps[index];
    propID = srcItem.m_propID;
    varType = srcItem.m_varType;
    name = srcItem.m_propName;
    return RC_S_OK;
}

HResult RCComHandler::GetNumberOfArchiveProperties(uint32_t& numProperties) 
{
    numProperties = sizeof(s_kComArcProps) / sizeof(s_kComArcProps[0]);
    return RC_S_OK;
}

HResult RCComHandler::GetArchivePropertyInfo(uint32_t index, RCString& name, RCPropertyID& propID, RCVariantType::RCVariantID& varType)
{
    if (index >= (sizeof(s_kComArcProps) / sizeof(s_kComArcProps[0])))
    {
        return RC_E_INVALIDARG;
    }
    const RCPropData& srcItem = s_kComArcProps[index];
    propID = srcItem.m_propID;
    varType = srcItem.m_varType;
    name = srcItem.m_propName;
    return RC_S_OK;
}

HResult RCComHandler::Open(IInStream* inStream, uint64_t maxCheckStartPosition, IArchiveOpenCallback* openArchiveCallback)
{
    Close();
    try
    {
        if (m_db.Open(inStream) != RC_S_OK)
        {
            return RC_S_FALSE;
        }
        m_stream = inStream ;
    }
    catch(...) 
    { 
        return RC_S_FALSE; 
    }
    return RC_S_OK;
}

HResult RCComHandler::Close()
{
    m_db.Clear();
    m_stream.Release();
    return RC_S_OK;
}

HResult RCComHandler::GetNumberOfItems(uint32_t& numItems)
{
    numItems = static_cast<uint32_t>(m_db.m_refs.size());
    return RC_S_OK;
}

HResult RCComHandler::GetProperty(uint32_t index, RCPropertyID propID, RCVariant& value) 
{
    ClearVariantValue(value) ;
    RCVariant& prop = value ;
    const RCComRef &ref = m_db.m_refs[index];
    const RCComItem &item = m_db.m_items[ref.m_did];

    switch(propID)
    {
    case RCPropID::kpidPath:
        prop = m_db.GetItemPath(index);
        break;
    case RCPropID::kpidIsDir:  
        prop = item.IsDir(); 
        break;
    case RCPropID::kpidCTime:  
        prop = RCComFuncImpl::RCFileTime2UInt64(item.m_cTime); 
        break;
    case RCPropID::kpidMTime:  
        prop = RCComFuncImpl::RCFileTime2UInt64(item.m_mTime); 
        break;
    case RCPropID::kpidPackSize:
        if (!item.IsDir())
        {
            prop = (uint64_t)m_db.GetItemPackSize(item.m_size) ; 
        }
        break ;
    case RCPropID::kpidSize:
        if (!item.IsDir())
        {
            prop = (uint64_t)item.m_size;
        }
        break;
    }
    return RC_S_OK;
}

HResult RCComHandler::Extract(const RCVector<uint32_t>& indices, int32_t testMode, IArchiveExtractCallback* extractCallback) 
{
    bool allFilesMode = indices.empty();
    uint32_t numItems = static_cast<uint32_t>(indices.size()) ;
    if(allFilesMode)
    {
        numItems = (uint32_t)m_db.m_refs.size() ;
    }
    if(numItems == 0)
    {
        return RC_S_OK ;
    }
    uint32_t i = 0 ;
    uint64_t totalSize = 0;
    for(i = 0; i < numItems; i++)
    {
        const RCComItem &item = m_db.m_items[m_db.m_refs[allFilesMode ? i : indices[i]].m_did];
        if (!item.IsDir())
        {
            totalSize += item.m_size;
        }
    }
    HResult hr = extractCallback->SetTotal(totalSize) ;
    if (!IsSuccess(hr))
    {
        return hr ;
    }
    
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

    RCLocalProgress* lps = new RCLocalProgress ;
    ICompressProgressInfoPtr progress = lps ;
    lps->Init(extractCallback, false);

    uint64_t totalPackSize = 0 ;
    totalSize = 0 ;
    for (i = 0; i < numItems; i++)
    {
        lps->SetInSize(totalPackSize) ;
        lps->SetOutSize(totalSize) ;
        hr = lps->SetCur() ;
        if (hr != RC_S_OK)
        {
            return hr;
        }
        
        int32_t index = allFilesMode ? i : indices[i] ;
        const RCComItem& item = m_db.m_items[m_db.m_refs[index].m_did] ;

        ISequentialOutStreamPtr outStream ;
        int32_t askMode = testMode ? RC_ARCHIVE_EXTRACT_ASK_MODE_TEST :
                                     RC_ARCHIVE_EXTRACT_ASK_MODE_EXTRACT ;
        hr = extractCallback->GetStream(index, outStream.GetAddress(), askMode) ;
        if (hr != RC_S_OK)
        {
            return hr;
        }

        if (item.IsDir())
        {
            hr = extractCallback->PrepareOperation(index, askMode) ;
            if(!IsSuccess(hr))
            {
                return hr ;
            }
            hr = extractCallback->SetOperationResult(index, RC_ARCHIVE_EXTRACT_RESULT_OK) ;
            if(!IsSuccess(hr))
            {
                return hr ;
            }
            continue;
        }

        totalPackSize += m_db.GetItemPackSize(item.m_size);
        totalSize += item.m_size;

        if (!testMode && !outStream)
        {
            continue;
        }
        hr = extractCallback->PrepareOperation(index, askMode) ;
        if(!IsSuccess(hr))
        {
            return hr ;
        }
        int32_t res = RC_ARCHIVE_EXTRACT_RESULT_DATA_ERROR;
        ISequentialInStreamPtr inStream ;
        HResult hres = GetStream(index, inStream.GetAddress()) ;
        if (hres == RC_S_FALSE)
        {
            res = RC_ARCHIVE_EXTRACT_RESULT_DATA_ERROR;
        }
        else if (hres == RC_E_NOTIMPL)
        {
            res = RC_ARCHIVE_EXTRACT_RESULT_UNSUPPORTED_METHOD;
        }
        else
        {
            if(!IsSuccess(hres))
            {
                return hres ;
            }
            if (inStream)
            {
                hr = copyCoder->Code(inStream.Get(), outStream.Get(), NULL, NULL, progress.Get()) ;
                if(!IsSuccess(hr))
                {
                    return hr ;
                }
                uint64_t totalProcessedSize = 0 ;
                hr = copyCoderSize->GetInStreamProcessedSize(totalProcessedSize) ;
                if(!IsSuccess(hr))
                {
                    return hr ;
                }
                if (totalProcessedSize == item.m_size)
                {
                    res = RC_ARCHIVE_EXTRACT_RESULT_OK ;
                }
            }
        }
        outStream.Release();
        hr = extractCallback->SetOperationResult(index, res) ;
        if(!IsSuccess(hr))
        {
            return hr ;
        }
    }
    return RC_S_OK;
}

HResult RCComHandler::SetCompressCodecsInfo(ICompressCodecsInfo* compressCodecsInfo) 
{
    if (compressCodecsInfo)
    {
        m_compressCodecsInfo = compressCodecsInfo;
        return RC_S_OK;
    }
    return RC_S_OK;
}

HResult RCComHandler::GetStream(uint32_t index, ISequentialInStream** stream)
{
    *stream = 0;
    const RCComItem& item = m_db.m_items[m_db.m_refs[index].m_did] ;
    RCClusterInStream* streamSpec = new RCClusterInStream;
    ISequentialInStreamPtr streamTemp = streamSpec ;
    streamSpec->m_stream = m_stream;
    streamSpec->m_startOffset = 0;

    bool isLargeStream = m_db.IsLargeStream(item.m_size);
    int bsLog = isLargeStream ? m_db.m_sectorSizeBits : m_db.m_miniSectorSizeBits;
    streamSpec->m_blockSizeLog = bsLog;
    streamSpec->m_size = item.m_size;

    uint32_t clusterSize = (uint32_t)1 << bsLog;
    uint64_t numClusters64 = (item.m_size + clusterSize - 1) >> bsLog;
    if (numClusters64 >= ((uint32_t)1 << 31))
    {
        return RC_E_NOTIMPL;
    }
    streamSpec->m_vector.reserve((int32_t)numClusters64) ;
    uint32_t sid = item.m_sid;
    uint64_t size = item.m_size;

    if (size != 0)
    {
        for (;; size -= clusterSize)
        {
            if (isLargeStream)
            {
                if (sid >= m_db.m_fatSize)
                {
                    return RC_S_FALSE;
                }
                streamSpec->m_vector.push_back(sid + 1);
                sid = m_db.m_fat[sid];
            }
            else
            {
                uint64_t val = 0 ;
                if (sid >= m_db.m_matSize || !m_db.GetMiniCluster(sid, val) || val >= (uint64_t)1 << 32)
                {
                    return RC_S_FALSE;
                }
                streamSpec->m_vector.push_back((uint32_t)val) ;
                sid = m_db.m_mat[sid];
            }
            if (size <= clusterSize)
            {
                break;
            }
        }
    }
    if (sid != RCComDefs::s_endOfChain)
    {
        return RC_S_FALSE ;
    }
    HResult hr = streamSpec->InitAndSeek() ;
    if(!IsSuccess(hr))
    {
        return hr ;
    }
    *stream = streamTemp.Detach() ;
    return RC_S_OK ;
}

END_NAMESPACE_RCZIP
