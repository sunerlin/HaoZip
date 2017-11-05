/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "format/wim/RCWimHandler.h"
#include "format/common/RCPropData.h"
#include "filesystem/RCFileTime.h"
#include "common/RCStringUtil.h"
#include "format/wim/RCWimUtils.h"
#include "format/wim/RCWimXml.h"
#include "common/RCLocalProgress.h"
#include "format/wim/RCWimUnpacker.h"
#include "interface/IArchive.h"
#include "filesystem/RCStreamUtils.h"
#include "interface/RCArchiveType.h"

/////////////////////////////////////////////////////////////////
//RCWimHandler class implementation

BEGIN_NAMESPACE_RCZIP

class RCWimVolumeName
{
    RCString _before ;
    RCString _after ;
public:
    RCWimVolumeName() {};

    void InitName(const RCString &name)
    {
        RCString::size_type dotPos = name.rfind(_T('.'));
        if (dotPos == RCString::npos)
        {
            dotPos = name.size();
        }
        _before = RCStringUtil::Left(name,(int32_t)dotPos);
        _after = RCStringUtil::Mid(name,(int32_t)dotPos);
    }

    RCString GetNextName(uint32_t index)
    {
        RCString::value_type s[32];
        RCStringUtil::ConvertUInt64ToString(uint64_t(index), s) ;
        return _before + RCString(s) + _after ;
    }
};

#define WIM_DETAILS

static RCPropData s_kWimProps[] =
{
    { RCString(), RCPropID::kpidPath,       RCVariantType::RC_VT_STRING},
    { RCString(), RCPropID::kpidIsDir,      RCVariantType::RC_VT_BOOL},
    { RCString(), RCPropID::kpidSize,       RCVariantType::RC_VT_UINT64},
    { RCString(), RCPropID::kpidPackSize,   RCVariantType::RC_VT_UINT64},
    { RCString(), RCPropID::kpidAttrib,     RCVariantType::RC_VT_UINT64},
    { RCString(), RCPropID::kpidMethod,     RCVariantType::RC_VT_STRING},
    { RCString(), RCPropID::kpidMTime,      RCVariantType::RC_VT_UINT64},
    { RCString(), RCPropID::kpidCTime,      RCVariantType::RC_VT_UINT64},
    { RCString(), RCPropID::kpidATime,      RCVariantType::RC_VT_UINT64}
    
#ifdef WIM_DETAILS
    , { RCString(), RCPropID::kpidVolume,   RCVariantType::RC_VT_UINT64}
    , { RCString(), RCPropID::kpidOffset,   RCVariantType::RC_VT_UINT64}
    , { RCString(), RCPropID::kpidLinks,    RCVariantType::RC_VT_UINT64}
#endif
};

static RCPropData s_kWimArcProps[] =
{
    { RCString(), RCPropID::kpidSize,       RCVariantType::RC_VT_UINT64},
    { RCString(), RCPropID::kpidPackSize,   RCVariantType::RC_VT_UINT64},
    { RCString(), RCPropID::kpidMethod,     RCVariantType::RC_VT_STRING},
    { RCString(), RCPropID::kpidCTime,      RCVariantType::RC_VT_UINT64},
    { RCString(), RCPropID::kpidMTime,      RCVariantType::RC_VT_UINT64},
    { RCString(), RCPropID::kpidComment,    RCVariantType::RC_VT_UINT64},
    { RCString(), RCPropID::kpidIsVolume,   RCVariantType::RC_VT_BOOL},
    { RCString(), RCPropID::kpidVolume,     RCVariantType::RC_VT_UINT64},
    { RCString(), RCPropID::kpidNumVolumes, RCVariantType::RC_VT_UINT64},
    { RCString(), RCPropID::kpidCommented,  RCVariantType::RC_VT_BOOL},
    { RCString(), RCPropID::kpidArchiveType,RCVariantType::RC_VT_UINT64 }
};

#ifdef RCZIP_OS_WIN
    static const RCString::value_type* s_kWimStreamsNamePrefix = _T("Files\\") ;
#else
    static const RCString::value_type* s_kWimStreamsNamePrefix = _T("Files/") ;
#endif

static const RCString::value_type* s_kWimMethodLZX       = _T("LZX") ;
static const RCString::value_type* s_kWimMethodXpress    = _T("XPress") ;
static const RCString::value_type* s_kWimMethodCopy      = _T("Copy") ;

