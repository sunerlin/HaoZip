/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "format/lzma/RCLzmaHandler.h"
#include "format/lzma/RCLzmaFiltersDecoder.h"
#include "format/common/RCDummyOutStream.h"
#include "common/RCStringUtil.h"
#include "common/RCLocalProgress.h"
#include "filesystem/RCStreamUtils.h"
#include "format/common/RCPropData.h"
#include "interface/RCArchiveType.h"

/////////////////////////////////////////////////////////////////
//RCLzmaHandler class implementation

BEGIN_NAMESPACE_RCZIP

static RCPropData s_lzmaProps[] =
{
    { RCString(_T("")), RCPropID::kpidSize, RCVariantType::RC_VT_UINT64},
    { RCString(_T("")), RCPropID::kpidPackSize, RCVariantType::RC_VT_UINT64},
    { RCString(_T("")), RCPropID::kpidMethod, RCVariantType::RC_VT_STRING}
};

static RCPropData s_lzmaArcProps[] =
{
    { RCString(_T("")), RCPropID::kpidArchiveType,    RCVariantType::RC_VT_UINT64 }
};

RCLzmaHandler::RCLzmaHandler(bool lzma86):
    m_lzma86(lzma86),
    m_startPosition(0),
    m_packSize(0),
    m_packSizeDefined(false)
{
}

HResult RCLzmaHandler::GetNumberOfProperties(uint32_t& numProperties)
{ 
    numProperties = sizeof(s_lzmaProps) / sizeof(s_lzmaProps[0]);

    return RC_S_OK; 
}

HResult RCLzmaHandler::GetPropertyInfo(uint32_t index, RCString& name, RCPropertyID& propID, RCVariantType::RCVariantID& varType)
{
    if (index >= sizeof(s_lzmaProps) / sizeof(s_lzmaProps[0])) 
    {
        return RC_E_INVALIDARG;
    }
    const RCPropData& srcItem = s_lzmaProps[index];

    propID  = srcItem.m_propID ; 
    varType = srcItem.m_varType ; 
    name    = srcItem.m_propName ;

    return RC_S_OK;
}

HResult RCLzmaHandler::GetNumberOfArchiveProperties(uint32_t& numProperties)
{
    numProperties = sizeof(s_lzmaArcProps) / sizeof(s_lzmaArcProps[0]);
    return RC_S_OK;
}

HResult RCLzmaHandler::GetArchivePropertyInfo(uint32_t index, RCString& name, RCPropertyID& propID, RCVariantType::RCVariantID& varType)
{
    if (index >= (sizeof(s_lzmaArcProps) / sizeof(s_lzmaArcProps[0])))
    {
        return RC_E_INVALIDARG;
    }
    const RCPropData& srcItem = s_lzmaArcProps[index];
    propID = srcItem.m_propID;
    varType = srcItem.m_varType;
    name = srcItem.m_propName;
    return RC_S_OK;
}

HResult RCLzmaHandler::GetArchiveProperty(RCPropertyID propID, RCVariant& value)
{
    ClearVariantValue(value) ;
    switch(propID)
    {
        case RCPropID::kpidArchiveType:
            if(m_lzma86)
            {
                value = uint64_t(RC_ARCHIVE_LZMA86) ;
            }
            else
            {
                value = uint64_t(RC_ARCHIVE_LZMA) ;
            }
            break;
        case RCPropID::kpidPhySize:
            if (m_packSizeDefined)
            {
                value = (uint64_t)m_packSize;
            }
            break;
    }
    return RC_S_OK ;
}

HResult RCLzmaHandler::GetNumberOfItems(uint32_t& numItems)
{
    numItems = 1;

    return RC_S_OK;
}

static void ConvertUInt32ToString(uint32_t value, char_t* s)
{
    RCStringUtil::ConvertUInt64ToString(value, s + RCStringUtil::StringLen(s));
}

static void DictSizeToString(uint32_t value, char_t* s)
{
    for (int32_t i = 0; i <= 31; i++)
    {
        if ((uint32_t(1) << i) == value)
        {
            ConvertUInt32ToString(i, s);
            return;
        }
    }

    char_t c = _T('b');
    if ((value & ((1 << 20) - 1)) == 0)
    {
        value >>= 20;
        c = _T('m');
    }
    else if ((value & ((1 << 10) - 1)) == 0)
    {
        value >>= 10;
        c = _T('k');
    }

    ConvertUInt32ToString(value, s);
    int32_t p = (int32_t)RCStringUtil::StringLen(s);
    s[p++] = c;
    s[p++] = _T('\0');
}

static void MyStrCat(char_t *d, const char_t *s)
{
    RCStringUtil::StringCopy(d + RCStringUtil::StringLen(d), s);
}

HResult RCLzmaHandler::GetProperty(uint32_t index, RCPropertyID propID, RCVariant& value)
{
    ClearVariantValue(value) ;
    switch(propID)
    {
    case RCPropID::kpidSize:
        if (m_stream && m_header.HasSize())
        {
            value = m_header.m_size ; 
        }
        break ;
    case RCPropID::kpidPackSize:
        if (m_packSizeDefined)
        {
            value = m_packSize;
        }
        break;
    case RCPropID::kpidMethod:
        if (m_stream)
        {
            const int32_t sMax = 64 ;
            char_t s[64] ;
            s[0] = '\0';
            if (m_header.m_filterID != 0)
            {
                MyStrCat(s, _T("BCJ "));
            }
            MyStrCat(s, _T("LZMA:"));
            DictSizeToString(m_header.GetDicSize(), s + RCStringUtil::StringLen(s));
            s[sMax - 1] = 0 ;
            value = RCString(s) ;
        }
        break;
    }
    return RC_S_OK;
}

