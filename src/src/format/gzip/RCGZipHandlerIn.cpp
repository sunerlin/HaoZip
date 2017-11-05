/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "format/gzip/RCGZipHandler.h"
#include "format/gzip/RCGZipHeader.h"
#include "interface/RCMethodDefs.h"
#include "common/RCCreateCoder.h"
#include "format/common/RCOutStreamWithCRC.h"
#include "format/common/RCSequentialInStreamWithCRC.h"
#include "common/RCLocalProgress.h"
#include "common/RCStringConvert.h"
#include "filesystem/RCFileTime.h"
#include "filesystem/RCFileSystemUtils.h"
#include "format/common/RCPropData.h"
#include "interface/RCArchiveType.h"

#include "common/RCStringUtil.h"
#include "filesystem/RCOutFileStream.h"
#include "filesystem/RCInFileStream.h"
#include "format/common/RCExtractSubStreamCallback.h"

/////////////////////////////////////////////////////////////////
//RCGZipHandler class implementation

BEGIN_NAMESPACE_RCZIP

const char_t* g_hostOS[] =
{
  _T("FAT"),
  _T("AMIGA"),
  _T("VMS"),
  _T("Unix"),
  _T("VM_CMS"),
  _T("Atari"),
  _T("HPFS"),
  _T("Macintosh"),
  _T("Z-System"),
  _T("CP/M"),
  _T("TOPS-20"),
  _T("NTFS"),
  _T("SMS/QDOS "),
  _T("Acorn"),
  _T("VFAT"),
  _T("MVS"),
  _T("BeOS"),
  _T("Tandem"),
  _T("OS/400"),
  _T("OS/X")
};

#define g_unknownOS _T("Unknown")

static RCPropData s_gzipProps[] =
{
    { RCString(_T("")), RCPropID::kpidPath,     RCVariantType::RC_VT_STRING},
    { RCString(_T("")), RCPropID::kpidSize,     RCVariantType::RC_VT_UINT64},
    { RCString(_T("")), RCPropID::kpidPackSize, RCVariantType::RC_VT_UINT64},
    { RCString(_T("")), RCPropID::kpidMTime,    RCVariantType::RC_VT_UINT64},
    { RCString(_T("")), RCPropID::kpidHostOS,   RCVariantType::RC_VT_STRING},
    { RCString(_T("")), RCPropID::kpidCRC,      RCVariantType::RC_VT_UINT64},
    { RCString(_T("")), RCPropID::kpidCommented,RCVariantType::RC_VT_BOOL},
    { RCString(_T("")), RCPropID::kpidComment,  RCVariantType::RC_VT_STRING}
};

static RCPropData s_gzipArcProps[] =
{
    { RCString(_T("")), RCPropID::kpidArchiveType,    RCVariantType::RC_VT_UINT64 }
};

HResult RCGZipHandler::GetNumberOfProperties(uint32_t& numProperties)
{ 
    numProperties = sizeof(s_gzipProps) / sizeof(s_gzipProps[0]);

    return RC_S_OK; 
}

HResult RCGZipHandler::GetPropertyInfo(uint32_t index, RCString& name, RCPropertyID& propID, RCVariantType::RCVariantID& varType)
{
    if (index >= sizeof(s_gzipProps) / sizeof(s_gzipProps[0])) 
    {
        return RC_E_INVALIDARG;
    }
    const RCPropData& srcItem = s_gzipProps[index];

    propID  = srcItem.m_propID ; 
    varType = srcItem.m_varType ; 
    name    = srcItem.m_propName ;

    return RC_S_OK;
}

HResult RCGZipHandler::GetNumberOfArchiveProperties(uint32_t& numProperties)
{
    numProperties = sizeof(s_gzipArcProps) / sizeof(s_gzipArcProps[0]);
    return RC_S_OK;
}

HResult RCGZipHandler::GetArchivePropertyInfo(uint32_t index, RCString& name, RCPropertyID& propID, RCVariantType::RCVariantID& varType)
{
    if (index >= (sizeof(s_gzipArcProps) / sizeof(s_gzipArcProps[0])))
    {
        return RC_E_INVALIDARG;
    }
    const RCPropData& srcItem = s_gzipArcProps[index];
    propID = srcItem.m_propID;
    varType = srcItem.m_varType;
    name = srcItem.m_propName;
    return RC_S_OK;
}

