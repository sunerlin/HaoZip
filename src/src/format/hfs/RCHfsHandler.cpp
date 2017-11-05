/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "format/hfs/RCHfsHandler.h"
#include "format/common/RCPropData.h"
#include "filesystem/RCFileTime.h"
#include "filesystem/RCStreamUtils.h"
#include "interface/RCArchiveType.h"

/////////////////////////////////////////////////////////////////
//RCHfsHandler class implementation

BEGIN_NAMESPACE_RCZIP

static RCPropData s_hfsProps[] =
{       
    { RCString(), RCPropID::kpidPath,       RCVariantType::RC_VT_STRING},
    { RCString(), RCPropID::kpidIsDir,      RCVariantType::RC_VT_BOOL},
    { RCString(), RCPropID::kpidSize,       RCVariantType::RC_VT_UINT64},
    { RCString(), RCPropID::kpidPackSize,   RCVariantType::RC_VT_UINT64},
    { RCString(), RCPropID::kpidCTime,      RCVariantType::RC_VT_UINT64},
    { RCString(), RCPropID::kpidMTime,      RCVariantType::RC_VT_UINT64},
    { RCString(), RCPropID::kpidATime,      RCVariantType::RC_VT_UINT64}
};

static RCPropData s_hfsArcProps[] =
{       
    { RCString(), RCPropID::kpidMethod,         RCVariantType::RC_VT_STRING},
    { RCString(), RCPropID::kpidClusterSize,    RCVariantType::RC_VT_UINT64},
    { RCString(), RCPropID::kpidFreeSpace,      RCVariantType::RC_VT_UINT64},
    { RCString(), RCPropID::kpidCTime,          RCVariantType::RC_VT_UINT64},
    { RCString(), RCPropID::kpidMTime,          RCVariantType::RC_VT_UINT64},
    { RCString(), RCPropID::kpidArchiveType,    RCVariantType::RC_VT_UINT64 }
};

static void HfsTimeToFileTime(uint32_t hfsTime, RC_FILE_TIME& ft)
{
    uint64_t v = ((uint64_t)3600 * 24 * (365 * 303 + 24 * 3) + hfsTime) * 10000000 ;
    ft.u32LowDateTime = (uint32_t)v;
    ft.u32HighDateTime = (uint32_t)(v >> 32) ;
}

static void HfsTimeToProp(uint32_t hfsTime, RCVariant& prop)
{
    RC_FILE_TIME ft ;
    HfsTimeToFileTime(hfsTime, ft) ;
    prop = RCFileTime::ConvertFromFileTime(ft) ;
}

class RCHfsProgressImp: 
    public RCHfsProgressVirt
{
public:

    /** 构造函数
    @param [in] callback 打开文档回调接口指针
    */
    RCHfsProgressImp(IArchiveOpenCallback* callback):
        m_callback(callback)
    {
    }      

public:

    /** 总数量
    @param [in] numFiles 总的文件个数
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult SetTotal(uint64_t numFiles);

    /** 完成数量
    @param [in] numFiles 已经完成的文件个数
    @return 成功返回RC_S_OK, 失败则返回错误码
    */
    HResult SetCompleted(uint64_t numFiles);
           
private:

    /** 打开文档回调接口指针
    */
    IArchiveOpenCallbackPtr m_callback ;
};

HResult RCHfsProgressImp::SetTotal(uint64_t numFiles)
{
    if (m_callback)
    {
        return m_callback->SetTotal(numFiles, static_cast<uint64_t>(-1));
    }
    return RC_S_OK;
}

HResult RCHfsProgressImp::SetCompleted(uint64_t numFiles)
{
    if (m_callback)
    {
        return m_callback->SetCompleted(numFiles, static_cast<uint64_t>(-1));
    }
    return RC_S_OK;
}

RCHfsHandler::RCHfsHandler()
{
}

RCHfsHandler::~RCHfsHandler()
{
}

HResult RCHfsHandler::SetCompressCodecsInfo(ICompressCodecsInfo* compressCodecsInfo)
{
    m_compressCodecsInfo = compressCodecsInfo ;
    return RC_S_OK ;
}

HResult RCHfsHandler::GetNumberOfProperties(uint32_t& numProperties)
{
    numProperties = sizeof(s_hfsProps) / sizeof(s_hfsProps[0]);
    return RC_S_OK; 
}

HResult RCHfsHandler::GetPropertyInfo(uint32_t index, RCString& name, RCPropertyID& propID, RCVariantType::RCVariantID& varType)
{
    if (index >= sizeof(s_hfsProps) / sizeof(s_hfsProps[0])) 
    {
        return RC_E_INVALIDARG ;
    }
    const RCPropData& srcItem = s_hfsProps[index];
    propID  = srcItem.m_propID ; 
    varType = srcItem.m_varType ; 
    name    = srcItem.m_propName ;
    return RC_S_OK;
}

