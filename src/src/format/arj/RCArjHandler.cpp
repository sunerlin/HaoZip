/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#include "format/arj/RCArjHandler.h"
#include "format/common/RCPropData.h"
#include "format/common/RCOutStreamWithCRC.h"
#include "format/common/RCItemNameUtils.h"
#include "common/RCLocalProgress.h"
#include "common/RCStringConvert.h"
#include "common/RCCreateCoder.h"
#include "filesystem/RCLimitedSequentialInStream.h"
#include "interface/RCArchiveType.h"

BEGIN_NAMESPACE_RCZIP

static RCPropData s_arjArcProps[] =
{
    { RCString(_T("")), RCPropID::kpidName, RCVariantType::RC_VT_STRING},
    { RCString(_T("")), RCPropID::kpidCTime, RCVariantType::RC_VT_STRING},
    { RCString(_T("")), RCPropID::kpidMTime, RCVariantType::RC_VT_STRING},
    { RCString(_T("")), RCPropID::kpidHostOS, RCVariantType::RC_VT_STRING},
    { RCString(_T("")), RCPropID::kpidComment, RCVariantType::RC_VT_STRING},
    { RCString(_T("")), RCPropID::kpidCommented, RCVariantType::RC_VT_BOOL},
    { RCString(_T("")), RCPropID::kpidArchiveType, RCVariantType::RC_VT_UINT64 }
};

static RCPropData s_arjProps[] =
{
    { RCString(_T("")), RCPropID::kpidPath, RCVariantType::RC_VT_STRING},
    { RCString(_T("")), RCPropID::kpidIsDir, RCVariantType::RC_VT_BOOL},
    { RCString(_T("")), RCPropID::kpidSize, RCVariantType::RC_VT_UINT64},
    { RCString(_T("")), RCPropID::kpidPackSize, RCVariantType::RC_VT_UINT64},
    { RCString(_T("")), RCPropID::kpidPosition, RCVariantType::RC_VT_UINT64},
    { RCString(_T("")), RCPropID::kpidMTime, RCVariantType::RC_VT_UINT64},
    { RCString(_T("")), RCPropID::kpidAttrib, RCVariantType::RC_VT_UINT64},
    { RCString(_T("")), RCPropID::kpidEncrypted, RCVariantType::RC_VT_BOOL},
    { RCString(_T("")), RCPropID::kpidCRC, RCVariantType::RC_VT_UINT64},
    { RCString(_T("")), RCPropID::kpidMethod, RCVariantType::RC_VT_UINT64},
    { RCString(_T("")), RCPropID::kpidHostOS, RCVariantType::RC_VT_STRING},
    { RCString(_T("")), RCPropID::kpidComment, RCVariantType::RC_VT_STRING},
    { RCString(_T("")), RCPropID::kpidCommented, RCVariantType::RC_VT_BOOL}
};

HResult RCArjHandler::GetNumberOfProperties(uint32_t& numProperties) 
{
    numProperties = sizeof(s_arjProps) / sizeof(s_arjProps[0]);
    return RC_S_OK;
}

HResult RCArjHandler::GetPropertyInfo(uint32_t index, RCString& name, RCPropertyID& propID, RCVariantType::RCVariantID& varType)
{
    if (index >= (sizeof(s_arjProps) / sizeof(s_arjProps[0])))
    {
        return RC_E_INVALIDARG;
    }
    const RCPropData& srcItem = s_arjProps[index];
    propID = srcItem.m_propID;
    varType = srcItem.m_varType;
    name = srcItem.m_propName;
    return RC_S_OK;
}

HResult RCArjHandler::GetNumberOfArchiveProperties(uint32_t& numProperties) 
{
    numProperties = sizeof(s_arjArcProps) / sizeof(s_arjArcProps[0]);
    return RC_S_OK;
}

HResult RCArjHandler::GetArchivePropertyInfo(uint32_t index, RCString& name, RCPropertyID& propID, RCVariantType::RCVariantID& varType)
{
    if (index >= (sizeof(s_arjArcProps) / sizeof(s_arjArcProps[0])))
    {
        return RC_E_INVALIDARG;
    }
    const RCPropData& srcItem = s_arjArcProps[index];
    propID = srcItem.m_propID;
    varType = srcItem.m_varType;
    name = srcItem.m_propName;
    return RC_S_OK;
}

HResult RCArjHandler::GetNumberOfItems(uint32_t& numItems)
{
    numItems = static_cast<uint32_t>(m_items.size());
    return RC_S_OK;
}

