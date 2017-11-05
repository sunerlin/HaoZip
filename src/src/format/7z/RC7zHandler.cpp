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
#include "common/RCSystemUtils.h"
#include "format/7z/RC7zDefs.h"
#include "interface/RCArchiveType.h"
#include "interface/RCMethodDefs.h"
#include "interface/IPassword.h"
#include "interface/RCMethodDefs.h"
#include "common/RCStringUtil.h"
#include "common/RCCreateCoder.h"
#include "format/7z/RC7zProperties.h"
#include "format/7z/RC7zInArchive.h"
#include "format/common/RCItemNameUtils.h"
#include "filesystem/RCFileTime.h"
#include "common/RCMethodIDUtil.h"
#include "format/common/RCPropData.h"
#include "algorithm/CpuArch.h"

/////////////////////////////////////////////////////////////////
//RC7zHandler class implementation

BEGIN_NAMESPACE_RCZIP

static const char_t* s_7zDefaultMethodName = RC_METHOD_NAME_COMPRESS_LZMA;

static const uint32_t s_7zLzmaAlgorithmX5 = 1;
static const char_t*  s_7zLzmaMatchFinderForHeaders = _T("BT2") ;
static const uint32_t s_7zDictionaryForHeaders = 1 << 20;
static const uint32_t s_7zNumFastBytesForHeaders = 273;
static const uint32_t s_7zAlgorithmForHeaders = s_7zLzmaAlgorithmX5;

static RCPropData s_k7zArcProps[] =
{
    { RCString(), RCPropID::kpidMethod,         RCVariantType::RC_VT_STRING},
    { RCString(), RCPropID::kpidSolid,          RCVariantType::RC_VT_BOOL  },
    { RCString(), RCPropID::kpidNumBlocks,      RCVariantType::RC_VT_UINT64},
    { RCString(), RCPropID::kpidPhySize,        RCVariantType::RC_VT_UINT64},
    { RCString(), RCPropID::kpidHeadersSize,    RCVariantType::RC_VT_UINT64},
    { RCString(), RCPropID::kpidOffset,         RCVariantType::RC_VT_UINT64},
    { RCString(), RCPropID::kpidCommented,      RCVariantType::RC_VT_BOOL},
    { RCString(), RCPropID::kpidComment,        RCVariantType::RC_VT_STRING },
    { RCString(), RCPropID::kpidCommentSizePosition, RCVariantType::RC_VT_UINT64 },
    { RCString(), RCPropID::kpidArchiveType,    RCVariantType::RC_VT_UINT64 }
};

RC7zHandler::RC7zHandler():
    m_passwordIsDefined(false),
    m_crcSize(4)    
{
#ifdef COMPRESS_MT
    m_numThreads = RCSystemUtils::GetNumberOfProcessors();
#else
    m_numThreads = 1 ;
#endif

#ifndef RC_STATIC_SFX
    Init() ;
#endif
}

RC7zHandler::~RC7zHandler()
{
}

#ifndef RC_STATIC_SFX

RCString RC7zHandler::ConvertUInt32ToString(uint32_t value)
{
    RCString::value_type buffer[32];
    RCStringUtil::ConvertUInt64ToString(value, buffer);
    return buffer;
}

RCString RC7zHandler::GetStringForSizeValue(uint32_t value)
{
    for (int32_t i = 31; i >= 0; i--)
    {
        if ((uint32_t(1) << i) == value)
        {
            return ConvertUInt32ToString(i);
        }
    }
    RCString result;
    if (value % (1 << 20) == 0)
    {
        result += ConvertUInt32ToString(value >> 20);
        result += _T("m") ;
    }
    else if (value % (1 << 10) == 0)
    {
        result += ConvertUInt32ToString(value >> 10);
        result += _T("k") ;
    }
    else
    {
        result += ConvertUInt32ToString(value);
        result += _T("b") ;
    }
    return result;
}

RCString::value_type RC7zHandler::GetHex(byte_t value)
{
    return (RCString::value_type)((value < 10) ? (_T('0') + value) : (_T('A') + (value - 10)));
}

void RC7zHandler::AddHexToString(RCString& result, byte_t value)
{
    result += GetHex((byte_t)(value >> 4));
    result += GetHex((byte_t)(value&  0xF));
}