HResult RCLzmaHandler::Open(IInStream* inStream,
                            uint64_t /*maxCheckStartPosition*/,
                            IArchiveOpenCallback* /*openArchiveCallback*/)
{
    if(inStream == NULL)
    {
        return RC_E_INVALIDARG ;
    }
    HResult hr = inStream->Seek(0, RC_STREAM_SEEK_CUR, &m_startPosition) ;
    if(!IsSuccess(hr))
    {
        return hr ;
    }  
    const uint32_t kBufSize = 1 + 5 + 8 + 1;
    byte_t buf[kBufSize];
  
    hr = RCStreamUtils::ReadStream_FALSE(inStream, buf, kBufSize) ;
    if(!IsSuccess(hr))
    {
        return hr ;
    }
  
    if (!m_header.Parse(buf, m_lzma86))
    {
        return RC_S_FALSE;
    }
    const byte_t* start = buf + GetHeaderSize() ;
    if (start[0] != 0)
    {
        return RC_S_FALSE ;
    }
  
    uint64_t endPos = 0 ;
    hr = inStream->Seek(0, RC_STREAM_SEEK_END, &endPos) ;
    m_packSize = endPos - m_startPosition ;
    m_packSizeDefined = true ;
  
    m_stream = inStream ;
    m_seqStream = inStream ;
    return RC_S_OK ;
}

HResult RCLzmaHandler::Close()
{
    m_packSizeDefined = false ;
    m_stream.Release() ;
    m_seqStream.Release() ;
    return RC_S_OK ;
}

HResult RCLzmaHandler::Extract(const RCVector<uint32_t>& indices,
                               int32_t testMode,
                               IArchiveExtractCallback* extractCallback)
{
    if (!indices.empty() && (indices[0] != 0) )
    {
        return RC_E_INVALIDARG ;
    }
    uint32_t numItems = (uint32_t)indices.size() ;
    if (numItems == 0)
    {
        numItems = 1 ;
    }
    
    if (m_stream)
    {
        extractCallback->SetTotal(m_packSize) ;
    }   
  
    ISequentialOutStreamPtr realOutStream ;
    int32_t askMode = testMode ? RC_ARCHIVE_EXTRACT_ASK_MODE_TEST :
                                 RC_ARCHIVE_EXTRACT_ASK_MODE_EXTRACT ;
    HResult hr = extractCallback->GetStream(0, realOutStream.GetAddress(), askMode) ;
    if(!IsSuccess(hr))
    {
        return hr ;
    }
    if (!testMode && !realOutStream)
    {
        return RC_S_OK ;
    }
  
    extractCallback->PrepareOperation(0, askMode) ;

    RCDummyOutStream* outStreamSpec = new RCDummyOutStream ;
    ISequentialOutStreamPtr outStream(outStreamSpec) ;
    outStreamSpec->SetStream(realOutStream.Get());
    outStreamSpec->Init();
    realOutStream.Release();

    RCLocalProgress* lps = new RCLocalProgress;
    ICompressProgressInfoPtr progress = lps;
    lps->Init(extractCallback, true) ;

    if (m_stream)
    {
        hr = m_stream->Seek(m_startPosition, RC_STREAM_SEEK_SET, NULL) ;
        if(!IsSuccess(hr))
        {
            return hr ;
        }
    }

    RCLzmaFiltersDecoder decoder ;
    hr = decoder.Create( m_compressCodecsInfo.Get(), m_lzma86, m_seqStream.Get()) ;
    if(!IsSuccess(hr))
    {
        return hr ;
    }
 
    int32_t opRes = RC_ARCHIVE_EXTRACT_RESULT_OK ;
    bool firstItem = true ;
    for (;;)
    {
        m_packSize = decoder.GetInputProcessedSize() ;
        lps->SetOutSize( outStreamSpec->GetSize() ) ;
        lps->SetInSize(m_packSize) ; 
        m_packSizeDefined = true;
        hr = lps->SetCur() ;
        if(!IsSuccess(hr))
        {
            return hr ;
        }

        RCLzmaHeader st ;
        const uint32_t kBufSize = 1 + 5 + 8 ;
        byte_t buf[kBufSize];
        const uint32_t headerSize = GetHeaderSize() ;
        uint32_t processed ;
        hr = decoder.ReadInput(buf, headerSize, &processed) ;
        if(!IsSuccess(hr))
        {
            return hr ;
        }
        if (processed != headerSize)
        {
            break;
        }
  
        if (!st.Parse(buf, m_lzma86))
        {
            break;
        }
        firstItem = false ;

        hr = decoder.Code(st, outStream.Get(), progress.Get()) ;
        if (hr == RC_E_NOTIMPL)
        {
            opRes = RC_ARCHIVE_EXTRACT_RESULT_UNSUPPORTED_METHOD;
            break;
        }
        if (hr == RC_S_FALSE)
        {
            opRes = RC_ARCHIVE_EXTRACT_RESULT_DATA_ERROR;
            break;
        }
        if(!IsSuccess(hr))
        {
            return hr ;
        }
    }
    if (firstItem)
    {
        return RC_E_FAIL;
    }
    outStream.Release();
    return extractCallback->SetOperationResult(0, opRes);
}

HResult RCLzmaHandler::SetCompressCodecsInfo(ICompressCodecsInfo* compressCodecsInfo)
{
    if (!compressCodecsInfo)
    {
        return RC_S_FALSE;
    }

    m_compressCodecsInfo = compressCodecsInfo;

    return RC_S_OK;
}

uint32_t RCLzmaHandler::GetHeaderSize() const
{
    return 5 + 8 + (m_lzma86 ? 1 : 0) ;
}

END_NAMESPACE_RCZIP