HResult RCArjHandler::GetArchiveProperty(RCPropertyID propID, RCVariant& value) 
{
    ClearVariantValue(value) ;
    RCVariant& prop = value ;
    switch(propID)
    {
    case RCPropID::kpidName:
        RCArjDefs::SetUnicodeString(m_archive.m_header.m_name, prop); 
        break;

    case RCPropID::kpidCTime:
        RCArjDefs::SetTime(m_archive.m_header.m_cTime, prop); 
        break;

    case RCPropID::kpidMTime:
        RCArjDefs::SetTime(m_archive.m_header.m_mTime, prop); 
        break;

    case RCPropID::kpidHostOS:
        RCArjDefs::SetHostOS(m_archive.m_header.m_hostOS, prop); 
        break;

    case RCPropID::kpidComment:
        RCArjDefs::SetUnicodeString(m_archive.m_header.m_comment, prop); 
        break;

    case RCPropID::kpidCommented:
        prop = m_archive.m_header.m_comment.empty() ? false : true ; 
        break;

    case RCPropID::kpidArchiveType:
        prop = uint64_t(RC_ARCHIVE_ARJ) ;
        break;

    default:
        return RC_S_OK;
    }
    return RC_S_OK;
}

HResult RCArjHandler::SetCompressCodecsInfo(ICompressCodecsInfo* compressCodecsInfo) 
{
    if (compressCodecsInfo)
    {
        m_compressCodecsInfo = compressCodecsInfo;
        return RC_S_OK;
    }
    return RC_S_OK;
}

HResult RCArjHandler::GetProperty(uint32_t index, RCPropertyID propID, RCVariant& value) 
{
    ClearVariantValue(value) ;
    RCVariant& prop = value ;
    const RCArjItem &item = m_items[index];
    switch(propID)
    {
    case RCPropID::kpidPath:  
        prop = RCItemNameUtils::GetOSName(RCStringConvert::MultiByteToUnicodeString(item.m_name, RC_CP_OEMCP)); 
        break;

    case RCPropID::kpidIsDir:  
        prop = item.IsDir(); 
        break;

    case RCPropID::kpidSize:  
        prop = static_cast<uint64_t>(item.m_size); 
        break;

    case RCPropID::kpidPosition:  
        {
            if (item.IsSplitBefore() || item.IsSplitAfter())
            {
                prop = (uint64_t)item.m_splitPos ; 
            }
        }
        break;

    case RCPropID::kpidPackSize:  
        prop = static_cast<uint64_t>(item.m_packSize); 
        break;

    case RCPropID::kpidAttrib:  
        prop = static_cast<uint64_t>(item.GetWinAttributes()); 
        break;

    case RCPropID::kpidEncrypted:  
        prop = item.IsEncrypted(); 
        break;

    case RCPropID::kpidCRC:  
        prop = static_cast<uint64_t>(item.m_fileCRC); 
        break;

    case RCPropID::kpidMethod:  
        prop = static_cast<uint64_t>(item.m_method); 
        break;

    case RCPropID::kpidHostOS:  
        RCArjDefs::SetHostOS(item.m_hostOS, prop); 
        break;

    case RCPropID::kpidMTime:  
        RCArjDefs::SetTime(item.m_mTime, prop); 
        break;

    case RCPropID::kpidComment:  
        RCArjDefs::SetUnicodeString(item.m_comment, prop); 
        break;

    case RCPropID::kpidCommented:  
        prop = item.m_comment.empty() ? false : true ; 
        break;

    default:
        return RC_S_OK ;
    }
    return RC_S_OK;
}