RCWimHandler::RCWimHandler():
    m_nameLenForStreams(0)
{
}

RCWimHandler::~RCWimHandler()
{
}

int32_t RCWimHandler::CompareFileTime(const RC_FILE_TIME& ft1, const RC_FILE_TIME& ft2)
{
    if(ft1.u32HighDateTime < ft2.u32HighDateTime)
    {
        return -1;
    }
    if(ft1.u32HighDateTime > ft2.u32HighDateTime)
    {
        return 1;
    }
    if(ft1.u32LowDateTime < ft2.u32LowDateTime)
    {
        return -1;
    }
    if(ft1.u32LowDateTime > ft2.u32LowDateTime)
    {
        return 1;
    }
    return 0;
}

HResult RCWimHandler::SetCompressCodecsInfo(ICompressCodecsInfo* compressCodecsInfo)
{
    m_compressCodecsInfo = compressCodecsInfo ;
    return RC_S_OK ;
}

HResult RCWimHandler::GetNumberOfProperties(uint32_t& numProperties)
{
    numProperties = sizeof(s_kWimProps) / sizeof(s_kWimProps[0]);
    return RC_S_OK; 
}

HResult RCWimHandler::GetPropertyInfo(uint32_t index, RCString& name, RCPropertyID& propID, RCVariantType::RCVariantID& varType)
{
    if (index >= sizeof(s_kWimProps) / sizeof(s_kWimProps[0])) 
    {
        return RC_E_INVALIDARG ;
    }
    const RCPropData& srcItem = s_kWimProps[index];
    propID  = srcItem.m_propID ; 
    varType = srcItem.m_varType ; 
    name    = srcItem.m_propName ;
    return RC_S_OK;
}

HResult RCWimHandler::GetNumberOfArchiveProperties(uint32_t& numProperties)
{
    numProperties = sizeof(s_kWimArcProps) / sizeof(s_kWimArcProps[0]);
    return RC_S_OK; 
}

HResult RCWimHandler::GetArchivePropertyInfo(uint32_t index, RCString& name, RCPropertyID& propID, RCVariantType::RCVariantID& varType)
{
    if (index >= sizeof(s_kWimArcProps) / sizeof(s_kWimArcProps[0])) 
    {
        return RC_E_INVALIDARG ;
    }
    const RCPropData& srcItem = s_kWimArcProps[index];
    propID  = srcItem.m_propID ; 
    varType = srcItem.m_varType ; 
    name    = srcItem.m_propName ;
    return RC_S_OK;
}

