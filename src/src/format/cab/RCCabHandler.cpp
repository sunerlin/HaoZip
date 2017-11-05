/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#include "format/cab/RCCabHandler.h"
#include "format/cab/RCCabHeader.h"
#include "format/cab/RCCabBlockInStream.h"
#include "format/cab/RCCabFolderOutStream.h"
#include "interface/RCPropertyID.h"
#include "common/RCStringConvert.h"
#include "common/RCStringUtil.h"
#include "common/RCLocalProgress.h"
#include "common/RCCreateCoder.h"
#include "format/common/RCItemNameUtils.h"
#include "format/common/RCPropData.h"
#include "filesystem/RCFileTime.h"
#include "interface/RCArchiveType.h"

BEGIN_NAMESPACE_RCZIP

#ifdef _CAB_DETAILS
enum
{
    kpidBlockReal = RCPropID::kpidUserDefined
};
#endif

static RCPropData s_cabProps[] =
{
    { RCString(_T("")), RCPropID::kpidPath, RCVariantType::RC_VT_STRING},
    { RCString(_T("")), RCPropID::kpidSize, RCVariantType::RC_VT_UINT64},
    { RCString(_T("")), RCPropID::kpidMTime, RCVariantType::RC_VT_UINT64},
    { RCString(_T("")), RCPropID::kpidAttrib, RCVariantType::RC_VT_UINT64},
    { RCString(_T("")), RCPropID::kpidMethod, RCVariantType::RC_VT_STRING},
    { RCString(_T("")), RCPropID::kpidBlock, RCVariantType::RC_VT_INT64}
#ifdef _CAB_DETAILS
    ,
    { RCString(_T("BlockReal")), kpidBlockReal, RCVariantType::RC_VT_UINT64},
    { RCString(_T("")), RCPropID::kpidOffset, RCVariantType::RC_VT_UINT64},
    { RCString(_T("")), RCPropID::kpidVolume, RCVariantType::RC_VT_UINT64}
#endif
};

static const wchar_t* s_cabMethods[] =
{
    L"None",
    L"MSZip",
    L"Quantum",
    L"LZX"
};
static const int32_t s_kCabNumMethods = (sizeof(s_cabMethods) / sizeof(s_cabMethods[0]));
static const wchar_t* s_cabUnknownMethod = L"Unknown";

static RCPropData s_cabArcProps[] =
{
    { RCString(_T("")), RCPropID::kpidMethod, RCVariantType::RC_VT_STRING},
    { RCString(_T("")), RCPropID::kpidNumBlocks, RCVariantType::RC_VT_UINT64},
    { RCString(_T("")), RCPropID::kpidNumVolumes, RCVariantType::RC_VT_UINT64},
    { RCString(_T("")), RCPropID::kpidArchiveType,    RCVariantType::RC_VT_UINT64 }
};

HResult RCCabHandler::GetNumberOfProperties(uint32_t& numProperties) 
{
    numProperties = sizeof(s_cabProps) / sizeof(s_cabProps[0]);
    return RC_S_OK;
}

HResult RCCabHandler::GetPropertyInfo(uint32_t index, RCString& name, RCPropertyID& propID, RCVariantType::RCVariantID& varType)
{
    if (index >= (sizeof(s_cabProps) / sizeof(s_cabProps[0])))
    {
        return RC_E_INVALIDARG;
    }
    const RCPropData& srcItem = s_cabProps[index];
    propID = srcItem.m_propID;
    varType = srcItem.m_varType;
    name = srcItem.m_propName;
    return RC_S_OK;
}

HResult RCCabHandler::GetNumberOfArchiveProperties(uint32_t& numProperties) 
{
    numProperties = sizeof(s_cabArcProps) / sizeof(s_cabArcProps[0]);
    return RC_S_OK;
}

HResult RCCabHandler::GetArchivePropertyInfo(uint32_t index, 
                                             RCString& name, 
                                             RCPropertyID& propID, 
                                             RCVariantType::RCVariantID& varType)
{
    if (index >= (sizeof(s_cabArcProps) / sizeof(s_cabArcProps[0])))
    {
        return RC_E_INVALIDARG;
    }
    const RCPropData& srcItem = s_cabArcProps[index];
    propID = srcItem.m_propID;
    varType = srcItem.m_varType;
    name = srcItem.m_propName;
    return RC_S_OK;
}