HResult RCArjHandler::Open2(IInStream* inStream, const uint64_t* maxCheckStartPosition, IArchiveOpenCallback* callback)
{
    HResult result;
    Close();
    uint64_t endPos = 0;
    if (callback != NULL)
    {
        result = inStream->Seek(0, RC_STREAM_SEEK_END, &endPos);
        if (!IsSuccess(result))
        {
            return result;
        }
        result = inStream->Seek(0, RC_STREAM_SEEK_SET, NULL);
        if (!IsSuccess(result))
        {
            return result;
        }
    }

    m_archive.m_stream = inStream;
    m_archive.m_openArchiveCallback = callback;
    m_archive.m_numFiles = m_archive.m_numBytes = 0;

    result = m_archive.Open(maxCheckStartPosition);
    if (!IsSuccess(result))
    {
        return result;
    }
    if (callback != NULL)
    {
        result = callback->SetTotal(NULL, endPos);
        if (!IsSuccess(result))
        {
            return result;
        }
    }
    for (;;)
    {
        RCArjItem item;
        bool filled;
        result = m_archive.GetNextItem(filled, item);
        if (!IsSuccess(result))
        {
            return result;
        }
        result = inStream->Seek(0, RC_STREAM_SEEK_CUR, &item.m_dataPosition);
        if (!IsSuccess(result))
        {
            return result;
        }
        if (!filled)
        {
            break;
        }
        m_items.push_back(item);

        if (inStream->Seek(item.m_packSize, RC_STREAM_SEEK_CUR, NULL) != RC_S_OK)
        {
            _ThrowCode(RC_E_UnexpectedEndOfArchive) ;
        }

        m_archive.m_numFiles = m_items.size();
        m_archive.m_numBytes = item.m_dataPosition;

        if (callback != NULL && m_items.size() % 100 == 0)
        {
            result = callback->SetCompleted(m_archive.m_numFiles, m_archive.m_numBytes);
            if (!IsSuccess(result))
            {
                return result;
            }
        }
    }
    return RC_S_OK;
}

HResult RCArjHandler::Open(IInStream* inStream, uint64_t maxCheckStartPosition, IArchiveOpenCallback* callback)
{
    HResult res;
    try
    {
        res = Open2(inStream, &maxCheckStartPosition, callback);
        if (res == RC_S_OK)
        {
            m_stream = inStream;
            return RC_S_OK;
        }
    }
    catch(HResult) 
    { 
        res = RC_S_FALSE; 
    }
    Close();
    return res;
}

HResult RCArjHandler::Close()
{
    m_items.clear();
    m_stream.Release();
    return RC_S_OK;
}

