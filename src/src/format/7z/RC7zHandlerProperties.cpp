/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "format/7z/RC7zHandler.h"
#include "format/common/RCPropData.h"
#include "format/7z/RC7zHeader.h"
#include "format/7z/RC7zProperties.h"

/////////////////////////////////////////////////////////////////
//RC7zHandler class implementation for properties

BEGIN_NAMESPACE_RCZIP

struct RC7zPropMap
{
    /** 文件属性ID
    */
    uint64_t m_filePropID;

    /** 属性值
    */
    RCPropData m_propData;
};

RC7zPropMap g_7zPropMap[] =
{
    { RC7zID::kName,     RCString(), RCPropID::kpidPath,        RCVariantType::RC_VT_STRING},
    { RC7zID::kSize,     RCString(), RCPropID::kpidSize,        RCVariantType::RC_VT_UINT64},
    { RC7zID::kPackInfo, RCString(), RCPropID::kpidPackSize,    RCVariantType::RC_VT_UINT64},

#ifdef _MULTI_PACK
    { 100, RCString(_T("Pack0")), RC7zPropID::kpidPackedSize0, RCVariantType::RC_VT_UINT64},
    { 101, RCString(_T("Pack1")), RC7zPropID::kpidPackedSize1, RCVariantType::RC_VT_UINT64},
    { 102, RCString(_T("Pack2")), RC7zPropID::kpidPackedSize2, RCVariantType::RC_VT_UINT64},
    { 103, RCString(_T("Pack3")), RC7zPropID::kpidPackedSize3, RCVariantType::RC_VT_UINT64},
    { 104, RCString(_T("Pack4")), RC7zPropID::kpidPackedSize4, RCVariantType::RC_VT_UINT64},
#endif

    { RC7zID::kCTime,         RCString(), RCPropID::kpidCTime,    RCVariantType::RC_VT_UINT64},
    { RC7zID::kMTime,         RCString(), RCPropID::kpidMTime,    RCVariantType::RC_VT_UINT64},
    { RC7zID::kATime,         RCString(), RCPropID::kpidATime,    RCVariantType::RC_VT_UINT64},
    { RC7zID::kWinAttributes, RCString(), RCPropID::kpidAttrib,   RCVariantType::RC_VT_UINT64},
    { RC7zID::kStartPos,      RCString(), RCPropID::kpidPosition, RCVariantType::RC_VT_UINT64},
    { RC7zID::kCRC,           RCString(), RCPropID::kpidCRC,      RCVariantType::RC_VT_UINT64},
    { RC7zID::kAnti,          RCString(), RCPropID::kpidIsAnti,   RCVariantType::RC_VT_BOOL},

#ifndef RC_STATIC_SFX
    { 97, RCString(), RCPropID::kpidEncrypted,  RCVariantType::RC_VT_BOOL},
    { 98, RCString(), RCPropID::kpidMethod,     RCVariantType::RC_VT_STRING},
    { 99, RCString(), RCPropID::kpidBlock,      RCVariantType::RC_VT_UINT64}
#endif
};

static const int32_t s_propMapSize = sizeof(g_7zPropMap) / sizeof(g_7zPropMap[0]);

static int32_t FindPropInMap(uint64_t filePropID)
{
    for (int32_t i = 0; i < s_propMapSize; i++)
    {
        if (g_7zPropMap[i].m_filePropID == filePropID)
        {
            return i;
        }
    }
    return -1;
}

static void CopyOneItem(RCVector<uint64_t>& src,
                        RCVector<uint64_t>& dest, uint32_t item)
{
    for (int32_t i = 0; i < (int32_t)src.size(); i++)
    {
        if (src[i] == item)
        {
            dest.push_back(item);
            RCVectorUtils::Delete(src,i);
            return;
        }
    }
}

static void RemoveOneItem(RCVector<uint64_t>& src, uint32_t item)
{
    for (int32_t i = 0; i < (int32_t)src.size(); i++)
    {
        if (src[i] == item)
        {
            RCVectorUtils::Delete(src,i);
            return;
        }
    }
}

#ifndef RC_STATIC_SFX