HResult RCHfsHandler::GetNumberOfArchiveProperties(uint32_t& numProperties)
{
    numProperties = sizeof(s_hfsArcProps) / sizeof(s_hfsArcProps[0]);
    return RC_S_OK; 
}

HResult RCHfsHandler::GetArchivePropertyInfo(uint32_t index, RCString& name, RCPropertyID& propID, RCVariantType::RCVariantID& varType)
{
    if (index >= sizeof(s_hfsArcProps) / sizeof(s_hfsArcProps[0])) 
    {
        return RC_E_INVALIDARG ;
    }
    const RCPropData& srcItem = s_hfsArcProps[index];
    propID  = srcItem.m_propID ; 
    varType = srcItem.m_varType ; 
    name    = srcItem.m_propName ;
    return RC_S_OK;
}

HResult RCHfsHandler::GetArchiveProperty(RCPropertyID propID, RCVariant& value)
{
    ClearVariantValue(value) ;
    RCVariant& prop = value ;
    switch(propID)
    {
    case RCPropID::kpidMethod:
        prop = m_db.m_header.IsHfsX() ? RCString(_T("HFSX")) : RCString( _T("HFS+") ) ;
        break;
    case RCPropID::kpidClusterSize: 
        prop = uint64_t(1) << m_db.m_header.m_blockSizeLog;
        break;
    case RCPropID::kpidFreeSpace:
        prop = (uint64_t)m_db.m_header.m_numFreeBlocks << m_db.m_header.m_blockSizeLog; 
        break;
    case RCPropID::kpidMTime: 
        HfsTimeToProp(m_db.m_header.m_mTime, prop);
        break;
    case RCPropID::kpidCTime:
        {
            RC_FILE_TIME localFt ;
            RC_FILE_TIME ft ;
            HfsTimeToFileTime(m_db.m_header.m_cTime, localFt);
            if(RCFileTime::LocalFileTimeToFileTime(localFt, ft))
            {
                prop = RCFileTime::ConvertFromFileTime(ft) ;
            }
            break;
        }
    case RCPropID::kpidArchiveType:
        {
            prop = uint64_t(RC_ARCHIVE_HFS) ;
            break;
        }
    }
    return RC_S_OK ;
}

HResult RCHfsHandler::GetProperty(uint32_t index, RCPropertyID propID, RCVariant& value)
{
    ClearVariantValue(value) ;
    if(index >= m_db.m_items.size())
    {
        return RC_E_INVALIDARG ;
    }
    RCVariant& prop= value ;
    const RCHfsItem& item = m_db.m_items[index];
    switch(propID)
    {
    case RCPropID::kpidPath: 
        prop = m_db.GetItemPath(index); 
        break;
    case RCPropID::kpidIsDir: 
        prop = item.IsDir(); 
        break;
    case RCPropID::kpidCTime:
        HfsTimeToProp(item.m_cTime, prop);
        break;
    case RCPropID::kpidMTime:
        HfsTimeToProp(item.m_mTime, prop);
        break;
    case RCPropID::kpidATime:
        HfsTimeToProp(item.m_aTime, prop);
        break;
    case RCPropID::kpidPackSize:
        if (!item.IsDir())
        {
            prop = (uint64_t)item.m_numBlocks << m_db.m_header.m_blockSizeLog;
        }
        break;
    case RCPropID::kpidSize:
        if (!item.IsDir())
        {
            prop = item.m_size;
        }
        break;
    }
    return RC_S_OK ;
}

HResult RCHfsHandler::GetNumberOfItems(uint32_t& numItems)
{
    numItems = (uint32_t)m_db.m_items.size() ;
    return RC_S_OK ;
}

HResult RCHfsHandler::Open(IInStream* stream, uint64_t maxCheckStartPosition, IArchiveOpenCallback* openArchiveCallback)
{
    Close() ;
    try
    {
        RCHfsProgressImp progressImp(openArchiveCallback);
        HResult res = m_db.Open(stream, &progressImp);
        if (res == RC_E_ABORT)
        {
            return res;
        }
        if (res != RC_S_OK)
        {
            return RC_S_FALSE;
        }
        m_stream = stream ;
    }
    catch(...)
    {
        return RC_S_FALSE ;
    }
    return RC_S_OK ;
}

HResult RCHfsHandler::Close()
{
    m_stream.Release();
    m_db.Clear();
    return RC_S_OK ;
}