#endif //end of RC_STATIC_SFX

void RC7zHandler::SetPropFromUInt64Def(RC7zUInt64DefVector& v, int32_t index, RCVariant& prop)
{
    uint64_t value = 0 ;
    if (v.GetItem(index, value))
    {
        RC_FILE_TIME ft ;
        ft.u32LowDateTime  = (uint32_t)value;
        ft.u32HighDateTime = (uint32_t)(value >> 32);
        prop = RCFileTime::ConvertFromFileTime(ft) ;
    }
}

bool RC7zHandler::IsCopyMethod(const RCString& methodName)
{
    return (RCStringUtil::CompareNoCase(methodName,RC_METHOD_NAME_COMPRESS_COPY) == 0);
}
  
bool RC7zHandler::IsEncrypted(uint32_t index2) const
{
    RC7zNum folderIndex = m_db.m_fileIndexToFolderIndexMap[index2];
    if (folderIndex != RC7zDefs::s_7zNumNoIndex)
    {
        const RC7zFolder& folderInfo = m_db.m_folders[folderIndex];
        for (int32_t i = (int32_t)folderInfo.m_coders.size() - 1; i >= 0; i--)
        {
            if (folderInfo.m_coders[i].m_methodID == RCMethod::ID_CRYPTO_7Z_AES)
            {
                return true;
            }
        }
    }
    return false;
}

#ifndef RC_STATIC_SFX

HResult RC7zHandler::SetPassword(RC7zCompressionMethodMode& methodMode, IArchiveUpdateCallback *updateCallback)
{
    ICryptoGetTextPassword2Ptr getTextPassword ;
    if (!getTextPassword)
    {
        IArchiveUpdateCallbackPtr udateCallback2(updateCallback) ;
        udateCallback2.QueryInterface(IID_ICryptoGetTextPassword2,getTextPassword.GetAddress());
    }

    if (getTextPassword)
    {
        RCString password ;
        int32_t passwordIsDefined = 0 ;
        HResult hr = getTextPassword->CryptoGetTextPassword2(&passwordIsDefined, password) ;
        if(!IsSuccess(hr))
        {
            return hr ;
        }
        methodMode.m_passwordIsDefined = (passwordIsDefined != 0 ) ? true : false ;
        if (methodMode.m_passwordIsDefined)
        {
            methodMode.m_password = password;
        }
    }
    else
    {
        methodMode.m_passwordIsDefined = false;
    }
    return RC_S_OK;
}

HResult RC7zHandler::SetCompressionMethod(RC7zCompressionMethodMode& methodMode, 
                                          RCVector<RCOneMethodInfo>& methodsInfo, 
                                          uint32_t numThreads,
                                          uint64_t totalSize)
{
    uint32_t level = m_level;

    if (methodsInfo.empty())
    {
        RCOneMethodInfo oneMethodInfo;
        oneMethodInfo.m_methodName = ((level == 0) ? RC_METHOD_NAME_COMPRESS_COPY : s_7zDefaultMethodName);
        methodsInfo.push_back(oneMethodInfo);
    }

    bool needSolid = false;
    for(int32_t i = 0; i < (int32_t)methodsInfo.size(); i++)
    {
        RCOneMethodInfo& oneMethodInfo = methodsInfo[i];
        SetCompressionMethod2(oneMethodInfo, numThreads, totalSize);
        if (!IsCopyMethod(oneMethodInfo.m_methodName))
        {
            needSolid = true;
        }

        RC7zArchiveMethodFull methodFull;
        ICodecInfoPtr spCodecInfo ;
        if (!RCCreateCoder::FindMethodByName(m_codecsInfo.Get(),
                                             oneMethodInfo.m_methodName, 
                                             spCodecInfo))
        {
            return RC_E_INVALIDARG ;
        }
        else
        {
            methodFull.m_id = spCodecInfo->GetMethodID() ;
            methodFull.m_numInStreams = spCodecInfo->GetNumInStreams() ; 
            methodFull.m_numOutStreams = spCodecInfo->GetNumOutStreams() ; 
        }
        methodFull.m_properties = oneMethodInfo.m_properties;
        methodMode.m_methods.push_back(methodFull);

        if (!m_numSolidBytesDefined)
        {
            for (int32_t j = 0; j < (int32_t)methodFull.m_properties.size(); j++)
            {
                const RCArchiveProp& prop = methodFull.m_properties[j];
                if ((prop.m_id == RCCoderPropID::kDictionarySize || prop.m_id == RCCoderPropID::kUsedMemorySize ) && 
                    IsInteger64Type(prop.m_value))
                {
                    m_numSolidBytes = ((uint64_t)GetInteger64Value(prop.m_value)) << 7;
                    const uint64_t kMinSize = (1 << 24);
                    if (m_numSolidBytes < kMinSize)
                    {
                        m_numSolidBytes = kMinSize;
                    }
                    m_numSolidBytesDefined = true;
                    break;
                }
            }
        }
    }

    if (!needSolid && !m_numSolidBytesDefined)
    {
        m_numSolidBytesDefined = true;
        m_numSolidBytes  = 0;
    }
    return RC_S_OK;
}