HResult RCCabHandler::Open(IInStream* inStream, uint64_t maxCheckStartPosition, IArchiveOpenCallback* callback)
{
    Close();
    HResult res = RC_S_FALSE;
    RCCabInArchive archive;
    IArchiveOpenVolumeCallbackPtr openVolumeCallback;
    {
        IArchiveOpenCallbackPtr openArchiveCallbackWrap = callback;
        openArchiveCallbackWrap.QueryInterface(IID_IArchiveOpenVolumeCallback, openVolumeCallback.GetAddress());
    }

    IInStreamPtr nextStream = inStream;
    bool prevChecked = false;
    uint64_t numItems = 0;
    try
    {
        while(nextStream != 0)
        {
            RCCabDatabaseEx db;
            db.m_stream = nextStream;
            res = archive.Open(&maxCheckStartPosition, db);
            if (IsSuccess(res))
            {
                if (!m_database.m_volumes.empty())
                {
                    const RCCabDatabaseEx &dbPrev = m_database.m_volumes[prevChecked ? m_database.m_volumes.size() - 1 : 0];
                    if (dbPrev.m_archiveInfo.m_setID != db.m_archiveInfo.m_setID ||
                            dbPrev.m_archiveInfo.m_cabinetNumber + (prevChecked ? 1: - 1) !=
                            db.m_archiveInfo.m_cabinetNumber)
                    {
                        res = RC_S_FALSE;
                    }
                }
            }
            if (IsSuccess(res))
            {
                RCVectorUtils::Insert(m_database.m_volumes, prevChecked ? static_cast<int32_t>(m_database.m_volumes.size()) : 0, db);
            }
            else if (res != RC_S_FALSE)
            {
                return res;
            }
            else
            {
                if (m_database.m_volumes.empty())
                {
                    return RC_S_FALSE;
                }
                if (prevChecked)
                {
                    break;
                }
                prevChecked = true;
            }

            numItems += db.m_items.size();
            {
                HResult __result_ = callback->SetCompleted(numItems, NULL);
                if (!IsSuccess(__result_))
                {
                    return __result_;
                }
            }

            nextStream = 0;
            for (;;)
            {
                const RCCabOtherArchive *otherArchive = 0;
                if (!prevChecked)
                {
                    const RCCabInArchiveInfo &ai = m_database.m_volumes.front().m_archiveInfo;
                    if (ai.IsTherePrev())
                    {
                        otherArchive = &ai.m_prevArc ;
                    }
                    else
                    {
                        prevChecked = true;
                    }
                }
                if (otherArchive == 0)
                {
                    const RCCabInArchiveInfo &ai = m_database.m_volumes.back().m_archiveInfo;
                    if (ai.IsThereNext())
                    {
                        otherArchive = &ai.m_nextArc ;
                    }
                }
                if (!otherArchive)
                {
                    break;
                }
                const RCString fullName = RCStringConvert::MultiByteToUnicodeString(otherArchive->m_fileName, RC_CP_ACP);
                HResult result = openVolumeCallback->GetStream(fullName, nextStream.GetAddress());
                if (IsSuccess(result))
                {
                    break;
                }
                if (result != RC_S_FALSE)
                {
                    return result;
                }
                if (prevChecked)
                {
                    break;
                }
                prevChecked = true;
            }
        }
        if (IsSuccess(res))
        {
            m_database.FillSortAndShrink();
            if (!m_database.Check())
            {
                res = RC_S_FALSE;
            }
        }
    }
    catch(...)
    {
        res = RC_S_FALSE;
    }
    if (!IsSuccess(res))
    {
        Close();
        return res;
    }
    return RC_S_OK;
}

HResult RCCabHandler::Close()
{
    m_database.Clear();
    return RC_S_OK;
}

HResult RCCabHandler::GetNumberOfItems(uint32_t& numItems)
{
    numItems = static_cast<uint32_t>(m_database.m_items.size());
    return RC_S_OK;
}