HResult RCWimHandler::GetArchiveProperty(RCPropertyID propID, RCVariant& value)
{
    ClearVariantValue(value) ;
    RCVariant& prop = value ;
    const RCWimImageInfo* image = NULL ;
    if (m_xmls.size() == 1)
    {
        const RCWimXml& xml = m_xmls[0];
        if(xml.m_images.size() == 1)
        {
            image = &xml.m_images[0] ;
        }
    }

    switch(propID)
    {
    case RCPropID::kpidSize: 
        prop = m_database.GetUnpackSize(); 
        break;
    case RCPropID::kpidPackSize: 
        prop = m_database.GetPackSize(); 
        break;
    case RCPropID::kpidCTime:
        if (m_xmls.size() == 1)
        {
            const RCWimXml& xml = m_xmls[0] ;
            int32_t index = -1;
            for (int32_t i = 0; i < (int32_t)xml.m_images.size(); ++i)
            {
                const RCWimImageInfo& image = xml.m_images[i];
                if (image.m_cTimeDefined)
                {
                    if (index < 0 || CompareFileTime(image.m_cTime, xml.m_images[index].m_cTime) < 0)
                    {
                        index = i;
                    }
                }
            }
            if (index >= 0)
            {
                prop = RCFileTime::ConvertFromFileTime(xml.m_images[index].m_cTime) ;
            }
        }
        break;

    case RCPropID::kpidMTime:
        if (m_xmls.size() == 1)
        {
            const RCWimXml& xml = m_xmls[0];
            int32_t index = -1;
            for (int32_t i = 0; i < (int32_t)xml.m_images.size(); ++i)
            {
                const RCWimImageInfo& image = xml.m_images[i];
                if (image.m_mTimeDefined)
                {
                    if (index < 0 || CompareFileTime(image.m_mTime, xml.m_images[index].m_mTime) > 0)
                    {
                        index = i;
                    }
                }
            }
            if (index >= 0)
            {
                prop = RCFileTime::ConvertFromFileTime(xml.m_images[index].m_mTime) ;
            }
        }
        break;

    case RCPropID::kpidCommented: 
        prop = false ;
        if (image != NULL && image->m_nameDefined)
        {
            prop = image->m_name.empty() ? false : true ; 
        }
        break;
    case RCPropID::kpidComment: 
        if (image != NULL && image->m_nameDefined)
        {
            prop = image->m_name; 
        }
        break;
    case RCPropID::kpidIsVolume:
        if (!m_xmls.empty())
        {
            uint16_t volIndex = m_xmls[0].m_volIndex;
            if (volIndex < m_volumes.size())
            {
                prop = (m_volumes[volIndex].m_header.m_numParts > 1);
            }
        }
        break;
    case RCPropID::kpidVolume:
        if (!m_xmls.empty())
        {
            uint16_t volIndex = m_xmls[0].m_volIndex;
            if (volIndex < m_volumes.size())
            {
                prop = (uint64_t)m_volumes[volIndex].m_header.m_partNumber;
            }
        }
        break;
    case RCPropID::kpidNumVolumes: 
        if (!m_volumes.empty())
        {
            prop = (uint64_t)(m_volumes.size() - 1);
        }
        break;
    case RCPropID::kpidMethod:
        {
            bool lzx = false ;
            bool xpress = false ;
            bool copy = false ;
            for (int32_t i = 0; i < (int32_t)m_xmls.size(); ++i)
            {
                const RCWimVolume& vol = m_volumes[m_xmls[i].m_volIndex];
                const RCWimHeader& header = vol.m_header;
                if (header.IsCompressed())
                {
                    if (header.IsLzxMode())
                    {
                        lzx = true;
                    }
                    else
                    {
                        xpress = true;
                    }
                }
                else
                {
                    copy = true;
                }
            }
            RCString res;
            if (lzx)
            {
                res = s_kWimMethodLZX;
            }
            if (xpress)
            {
                if (!res.empty())
                {
                    res += _T(' ');
                }
                res += s_kWimMethodXpress;
            }
            if (copy)
            {
                if (!res.empty())
                {
                    res += _T(' ');
                }
                res += s_kWimMethodCopy;
            }
            prop = res ;
        }
    case RCPropID::kpidArchiveType:
        {
            prop = uint64_t(RC_ARCHIVE_WIM) ;
            break;
        }
    }
    return RC_S_OK ;
}

