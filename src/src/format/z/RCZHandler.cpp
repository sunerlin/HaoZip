/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#include "format/z/RCZHandler.h"
#include "format/common/RCPropData.h"
#include "format/common/RCDummyOutStream.h"
#include "common/RCLocalProgress.h"
#include "common/RCCreateCoder.h"
#include "common/RCStringUtil.h"
#include "filesystem/RCStreamUtils.h"
#include "filesystem/RCFileSystemUtils.h"
#include "interface/RCArchiveType.h"
#include "filesystem/RCInFileStream.h"
#include "format/common/RCExtractSubStreamCallback.h"

BEGIN_NAMESPACE_RCZIP

static RCPropData s_zProps[] =
{
    { RCString(_T("")), RCPropID::kpidPackSize, RCVariantType::RC_VT_UINT64}
};

static RCPropData s_zArcProps[] =
{
    { RCString(_T("")), RCPropID::kpidArchiveType,    RCVariantType::RC_VT_UINT64 }
};

HResult RCZHandler::GetNumberOfProperties(uint32_t& numProperties) 
{
    numProperties = sizeof(s_zProps) / sizeof(s_zProps[0]);
    return RC_S_OK;
}

HResult RCZHandler::GetPropertyInfo(uint32_t index, 
                                    RCString& name, 
                                    RCPropertyID& propID, 
                                    RCVariantType::RCVariantID& varType)
{
    if (index >= (sizeof(s_zProps) / sizeof(s_zProps[0])))
    {
        return RC_E_INVALIDARG;
    }
    const RCPropData& srcItem = s_zProps[index];
    propID = srcItem.m_propID;
    varType = srcItem.m_varType;
    name = srcItem.m_propName;
    return RC_S_OK;
}

HResult RCZHandler::GetNumberOfArchiveProperties(uint32_t& numProperties) 
{
    numProperties = sizeof(s_zArcProps) / sizeof(s_zArcProps[0]);
    return RC_S_OK;
}

HResult RCZHandler::GetArchivePropertyInfo(uint32_t index, 
                                           RCString& name, 
                                           RCPropertyID& propID, 
                                           RCVariantType::RCVariantID& varType)
{
    if (index >= (sizeof(s_zArcProps) / sizeof(s_zArcProps[0])))
    {
        return RC_E_INVALIDARG;
    }
    const RCPropData& srcItem = s_zArcProps[index];
    propID = srcItem.m_propID;
    varType = srcItem.m_varType;
    name = srcItem.m_propName;
    return RC_S_OK;
}

HResult RCZHandler::GetArchiveProperty(RCPropertyID propID, RCVariant& value) 
{
    ClearVariantValue(value) ;
    switch(propID)
    {
        case RCPropID::kpidArchiveType:
        {
            value = uint64_t(RC_ARCHIVE_Z) ;
            break;
        }
    }
    return RC_S_OK;
}

HResult RCZHandler::Open(IInStream* stream, 
                         uint64_t maxCheckStartPosition, 
                         IArchiveOpenCallback* openArchiveCallback)
{
    HResult result = stream->Seek(0, RC_STREAM_SEEK_CUR, &m_streamStartPosition);
    if (!IsSuccess(result))
    {
        return result;
    }
    byte_t buffer[s_signatureSize];
    result = RCStreamUtils::ReadStream_FALSE(stream, buffer, s_signatureSize);
    if (!IsSuccess(result))
    {
        return result;
    }
    if (buffer[0] != 0x1F || buffer[1] != 0x9D)
    {
        return RC_S_FALSE;
    }
    m_properties = buffer[2];
    uint64_t endPosition;
    result = stream->Seek(0, RC_STREAM_SEEK_END, &endPosition);
    if (!IsSuccess(result))
    {
        return result;
    }
    m_packSize = endPosition - m_streamStartPosition - s_signatureSize;
    m_stream = stream;
    return RC_S_OK;
}

HResult RCZHandler::Close()
{
    m_stream.Release();
    return RC_S_OK;
}

HResult RCZHandler::GetNumberOfItems(uint32_t& numItems)
{
    numItems = 1;
    return RC_S_OK;
}