HResult RCGZipHandler::GetArchiveProperty(RCPropertyID propID, RCVariant& value)
{
    ClearVariantValue(value) ;
    switch(propID)
    {
    case RCPropID::kpidArchiveType:
        value = uint64_t(RC_ARCHIVE_GZIP) ;
        break;
    case RCPropID::kpidPhySize:
        if (m_packSizeDefined)
        {
            value = (uint64_t)m_packSize;
        }
        break ;
    }
    return RC_S_OK;
}

HResult RCGZipHandler::GetNumberOfItems(uint32_t& numItems)
{
    numItems = 1;
    return RC_S_OK;
}

HResult RCGZipHandler::GetProperty(uint32_t index, RCPropertyID propID, RCVariant& value)
{
    ClearVariantValue(value) ;
    RCVariant& prop = value;
    switch(propID)
    {
    case RCPropID::kpidPath:
        if (m_item.NameIsPresent())
        {
            prop = RCStringConvert::MultiByteToUnicodeString(m_item.m_name, RC_CP_ACP);
        }
        break;

    case RCPropID::kpidMTime:
        if (m_item.m_time != 0)
        {
            RC_FILE_TIME utcTime;
            RCFileTime::UnixTimeToFileTime((uint32_t)m_item.m_time, utcTime);
            prop = RCFileTime::ConvertFromFileTime(utcTime);
        }
        break;

    case RCPropID::kpidSize:
        if(m_inStream != NULL)
        {
            prop = uint64_t(m_item.m_size32);
        }
        break;

    case RCPropID::kpidPackSize:
        if(m_packSizeDefined)
        {
            prop = (uint64_t)m_packSize ;
        }
        break;

    case RCPropID::kpidCommented:
        {
            prop = m_item.CommentIsPresent();
        }
        break;
    case RCPropID::kpidComment:
        {
            prop = RCStringConvert::MultiByteToUnicodeString(m_item.m_comment, RC_CP_ACP);
        }
        break;

    case RCPropID::kpidHostOS:
        {
            prop = RCString((m_item.m_hostOS < (sizeof(g_hostOS) / sizeof(g_hostOS[0]))) ? g_hostOS[m_item.m_hostOS] : g_unknownOS);
        }
        break;
    case RCPropID::kpidCRC:
        if(m_inStream != NULL)
        {
            prop = (uint64_t)m_item.m_crc ;
        }
        break;
    }
    return RC_S_OK;
}

HResult RCGZipHandler::Open(IInStream* stream, uint64_t , IArchiveOpenCallback* )
{
    if(stream == NULL)
    {
        return RC_E_INVALIDARG ;
    }
    HResult hr = RC_S_OK ;
    try
    {
        hr = stream->Seek(0, RC_STREAM_SEEK_CUR, &m_startPosition) ;
        if(!IsSuccess(hr))
        {
            return hr ;
        }
        hr = OpenSeq(stream);
        if (IsSuccess(hr))
        {
            uint64_t endPos = 0 ;
            hr = stream->Seek(-8, RC_STREAM_SEEK_END, &endPos) ; 
            if (IsSuccess(hr))
            {
                m_packSize = endPos + 8 - m_startPosition;
                m_packSizeDefined = true ;
                hr = m_item.ReadFooter2(stream) ;
                m_inStream = stream;
            }
        }
    }
    catch(...)
    {
        hr = RC_S_FALSE ;
    }
    if (hr != RC_S_OK)
    {
        Close();
    }
    return hr ;
}

HResult RCGZipHandler::OpenSeq(ISequentialInStream *stream)
{
    if(m_decoderSpec == NULL)
    {
        return RC_E_INVALIDARG ;
    }
    HResult res = RC_S_OK ;
    try
    {
        Close();
        ICompressSetInStreamPtr spSetInStream ;
        res = m_decoderSpec->QueryInterface(ICompressSetInStream::IID,(void**)spSetInStream.GetAddress()) ;
        if(IsSuccess(res))
        {
            spSetInStream->SetInStream(stream);
            m_decoderSpec->InitInStream(true);
            res = m_item.ReadHeader(m_decoderSpec.Get());
            m_headerSize = m_decoderSpec->GetInputProcessedSize();
        }
    }
    catch(...)
    {
        res = RC_S_FALSE;
    }
    if (res != RC_S_OK)
    {
        Close();
    }
    return res ;
}