HResult RCWimHandler::GetProperty(uint32_t index, RCPropertyID propID, RCVariant& value)
{
    ClearVariantValue(value) ;
    RCVariant& prop = value ;
    if (index < (uint32_t)m_database.m_items.size())
    {
        const RCWimItem& item = m_database.m_items[index];
        const RCWimStreamInfo* si = NULL;
        const RCWimVolume* vol = NULL;
        if (item.m_streamIndex >= 0)
        {
            si = &m_database.m_streams[item.m_streamIndex];
            vol = &m_volumes[si->m_partNumber];
        }

        switch(propID)
        {
        case RCPropID::kpidPath:
            if (item.m_hasMetadata)
            {
                prop = item.m_name;
            }
            else
            {
                RCString::value_type sz[32];
                RCStringUtil::ConvertUInt64ToString(item.m_streamIndex, sz);
                RCString s = sz;
                while ((int32_t)s.size() < m_nameLenForStreams)
                {
                    s = _T('0') + s;
                }
                s = RCString(s_kWimStreamsNamePrefix) + s;
                prop = s ;
                break;
            }
            break;
        case RCPropID::kpidIsDir:
            prop = item.isDir(); 
            break;
        case RCPropID::kpidAttrib: 
            if (item.m_hasMetadata)
            {
                prop = uint64_t(item.m_attrib);
            }
            break;
        case RCPropID::kpidCTime:
            if (item.m_hasMetadata)
            {
                prop = RCFileTime::ConvertFromFileTime(item.m_cTime);
            }
            break;
        case RCPropID::kpidATime:
            if (item.m_hasMetadata)
            {
                prop = RCFileTime::ConvertFromFileTime(item.m_aTime);
            }
            break;
        case RCPropID::kpidMTime:
            if (item.m_hasMetadata)
            {
                prop = RCFileTime::ConvertFromFileTime(item.m_mTime);
            }
            break;
        case RCPropID::kpidPackSize:
            prop = si ? si->m_resource.m_packSize : (uint64_t)0 ;
            break;
        case RCPropID::kpidSize:
            prop = si ? si->m_resource.m_unpackSize : (uint64_t)0;
            break;
        case RCPropID::kpidMethod:
            if (si)
            {
                prop = RCString(si->m_resource.IsCompressed() ?
                                (vol->m_header.IsLzxMode() ? s_kWimMethodLZX : s_kWimMethodXpress) : s_kWimMethodCopy ) ;
            }
            break;
#ifdef WIM_DETAILS
        case RCPropID::kpidVolume:
            if (si)
            {
                prop = (uint64_t)si->m_partNumber;
            }
            break;
        case RCPropID::kpidOffset:
            if (si)
            {
                prop = (uint64_t)si->m_resource.m_offset;
            }
            break;
        case RCPropID::kpidLinks:
            prop = si ? (uint64_t)si->m_refCount : (uint64_t)0;
            break;
#endif
        }
    }
    else
    {
        index -= (uint32_t)m_database.m_items.size();
        {
            switch(propID)
            {
            case RCPropID::kpidPath:
                {
                    RCString::value_type sz[32];
                    RCStringUtil::ConvertUInt64ToString(m_xmls[index].m_volIndex, sz);
                    RCString s = RCString(sz) + _T(".xml") ;
                    prop = s;
                    break;
                }
            case RCPropID::kpidIsDir:
                prop = false ;
                break;
            case RCPropID::kpidPackSize:
            case RCPropID::kpidSize: 
                prop = (uint64_t)m_xmls[index].m_data.GetCapacity(); 
                break;
            case RCPropID::kpidMethod: 
                prop = RCString( _T("Copy") ) ; 
                break;
            }
        }
    }
    return RC_S_OK;
}

HResult RCWimHandler::Open(IInStream* stream, uint64_t maxCheckStartPosition, IArchiveOpenCallback* openArchiveCallback)
{
    try
    {
        Close();
        IArchiveOpenVolumeCallbackPtr openVolumeCallback ;
        RCWimVolumeName seqName ;
        if (openArchiveCallback != NULL)
        {
            openArchiveCallback->QueryInterface(IID_IArchiveOpenVolumeCallback, (void **)openVolumeCallback.GetAddress());
        }

        uint32_t numVolumes = 1 ;
        int32_t firstVolumeIndex = -1 ;
        for (uint32_t i = 1; i <= numVolumes; i++)
        {
            IInStreamPtr curStream ;
            if (i != 1)
            {
                RCString fullName = seqName.GetNextName(i);
                HResult result = openVolumeCallback ? openVolumeCallback->GetStream(fullName, curStream.GetAddress()) : RC_S_FALSE ;
                if (result == RC_S_FALSE)
                {
                    continue;
                }
                if (result != RC_S_OK)
                {
                    return result;
                }
                if (!curStream)
                {
                    break;
                }
            }
            else
            {
                curStream = stream ;
            }
            RCWimHeader header;
            RCWimUtils wimUtils(m_compressCodecsInfo.Get()) ;
            HResult res = wimUtils.ReadHeader(curStream.Get(), header) ;
            if (res != RC_S_OK)
            {
                if (i == 1)
                {
                    return res;
                }
                if (res == RC_S_FALSE)
                {
                    continue;
                }
                return res;
            }
            if (firstVolumeIndex >= 0)
            {
                if (!header.AreFromOnArchive(m_volumes[firstVolumeIndex].m_header))
                {
                    break;
                }
            }
            if (m_volumes.size() > header.m_partNumber && m_volumes[header.m_partNumber].m_stream)
            {
                break ;
            }
            RCWimXml xml;
            xml.m_volIndex = header.m_partNumber;
            res = wimUtils.OpenArchive(curStream.Get(), header, xml.m_data, m_database);
            if (res != RC_S_OK)
            {
                if (i == 1)
                {
                    return res;
                }
                if (res == RC_S_FALSE)
                {
                    continue;
                }
                return res;
            }

            while (m_volumes.size() <= (size_t)header.m_partNumber)
            {
                m_volumes.push_back(RCWimVolume());
            }
            RCWimVolume& volume = m_volumes[header.m_partNumber];
            volume.m_header = header;
            volume.m_stream = curStream;

            firstVolumeIndex = header.m_partNumber;

            bool needAddXml = true;
            if (!m_xmls.empty())
            {
                if (xml.m_data == m_xmls[0].m_data)
                {
                    needAddXml = false;
                }
            }
            if (needAddXml)
            {
                xml.Parse();
                m_xmls.push_back(xml);
            }

            if (i == 1)
            {
                if (header.m_partNumber != 1)
                {
                    break;
                }
                if (!openVolumeCallback)
                {
                    break;
                }
                numVolumes = header.m_numParts;
                {
                    RCVariant prop;
                    HResult hr = openVolumeCallback->GetProperty(RCPropID::kpidName, prop) ;
                    if(!IsSuccess(hr))
                    {
                        return hr ;
                    }
                    if(!IsStringType(prop))
                    {
                        break;
                    }
                    seqName.InitName(GetStringValue(prop));
                }
            }
        }
        RCWimUtils wimUtils(m_compressCodecsInfo.Get()) ;
        HResult hr = wimUtils.SortDatabase(m_database) ;
        if(!IsSuccess(hr))
        {
            return hr ;
        }
        RCString::value_type sz[32] ;
        RCStringUtil::ConvertUInt64ToString(m_database.m_streams.size(), sz);
        m_nameLenForStreams = (int32_t)RCStringUtil::StringLen(sz);
        return RC_S_OK ;
    }
    catch(...)
    {
        return RC_S_FALSE ;
    }
}