HResult RC7zHandler::SetCompressionMethod(RC7zCompressionMethodMode& methodMode,
                                          RC7zCompressionMethodMode& headerMethod,
                                          uint64_t totalSize)
{
    HResult res = SetCompressionMethod(methodMode, m_methods, m_numThreads, totalSize);
    if(!IsSuccess(res))
    {
        return res ;
    }
    methodMode.m_binds = m_binds ;
    if (m_compressHeaders)
    {
        RCVector<RCOneMethodInfo> headerMethodInfoVector;
        RCOneMethodInfo oneMethodInfo;
        oneMethodInfo.m_methodName = RC_METHOD_NAME_COMPRESS_LZMA ;
        {
            RCArchiveProp property;
            property.m_id = RCCoderPropID::kMatchFinder;
            property.m_value = RCString(s_7zLzmaMatchFinderForHeaders) ;
            oneMethodInfo.m_properties.push_back(property);
        }
        {
            RCArchiveProp property;
            property.m_id = RCCoderPropID::kAlgorithm;
            property.m_value = (uint64_t)s_7zAlgorithmForHeaders ;
            oneMethodInfo.m_properties.push_back(property);
        }
        {
            RCArchiveProp property;
            property.m_id = RCCoderPropID::kNumFastBytes;
            property.m_value = (uint64_t)s_7zNumFastBytesForHeaders ;
            oneMethodInfo.m_properties.push_back(property);
        }
        {
            RCArchiveProp property;
            property.m_id = RCCoderPropID::kDictionarySize;
            property.m_value = (uint64_t)s_7zDictionaryForHeaders ;
            oneMethodInfo.m_properties.push_back(property);
        }
        headerMethodInfoVector.push_back(oneMethodInfo);
        HResult res = SetCompressionMethod(headerMethod, headerMethodInfoVector, 1, totalSize);
        if(!IsSuccess(res))
        {
            return res ;
        }
    }
    return RC_S_OK;
}


#endif //RC_STATIC_SFX

HResult RC7zHandler::Open(IInStream* stream, uint64_t maxCheckStartPosition, IArchiveOpenCallback* openArchiveCallback)
{
    try
    {
        Close();
#ifndef RC_STATIC_SFX
        m_fileInfoPopIDs.clear() ;
#endif
        try
        {
            IArchiveOpenCallbackPtr openArchiveCallbackTemp = openArchiveCallback;
            ICryptoGetTextPasswordPtr getTextPassword;
            if (openArchiveCallback)
            {
                openArchiveCallbackTemp.QueryInterface(IID_ICryptoGetTextPassword, getTextPassword.GetAddress());
            }
            RC7zInArchive archive ;
            HResult hr = archive.Open(stream, &maxCheckStartPosition) ;
            if(!IsSuccess(hr))
            {
                return hr ;
            }
            m_passwordIsDefined = false;
            RCString password;
            HResult result = archive.ReadDatabase(m_codecsInfo.Get(),m_db,getTextPassword.Get(), m_passwordIsDefined) ;
            if(!IsSuccess(result))
            {
                return result ;
            }
            m_db.Fill() ;
            m_inStream = stream ;
        }
        catch(...)
        {
            Close();
            return RC_S_FALSE;
        }
#ifndef RC_STATIC_SFX
        FillPopIDs();
#endif
        return RC_S_OK;
    }
    catch(...)
    {
        return RC_S_FALSE ;
    }
}

