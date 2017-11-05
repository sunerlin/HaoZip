/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "format/split/RCSplitHandler.h"
#include "interface/RCMethodDefs.h"
#include "common/RCCreateCoder.h"
#include "common/RCLocalProgress.h"
#include "common/RCStringUtil.h"
#include "format/common/RCMultiStream.h"
#include "format/common/RCPropData.h"
#include "interface/RCArchiveType.h"

/////////////////////////////////////////////////////////////////
//RCSplitHandler class implementation

BEGIN_NAMESPACE_RCZIP

static RCPropData s_splitProps[] =
{
    { RCString(_T("")), RCPropID::kpidPath,     RCVariantType::RC_VT_STRING },
    { RCString(_T("")), RCPropID::kpidSize,     RCVariantType::RC_VT_UINT64 },
};

static RCPropData s_splitArcProps[] =
{
    { RCString(_T("")), RCPropID::kpidArchiveType,  RCVariantType::RC_VT_UINT64 },
    { RCString(_T("")), RCPropID::kpidNumVolumes,   RCVariantType::RC_VT_UINT64 }
};

/** 分卷名称
*/
class RCSeqName
{
public:

    /** 分卷名固定部分
    */
    RCString m_unchangedPart;

    /** 分卷名变动部分
    */
    RCString m_changedPart;

    /** 分卷类型
    */
    bool m_splitStyle;

public:

    /** 取得下一个分卷名称
    @return 返回下一分卷名称
    */
    RCString GetNextName()
    {
        RCString newName;
        if (m_splitStyle)
        {
            int32_t i;
            int32_t numLetters = (int32_t)m_changedPart.length();
            for (i = numLetters - 1; i >= 0; i--)
            {
                wchar_t c = m_changedPart[i];
                if (c == 'z')
                {
                    c = 'a';
                    newName = c + newName;
                    continue;
                }
                else if (c == 'Z')
                {
                    c = 'A';
                    newName = c + newName;
                    continue;
                }
                c++;
                if ((c == 'z' || c == 'Z') && i == 0)
                {
                    m_unchangedPart += c;
                    wchar_t newChar = (c == 'z') ? L'a' : L'A';
                    newName.clear();
                    numLetters++;
                    for (int32_t k = 0; k < numLetters; k++)
                    {
                        newName += newChar;
                    }
                    break;
                }
                newName = c + newName;
                i--;
                for (; i >= 0; i--)
                {
                    newName = m_changedPart[i] + newName;
                }
                break;
            }
        }
        else
        {
            int32_t i;
            int32_t numLetters = (int32_t)m_changedPart.length();
            for (i = numLetters - 1; i >= 0; i--)
            {
                wchar_t c = m_changedPart[i];
                if (c == L'9')
                {
                    c = L'0';
                    newName = c + newName;
                    if (i == 0)
                    {
                        newName = RCString(L"1") + newName;
                    }
                    continue;
                }
                c++;
                newName = c + newName;
                i--;
                for (; i >= 0; i--)
                {
                    newName = m_changedPart[i] + newName;
                }
                break;
            }
        }
        m_changedPart = newName;
        return m_unchangedPart + m_changedPart;
    }
};

HResult RCSplitHandler::GetNumberOfProperties(uint32_t& numProperties)
{ 
    numProperties = sizeof(s_splitProps) / sizeof(s_splitProps[0]);

    return RC_S_OK; 
}

HResult RCSplitHandler::GetPropertyInfo(uint32_t index, RCString& name, RCPropertyID& propID, RCVariantType::RCVariantID& varType)
{
    if (index >= sizeof(s_splitProps) / sizeof(s_splitProps[0])) 
    {
        return RC_E_INVALIDARG;
    }
    const RCPropData &srcItem = s_splitProps[index];

    propID  = srcItem.m_propID ; 
    varType = srcItem.m_varType ; 
    name    = srcItem.m_propName ;

    return RC_S_OK;
}

HResult RCSplitHandler::GetNumberOfArchiveProperties(uint32_t& numProperties)
{
    numProperties = sizeof(s_splitArcProps) / sizeof(s_splitArcProps[0]);
    return RC_S_OK;
}

HResult RCSplitHandler::GetArchivePropertyInfo(uint32_t index, RCString& name, RCPropertyID& propID, RCVariantType::RCVariantID& varType)
{
    if (index >= (sizeof(s_splitArcProps) / sizeof(s_splitArcProps[0])))
    {
        return RC_E_INVALIDARG;
    }
    const RCPropData& srcItem = s_splitArcProps[index];
    propID = srcItem.m_propID;
    varType = srcItem.m_varType;
    name = srcItem.m_propName;
    return RC_S_OK;
}

HResult RCSplitHandler::GetArchiveProperty(RCPropertyID propID, RCVariant& value)
{
    ClearVariantValue(value) ;
    switch(propID)
    {
        case RCPropID::kpidArchiveType:
        {
            value = uint64_t(RC_ARCHIVE_SPLIT) ;
            break;
        }
        case RCPropID::kpidNumVolumes:
        {
            value = (uint64_t)m_inStreams.size() ;
            break;
        }
    }
    return RC_S_OK;
}