HResult RCZHandler::GetProperty(uint32_t index, RCPropertyID propID, RCVariant& value) 
{
    ClearVariantValue(value) ;
    switch(propID)
    {
    case RCPropID::kpidPackSize: 
        value = m_packSize; 
        break;
    case RCPropID::kpidExtension: 
        {
            if(IsNestedTarFile() == RC_S_OK)
            {
                value = RCString(_T("tar")) ;
            }
        }
        break;
    }
    return RC_S_OK;
}

HResult RCZHandler::Extract(const std::vector<uint32_t>& indices, 
                            int32_t testMode, 
                            IArchiveExtractCallback* extractCallback) 
{
    uint32_t numItems = static_cast<uint32_t>(indices.size()) ;
    if (numItems != 0 )
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

    extractCallback->SetTotal(m_packSize);

    uint64_t currentTotalPacked = 0;

    HResult rs = extractCallback->SetCompleted(currentTotalPacked);
    if (!IsSuccess(rs))
    {
        return rs;
    }

    ISequentialOutStreamPtr realOutStream;
    int32_t askMode = testMode ? RC_ARCHIVE_EXTRACT_ASK_MODE_TEST : RC_ARCHIVE_EXTRACT_ASK_MODE_EXTRACT;

    rs = extractCallback->GetStream(0, realOutStream.GetAddress(), askMode);
    if (!IsSuccess(rs))
    {
        return rs;
    }

    if (!testMode && !realOutStream)
    {
        return RC_S_OK;
    }
    extractCallback->PrepareOperation(0,askMode);
    RCDummyOutStream *outStreamSpec = new RCDummyOutStream;
    ISequentialOutStreamPtr outStream(outStreamSpec);
    outStreamSpec->SetStream(realOutStream.Get());
    outStreamSpec->Init();
    realOutStream.Release();

    RCLocalProgress *lps = new RCLocalProgress;
    ICompressProgressInfoPtr progress = lps;
    lps->Init(extractCallback, true);

    rs = m_stream->Seek(m_streamStartPosition + s_signatureSize, RC_STREAM_SEEK_SET, NULL);
    if (!IsSuccess(rs))
    {
        return rs;
    }

    ICompressCoderPtr decoder;
    rs = RCCreateCoder::CreateCoder(m_compressCodecsInfo.Get(),RCMethod::ID_COMPRESS_Z, decoder, false);
    if (!IsSuccess(rs))
    {
        return rs;
    }

    ICompressSetDecoderProperties2Ptr lsetprop;
    rs = decoder.QueryInterface(IID_ICompressSetDecoderProperties2, lsetprop.GetAddress());
    if (!IsSuccess(rs))
    {
        return rs;
    }
    HResult result = lsetprop->SetDecoderProperties2(&m_properties, 1);

    int opResult;
    if (result != RC_S_OK)
    {
        opResult = RC_ARCHIVE_EXTRACT_RESULT_UNSUPPORTED_METHOD;
    }
    else
    {
        result = decoder->Code(m_stream.Get(), outStream.Get(), NULL, NULL, progress.Get());
        if (result == RC_S_FALSE)
        {
            opResult = RC_ARCHIVE_EXTRACT_RESULT_DATA_ERROR;
        }
        else
        {
            if (!IsSuccess(result))
            {
                return result;
            }
            opResult = RC_ARCHIVE_EXTRACT_RESULT_OK;
        }
    }
    outStream.Release();
    return extractCallback->SetOperationResult(0,opResult);
}

HResult RCZHandler::SetCompressCodecsInfo(ICompressCodecsInfo* compressCodecsInfo)
{
    if (compressCodecsInfo)
    {
        m_compressCodecsInfo = compressCodecsInfo;
        return RC_S_OK;
    }
    return RC_S_OK;
}

HResult RCZHandler::IsNestedTarFile(void)
{
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
    //判断是否为tar与z的组合包
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
    if(RCStringUtil::CompareNoCase(extName, _T(".taz")) == 0)
    {
        isNestedTar = true ;
    }
    else
    {
        //判断扩展名是否为 tar.z 等
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
    return RC_S_OK ;
}

HResult RCZHandler::GetStream(uint32_t index, ISequentialInStream** stream, IArchiveOpenCallback* openCallback)
{
    if( (index > 0) || (stream == NULL) || (m_stream == NULL))
    {
        return RC_S_FALSE;
    }
    if(m_packSize > uint32_t(1024*1024*100))
    {
        return RC_S_FALSE ;
    }
    if(IsNestedTarFile() != RC_S_OK)
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