HResult RC7zHandler::Close()
{
    m_inStream.Release();
    m_db.Clear();
    return RC_S_OK;
}

HResult RC7zHandler::GetNumberOfItems(uint32_t& numItems)
{
    numItems = (uint32_t)m_db.m_files.size() ;
    return RC_S_OK ;
}

HResult RC7zHandler::DoGetProperty(uint32_t index, RCPropertyID propID, RCVariant& value)
{
    ClearVariantValue(value) ;
    RCVariant& prop = value ;
    const RC7zFileItemPtr& item = m_db.m_files[index] ;
    uint32_t index2 = index ;
    switch(propID)
    {
    case RCPropID::kpidPath:
        if (!item->m_name.empty())
        {
            prop = RCItemNameUtils::GetOSName(item->m_name) ;
        }
        break;
    case RCPropID::kpidIsDir:
        prop = item->m_isDir; 
        break;
    case RCPropID::kpidSize:
        {
            prop = item->m_size;
            break;
        }
    case RCPropID::kpidPackSize:
        {
            RC7zNum folderIndex = m_db.m_fileIndexToFolderIndexMap[index2];
            if (folderIndex != RC7zDefs::s_7zNumNoIndex)
            {
                if (m_db.m_folderStartFileIndex[folderIndex] == (RC7zNum)index2)
                {
                    prop = m_db.GetFolderFullPackSize(folderIndex);
                }
            }
            else
            {
                prop = uint64_t(0) ;
            }
            break;
        }
    case RCPropID::kpidPosition:
        { 
            uint64_t v; 
            if (m_db.m_startPos.GetItem(index2, v)) 
            {
                prop = v;
            }
            break ;
        }
    case RCPropID::kpidCTime:
        SetPropFromUInt64Def(m_db.m_cTime, index2, prop); 
        break;
    case RCPropID::kpidATime:
        SetPropFromUInt64Def(m_db.m_aTime, index2, prop);
        break;
    case RCPropID::kpidMTime:
        SetPropFromUInt64Def(m_db.m_mTime, index2, prop);
        break;
    case RCPropID::kpidAttrib:
        if (item->m_attribDefined)
        {
            prop = uint64_t(item->m_attrib);
        }
        break;
    case RCPropID::kpidCRC:
        if (item->m_crcDefined)
        {
            prop = uint64_t(item->m_crc);
        }
        break;
    case RCPropID::kpidEncrypted:
        prop = IsEncrypted(index2);
        break;
    case RCPropID::kpidIsAnti:
        prop = m_db.IsItemAnti(index2); 
        break;
#ifndef RC_STATIC_SFX
    case RCPropID::kpidMethod:
        {
            RC7zNum folderIndex = m_db.m_fileIndexToFolderIndexMap[index2];
            if (folderIndex != RC7zDefs::s_7zNumNoIndex)
            {
                const RC7zFolder& folderInfo = m_db.m_folders[folderIndex];
                RCString methodsString ;
                for (int32_t i = (int32_t)folderInfo.m_coders.size() - 1; i >= 0; i--)
                {
                    const RC7zCoderInfo& coder = folderInfo.m_coders[i] ;
                    if (!methodsString.empty())
                    {
                        methodsString += _T(' ') ;
                    }

                    RCString methodName ;
                    RCString propsString ;
                    ICodecInfoPtr spCodecInfo ;
                    bool methodIsKnown = RCCreateCoder::FindMethodByID(m_codecsInfo.Get(),
                                                                       coder.m_methodID, 
                                                                       spCodecInfo);

                    if (!methodIsKnown)
                    {
                        methodsString += RCMethodIDUtil::ConvertMethodIdToString(coder.m_methodID) ;
                    }
                    else
                    {
                        methodName = spCodecInfo->GetMethodName() ;
                        methodsString += methodName;
                        if (coder.m_methodID == RCMethod::ID_COMPRESS_DELTA && coder.m_properties.GetCapacity() == 1)
                        {
                            propsString = ConvertUInt32ToString((uint32_t)coder.m_properties[0] + 1);
                        }
                        else if (coder.m_methodID == RCMethod::ID_COMPRESS_LZMA && coder.m_properties.GetCapacity() == 5)
                        {
                            uint32_t dicSize = GetUi32((const byte_t *)coder.m_properties.data() + 1);
                            propsString = GetStringForSizeValue(dicSize);
                        }
                        else if (coder.m_methodID == RCMethod::ID_COMPRESS_LZMA2 && coder.m_properties.GetCapacity() == 1)
                        {
                            byte_t p = coder.m_properties[0];
                            uint32_t dicSize = (((uint32_t)2 | ((p) & 1)) << ((p) / 2 + 11));
                            propsString = GetStringForSizeValue(dicSize);
                        }
                        else if (coder.m_methodID == RCMethod::ID_COMPRESS_PPMD && coder.m_properties.GetCapacity() == 5)
                        {
                            byte_t order = *(const byte_t *)coder.m_properties.data();
                            propsString = _T('o');
                            propsString += ConvertUInt32ToString(order);
                            propsString += _T(":mem") ;
                            uint32_t dicSize = GetUi32((const byte_t *)coder.m_properties.data() + 1);
                            propsString += GetStringForSizeValue(dicSize);
                        }
                        else if (coder.m_methodID == RCMethod::ID_CRYPTO_7Z_AES && coder.m_properties.GetCapacity() >= 1)
                        {
                            const byte_t *data = (const byte_t *)coder.m_properties.data();
                            byte_t firstByte = *data++;
                            uint32_t numCyclesPower = firstByte & 0x3F;
                            propsString = ConvertUInt32ToString(numCyclesPower);
                            /*
                            if ((firstByte & 0xC0) != 0)
                            {
                            uint32_t saltSize = (firstByte >> 7) & 1;
                            uint32_t ivSize = (firstByte >> 6) & 1;
                            if (coder.m_properties.GetCapacity() >= 2)
                            {
                            byte_t secondByte = *data++;
                            saltSize += (secondByte >> 4);
                            ivSize += (secondByte & 0x0F);
                            }
                            }
                            */
                        }
                    }
                    if (!propsString.empty())
                    {
                        methodsString += _T(':') ;
                        methodsString += propsString;
                    }
                    else if (coder.m_properties.GetCapacity() > 0)
                    {
                        methodsString += _T(":[") ;
                        for (size_t bi = 0; bi < coder.m_properties.GetCapacity(); bi++)
                        {
                            if (bi > 5 && bi + 1 < coder.m_properties.GetCapacity())
                            {
                                methodsString += _T("..") ;
                                break;
                            }
                            else
                            {
                                AddHexToString(methodsString, coder.m_properties[bi]);
                            }
                        }
                        methodsString += _T(']');
                    }
                }
                prop = methodsString ;
            }
        }
        break;
    case RCPropID::kpidBlock:
        {
            RC7zNum folderIndex = m_db.m_fileIndexToFolderIndexMap[index2];
            if (folderIndex != RC7zDefs::s_7zNumNoIndex)
            {
                prop = uint64_t(folderIndex) ;
            }
        }
        break;
    case RC7zPropID::kpidPackedSize0:
    case RC7zPropID::kpidPackedSize1:
    case RC7zPropID::kpidPackedSize2:
    case RC7zPropID::kpidPackedSize3:
    case RC7zPropID::kpidPackedSize4:
        {
            RC7zNum folderIndex = m_db.m_fileIndexToFolderIndexMap[index2];
            if (folderIndex != RC7zDefs::s_7zNumNoIndex)
            {
                const RC7zFolder& folderInfo = m_db.m_folders[folderIndex];
                if (m_db.m_folderStartFileIndex[folderIndex] == (RC7zNum)index2 &&
                    folderInfo.m_packStreams.size() > (int32_t)(propID - RC7zPropID::kpidPackedSize0))
                {
                    prop = m_db.GetFolderPackStreamSize(folderIndex, propID - RC7zPropID::kpidPackedSize0);
                }
                else
                {
                    prop = uint64_t(0) ;
                }
            }
            else
            {
                prop = uint64_t(0) ;
            }
        }
        break;
#endif
    }
    return RC_S_OK;
}