HResult RCCabHandler::GetProperty(uint32_t index, RCPropertyID propID, RCVariant& value)
{
    ClearVariantValue(value) ;
    RCVariant& prop = value ;
    const RCCabMvItem &mvItem = m_database.m_items[index];
    const RCCabDatabaseEx &db = m_database.m_volumes[mvItem.m_volumeIndex];
    int32_t itemIndex = mvItem.m_itemIndex;
    const RCCabItem &item = db.m_items[itemIndex];
    switch(propID)
    {
        case RCPropID::kpidPath:
        {
            RCString unicodeName;
            if (item.IsNameUTF())
            {
                RCStringConvert::ConvertUTF8ToUnicode(item.m_name.c_str(), item.m_name.size(), unicodeName);
            }
            else
            {
                unicodeName = RCStringConvert::MultiByteToUnicodeString(item.m_name, RC_CP_ACP);
            }
            prop = RCItemNameUtils::WinNameToOSName(unicodeName);
            break;
        }
        case RCPropID::kpidIsDir:  
            prop = item.IsDir(); 
            break;
        case RCPropID::kpidSize:  
            prop = static_cast<uint64_t>(item.m_size); 
            break;
        case RCPropID::kpidAttrib:  
            prop = static_cast<uint64_t>(item.GetWinAttributes()); 
            break;
        case RCPropID::kpidMTime:
        {
            RC_FILE_TIME localFileTime, utcFileTime;
            if (RCFileTime::DosTimeToFileTime(item.m_time, localFileTime))
            {
                if (!RCFileTime::LocalFileTimeToFileTime(localFileTime, utcFileTime))
                {
                    utcFileTime.u32HighDateTime = 0;
                    utcFileTime.u32LowDateTime = 0;
                }
            }
            else
            {
                utcFileTime.u32HighDateTime = 0;
                utcFileTime.u32LowDateTime = 0;
            }
            uint64_t ltime;
            ltime = utcFileTime.u32HighDateTime;
            ltime = ltime << 32;
            ltime += utcFileTime.u32LowDateTime;
            prop = ltime;
            break;
        }

        case RCPropID::kpidMethod:
        {
            uint32_t realFolderIndex = item.GetFolderIndex(static_cast<int32_t>(db.m_folders.size()));
            const RCCabFolder &folder = db.m_folders[realFolderIndex];
            int32_t methodIndex = folder.GetCompressionMethod();
            RCString method = (methodIndex < s_kCabNumMethods) ? s_cabMethods[methodIndex] : s_cabUnknownMethod;
            if (methodIndex == RCCabHeaderDefs::kLZX || methodIndex == RCCabHeaderDefs::kQuantum)
            {
                method += _T(":");
                wchar_t temp[32];
                RCStringUtil::ConvertUInt64ToString(folder.m_compressionTypeMinor, temp);
                method += temp;
            }
            prop = method;
            break;
        }
        case RCPropID::kpidBlock:  
            prop = static_cast<int64_t>(m_database.GetFolderIndex(&mvItem)); 
            break;

#ifdef _CAB_DETAILS
        case RCPropID::kpidBlockReal:  
            prop = static_cast<uint32_t>(item.m_folderIndex); 
            break;
        case RCPropID::kpidOffset:  
            prop = static_cast<uint32_T>(item.m_offset); 
            break;
        case RCPropID::kpidVolume:  
            prop = static_cast<uint32_t>(mvItem.m_volumeIndex); 
            break;
#endif
    }
    return RC_S_OK;
}