HResult RCGZipHandler::Close()
{
    m_packSizeDefined = false ;
    m_inStream.Release() ;
    ICompressSetInStreamPtr spSetInStream ;
    HResult res = m_decoderSpec->QueryInterface(ICompressSetInStream::IID,(void**)spSetInStream.GetAddress()) ;
    if(IsSuccess(res))
    {
        spSetInStream->ReleaseInStream() ;
    }
    m_subStream.Release() ;
    m_tmpFile.Remove() ;
    return RC_S_OK;
}

HResult RCGZipHandler::Extract(const RCVector<uint32_t>& indices,
                               int32_t testMode,
                               IArchiveExtractCallback* extractCallback)
{
    uint32_t numItems = (uint32_t)indices.size() ;
    if(numItems == 0)
    {
        numItems = 1 ;
    }
    if (!indices.empty())
    {
        if (numItems != 1)
        {
            return RC_E_INVALIDARG;
        }

        if (indices[0] != 0)
        {
            return RC_E_INVALIDARG;
        }
    }
    if(m_inStream != NULL)
    {
        extractCallback->SetTotal(m_packSize) ;
    }

    uint64_t currentTotalPacked = 0;

    HResult hr = extractCallback->SetCompleted(currentTotalPacked);
    if (hr != RC_S_OK)
    {
        return hr;
    }

    int32_t askMode = testMode ? RC_ARCHIVE_EXTRACT_ASK_MODE_TEST :
                                 RC_ARCHIVE_EXTRACT_ASK_MODE_EXTRACT;

    ISequentialOutStreamPtr realOutStream ;
    hr = extractCallback->GetStream(0, realOutStream.GetAddress(), askMode);
    if (hr != RC_S_OK)
    {
        return hr;
    }

    if(!testMode && !realOutStream)
    {
        return RC_S_OK;
    }

    extractCallback->PrepareOperation(0,askMode);

    RCOutStreamWithCRC* outStreamSpec = new RCOutStreamWithCRC;
    ISequentialOutStreamPtr outStream(outStreamSpec);
    outStreamSpec->SetStream(realOutStream.Get());
    outStreamSpec->Init();
    realOutStream.Release();

    RCLocalProgress *lps = new RCLocalProgress;
    ICompressProgressInfoPtr progress = lps;
    lps->Init(extractCallback, true);
    
    if (m_inStream)
    {
        hr = m_inStream->Seek(m_startPosition, RC_STREAM_SEEK_SET, NULL) ;
        if(!IsSuccess(hr))
        {
            return hr ;
        }
        m_decoderSpec->InitInStream(true);
    }
    bool firstItem = true;
    int32_t opRes = 0 ;
    for (;;)
    {
        m_packSize = m_decoderSpec->GetInputProcessedSize();
        lps->SetInSize(m_packSize) ;
        m_packSizeDefined = true;
        lps->SetOutSize( outStreamSpec->GetSize() );
        hr = lps->SetCur() ;
        if(!IsSuccess(hr))
        {
            return hr ;
        }
        RCGZipItem item ;
        if (!firstItem || m_inStream)
        {
            HResult result = item.ReadHeader(m_decoderSpec.Get());
            if (result != RC_S_OK)
            {
                if (result != RC_S_FALSE)
                {
                    return result;
                }
                opRes = firstItem ? RC_ARCHIVE_EXTRACT_RESULT_DATA_ERROR :
                                    RC_ARCHIVE_EXTRACT_RESULT_OK;
                break;
            }
        }
        firstItem = false ;

        uint64_t startOffset = outStreamSpec->GetSize() ;
        outStreamSpec->InitCRC() ;

        HResult result = m_decoderSpec->CodeResume(outStream.Get(), NULL, progress.Get());
        if (result != RC_S_OK)
        {
            if (result != RC_S_FALSE)
            {
                return result;
            }
            opRes = RC_ARCHIVE_EXTRACT_RESULT_DATA_ERROR;
            break;
        }

        m_decoderSpec->AlignToByte();
        if (item.ReadFooter1(m_decoderSpec.Get()) != RC_S_OK)
        {
            opRes = RC_ARCHIVE_EXTRACT_RESULT_DATA_ERROR;
            break;
        }
        if (item.m_crc != outStreamSpec->GetCRC() ||
            item.m_size32 != (uint32_t)(outStreamSpec->GetSize() - startOffset))
        {
            opRes = RC_ARCHIVE_EXTRACT_RESULT_CRC_ERROR;
            break;
        }
    }
    outStream.Release();
    return extractCallback->SetOperationResult(0, opRes);
}