HResult RCSplitHandler::Open(IInStream* stream, 
                             uint64_t maxCheckStartPosition,
                             IArchiveOpenCallback* openArchiveCallback)
{
    try
    {
        return OpenReal(stream, maxCheckStartPosition, openArchiveCallback) ;
    }
    catch(...)
    {
        return RC_S_FALSE ;
    }
}

HResult RCSplitHandler::OpenReal(IInStream* stream, 
                                 uint64_t maxCheckStartPosition,
                                 IArchiveOpenCallback* openArchiveCallback)
{
    Close();
    if (openArchiveCallback == 0)
    {
        return RC_S_FALSE;
    }

    IArchiveOpenVolumeCallbackPtr openVolumeCallback;
    IArchiveOpenCallbackPtr openArchiveCallbackWrap = openArchiveCallback;
    if (openArchiveCallbackWrap.QueryInterface(IID_IArchiveOpenVolumeCallback,
                                               openVolumeCallback.GetAddress()) != RC_S_OK)
    {
        return RC_S_FALSE;
    }

    RCString name ;
    {
        RCVariant prop;
        HResult hr = openVolumeCallback->GetProperty(RCPropID::kpidName, prop);
        if (!IsSuccess(hr))
        {
            return RC_S_FALSE;
        }

        if (!IsStringType(prop))
        {
            return RC_S_FALSE;
        }
        name = GetStringValue(prop) ;
    }

    RCString::size_type dotPos = name.find_last_of('.');
    RCString prefix, ext;
    if (dotPos != RCString::npos)
    {
        prefix = RCStringUtil::Left(name,(int32_t)dotPos + 1);
        ext = RCStringUtil::Mid(name, (int32_t)dotPos + 1);
    }
    else
    {
        ext = name ;
    }

    RCString extBig = ext;
    RCStringUtil::MakeUpper(extBig);

    RCSeqName seqName;

    RCString::size_type numLetters = 2;
    bool splitStyle = false;
    if (RCStringUtil::Right(extBig, 2) == L"AA")
    {
        splitStyle = true;
        while (numLetters < extBig.length())
        {
            if (extBig[extBig.length() - numLetters - 1] != 'A')
            {
                break;
            }
            numLetters++;
        }
    }
    else if (RCStringUtil::Right(ext, 2) == L"01")
    {
        while (numLetters < extBig.length())
        {
            if (extBig[extBig.length() - numLetters - 1] != '0')
            {
                break;
            }
            numLetters++;
        }
        if (numLetters != ext.length())
        {
            return RC_S_FALSE;
        }
    }
    else
    {
        return RC_S_FALSE;;
    }

    m_inStreams.push_back(stream);

    seqName.m_unchangedPart = prefix + RCStringUtil::Left(ext, (int32_t)(extBig.length() - numLetters));
    seqName.m_changedPart =  RCStringUtil::Right(ext, (int32_t)numLetters);
    seqName.m_splitStyle = splitStyle;

    if (prefix.length() < 1)
    {
        m_subName = L"file";
    }
    else
    {
        m_subName = RCStringUtil::Left(prefix, (int32_t)prefix.length() - 1);
    }

    m_totalSize = 0;
    uint64_t size;

    {
        RCVariant prop;
        HResult hr  = openVolumeCallback->GetProperty(RCPropID::kpidSize, prop);
        if (!IsSuccess(hr))
        {
            return RC_S_FALSE;
        }

        if (!IsUInt64Type(prop))
        {
            return RC_E_INVALIDARG;
        }
        size = GetUInt64Value(prop);
    }

    m_totalSize += size;
    m_sizes.push_back(size);

    if (openArchiveCallback != NULL)
    {
        uint64_t numFiles = m_inStreams.size();
        HResult hr = openArchiveCallback->SetCompleted(numFiles, NULL);
        if (!IsSuccess(hr))
        {
            return RC_S_FALSE;
        }
    }

    for (;;)
    {
        RCString fullName = seqName.GetNextName();
        IInStreamPtr nextStream;
        HResult hr = openVolumeCallback->GetStream(fullName, nextStream.GetAddress());
        if (!IsSuccess(hr))
        {
            break;
        }

        if (hr != RC_S_OK)
        {
            return hr;
        }

        if (!stream)
        {
            break;
        }

        {
            RCVariant prop;
            HResult hr = openVolumeCallback->GetProperty(RCPropID::kpidSize, prop);
            if (!IsSuccess(hr))
            {
                return hr;
            }

            if (!IsUInt64Type(prop))
            {
                return RC_E_INVALIDARG;
            }
            size = GetUInt64Value(prop);
        }

        m_totalSize += size;
        m_sizes.push_back(size);
        m_inStreams.push_back(nextStream);

        if (openArchiveCallback != NULL)
        {
            uint64_t numFiles = m_inStreams.size();
            HResult hr = openArchiveCallback->SetCompleted(numFiles, NULL);
            if (!IsSuccess(hr))
            {
                return hr;
            }
        }
    }

    return RC_S_OK;
}

