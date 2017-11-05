/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "format/bzip2/RCBZip2Handler.h"
#include "filesystem/RCStreamUtils.h"
#include "filesystem/RCFileSystemUtils.h"
#include "format/common/RCDummyOutStream.h"
#include "common/RCCreateCoder.h"
#include "common/RCLocalProgress.h"
#include "interface/IArchive.h"
#include "interface/RCMethodDefs.h"
#include "format/common/RCPropData.h"
#include "common/RCStringUtil.h"
#include "filesystem/RCOutFileStream.h"
#include "filesystem/RCInFileStream.h"
#include "format/common/RCExtractSubStreamCallback.h"

/////////////////////////////////////////////////////////////////
//RCBZip2Handler class implementation

BEGIN_NAMESPACE_RCZIP

static RCPropData s_bzip2Props[] =
{
    { RCString(_T("")), RCPropID::kpidPackSize, RCVariantType::RC_VT_UINT64}
};

static RCPropData s_bzip2ArcProps[] =
{
    { RCString(_T("")), RCPropID::kpidArchiveType, RCVariantType::RC_VT_UINT64 }
};

HResult RCBZip2Handler::GetNumberOfProperties(uint32_t& numProperties)
{ 
    numProperties = sizeof(s_bzip2Props) / sizeof(s_bzip2Props[0]);

    return RC_S_OK; 
}

HResult RCBZip2Handler::GetPropertyInfo(uint32_t index, RCString& name, RCPropertyID& propID, RCVariantType::RCVariantID& varType)
{
    if (index >= sizeof(s_bzip2Props) / sizeof(s_bzip2Props[0])) 
    {
        return RC_E_INVALIDARG;
    }
    const RCPropData& srcItem = s_bzip2Props[index];

    propID  = srcItem.m_propID ; 
    varType = srcItem.m_varType ; 
    name    = srcItem.m_propName ;

    return RC_S_OK;
}

HResult RCBZip2Handler::GetNumberOfArchiveProperties(uint32_t& numProperties)
{
    numProperties = sizeof(s_bzip2ArcProps) / sizeof(s_bzip2ArcProps[0]) ;
    return RC_S_OK;
}

HResult RCBZip2Handler::GetArchivePropertyInfo(uint32_t index, RCString& name, RCPropertyID& propID, RCVariantType::RCVariantID& varType)
{
    if (index >= (sizeof(s_bzip2ArcProps) / sizeof(s_bzip2ArcProps[0])))
    {
        return RC_E_INVALIDARG;
    }
    const RCPropData& srcItem = s_bzip2ArcProps[index];
    propID = srcItem.m_propID;
    varType = srcItem.m_varType;
    name = srcItem.m_propName;
    return RC_S_OK;
}

HResult RCBZip2Handler::GetArchiveProperty(RCPropertyID propID, RCVariant& value)
{
    ClearVariantValue(value) ;
    RCVariant& prop = value ;
    switch(propID)
    {
    case RCPropID::kpidArchiveType:
        prop = uint64_t(RC_ARCHIVE_BZIP2) ;
        break;

    case RCPropID::kpidPhySize:
        if (m_packSizeDefined)
        {
            prop = (uint64_t)m_packSize ; 
        }
        break;

    default:
        return RC_S_OK;
    }
    return RC_S_OK;
}

HResult RCBZip2Handler::GetNumberOfItems(uint32_t& numItems)
{
    numItems = 1;

    return RC_S_OK;
}

HResult RCBZip2Handler::GetProperty(uint32_t index, RCPropertyID propID, RCVariant& value)
{
    ClearVariantValue(value) ;
    RCVariant& prop = value;
    switch(propID)
    {
    case RCPropID::kpidPackSize:
        {
            if (m_packSizeDefined)
            {
                prop = (uint64_t)m_packSize ; 
            }
        }
        break;
    }

    return RC_S_OK;
}

HResult RCBZip2Handler::Open(IInStream* stream, uint64_t maxCheckStartPosition, IArchiveOpenCallback* openArchiveCallback)
{
    Close() ;
    HResult hr = stream->Seek(0, RC_STREAM_SEEK_CUR, &m_startPosition);
    if (hr != RC_S_OK)
    {
        return hr;
    }

    const int kSignatureSize = 3;
    byte_t buffer[kSignatureSize];
    hr = RCStreamUtils::ReadStream_FALSE(stream, buffer, kSignatureSize);
    if (hr != RC_S_OK)
    {
        return hr;
    }

    if (buffer[0] != 'B' || buffer[1] != 'Z' || buffer[2] != 'h')
    {
        return RC_S_FALSE;
    }

    uint64_t endPosition;
    hr = stream->Seek(0, RC_STREAM_SEEK_END, &endPosition);
    if (hr != RC_S_OK)
    {
        return hr;
    }

    m_packSize = endPosition - m_startPosition ;
    m_packSizeDefined = true ;
    m_stream = stream ;
    m_seqStream = stream ;

    return RC_S_OK;
}

HResult RCBZip2Handler::Close()
{
    m_packSizeDefined = false;
    m_seqStream.Release();
    m_stream.Release();
    m_subStream.Release();

    return RC_S_OK;
}

