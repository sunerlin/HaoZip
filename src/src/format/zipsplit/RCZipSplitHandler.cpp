/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#include "format/zipsplit/RCZipSplitHandler.h"
#include "interface/RCMethodDefs.h"
#include "common/RCCreateCoder.h"
#include "common/RCLocalProgress.h"
#include "common/RCStringUtil.h"
#include "format/common/RCMultiStream.h"
#include "format/common/RCPropData.h"
#include "interface/RCArchiveType.h"

BEGIN_NAMESPACE_RCZIP

static RCPropData s_zipSplitProps[] =
{
    { RCString(_T("")), RCPropID::kpidPath,      RCVariantType::RC_VT_STRING },
    { RCString(_T("")), RCPropID::kpidSize,      RCVariantType::RC_VT_UINT64 },
    { RCString(_T("")), RCPropID::kpidPackSize,  RCVariantType::RC_VT_UINT64 }
};

static RCPropData s_zipSplitArcProps[] =
{
    { RCString(_T("")), RCPropID::kpidArchiveType, RCVariantType::RC_VT_STRING }
};

HResult RCZipSplitHandler::GetNumberOfProperties(uint32_t& numProperties)
{
    numProperties = sizeof(s_zipSplitProps) / sizeof(s_zipSplitProps[0]);
    return RC_S_OK;
}

HResult RCZipSplitHandler::GetPropertyInfo(uint32_t index, RCString& name, RCPropertyID& propID, RCVariantType::RCVariantID& varType)
{
    if (index >= (sizeof(s_zipSplitProps) / sizeof(s_zipSplitProps[0])))
    {
        return RC_E_INVALIDARG;
    }
    const RCPropData& srcItem = s_zipSplitProps[index];

    propID  = srcItem.m_propID;
    varType = srcItem.m_varType;
    name    = srcItem.m_propName;
    return RC_S_OK;
}

HResult RCZipSplitHandler::GetNumberOfArchiveProperties(uint32_t& numProperties) 
{
    numProperties = sizeof(s_zipSplitArcProps) / sizeof(s_zipSplitArcProps[0]);
    return RC_S_OK;
}

HResult RCZipSplitHandler::GetArchivePropertyInfo(uint32_t index, RCString& name, RCPropertyID& propID, RCVariantType::RCVariantID& varType)
{
    if (index >= (sizeof(s_zipSplitArcProps) / sizeof(s_zipSplitArcProps[0])))
    {
        return RC_E_INVALIDARG;
    }
    const RCPropData& srcItem = s_zipSplitArcProps[index];
    propID  = srcItem.m_propID;
    varType = srcItem.m_varType;
    name    = srcItem.m_propName;
    return RC_S_OK;
}

HResult RCZipSplitHandler::GetArchiveProperty(RCPropertyID propID, RCVariant& value)
{
    ClearVariantValue(value);
    switch(propID)
    {
        case RCPropID::kpidArchiveType:
        {
            value = uint64_t(RC_ARCHIVE_ZIP_SPLIT);
            break;
        }
    }
    return RC_S_OK;
}

HResult RCZipSplitHandler::GetProperty(uint32_t index, RCPropertyID propID, RCVariant& value) 
{
    ClearVariantValue(value);
    RCVariant& prop = value;
    switch(propID)
    {
    case RCPropID::kpidPath:
        prop = m_subName;
        break;
    case RCPropID::kpidSize:
    case RCPropID::kpidPackSize:
        prop = m_totalSize;
        break;
    }
    return RC_S_OK;
}