HResult RCCabHandler::Extract(const RCVector<uint32_t>& indices, 
                              int32_t testModeSpec, 
                              IArchiveExtractCallback* extractCallback) 
{
    bool allFilesMode = indices.empty();
    uint32_t numItems;
    if (allFilesMode)
    {
        numItems = static_cast<uint32_t>(m_database.m_items.size());
    }
    else
    {
        //numItems = static_cast<uint32_t>(m_database.m_items.size());
        numItems = static_cast<uint32_t>(indices.size());
    }
    if(numItems == 0)
    {
        return RC_S_OK;
    }
    bool testMode = (testModeSpec != 0);
    uint64_t totalUnPacked = 0;
    uint32_t i;
    int32_t lastFolder = -2;
    uint64_t lastFolderSize = 0;
    for(i = 0; i < numItems; i++)
    {
        int32_t index = allFilesMode ? i : indices[i];
        const RCCabMvItem &mvItem = m_database.m_items[index];
        const RCCabItem &item = m_database.m_volumes[mvItem.m_volumeIndex].m_items[mvItem.m_itemIndex];
        if (item.IsDir())
        {
            continue;
        }
        int32_t folderIndex = m_database.GetFolderIndex(&mvItem);
        if (folderIndex != lastFolder)
        {
            totalUnPacked += lastFolderSize;
        }
        lastFolder = folderIndex;
        lastFolderSize = item.GetEndOffset();
    }
    totalUnPacked += lastFolderSize;
    extractCallback->SetTotal(totalUnPacked);

    totalUnPacked = 0;
    uint64_t totalPacked = 0;
    RCLocalProgress *lps = new RCLocalProgress;
    ICompressProgressInfoPtr progress = lps;
    lps->Init(extractCallback, false);

    if (!m_compressCodecsInfo)
    {
        return RC_S_FALSE;
    }

    ICompressCoderPtr copyCoder;
    HResult rs = RCCreateCoder::CreateCoder(m_compressCodecsInfo.Get(),RCMethod::ID_COMPRESS_COPY, copyCoder, false);
    if (!IsSuccess(rs))
    {
        return rs;
    }

    ICompressCoder *deflateDecoderSpec = NULL;
    ICompressCoderPtr deflateDecoder;

    ICompressCoder *lzxDecoderSpec = NULL;
    ICompressCoderPtr lzxDecoder;

    ICompressCoder *quantumDecoderSpec = NULL;
    ICompressCoderPtr quantumDecoder;

    ICompressSetCoderPropertiesPtr propset;
    RCPropertyIDPairArray proparray;

    RCCabBlockInStream *cabBlockInStreamSpec = new RCCabBlockInStream();
    ISequentialInStreamPtr cabBlockInStream = cabBlockInStreamSpec;
    if (!cabBlockInStreamSpec->Create())
    {
        return RC_E_OUTOFMEMORY;
    }

    RCVector<bool> extractStatuses;
    for(i = 0; i < numItems;)
    {
        int32_t index = allFilesMode ? i : indices[i];
        const RCCabMvItem &mvItem = m_database.m_items[index];
        const RCCabDatabaseEx &db = m_database.m_volumes[mvItem.m_volumeIndex];
        int32_t itemIndex = mvItem.m_itemIndex;
        const RCCabItem &item = db.m_items[itemIndex];

        i++;
        if (item.IsDir())
        {
            int32_t askMode= testMode ?
            RC_ARCHIVE_EXTRACT_ASK_MODE_TEST :
            RC_ARCHIVE_EXTRACT_ASK_MODE_EXTRACT;
            ISequentialOutStreamPtr realOutStream;
            rs = extractCallback->GetStream(index, realOutStream.GetAddress(), askMode);
            if (!IsSuccess(rs))
            {
                return rs;
            }
            rs = extractCallback->PrepareOperation(index,askMode);
            if (!IsSuccess(rs))
            {
                return rs;
            }
            realOutStream.Release();
            rs = extractCallback->SetOperationResult(index,RC_ARCHIVE_EXTRACT_RESULT_OK);
            if (!IsSuccess(rs))
            {
                return rs;
            }
            continue;
        }
        int32_t folderIndex = m_database.GetFolderIndex(&mvItem);
        if (folderIndex < 0)
        {
            // If we need previous archive
            int32_t askMode= testMode ?
            RC_ARCHIVE_EXTRACT_ASK_MODE_TEST :
            RC_ARCHIVE_EXTRACT_ASK_MODE_EXTRACT;
            ISequentialOutStreamPtr realOutStream;
            rs = extractCallback->GetStream(index, realOutStream.GetAddress(), askMode);
            if (!IsSuccess(rs))
            {
                return rs;
            }
            rs = extractCallback->PrepareOperation(index,askMode);
            if (!IsSuccess(rs))
            {
                return rs;
            }
            realOutStream.Release();
            rs = extractCallback->SetOperationResult(index,RC_ARCHIVE_EXTRACT_RESULT_DATA_ERROR);
            if (!IsSuccess(rs))
            {
                return rs;
            }
            continue;
        }
        int32_t startIndex2 = m_database.m_folderStartFileIndex[folderIndex];
        int32_t startIndex = startIndex2;
        extractStatuses.clear();
        for (; startIndex < index; startIndex++)
        {
            extractStatuses.push_back(false);
        }
        extractStatuses.push_back(true);
        startIndex++;
        uint64_t curUnpack = item.GetEndOffset();
        for(;i < numItems; i++)
        {
            int32_t indexNext = allFilesMode ? i : indices[i];
            const RCCabMvItem &mvItem = m_database.m_items[indexNext];
            const RCCabItem &item = m_database.m_volumes[mvItem.m_volumeIndex].m_items[mvItem.m_itemIndex];
            if (item.IsDir())
            {
                continue;
            }
            int32_t newFolderIndex = m_database.GetFolderIndex(&mvItem);

            if (newFolderIndex != folderIndex)
            {
                break;
            }
            for (; startIndex < indexNext; startIndex++)
            {
                extractStatuses.push_back(false);
            }
            extractStatuses.push_back(true);
            startIndex++;
            curUnpack = item.GetEndOffset();
        }

        lps->SetOutSize(totalUnPacked);
        lps->SetInSize(totalPacked);
        rs = lps->SetCur();
        if (!IsSuccess(rs))
        {
            return rs;
        }

        RCCabFolderOutStream *cabFolderOutStream = new RCCabFolderOutStream;
        ISequentialOutStreamPtr outStream(cabFolderOutStream);
        const RCCabFolder &folder = db.m_folders[item.GetFolderIndex(static_cast<int32_t>(db.m_folders.size()))];
        cabFolderOutStream->Init(&m_database, &extractStatuses, startIndex2, 
            curUnpack, extractCallback, testMode);

        cabBlockInStreamSpec->m_msZip = false;
        switch(folder.GetCompressionMethod())
        {
            case RCCabHeaderDefs::kNone:
                break;
            case RCCabHeaderDefs::kMSZip:
                if(deflateDecoderSpec == NULL)
                {
                    //deflateDecoderSpec = new NCompress::NDeflate::NDecoder::CCOMCoder;
                    //deflateDecoder = deflateDecoderSpec;
                    rs = RCCreateCoder::CreateCoder(m_compressCodecsInfo.Get(),RCMethod::ID_COMPRESS_DEFLATE_COM, deflateDecoder, false);
                    if (!IsSuccess(rs))
                    {
                        return rs;
                    }
                }
                cabBlockInStreamSpec->m_msZip = true;
                break;
            case RCCabHeaderDefs::kLZX:
                if(lzxDecoderSpec == NULL)
                {
                    //lzxDecoderSpec = new NCompress::NLzx::CDecoder;
                    //lzxDecoder = lzxDecoderSpec;
                    rs = RCCreateCoder::CreateCoder(m_compressCodecsInfo.Get(),RCMethod::ID_COMPRESS_LZX, lzxDecoder, false);
                    if (!IsSuccess(rs))
                    {
                        return rs;
                    }
                }
                rs = lzxDecoder.QueryInterface(IID_ICompressSetCoderProperties, propset.GetAddress());
                if (!IsSuccess(rs))
                {
                    return rs;
                }
                proparray.clear();
                proparray.push_back(RCPropertyIDPairArray::value_type(RCMethodProperty::ID_SET_PARAMS, static_cast<uint64_t>(folder.m_compressionTypeMinor)));
                //rs = lzxDecoderSpec->SetParams(folder.m_compressionTypeMinor);
                rs = propset->SetCoderProperties(proparray);
                if (!IsSuccess(rs))
                {
                    return rs;
                }
                break;
            case RCCabHeaderDefs::kQuantum:
                if(quantumDecoderSpec == NULL)
                {
                    //quantumDecoderSpec = new NCompress::NQuantum::CDecoder;
                    //quantumDecoder = quantumDecoderSpec;
                    rs = RCCreateCoder::CreateCoder(m_compressCodecsInfo.Get(),RCMethod::ID_COMPRESS_QUANTUM, quantumDecoder, false);
                    if (!IsSuccess(rs))
                    {
                        return rs;
                    }
                }
                rs = quantumDecoder.QueryInterface(IID_ICompressSetCoderProperties, propset.GetAddress());
                if (!IsSuccess(rs))
                {
                    return rs;
                }
                proparray.clear();
                proparray.push_back(RCPropertyIDPairArray::value_type(RCMethodProperty::ID_SET_PARAMS, static_cast<uint64_t>(folder.m_compressionTypeMinor)));
                //quantumDecoderSpec->SetParams(folder.m_compressionTypeMinor);
                rs = propset->SetCoderProperties(proparray);
                if (!IsSuccess(rs))
                {
                    return rs;
                }
                break;
            default:
                {
                    rs = cabFolderOutStream->Unsupported();
                    if (!IsSuccess(rs))
                    {
                        return rs;
                    }
                    totalUnPacked += curUnpack;
                    continue;
                }
        }
        cabBlockInStreamSpec->InitForNewFolder();
        HResult res = RC_S_OK;
        {
            int32_t volIndex = mvItem.m_volumeIndex;
            int32_t locFolderIndex = item.GetFolderIndex(static_cast<int32_t>(db.m_folders.size()));
            bool keepHistory = false;
            bool keepInputBuffer = false;
            for (uint32_t f = 0; cabFolderOutStream->GetRemain() != 0;)
            {
                if (volIndex >= static_cast<int32_t>(m_database.m_volumes.size()))
                {
                    res = RC_S_FALSE;
                    break;
                }

                const RCCabDatabaseEx &db = m_database.m_volumes[volIndex];
                const RCCabFolder &folder = db.m_folders[locFolderIndex];
                if (f == 0)
                {
                    cabBlockInStreamSpec->SetStream(db.m_stream.Get());
                    cabBlockInStreamSpec->m_reservedSize = db.m_archiveInfo.GetDataBlockReserveSize();
                    rs = db.m_stream->Seek(db.m_startPosition + folder.m_dataStart, RC_STREAM_SEEK_SET, NULL);
                    if (!IsSuccess(rs))
                    {
                        return rs;
                    }
                }
                if (f == folder.m_numDataBlocks)
                {
                    volIndex++;
                    locFolderIndex = 0;
                    f = 0;
                    continue;
                }
                f++;
                cabBlockInStreamSpec->m_dataError = false;
                if (!keepInputBuffer)
                {
                    cabBlockInStreamSpec->InitForNewBlock();
                }
                uint32_t packSize, unpackSize;
                res = cabBlockInStreamSpec->PreRead(packSize, unpackSize);
                if (res == RC_S_FALSE)
                {
                    break;
                }
                if (!IsSuccess(res))
                {
                    return res;
                }
                keepInputBuffer = (unpackSize == 0);
                if (keepInputBuffer)
                {
                    continue;
                }

                uint64_t totalUnPacked2 = totalUnPacked + cabFolderOutStream->GetPosInFolder();
                totalPacked += packSize;

                lps->SetOutSize(totalUnPacked2);
                lps->SetInSize(totalPacked);
                rs = lps->SetCur();
                if (!IsSuccess(rs))
                {
                    return rs;
                }

                uint64_t unpackRemain = cabFolderOutStream->GetRemain();

                const uint32_t kBlockSizeMax = (1 << 15);
                if (unpackRemain > kBlockSizeMax)
                {
                    unpackRemain = kBlockSizeMax;
                }
                if (unpackRemain > unpackSize)
                {
                    unpackRemain  = unpackSize;
                }
   
                switch(folder.GetCompressionMethod())
                {
                    case RCCabHeaderDefs::kNone:
                        res = copyCoder->Code(cabBlockInStream.Get(), outStream.Get(), NULL, &unpackRemain, NULL);
                        break;
                    case RCCabHeaderDefs::kMSZip:
                        //deflateDecoderSpec->SetKeepHistory(keepHistory);
                        rs = deflateDecoder.QueryInterface(IID_ICompressSetCoderProperties, propset.GetAddress());
                        if (!IsSuccess(rs))
                        {
                            return rs;
                        }
                        proparray.clear();
                        proparray.push_back(RCPropertyIDPairArray::value_type(RCMethodProperty::ID_SET_KEEP_HISTORY, keepHistory));
                        rs = propset->SetCoderProperties(proparray);
                        if (!IsSuccess(rs))
                        {
                            return rs;
                        }
                        res = deflateDecoder->Code(cabBlockInStream.Get(), outStream.Get(), NULL, &unpackRemain, NULL);
                        break;
                    case RCCabHeaderDefs::kLZX:
                        //lzxDecoderSpec->SetKeepHistory(keepHistory);
                        rs = lzxDecoder.QueryInterface(IID_ICompressSetCoderProperties, propset.GetAddress());
                        if (!IsSuccess(rs))
                        {
                            return rs;
                        }
                        proparray.clear();
                        proparray.push_back(RCPropertyIDPairArray::value_type(RCMethodProperty::ID_SET_KEEP_HISTORY, keepHistory));
                        rs = propset->SetCoderProperties(proparray);
                        if (!IsSuccess(rs))
                        {
                            return rs;
                        }
                        res = lzxDecoder->Code(cabBlockInStream.Get(), outStream.Get(), NULL, &unpackRemain, NULL);
                        break;
                    case RCCabHeaderDefs::kQuantum:
                        //quantumDecoderSpec->SetKeepHistory(keepHistory);
                        rs = quantumDecoder.QueryInterface(IID_ICompressSetCoderProperties, propset.GetAddress());
                        if (!IsSuccess(rs))
                        {
                            return rs;
                        }
                        proparray.clear();
                        proparray.push_back(RCPropertyIDPairArray::value_type(RCMethodProperty::ID_SET_KEEP_HISTORY, keepHistory));
                        rs = propset->SetCoderProperties(proparray);
                        if (!IsSuccess(rs))
                        {
                            return rs;
                        }
                        res = quantumDecoder->Code(cabBlockInStream.Get(), outStream.Get(), NULL, &unpackRemain, NULL);
                        break;
                }
                if (res != RC_S_OK)
                {
                    if (res != RC_S_FALSE)
                    {
                        if (!IsSuccess(res))
                        {
                            return res;
                        }
                    }
                    break;
                }
                keepHistory = true;
            }
            if (res == RC_S_OK)
            {
                rs = cabFolderOutStream->WriteEmptyFiles();
                if (!IsSuccess(rs))
                {
                    return rs;
                }
            }
        }
        if (res != RC_S_OK || cabFolderOutStream->GetRemain() != 0)
        {
            rs = cabFolderOutStream->FlushCorrupted();
            if (!IsSuccess(rs))
            {
                return rs;
            }
        }
        totalUnPacked += curUnpack;
    }
    return RC_S_OK;
}

