/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#include "format/rpm/RCRpmHandler.h"
#include "common/RCLocalProgress.h"
#include "common/RCCreateCoder.h"
#include "filesystem/RCStreamUtils.h"
#include "format/common/RCPropData.h"
#include "interface/RCArchiveType.h"

BEGIN_NAMESPACE_RCZIP

static RCPropData s_rpmProps[] =
{
    { RCString(_T("")), RCPropID::kpidSize, RCVariantType::RC_VT_UINT64},
    { RCString(_T("")), RCPropID::kpidPackSize, RCVariantType::RC_VT_UINT64}
};

static RCPropData s_rpmArcProps[] =
{
    { RCString(_T("")), RCPropID::kpidArchiveType, RCVariantType::RC_VT_UINT64 }
};

HResult RCRpmHandler::GetNumberOfProperties(uint32_t& numProperties) 
{
    numProperties = sizeof(s_rpmProps) / sizeof(s_rpmProps[0]);
    return RC_S_OK;
}

HResult RCRpmHandler::GetPropertyInfo(uint32_t index, RCString& name, RCPropertyID& propID, RCVariantType::RCVariantID& varType)
{
    if (index >= (sizeof(s_rpmProps) / sizeof(s_rpmProps[0])))
    {
        return RC_E_INVALIDARG;
    }
    const RCPropData& srcItem = s_rpmProps[index];
    propID = srcItem.m_propID;
    varType = srcItem.m_varType;
    name = srcItem.m_propName;
    return RC_S_OK;
}

HResult RCRpmHandler::GetNumberOfArchiveProperties(uint32_t& numProperties) 
{
    numProperties = sizeof(s_rpmArcProps) / sizeof(s_rpmArcProps[0]);
    return RC_S_OK;
}

HResult RCRpmHandler::GetArchivePropertyInfo(uint32_t index, RCString& name, RCPropertyID& propID, RCVariantType::RCVariantID& varType)
{
    if (index >= (sizeof(s_rpmArcProps) / sizeof(s_rpmArcProps[0])))
    {
        return RC_E_INVALIDARG;
    }
    const RCPropData& srcItem = s_rpmArcProps[index];
    propID = srcItem.m_propID;
    varType = srcItem.m_varType;
    name = srcItem.m_propName;
    return RC_S_OK;
}

HResult RCRpmHandler::GetArchiveProperty(RCPropertyID propID, RCVariant& value) 
{
    ClearVariantValue(value) ;
    switch(propID)
    {
        case RCPropID::kpidArchiveType:
        {
            value = uint64_t(RC_ARCHIVE_RPM) ;
            break;
        }
        case RCPropID::kpidMainSubfile:
        {
            value = uint64_t(0) ;
            break;
        }
    }
    return RC_S_OK;
}

HResult RCRpmHandler::Open(IInStream* inStream, uint64_t maxCheckStartPosition, IArchiveOpenCallback* openArchiveCallback)
{
    try
    {
        Close() ;
        if(RCRpmFuncWrapper::OpenArchive(inStream) != RC_S_OK)
        {
            return RC_S_FALSE;
        }
        HResult result = (inStream->Seek(0, RC_STREAM_SEEK_CUR, &m_pos));
        if (!IsSuccess(result))
        {
            return result;
        }
        result = RCStreamUtils::ReadStream_FALSE(inStream, m_sig, sizeof(m_sig) / sizeof(m_sig[0])) ;
        if (!IsSuccess(result))
        {
            return result;
        }
        uint64_t endPosition = 0 ;
        result = (inStream->Seek(0, RC_STREAM_SEEK_END, &endPosition));
        if (!IsSuccess(result))
        {
            return result;
        }
        m_size = endPosition - m_pos;
        m_inStream = inStream;
        return RC_S_OK;
    }
    catch(...)
    {
        return RC_S_FALSE;
    }
}