HResult RCWimHandler::Close() 
{
    m_database.Clear();
    m_volumes.clear();
    m_xmls.clear();
    m_nameLenForStreams = 0;
    return RC_S_OK ;
}

HResult RCWimHandler::GetNumberOfItems(uint32_t& numItems)
{
    numItems = (uint32_t)(m_database.m_items.size() + m_xmls.size()) ;
    return RC_S_OK ;
}

HResult RCWimHandler::Extract(const std::vector<uint32_t>& indices, int32_t testMode, IArchiveExtractCallback* extractCallback)
{
    uint32_t numItems = (uint32_t)indices.size() ;
    bool allFilesMode = indices.empty() ;
    if (allFilesMode)
    {
        numItems = (uint32_t)(m_database.m_items.size() + m_xmls.size()) ;
    }
    if (numItems == 0)
    {
        return RC_S_OK ;
    }
    bool isTestMode = (testMode != 0) ;

    uint32_t i = 0 ;
    uint64_t totalSize = 0;
    for (i = 0; i < numItems; ++i)
    {
        uint32_t index = allFilesMode ? i : indices[i] ;
        if (index < (uint32_t)m_database.m_items.size())
        {
            int streamIndex = m_database.m_items[index].m_streamIndex;
            if (streamIndex >= 0)
            {
                const RCWimStreamInfo& si = m_database.m_streams[streamIndex];
                totalSize += si.m_resource.m_unpackSize ;
            }
        }
        else
        {
            totalSize += m_xmls[index - (uint32_t)m_database.m_items.size()].m_data.GetCapacity();
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

    uint64_t currentTotalPacked = 0;
    uint64_t currentTotalUnPacked = 0;
    uint64_t currentItemUnPacked = 0 ;
    uint64_t currentItemPacked = 0 ;

    int32_t prevSuccessStreamIndex = -1 ;

    RCWimUnpacker unpacker(m_compressCodecsInfo.Get()) ;
    RCLocalProgress* lps = new RCLocalProgress;
    ICompressProgressInfoPtr progress = lps ;
    lps->Init(extractCallback, false) ;

    for (i = 0; i < numItems; currentTotalUnPacked += currentItemUnPacked,
                              currentTotalPacked += currentItemPacked)
    {
        currentItemUnPacked = 0 ;
        currentItemPacked = 0 ;

        lps->SetInSize(currentTotalPacked) ;
        lps->SetOutSize(currentTotalUnPacked) ;

        HResult hr = lps->SetCur() ;
        if(!IsSuccess(hr))
        {
            return hr ;
        }
        uint32_t index = allFilesMode ? i : indices[i] ;
        i++ ;
        int32_t askMode = isTestMode ? RC_ARCHIVE_EXTRACT_ASK_MODE_TEST : RC_ARCHIVE_EXTRACT_ASK_MODE_EXTRACT;

        ISequentialOutStreamPtr realOutStream;
        if(extractCallback)
        {
            hr = extractCallback->GetStream(index, realOutStream.GetAddress(), askMode) ;
            if(!IsSuccess(hr))
            {
                return hr ;
            }
        }
        if (index >= (uint32_t)m_database.m_items.size())
        {
            if(!isTestMode && (!realOutStream))
            {
                continue;
            }
            if(extractCallback)
            {
                hr = extractCallback->PrepareOperation(index,askMode) ;
                if(!IsSuccess(hr))
                {
                    return hr ;
                }
            }
            const RCByteBuffer& data = m_xmls[index - (uint32_t)m_database.m_items.size()].m_data ;
            currentItemUnPacked = data.GetCapacity() ;
            if (realOutStream)
            {
                hr = RCStreamUtils::WriteStream(realOutStream.Get(), (const byte_t*)data.data(), data.GetCapacity()) ;
                if(!IsSuccess(hr))
                {
                    return hr ;
                }
                realOutStream.Release() ;
            }
            if(extractCallback)
            {
                hr = extractCallback->SetOperationResult(index,RC_ARCHIVE_EXTRACT_RESULT_OK) ;
                if(!IsSuccess(hr))
                {
                    return hr ;
                }
            }
            continue;
        }

        const RCWimItem& item = m_database.m_items[index];
        int32_t streamIndex = item.m_streamIndex ;
        if (streamIndex < 0)
        {
            if(!isTestMode && (!realOutStream))
            {
                continue;
            }
            if(extractCallback)
            {
                hr = extractCallback->PrepareOperation(index,askMode) ;
                if(!IsSuccess(hr))
                {
                    return hr ;
                }
            }
            realOutStream.Release() ;
            if(extractCallback)
            {
                hr = extractCallback->SetOperationResult(index,
                                                         item.HasStream() ?
                                                         RC_ARCHIVE_EXTRACT_RESULT_DATA_ERROR :
                                                         RC_ARCHIVE_EXTRACT_RESULT_OK) ;
                if(!IsSuccess(hr))
                {
                    return hr ;
                }
            }
            continue;
        }

        const RCWimStreamInfo& si = m_database.m_streams[streamIndex];
        currentItemUnPacked = si.m_resource.m_unpackSize;
        currentItemPacked = si.m_resource.m_packSize;

        if(!isTestMode && (!realOutStream))
        {
            continue;
        }
        if(extractCallback)
        {
            hr = extractCallback->PrepareOperation(index,askMode) ;
            if(!IsSuccess(hr))
            {
                return hr ;
            }
        }
        int32_t opRes = RC_ARCHIVE_EXTRACT_RESULT_OK;
        if (streamIndex != prevSuccessStreamIndex || realOutStream)
        {
            byte_t digest[20];
            const RCWimVolume& vol = m_volumes[si.m_partNumber];
            HResult res = unpacker.Unpack(vol.m_stream.Get(), 
                                          si.m_resource, 
                                          vol.m_header.IsLzxMode(),
                                          realOutStream.Get(), 
                                          progress.Get(), 
                                          digest);
            if (res == RC_S_OK)
            {
                if (memcmp(digest, si.m_hash, s_kWimHashSize) == 0)
                {
                    prevSuccessStreamIndex = streamIndex;
                }
                else
                {
                    opRes = RC_ARCHIVE_EXTRACT_RESULT_CRC_ERROR;
                }
            }
            else if (res == RC_S_FALSE)
            {
                opRes = RC_ARCHIVE_EXTRACT_RESULT_DATA_ERROR;
            }
            else
            {
                return res;
            }
        }
        realOutStream.Release();
        if(extractCallback)
        {
            HResult hr = extractCallback->SetOperationResult(index,opRes) ;
            if(!IsSuccess(hr))
            {
                return hr ;
            }
        }
    }
    return RC_S_OK;
}

END_NAMESPACE_RCZIP