HResult RCBZip2Handler::Extract(const RCVector<uint32_t>& indices, int32_t testMode, IArchiveExtractCallback* extractCallback)
{
    uint32_t numItems = static_cast<uint32_t>(indices.size());
    bool allFilesMode = (numItems == uint32_t(0));
    if (!allFilesMode)
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
    if(m_stream)
    {
        extractCallback->SetTotal(m_packSize) ;
    }

    uint64_t currentTotalPacked = 0;

    HResult hr = extractCallback->SetCompleted(currentTotalPacked);
    if (hr != RC_S_OK)
    {
        return hr;
    }

    ISequentialOutStreamPtr realOutStream;
    int32_t askMode;
    askMode = testMode ?  RC_ARCHIVE_EXTRACT_ASK_MODE_TEST :
                          RC_ARCHIVE_EXTRACT_ASK_MODE_EXTRACT;

    hr = extractCallback->GetStream(0, realOutStream.GetAddress(), askMode);
    if (hr != RC_S_OK)
    {
        return hr;
    }

    if (!testMode && !realOutStream)
    {
        return RC_S_OK;
    }

    extractCallback->PrepareOperation(0,askMode);

    ICompressCoderPtr decoder;
    hr = RCCreateCoder::CreateCoder(m_compressCodecsInfo.Get(),
                                    RCMethod::ID_COMPRESS_BZIP2,
                                    decoder,
                                    false);
    if (hr != RC_S_OK || !decoder)
    {
        hr = extractCallback->SetOperationResult(0,RC_ARCHIVE_EXTRACT_RESULT_UNSUPPORTED_METHOD);
        if (hr != RC_S_OK)
        {
            return hr;
        }
        return RC_S_OK;
    }
    
    IBZip2ResumeDecoderPtr decoderSpec ;
    hr = decoder->QueryInterface(IBZip2ResumeDecoder::IID, (void**)decoderSpec.GetAddress()) ;
    if (hr != RC_S_OK || !decoderSpec)
    {
        hr = extractCallback->SetOperationResult(0,RC_ARCHIVE_EXTRACT_RESULT_UNSUPPORTED_METHOD);
        if (hr != RC_S_OK)
        {
            return hr;
        }
        return RC_S_OK;
    }
    
    if (m_stream)
    {
        hr = m_stream->Seek(m_startPosition, RC_STREAM_SEEK_SET, NULL) ;
        if (hr != RC_S_OK)
        {
            return hr;
        }
    }

    decoderSpec->SetInStream(m_seqStream.Get());

#if defined( COMPRESS_MT) && defined( COMPRESS_BZIP2_MT)
    {
        ICompressSetCoderMtPtr setCoderMt;
        decoder.QueryInterface(IID_ICompressSetCoderMt, setCoderMt.GetAddress());
        if (setCoderMt)
        {
            hr = setCoderMt->SetNumberOfThreads(m_numThreads);
            if (hr != RC_S_OK)
            {
                return hr;
            }
        }
    }
#endif

    RCDummyOutStream* outStreamSpec = new RCDummyOutStream;
    ISequentialOutStreamPtr outStream(outStreamSpec);
    outStreamSpec->SetStream(realOutStream.Get());
    outStreamSpec->Init();

    realOutStream.Release();

    RCLocalProgress *lps = new RCLocalProgress;
    ICompressProgressInfoPtr progress = lps;
    lps->Init(extractCallback, true);

    HResult result = RC_S_OK ;

    bool firstItem = true;
    for (;;)
    {
        lps->SetInSize(currentTotalPacked);
        lps->SetOutSize(outStreamSpec->GetSize());

        hr = lps->SetCur();
        if (hr != RC_S_OK)
        {
            return hr;
        }
        
        bool isBz2 = false ;
        result = decoderSpec->CodeResume(outStream.Get(), isBz2, progress.Get()) ;
        if (hr != RC_S_OK)
        {
            return hr;
        }
        if (!isBz2)
        {
            if (firstItem)
            {
                result = RC_S_FALSE ;
            }
            break;
        }
        firstItem = false ;
        m_packSize = decoderSpec->GetInputProcessedSize() ;
        currentTotalPacked = m_packSize ;
        m_packSizeDefined = true ;
    }
    decoderSpec->ReleaseInStream() ;
    outStream.Release() ;

    int32_t retResult;
    if (result == RC_S_OK)
    {
        retResult = RC_ARCHIVE_EXTRACT_RESULT_OK;
    }
    else if (result == RC_S_FALSE)
    {
        retResult = RC_ARCHIVE_EXTRACT_RESULT_DATA_ERROR;
    }
    else
    {
        return result;
    }
    return extractCallback->SetOperationResult(0,retResult);
}

HResult RCBZip2Handler::GetStream(uint32_t index, ISequentialInStream** stream, IArchiveOpenCallback* openCallback)
{
    if ((index > 0) || (stream == NULL) || (m_stream == NULL))
    {
        return RC_S_FALSE;
    }
    if (m_packSize > uint32_t(1024*1024*100))
    {
        return RC_S_FALSE ;
    }
    RCString itemName ;
    IStreamGetFileNamePtr spGetFileName;
    HResult hret = m_stream->QueryInterface(IID_IStreamGetFileName, (void**)spGetFileName.GetAddress());
    if (!IsSuccess(hret))
    {
        return RC_S_FALSE;
    }
    hret = spGetFileName->GetFileName(itemName);
    if (!IsSuccess(hret))
    {
        return RC_S_FALSE;
    }
    //判断是否为tar与BZip2的组合包
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
    if( (RCStringUtil::CompareNoCase(extName, _T(".tbz2")) == 0) ||
        (RCStringUtil::CompareNoCase(extName, _T(".tbz")) == 0) )
    {
        isNestedTar = true ;
    }
    else
    {
        //判断扩展名是否为 tar.bzip2 等
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
    if(!subStreamCallback->Init(tempFileName, uint64_t( int64_t(-1) ) , openCallback))
    {
        m_tmpFile.Remove() ;
        return RC_S_FALSE;
    }
    //解压文件
    HResult hr = RC_S_OK ;
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