HResult RCRpmHandler::Close()
{
    m_inStream.Release();
    return RC_S_OK;
}

HResult RCRpmHandler::GetNumberOfItems(uint32_t& numItems)
{
    numItems = 1;
    return RC_S_OK;
}

HResult RCRpmHandler::GetProperty(uint32_t index, RCPropertyID propID, RCVariant& value) 
{
    ClearVariantValue(value) ;
    RCVariant& prop = value ;
    switch(propID)
    {
    case RCPropID::kpidSize:
    case RCPropID::kpidPackSize:
        prop = m_size;
        break;
    case RCPropID::kpidExtension:
        {
            char_t s[32];
            RCString tt;
            RCRpmDefs::MyStringCopy(s, _T("cpio."));
            const char_t *ext;
            if (m_sig[0] == 0x1F && m_sig[1] == 0x8B)
            {
                ext = _T("gz");
            }
            else if (m_sig[0] == 'B' && m_sig[1] == 'Z' && m_sig[2] == 'h')
            {
                ext = _T("bz2");
            }
            else
            {
                ext = _T("lzma");
            }
            RCRpmDefs::MyStringCopy(s + RCRpmDefs::MyStringLen(s), ext);
            tt = s;
            prop = tt;
            break;
        }
    }
    return RC_S_OK;
}

HResult RCRpmHandler::Extract(const RCVector<uint32_t>& indices, int32_t testMode, IArchiveExtractCallback* extractCallback) 
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
    else
    {
        return RC_S_OK ;
    }
    HResult result = extractCallback->SetTotal(m_size) ;
    if (!IsSuccess(result))
    {
        return result;
    }
    
    int32_t askMode = testMode ? RC_ARCHIVE_EXTRACT_ASK_MODE_TEST : RC_ARCHIVE_EXTRACT_ASK_MODE_EXTRACT;
    ISequentialOutStreamPtr realOutStream;
    uint32_t index = 0 ;
    result = extractCallback->GetStream(index, realOutStream.GetAddress(), askMode) ;
    if (!IsSuccess(result))
    {
        return result;
    }
    if(!testMode && !realOutStream)
    {
        return RC_S_OK;
    }

    result = extractCallback->PrepareOperation(index,askMode) ;
    if (!IsSuccess(result))
    {
        return result;
    }

    ICompressCoderPtr copyCoder;
    result = RCCreateCoder::CreateCoder(m_compressCodecsInfo.Get(),RCMethod::ID_COMPRESS_COPY, copyCoder, false);
    if (!IsSuccess(result))
    {
        return result;
    }

    RCLocalProgress *lps = new RCLocalProgress;
    ICompressProgressInfoPtr progress = lps;
    lps->Init(extractCallback, false);
    
    result = m_inStream->Seek(m_pos, RC_STREAM_SEEK_SET, NULL) ;
    if (!IsSuccess(result))
    {
        return result;
    }

    result = copyCoder->Code(m_inStream.Get(), realOutStream.Get(), NULL, NULL, progress.Get()) ;
    if (!IsSuccess(result))
    {
        return result;
    }
    realOutStream.Release();
    return extractCallback->SetOperationResult(index,RC_ARCHIVE_EXTRACT_RESULT_OK);
}

HResult RCRpmHandler::SetCompressCodecsInfo(ICompressCodecsInfo* compressCodecsInfo) 
{
    if (compressCodecsInfo)
    {
        m_compressCodecsInfo = compressCodecsInfo;
        return RC_S_OK;
    }
    return RC_S_OK;
}

HResult RCRpmHandler::GetStream(uint32_t /*index*/, ISequentialInStream** stream, IArchiveOpenCallback* openCallback) 
{
    try
    {
        return RCStreamUtils::CreateLimitedInStream(m_inStream.Get(), m_pos, m_size, stream);
    }
    catch(...)
    {
        return RC_S_FALSE;
    }
}

END_NAMESPACE_RCZIP