static void InsertToHead(RCVector<uint64_t>& dest, uint32_t item)
{
    for (int32_t i = 0; i < (int32_t)dest.size(); i++)
    {
        if (dest[i] == item)
        {
            RCVectorUtils::Delete(dest,i);
            break;
        }
    }
    dest.insert(dest.begin(), item);
}

#endif

void RC7zHandler::FillPopIDs()
{
    m_fileInfoPopIDs.clear();

#ifdef _7Z_VOL
    if(m_volumes.size() < 1)
    {
        return;
    }
    const CVolume& volume = _volumes.Front();
    const RC7zArchiveDatabaseEx& m_db = volume.Database;
#endif

    RCVector<uint64_t> fileInfoPopIDs = m_db.m_archiveInfo.m_fileInfoPopIDs;

    RemoveOneItem(fileInfoPopIDs, RC7zID::kEmptyStream);
    RemoveOneItem(fileInfoPopIDs, RC7zID::kEmptyFile);

    CopyOneItem(fileInfoPopIDs, m_fileInfoPopIDs, RC7zID::kName);
    CopyOneItem(fileInfoPopIDs, m_fileInfoPopIDs, RC7zID::kAnti);
    CopyOneItem(fileInfoPopIDs, m_fileInfoPopIDs, RC7zID::kSize);
    CopyOneItem(fileInfoPopIDs, m_fileInfoPopIDs, RC7zID::kPackInfo);
    CopyOneItem(fileInfoPopIDs, m_fileInfoPopIDs, RC7zID::kCTime);
    CopyOneItem(fileInfoPopIDs, m_fileInfoPopIDs, RC7zID::kMTime);
    CopyOneItem(fileInfoPopIDs, m_fileInfoPopIDs, RC7zID::kATime);
    CopyOneItem(fileInfoPopIDs, m_fileInfoPopIDs, RC7zID::kWinAttributes);
    CopyOneItem(fileInfoPopIDs, m_fileInfoPopIDs, RC7zID::kCRC);
    CopyOneItem(fileInfoPopIDs, m_fileInfoPopIDs, RC7zID::kComment);
    m_fileInfoPopIDs.insert(m_fileInfoPopIDs.end(),fileInfoPopIDs.begin(),fileInfoPopIDs.end() );

#ifndef RC_STATIC_SFX
    m_fileInfoPopIDs.push_back(97);
    m_fileInfoPopIDs.push_back(98);
    m_fileInfoPopIDs.push_back(99);
#endif
#ifdef _MULTI_PACK
    m_fileInfoPopIDs.push_back(100);
    m_fileInfoPopIDs.push_back(101);
    m_fileInfoPopIDs.push_back(102);
    m_fileInfoPopIDs.push_back(103);
    m_fileInfoPopIDs.push_back(104);
#endif

#ifndef RC_STATIC_SFX
    InsertToHead(m_fileInfoPopIDs, RC7zID::kMTime);
    InsertToHead(m_fileInfoPopIDs, RC7zID::kPackInfo);
    InsertToHead(m_fileInfoPopIDs, RC7zID::kSize);
    InsertToHead(m_fileInfoPopIDs, RC7zID::kName);
#endif
}

HResult RC7zHandler::GetNumberOfProperties(uint32_t& numProperties)
{
    numProperties = (uint32_t)m_fileInfoPopIDs.size();
    return RC_S_OK;
}

HResult RC7zHandler::GetPropertyInfo(uint32_t index, RCString& name, RCPropertyID& propID, RCVariantType::RCVariantID& varType)
{
    if ((int32_t)index >= m_fileInfoPopIDs.size())
    {
        return RC_E_INVALIDARG;
    }
    int32_t indexInMap = FindPropInMap(m_fileInfoPopIDs[index]);
    if (indexInMap == -1)
    {
        return RC_E_INVALIDARG;
    }
    const RCPropData& srcItem = g_7zPropMap[indexInMap].m_propData;
    propID  = srcItem.m_propID;
    varType = srcItem.m_varType ;
    name = srcItem.m_propName ;
    return RC_S_OK;
}

END_NAMESPACE_RCZIP