HResult RCGZipHandler::GetStream(uint32_t index, ISequentialInStream** stream, IArchiveOpenCallback* openCallback)
{
    if( (index > 0) || (stream == NULL) || (m_inStream == NULL))
    {
        return RC_S_FALSE;
    }
    if(m_item.m_size32 > uint32_t(1024*1024*300))
    {
        return RC_S_FALSE ;
    }

    RCString itemName ;
    IStreamGetFileNamePtr spGetFileName ;
    HResult hr = m_inStream->QueryInterface(IID_IStreamGetFileName, (void**)spGetFileName.GetAddress());
    if (!IsSuccess(hr) || (spGetFileName == NULL))
    {
        return RC_S_FALSE;
    }
    hr = spGetFileName->GetFileName(itemName);
    if (!IsSuccess(hr))
    {
        return RC_S_FALSE;
    }
    //判断是否为tar与gzip的组合包
    RCString onlyFileName ;
    if(!RCFileSystemUtils::GetOnlyName(itemName,onlyFileName))
    {
        return RC_S_FALSE ;
    }
    RCString::size_type pos = onlyFileName.rfind(_T('.')) ;
    if(pos == RCString::npos)
    {
        return RC_S_FALSE ;
    }
    bool isNestedTar = false ;
    RCString extName = onlyFileName.substr(pos) ;
    if( (RCStringUtil::CompareNoCase(extName, _T(".tgz")) == 0) ||
        (RCStringUtil::CompareNoCase(extName, _T(".tpz")) == 0) )
    {
        isNestedTar = true ;
    }
    else
    {
        //判断扩展名是否为 tar.gz
        onlyFileName = onlyFileName.substr(0, pos) ;
        pos = onlyFileName.rfind(_T('.')) ;
        if(pos == RCString::npos)
        {
            return RC_S_FALSE ;
        }
        extName = onlyFileName.substr(pos) ;
        if(RCStringUtil::CompareNoCase(extName, _T(".tar")) == 0)
        {
            isNestedTar = true ;
        }
    }
    if (!isNestedTar)
    {
        return RC_S_FALSE ;
    }
            
    RCString tempFileName;
    if (!m_tmpFile.CreateTempFile(tempFileName))
    {
        //如果临时文件创建失败,退出
        m_tmpFile.Remove() ;
        return RC_S_FALSE;
    }
    RCExtractSubStreamCallback* subStreamCallback = new RCExtractSubStreamCallback ;
    IArchiveExtractCallbackPtr spSubStreamCallback(subStreamCallback) ;
    if(!subStreamCallback->Init(tempFileName, m_item.m_size32, openCallback))
    {
        m_tmpFile.Remove() ;
        return RC_S_FALSE;
    }
    //解压文件
    hr = RC_S_OK ;
    try
    {
        RCVector<uint32_t> indices ;
        hr = Extract(indices, 0, spSubStreamCallback.Get());
    }
    catch(...)
    {
        hr = RC_E_FAIL ;
    }
    spSubStreamCallback = NULL ;
    if(!IsSuccess(hr))
    {
        m_tmpFile.Remove() ;
        return RC_S_FALSE ;
    }
    
    //解压文件成功， 打开临时文件    
    RCInFileStream* realIn = new RCInFileStream ;    
    if (realIn->Open(tempFileName))
    {
        m_subStream = realIn ;
        *stream = realIn;
        realIn->AddRef();
        return RC_S_OK;
    }
    else
    {
        delete realIn ;
        m_tmpFile.Remove() ;
        return RC_S_FALSE ;
    }
}

END_NAMESPACE_RCZIP