HResult RCCabHandler::GetArchiveProperty(RCPropertyID propID, RCVariant& value) 
{
    ClearVariantValue(value) ;
    RCVariant& prop = value ;
    switch(propID)
    {
        case RCPropID::kpidMethod:
        {
            RCString resString;
            RCVector<byte_t> ids;
            int32_t i;
            for (int32_t v = 0; v < static_cast<int32_t>(m_database.m_volumes.size()); v++)
            {
                const RCCabDatabaseEx &de = m_database.m_volumes[v];
                for (i = 0; i < static_cast<int32_t>(de.m_folders.size()); i++)
                {
                    RCVectorUtils::AddToUniqueSorted(ids, de.m_folders[i].GetCompressionMethod());
                }
            }
            for (i = 0; i < static_cast<int32_t>(ids.size()); i++)
            {
                byte_t id = ids[i];
                RCString method = (id < s_kCabNumMethods) ? s_cabMethods[id] : s_cabUnknownMethod;
                if (!resString.empty())
                resString += _T(' ');
                resString += method;
            }
            prop = resString;
            break;
        }
        case RCPropID::kpidNumBlocks:
        {
            uint32_t numFolders = 0;
            for (int32_t v = 0; v < static_cast<int32_t>(m_database.m_volumes.size()); v++)
            {
                numFolders += static_cast<uint32_t>(m_database.m_volumes[v].m_folders.size());
            }
            prop = static_cast<uint64_t>(numFolders);
            break;
        }
        case RCPropID::kpidNumVolumes:
        {
            prop = static_cast<uint64_t>(m_database.m_volumes.size());
            break;
        }
        case RCPropID::kpidArchiveType:
        {
            prop = uint64_t(RC_ARCHIVE_CAB) ;
            break;
        }
    }
    return RC_S_OK;
}

HResult RCCabHandler::SetCompressCodecsInfo(ICompressCodecsInfo* compressCodecsInfo)
{
    if (compressCodecsInfo)
    {
        m_compressCodecsInfo = compressCodecsInfo;
        return RC_S_OK;
    }
    return RC_S_FALSE;
}

END_NAMESPACE_RCZIP