HResult RCSplitHandler::Close()
{  
    m_sizes.clear();
    m_inStreams.clear();

    return RC_S_OK;
}

HResult RCSplitHandler::GetNumberOfItems(uint32_t& numItems)
{
    numItems = m_inStreams.empty() ? 0 : 1;

    return RC_S_OK;
}

HResult RCSplitHandler::GetProperty(uint32_t index, RCPropertyID propID, RCVariant& value)
{    
    ClearVariantValue(value) ;
    RCVariant& prop = value;

    switch(propID)
    {
    case RCPropID::kpidPath:
        {
            prop = m_subName;
        }
        break;

    case RCPropID::kpidSize:
    case RCPropID::kpidPackSize:
        {
            prop = m_totalSize;
        }
        break;
    }

    return RC_S_OK;
}


HResult RCSplitHandler::Extract(const std::vector<uint32_t>& indices,
                                int32_t testMode,
                                IArchiveExtractCallback* extractCallback)

{ 
    uint32_t numItems = static_cast<uint32_t>(indices.size());
    if (numItems != 0)
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

    HResult hr = extractCallback->SetTotal(m_totalSize);
    if (!IsSuccess(hr))
    {
        return hr;
    }

    uint64_t currentTotalSize = 0 ;
    ISequentialOutStreamPtr outStream ;
    int32_t askMode = testMode ? RC_ARCHIVE_EXTRACT_ASK_MODE_TEST :
                                 RC_ARCHIVE_EXTRACT_ASK_MODE_EXTRACT;
    int32_t index = 0;
    hr = extractCallback->GetStream(index, outStream.GetAddress(), askMode);
    if (!IsSuccess(hr))
    {
        return hr;
    }
    
    if (!testMode && !outStream)
    {
        return RC_S_OK;
    }

    hr = extractCallback->PrepareOperation(index,askMode);
    if (!IsSuccess(hr))
    {
        return hr;
    }   

    ICompressCoderPtr copyCoder;
    hr = RCCreateCoder::CreateCoder(m_compressCodecsInfo.Get(), RCMethod::ID_COMPRESS_COPY, copyCoder, false);
    if (!IsSuccess(hr))
    {
        return hr;
    }

    ICompressGetInStreamProcessedSizePtr copyCoderSize;
    hr = copyCoder.QueryInterface(ICompressGetInStreamProcessedSize::IID, copyCoderSize.GetAddress());
    if (!IsSuccess(hr))
    {
        return hr;
    }

    RCLocalProgress* lps = new RCLocalProgress;
    ICompressProgressInfoPtr progress = lps;
    lps->Init(extractCallback, false);

    for (int32_t i = 0; i < (int32_t)m_inStreams.size(); i++)
    {
        lps->SetInSize(currentTotalSize);
        lps->SetOutSize(currentTotalSize);
        hr = lps->SetCur();
        if (!IsSuccess(hr))
        {
            return hr;
        }

        IInStream* inStream = m_inStreams[i].Get();
        hr = inStream->Seek(0, RC_STREAM_SEEK_SET, NULL);
        if (!IsSuccess(hr))
        {
            return hr;
        }

        hr = copyCoder->Code(inStream, outStream.Get(), NULL, NULL, progress.Get());
        if (!IsSuccess(hr))
        {
            return hr;
        }
        uint64_t currentItemSize = 0 ;
        hr = copyCoderSize->GetInStreamProcessedSize(currentItemSize);
        if (!IsSuccess(hr))
        {
            return hr;
        }         
        currentTotalSize += currentItemSize ;
    }
    outStream.Release();

    return extractCallback->SetOperationResult(index,RC_ARCHIVE_EXTRACT_RESULT_OK);
}

HResult RCSplitHandler::GetStream(uint32_t index, ISequentialInStream** stream, IArchiveOpenCallback* openCallback)
{
    if (index != 0)
    {
        return RC_E_INVALIDARG;
    }

    *stream = 0;
    RCMultiStream *streamSpec = new RCMultiStream;
    ISequentialInStreamPtr streamTemp = streamSpec;
    for (int32_t i = 0; i < (int32_t)m_inStreams.size(); i++)
    {
        RCMultiStream::CSubStreamInfo subStreamInfo;
        subStreamInfo.m_spStream = m_inStreams[i];
        subStreamInfo.m_pos = 0;
        subStreamInfo.m_size = m_sizes[i];
        streamSpec->m_streams.push_back(subStreamInfo);
    }
    streamSpec->Init();
    *stream = streamTemp.Detach();

    return RC_S_OK;
}

END_NAMESPACE_RCZIP