HResult RCHfsHandler::Extract(const std::vector<uint32_t>& indices, 
                              int32_t testMode, 
                              IArchiveExtractCallback* extractCallback)
{
    bool isTestMode = (testMode != 0);
    bool allFilesMode = indices.empty() ;
    uint32_t numItems = (uint32_t)indices.size() ;
    if (allFilesMode)
    {
        numItems = (uint32_t)m_db.m_items.size();
    }
    if (numItems == 0)
    {
        return RC_S_OK ;
    }
    uint32_t i = 0 ;
    uint64_t totalSize = 0 ;
    for (i = 0; i < numItems; ++i)
    {
        const RCHfsItem& item = m_db.m_items[allFilesMode ? i : indices[i]];
        if (!item.IsDir())
        {
            totalSize += item.m_size;
        }
    }
    if(extractCallback)
    {
        HResult hr = extractCallback->SetTotal(totalSize) ;
        if(!IsSuccess(hr))
        {
            return hr ;
        }
    }

    uint64_t currentTotalSize = 0 ;
    uint64_t currentItemSize = 0 ;

    RCByteBuffer buf;
    const uint32_t kBufSize = (1 << 16);
    buf.SetCapacity(kBufSize);

    for (i = 0; i < numItems; i++, currentTotalSize += currentItemSize)
    {
        if(extractCallback)
        {
            HResult hr = extractCallback->SetCompleted(currentTotalSize) ;
            if(!IsSuccess(hr))
            {
                return hr ;
            }
        }
        int32_t index = allFilesMode ? i : indices[i];
        const RCHfsItem& item = m_db.m_items[index];
        currentItemSize = 0;
        if (!item.IsDir())
        {
            currentItemSize = item.m_size;
        }

        ISequentialOutStreamPtr realOutStream ;
        int32_t askMode = isTestMode ?
                                       RC_ARCHIVE_EXTRACT_ASK_MODE_TEST :
                                       RC_ARCHIVE_EXTRACT_ASK_MODE_EXTRACT;
        if(extractCallback)
        {
            HResult hr = extractCallback->GetStream(index, realOutStream.GetAddress(), askMode) ;
            if(!IsSuccess(hr))
            {
                return hr ;
            }
        }

        if (item.IsDir())
        {
            if(extractCallback)
            {
                HResult hr = extractCallback->PrepareOperation(index,askMode) ;
                if(!IsSuccess(hr))
                {
                    return hr ;
                }
                hr = extractCallback->SetOperationResult(index,RC_ARCHIVE_EXTRACT_RESULT_OK) ;
                if(!IsSuccess(hr))
                {
                    return hr ;
                }
            }
            continue;
        }
        if (!isTestMode && (!realOutStream))
        {
            continue;
        }
        if(extractCallback)
        {
            HResult hr = extractCallback->PrepareOperation(index,askMode) ;
            if(!IsSuccess(hr))
            {
                return hr ;
            }
        }
        uint64_t pos = 0;
        int32_t res = RC_ARCHIVE_EXTRACT_RESULT_OK;
        int32_t i = 0 ;
        for (i = 0; i < (int32_t)item.m_extents.size(); ++i)
        {
            if (item.m_size == pos)
            {
                break;
            }
            if (res != RC_ARCHIVE_EXTRACT_RESULT_OK)
            {
                break;
            }
            const RCHfsExtent& e = item.m_extents[i];
            HResult hr = m_stream->Seek((uint64_t)e.m_pos << m_db.m_header.m_blockSizeLog, RC_STREAM_SEEK_SET, NULL) ;
            if(!IsSuccess(hr))
            {
                return hr ;
            }
            uint64_t extentSize = (uint64_t)e.m_numBlocks << m_db.m_header.m_blockSizeLog;
            for (;;)
            {
                if (extentSize == 0)
                {
                    break;
                }
                uint64_t rem = item.m_size - pos;
                if (rem == 0)
                {
                    if (extentSize >= (uint64_t)((uint32_t)1 << m_db.m_header.m_blockSizeLog))
                    {
                        res = RC_ARCHIVE_EXTRACT_RESULT_DATA_ERROR;
                    }
                    break;
                }
                uint32_t curSize = kBufSize;
                if (curSize > rem)
                {
                    curSize = (uint32_t)rem;
                }
                if (curSize > extentSize)
                {
                    curSize = (uint32_t)extentSize;
                }
                HResult hr = RCStreamUtils::ReadStream_FALSE(m_stream.Get(), buf.data(), curSize) ;
                if(!IsSuccess(hr))
                {
                    return hr ;
                }
                if (realOutStream)
                {
                    hr = RCStreamUtils::WriteStream(realOutStream.Get(), buf.data(), curSize) ;
                    if(!IsSuccess(hr))
                    {
                        return hr ;
                    }
                }
                pos += curSize;
                extentSize -= curSize;
                uint64_t processed = currentTotalSize + pos;
                if(extractCallback)
                {
                    hr = extractCallback->SetCompleted(processed) ;
                    if(!IsSuccess(hr))
                    {
                        return hr ;
                    }
                }
            }
        }
        if (i != (int32_t)item.m_extents.size() || item.m_size != pos)
        {
            res = RC_ARCHIVE_EXTRACT_RESULT_DATA_ERROR;
        }
        realOutStream.Release();
        if(extractCallback)
        {
            HResult hr = extractCallback->SetOperationResult(index,res) ;
            if(!IsSuccess(hr))
            {
                return hr ;
            }
        }
    }
    return RC_S_OK ;
}

END_NAMESPACE_RCZIP