HResult RCArjHandler::Extract(const std::vector<uint32_t>& indices, int32_t testModeSpec, IArchiveExtractCallback* extractCallback) 
{
    HResult rs;
    bool testMode = (testModeSpec != 0);
    uint64_t totalUnpacked = 0, totalPacked = 0;
    uint32_t numItems;
    bool allFilesMode = indices.empty();
    if (allFilesMode)
    {
        numItems = static_cast<uint32_t>(m_items.size());
    }
    else
    {
        numItems = static_cast<uint32_t>(indices.size());
    }
    if (numItems == 0)
    {
        return RC_S_OK;
    }
    uint32_t i;
    for (i = 0; i < numItems; i++)
    {
        const RCArjItem &item = m_items[allFilesMode ? i : indices[i]];
        totalUnpacked += item.m_size;
        totalPacked += item.m_packSize;
    }
    extractCallback->SetTotal(totalUnpacked);

    totalUnpacked = totalPacked = 0;
    uint64_t curUnpacked, curPacked;

    ICompressCoderPtr arj1Decoder;
    ICompressCoderPtr arj2Decoder;
    //NCompress::CCopyCoder *copyCoderSpec = new NCompress::CCopyCoder();
    //CMyComPtr<ICompressCoder> copyCoder = copyCoderSpec;
    ICompressCoderPtr copyCoder;
    rs = RCCreateCoder::CreateCoder(m_compressCodecsInfo.Get(),RCMethod::ID_COMPRESS_COPY, copyCoder, false);
    if (!IsSuccess(rs))
    {
        return rs;
    }


    RCLocalProgress *lps = new RCLocalProgress;
    ICompressProgressInfoPtr progress = lps;
    lps->Init(extractCallback, false);

    RCLimitedSequentialInStream *inStreamSpec = new RCLimitedSequentialInStream;
    ISequentialInStreamPtr inStream(inStreamSpec);
    inStreamSpec->SetStream(m_stream.Get());

    for (i = 0; i < numItems; i++, totalUnpacked += curUnpacked, totalPacked += curPacked)
    {
        lps->SetInSize(totalPacked);
        lps->SetOutSize(totalUnpacked);
        rs = lps->SetCur();
        if (!IsSuccess(rs))
        {
            return rs;
        }
        curUnpacked = curPacked = 0;
        ISequentialOutStreamPtr realOutStream;
        int32_t askMode = testMode ? RC_ARCHIVE_EXTRACT_ASK_MODE_TEST : RC_ARCHIVE_EXTRACT_ASK_MODE_EXTRACT;
        int32_t index = allFilesMode ? i : indices[i];
        const RCArjItem &item = m_items[index];
        rs = extractCallback->GetStream(index, realOutStream.GetAddress(), askMode);
        if (!IsSuccess(rs))
        {
            return rs;
        }
        if (item.IsDir())
        {
            // if (!testMode)
            {
                rs = extractCallback->PrepareOperation(index,askMode);
                if (!IsSuccess(rs))
                {
                    return rs;
                }
                rs = extractCallback->SetOperationResult(index,RC_ARCHIVE_EXTRACT_RESULT_OK);
                if (!IsSuccess(rs))
                {
                    return rs;
                }
            }
            continue;
        }

        if (!testMode && (!realOutStream))
        {
            continue;
        }

        rs = extractCallback->PrepareOperation(index,askMode);
        if (!IsSuccess(rs))
        {
            return rs;
        }
        curUnpacked = item.m_size;
        curPacked = item.m_packSize;

        {
            RCOutStreamWithCRC *outStreamSpec = new RCOutStreamWithCRC;
            ISequentialOutStreamPtr outStream(outStreamSpec);
            outStreamSpec->SetStream(realOutStream.Get());
            realOutStream.Release();
            outStreamSpec->Init();

            inStreamSpec->Init(item.m_packSize);

            uint64_t pos;
            m_stream->Seek(item.m_dataPosition, RC_STREAM_SEEK_SET, &pos);

            HResult result = RC_S_OK;
            int32_t opRes = RC_ARCHIVE_EXTRACT_RESULT_OK;

            if (item.IsEncrypted())
            {
                opRes = RC_ARCHIVE_EXTRACT_RESULT_UNSUPPORTED_METHOD;
            }
            else
            {
                switch(item.m_method)
                {
                case RCArjDefs::kStored:
                    {
                        result = copyCoder->Code(inStream.Get(), outStream.Get(), NULL, NULL, progress.Get());
                        ICompressGetInStreamProcessedSizePtr lptr;
                        HResult rs;
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
                        //if (result == RC_S_OK && copyCoderSpec->TotalSize != item.PackSize)
                        if (result == RC_S_OK && proc_size != item.m_packSize)
                        {
                            result = RC_S_FALSE;
                        }
                    }
                    break;

                case RCArjDefs::kCompressed1a:
                case RCArjDefs::kCompressed1b:
                case RCArjDefs::kCompressed1c:
                    {
                        if (!arj1Decoder)
                        {
                            //arj1Decoder = new NCompress::NArj::NDecoder1::CCoder;
                            rs = RCCreateCoder::CreateCoder(m_compressCodecsInfo.Get(),RCMethod::ID_COMPRESS_ARJ_1, arj1Decoder, false);
                            if (!IsSuccess(rs))
                            {
                                return rs;
                            }
                        }
                        result = arj1Decoder->Code(inStream.Get(), outStream.Get(), NULL, &curUnpacked, progress.Get());
                    }
                    break;

               case RCArjDefs::kCompressed2:
                    {
                        if (!arj2Decoder)
                        {
                            rs = RCCreateCoder::CreateCoder(m_compressCodecsInfo.Get(),RCMethod::ID_COMPRESS_ARJ_2, arj2Decoder, false);
                            if (!IsSuccess(rs))
                            {
                                return rs;
                            }
                        }
                        result = arj2Decoder->Code(inStream.Get(), outStream.Get(), NULL, &curUnpacked, progress.Get());
                    }
                    break;

               default:
                    opRes = RC_ARCHIVE_EXTRACT_RESULT_UNSUPPORTED_METHOD;
                }
            }
            if (opRes == RC_ARCHIVE_EXTRACT_RESULT_OK)
            {
                if (result == RC_S_FALSE)
                {
                    opRes = RC_ARCHIVE_EXTRACT_RESULT_DATA_ERROR;
                }
                else
                {
                    if (!IsSuccess(result))
                    {
                        return result;
                    }
                    opRes = (outStreamSpec->GetCRC() == item.m_fileCRC) ? RC_ARCHIVE_EXTRACT_RESULT_OK:
                        RC_ARCHIVE_EXTRACT_RESULT_CRC_ERROR;
                }
            }
            outStream.Release();
            rs = extractCallback->SetOperationResult(index,opRes);
            if (!IsSuccess(rs))
            {
                return rs;
            }
        }
    }
    return RC_S_OK;
}

END_NAMESPACE_RCZIP