HResult RCZipSplitHandler::Open(IInStream* stream, 
                                uint64_t maxCheckStartPosition, 
                                IArchiveOpenCallback* openArchiveCallback)
{
    Close();
    if (openArchiveCallback == NULL) 
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

    uint64_t streamSize = 0 ;
    {
        RCVariant prop;
        HResult  hr = openVolumeCallback->GetProperty(RCPropID::kpidName, prop);
        if (!IsSuccess(hr))
        {
            return RC_S_FALSE;
        }
        if (!IsStringType(prop))
        {
            return RC_S_FALSE;
        }
        m_name = GetStringValue(prop);
        hr = openVolumeCallback->GetProperty(RCPropID::kpidSize, prop);
        if (!IsSuccess(hr))
        {
            return RC_S_FALSE;
        }
        if (!IsUInt64Type(prop))
        {
            return RC_S_FALSE;
        }
        streamSize = GetUInt64Value(prop);
    }

    RCString::size_type dotPos = m_name.find_last_of(_T('.'));
    RCString prefix ;
    RCString ext;
    if (dotPos != RCString::npos)
    {
        prefix = RCStringUtil::Left(m_name, (int32_t)dotPos + 1);
        ext = RCStringUtil::Mid(m_name, (int32_t)dotPos + 1);
    }
    else 
    {
        return RC_S_FALSE;
    }

    RCString extBig = ext;
    RCStringUtil::MakeUpper(extBig);

    //检查打开的文件后缀名
    //如果不是 ZXX 或 ZIP,直接返回错误
    bool checkExt = false;

    if (extBig == RCString(_T("ZIP")))
    {
        checkExt = true;
    }
    else
    {
        for (uint32_t i = 1; i <= 99; i++)
        {
            RCString ext;
            RCStringUtil::Format(ext, _T("Z%.2d"), i);
            if (ext == extBig)
            {
                checkExt = true;
                break;
            }
        }
    }

    if (checkExt == false)
    {   //不是 zip 或 zxx, 直接返回错误
        return RC_S_FALSE;
    }

    if (prefix.size() < 1)
    {
        m_subName = _T("file");
    }
    else
    {
        m_subName = RCStringUtil::Left(prefix, (int32_t)prefix.size() - 1);
    }

    int32_t seekIndex = 1;
    m_totalSize = 0;
    uint64_t size = 0 ;

    RCString fullName;
    for (;;)
    {
        RCString seekExt;
        if (seekIndex < 100)
        {
            RCStringUtil::Format(seekExt, _T("Z%.2d"), seekIndex);
        }
        else
        {
            RCStringUtil::Format(seekExt, _T("Z%d"), seekIndex);
        }
        RCStringUtil::MakeUpper(seekExt);
        if (seekExt == extBig)
        {
            m_totalSize += streamSize;
            m_sizes.push_back(streamSize);
            m_inStreams.push_back(stream);
        }
        else
        {
            fullName = prefix + seekExt;
            IInStreamPtr nextStream;
            HResult hr = openVolumeCallback->GetStream(fullName, nextStream.GetAddress());
            if (!IsSuccess(hr))
            {
                if(seekIndex == 1)
                {
                    //第一个卷打不开，说明不是分卷
                    return RC_S_FALSE ;
                }
                else
                {
                    break;
                }
            }

            {
                RCVariant prop;
                HResult hr = openVolumeCallback->GetProperty(RCPropID::kpidSize, prop);
                if (!IsSuccess(hr))
                {
                    return RC_S_FALSE;
                }

                if (!IsUInt64Type(prop))
                {
                    return RC_S_FALSE;
                }
                size = GetUInt64Value(prop);
            }
            m_totalSize += size;
            m_sizes.push_back(size);
            m_inStreams.push_back(nextStream);
        }
        
        if (openArchiveCallback != NULL)
        {
            uint64_t numFiles = m_inStreams.size();
            HResult hr = openArchiveCallback->SetCompleted(numFiles, 0);
            if (!IsSuccess(hr))
            {
                return hr;
            }
        }
        seekIndex++;
    }

    //添加最后的.zip文件
    fullName = prefix + _T("zip");
    IInStreamPtr lastStream;
    HResult hr = openVolumeCallback->GetStream(fullName, lastStream.GetAddress());
    if (!IsSuccess(hr))
    {
        return RC_S_FALSE;
    }

    {
        RCVariant prop;
        HResult hr = openVolumeCallback->GetProperty(RCPropID::kpidSize, prop);
        if (!IsSuccess(hr))
        {
            return RC_S_FALSE;
        }
        if (!IsUInt64Type(prop))
        {
            return RC_S_FALSE;
        }
        size = GetUInt64Value(prop);
        m_totalSize += size;
        m_sizes.push_back(size);
        m_inStreams.push_back(lastStream);
    }

    return RC_S_OK;
}

HResult RCZipSplitHandler::Close() 
{
    m_sizes.clear();
    m_inStreams.clear();
    m_totalSize = 0 ;
    return RC_S_OK;
}

HResult RCZipSplitHandler::GetNumberOfItems(uint32_t& numItems) 
{
    numItems = m_inStreams.empty() ? 0 : 1;
    return RC_S_OK;
}

HResult RCZipSplitHandler::SetCompressCodecsInfo(ICompressCodecsInfo* compressCodecsInfo) 
{
    if (!compressCodecsInfo)
    {
        return RC_S_FALSE;
    }
    m_compressCodecsInfo = compressCodecsInfo;
    return RC_S_OK;
}

HResult RCZipSplitHandler::Extract(const std::vector<uint32_t>& indices, int32_t testMode, IArchiveExtractCallback* extractCallback)
{
    return RC_S_FALSE;
}

HResult RCZipSplitHandler::GetStream(uint32_t index, 
                                     ISequentialInStream** stream, 
                                     IArchiveOpenCallback* openCallback) 
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