HResult RC7zHandler::GetProperty(uint32_t index, RCPropertyID propID, RCVariant& value)
{
    try
    {
        return DoGetProperty(index,propID,value) ;
    }
    catch(...)
    {
        return RC_S_FALSE ;
    }
}

HResult RC7zHandler::GetArchiveProperty(RCPropertyID propID, RCVariant& value)
{
    try
    {
        ClearVariantValue(value) ;
        RCVariant& prop = value ;
        switch(propID)
        {
        case RCPropID::kpidMethod:
            {
                RCString resString;
                RCVector<RCMethodID> ids;
                int32_t i = 0 ;
                for (i = 0; i < (int32_t)m_db.m_folders.size(); ++i)
                {
                    const RC7zFolder& f = m_db.m_folders[i];
                    for (int32_t j = (int32_t)f.m_coders.size() - 1; j >= 0; j--)
                    {
                        RCVectorUtils::AddToUniqueSorted(ids,f.m_coders[j].m_methodID);
                    }
                }

                for (i = 0; i < (int32_t)ids.size(); i++)
                {
                    RCMethodID id = ids[i];
                    RCString methodName;
                    ICodecInfoPtr spCodecInfo ;
                    bool methodIsKnown = RCCreateCoder::FindMethodByID(m_codecsInfo.Get(),
                                                                       id, 
                                                                       spCodecInfo);
                    if(methodIsKnown && (spCodecInfo != NULL))
                    {
                        methodName = spCodecInfo->GetMethodName() ;
                    }
                    if (methodName.empty())
                    {
                        methodName = RCMethodIDUtil::ConvertMethodIdToString(id) ;
                    }
                    if (!resString.empty())
                    {
                        resString += _T(' ') ;
                    }
                    resString += methodName ;
                }
                prop = resString ;
                break;
            }
        case RCPropID::kpidSolid: 
            prop = m_db.IsSolid(); 
            break;
        case RCPropID::kpidNumBlocks: 
            prop = uint64_t(m_db.m_folders.size()); 
            break;
        case RCPropID::kpidHeadersSize:  
            prop = m_db.m_headersSize; 
            break;
        case RCPropID::kpidPhySize:  
            prop = m_db.m_phySize; 
            break;
        case RCPropID::kpidOffset: 
            if (m_db.m_archiveInfo.m_startPosition != 0) 
            {
                prop = m_db.m_archiveInfo.m_startPosition;
            }
            break;
        case RCPropID::kpidCommented:
            prop = m_db.m_isCommented;
            break;
        case RCPropID::kpidComment:
            prop = m_db.m_comment;
            break;
        case RCPropID::kpidCommentSizePosition:
            prop = m_db.m_commentStartPos;
            break;
        case RCPropID::kpidArchiveType:
            prop = uint64_t(RC_ARCHIVE_7Z) ;
            break;
        }
        return RC_S_OK;
    }
    catch(...)
    {
        return RC_S_FALSE ;
    }
}

HResult RC7zHandler::GetNumberOfArchiveProperties(uint32_t& numProperties)
{
    numProperties = sizeof(s_k7zArcProps) / sizeof(s_k7zArcProps[0]);
    return RC_S_OK ;
}

HResult RC7zHandler::GetArchivePropertyInfo(uint32_t index, RCString& name, RCPropertyID& propID, RCVariantType::RCVariantID& varType)
{
    if(index >= (sizeof(s_k7zArcProps) / sizeof(s_k7zArcProps[0])))
    {
        return RC_E_INVALIDARG ;
    }
    const RCPropData& srcItem = s_k7zArcProps[index];
    name = srcItem.m_propName ;
    propID = srcItem.m_propID ;
    varType = srcItem.m_varType ;
    
    return RC_S_OK ;
}

HResult RC7zHandler::SetCompressCodecsInfo(ICompressCodecsInfo* compressCodecsInfo)
{
    m_codecsInfo = compressCodecsInfo ;
    return RC_S_OK ;
}

END_NAMESPACE_RCZIP
