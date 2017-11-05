/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "format/nsis/RCNsisHandler.h"
#include "filesystem/RCFileTime.h"
#include "filesystem/RCStreamUtils.h"
#include "format/common/RCItemNameUtils.h"
#include "common/RCStringConvert.h"
#include "format/common/RCPropData.h"
#include "interface/RCArchiveType.h"
#include "algorithm/CpuArch.h"

#define Get32(p) GetUi32(p)

/////////////////////////////////////////////////////////////////
//RCNsisHandler class implementation

BEGIN_NAMESPACE_RCZIP

static const char_t* s_nsisBcjMethod = _T("BCJ");

static const char_t* s_nsisUnknownMethod = _T("Unknown");

static const char_t* s_nsisMethods[] =
{
    _T("Copy"),
    _T("Deflate"),
    _T("BZip2"),
    _T("LZMA")
};

static const int s_nsisNumMethods = sizeof(s_nsisMethods) / sizeof(s_nsisMethods[0]);

static RCPropData s_nsisProps[] =
{
    { RCString(_T("")), RCPropID::kpidPath,     RCVariantType::RC_VT_STRING },
    { RCString(_T("")), RCPropID::kpidSize,     RCVariantType::RC_VT_UINT64 },
    { RCString(_T("")), RCPropID::kpidPackSize, RCVariantType::RC_VT_UINT64 },
    { RCString(_T("")), RCPropID::kpidMTime,    RCVariantType::RC_VT_UINT64 },
    { RCString(_T("")), RCPropID::kpidMethod,   RCVariantType::RC_VT_STRING },
    { RCString(_T("")), RCPropID::kpidSolid,    RCVariantType::RC_VT_BOOL }
};

static RCPropData s_nsisArcProps[] =
{
    { RCString(_T("")), RCPropID::kpidMethod, RCVariantType::RC_VT_STRING},
    { RCString(_T("")), RCPropID::kpidSolid,  RCVariantType::RC_VT_BOOL},
    { RCString(_T("")), RCPropID::kpidArchiveType, RCVariantType::RC_VT_UINT64 }
};

static RCString ConvertUInt32ToString(uint32_t value)
{
    char_t buffer[32];
    RCStringUtil::ConvertUInt64ToString(value, buffer);
    return buffer;
}

static RCString GetStringForSizeValue(uint32_t value)
{
    for (int i = 31; i >= 0; i--)
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
        result += _T("m");
    }
    else if (value % (1 << 10) == 0)
    {
        result += ConvertUInt32ToString(value >> 10);
        result += _T("k");
    }
    else
    {
        result += ConvertUInt32ToString(value);
        result += _T("b");
    }
    return result;
}

RCString RCNsisHandler::GetMethod(bool useItemFilter, uint32_t dictionary) const
{
    RCNsisMethodType::EEnum methodIndex = m_inArchive.m_method;
    RCString method;
    if (m_inArchive.m_isSolid && m_inArchive.m_useFilter || !m_inArchive.m_isSolid && useItemFilter)
    {
        method += s_nsisBcjMethod;
        method += _T(" ");
    }

    method += (methodIndex < s_nsisNumMethods) ? s_nsisMethods[methodIndex] : s_nsisUnknownMethod;
    if (methodIndex == RCNsisMethodType::kLZMA)
    {
        method += _T(":");
        method += GetStringForSizeValue(m_inArchive.m_isSolid ? m_inArchive.m_dictionarySize: dictionary);
    }
    return method;
}

bool RCNsisHandler::GetUncompressedSize(int index, uint32_t &size)
{
    size = 0;
    const RCNsisItem& item = m_inArchive.m_items[index];
    if (item.m_sizeIsDefined)
    {
        size = item.m_size;
    }
    else if (m_inArchive.m_isSolid && item.m_estimatedSizeIsDefined)
    {
        size = item.m_estimatedSize;
    }
    else
    {
        return false;
    }
    return true;
}

bool RCNsisHandler::GetCompressedSize(int index, uint32_t &size)
{
    size = 0;
    const RCNsisItem &item = m_inArchive.m_items[index];
    if (item.m_compressedSizeIsDefined)
    {
        size = item.m_compressedSize;
    }
    else
    {
        if (m_inArchive.m_isSolid)
        {
            if (index == 0)
            {
                size = m_inArchive.m_firstHeader.GetDataSize();
            }
            else
            {
                return false;
            }
        }
        else
        {
            if (!item.m_isCompressed)
            {
                size = item.m_size;
            }
            else
            {
                return false;
            }
        }
    }
    return true;
}

HResult RCNsisHandler::GetNumberOfProperties(uint32_t& numProperties)
{ 
    numProperties = sizeof(s_nsisProps) / sizeof(s_nsisProps[0]);

    return RC_S_OK; 
}

HResult RCNsisHandler::GetPropertyInfo(uint32_t index, RCString& name, RCPropertyID& propID, RCVariantType::RCVariantID& varType)
{
    if (index >= sizeof(s_nsisProps) / sizeof(s_nsisProps[0])) 
    {
        return RC_E_INVALIDARG;
    }
    const RCPropData& srcItem = s_nsisProps[index];

    propID  = srcItem.m_propID ; 
    varType = srcItem.m_varType ; 
    name    = srcItem.m_propName ;

    return RC_S_OK;
}

HResult RCNsisHandler::GetNumberOfArchiveProperties(uint32_t& numProperties)
{

    numProperties = sizeof(s_nsisArcProps) / sizeof(s_nsisArcProps[0]);

    return RC_S_OK;
}

HResult RCNsisHandler::GetArchivePropertyInfo(uint32_t index, RCString& name, RCPropertyID& propID, RCVariantType::RCVariantID& varType)
{
    if(index >= sizeof(s_nsisArcProps) / sizeof(s_nsisArcProps[0])) 
    {
        return RC_E_INVALIDARG;
    }

    const RCPropData& srcItem = s_nsisArcProps[index];

    propID  = srcItem.m_propID ; 
    varType = srcItem.m_varType ; 
    name    = srcItem.m_propName ;

    return RC_S_OK; 
}

HResult RCNsisHandler::GetArchiveProperty(RCPropertyID propID, RCVariant& value)
{
    ClearVariantValue(value) ;
    RCVariant& prop = value;
    switch(propID)
    {
    case RCPropID::kpidMethod:
        {
            uint32_t dict = 1;
            bool filter = false;
            for (int32_t i = 0; i < (int32_t)m_inArchive.m_items.size(); i++)
            {
                const RCNsisItem& item = m_inArchive.m_items[i];
                filter |= item.m_useFilter;
                if (item.m_dictionarySize > dict)
                {
                    dict = item.m_dictionarySize;
                }
            }
            prop = GetMethod(filter, dict);
        }
        break;

    case RCPropID::kpidSolid:
        {
            prop = m_inArchive.m_isSolid;
        }
        break;
    case RCPropID::kpidArchiveType:
        {
            prop = uint64_t(RC_ARCHIVE_NSIS) ;
            break;
        }
    }

    return RC_S_OK;
}

HResult RCNsisHandler::GetNumberOfItems(uint32_t& numItems)
{
    numItems = (uint32_t)m_inArchive.m_items.size()
#ifdef NSIS_SCRIPT
        + 1
#endif
        ;
    return RC_S_OK;
}

HResult RCNsisHandler::GetProperty(uint32_t index, RCPropertyID propID, RCVariant& value)
{
    ClearVariantValue(value) ;
    RCVariant& prop = value;
#ifdef NSIS_SCRIPT
    if (index >= (uint32_t)m_inArchive.m_items.Size())
    {
        switch(propID)
        {
        case RCPropID::kpidPath:
            {
                prop = RCString(_T("[NSIS].nsi"));
            }
            break;

        case RCPropID::kpidSize:
        case RCPropID::kpidPackSize:
            {
                prop = (uint64_t)m_inArchive.m_script.length();
            }
            break;

        case RCPropID::kpidSolid:
            {
                prop = false;
            }
            break;
        }
    }
    else
#endif
    {
        const RCNsisItem& item = m_inArchive.m_items[index];
        switch(propID)
        {
        case RCPropID::kpidPath:
            {
                RCString s = RCItemNameUtils::WinNameToOSName(item.GetReducedName(m_inArchive.m_isUnicode));
                if (!s.empty())
                {
                    prop = s;
                }
            }
            break;

        case RCPropID::kpidSize:
            {
                uint32_t size;
                if (GetUncompressedSize(index, size))
                {
                    prop = (uint64_t)size;
                }
            }
            break;

        case RCPropID::kpidPackSize:
            {
                uint32_t size;
                if (GetCompressedSize(index, size))
                {
                    prop = (uint64_t)size;
                }
            }
            break;

        case RCPropID::kpidMTime:
            {
                if (item.m_mTime.u32HighDateTime > 0x01000000 &&
                    item.m_mTime.u32HighDateTime < 0xFF000000)
                {
                    prop = RCFileTime::ConvertFromFileTime(item.m_mTime);
                }
            }
            break;

        case RCPropID::kpidMethod:
            {
                prop = GetMethod(item.m_useFilter, item.m_dictionarySize);
            }
            break;

        case RCPropID::kpidSolid:
            {
                prop = m_inArchive.m_isSolid;
            }
            break;
        }
    }
    return RC_S_OK;
}

HResult RCNsisHandler::Open(IInStream* stream,
                            uint64_t maxCheckStartPosition,
                            IArchiveOpenCallback* openArchiveCallback)
{
    Close();

    if (m_inArchive.Open(m_compressCodecsInfo.Get(),
                         stream,
                         &maxCheckStartPosition) != RC_S_OK)
    {
        return RC_S_FALSE;
    }

    m_inStream = stream;

    return RC_S_OK;
}

HResult RCNsisHandler::Close()
{
    m_inArchive.Clear();
    m_inArchive.Release();
    m_inStream.Release();

    return RC_S_OK;
}

HResult RCNsisHandler::Extract(const std::vector<uint32_t>& indices,
                               int32_t testMode,
                               IArchiveExtractCallback* extractCallback)
{
    uint32_t numItems = (uint32_t)indices.size();
    bool btestMode = (testMode != 0);
    bool allFilesMode = (numItems == uint32_t(0));
    if (allFilesMode)
    {
        GetNumberOfItems(numItems);
    }

    if(numItems == 0)
    {
        return RC_S_OK;
    }

    HResult hr;
    uint64_t totalSize = 0;

    uint32_t i;
    for(i = 0; i < numItems; i++)
    {
        uint32_t index = (allFilesMode ? i : indices[i]);
#ifdef NSIS_SCRIPT
        if (index >= (uint32_t)m_inArchive.m_items.size())
        {
            totalSize += m_inArchive.m_script.length();
        }
        else
#endif
        { 
            uint32_t size;
            if (m_inArchive.m_isSolid)
            {
                GetUncompressedSize(index, size);
                uint64_t pos = m_inArchive.GetPosOfSolidItem(index);
                if (pos > totalSize)
                {
                    totalSize = pos + size;
                }
            }
            else
            {
                GetCompressedSize(index, size);
                totalSize += size;
            }
        }
    }
    extractCallback->SetTotal(totalSize);

    uint64_t currentTotalSize = 0;
    uint32_t currentItemSize = 0;

    uint64_t streamPos = 0;
    if (m_inArchive.m_isSolid)
    {
        hr = m_inStream->Seek(m_inArchive.m_streamOffset, RC_STREAM_SEEK_SET, NULL);
        if (hr != RC_S_OK)
        {
            return hr;
        }
  
        bool useFilter;
        hr = m_inArchive.m_decoder.Init(m_compressCodecsInfo.Get(),
                                        m_inStream.Get(),
                                        m_inArchive.m_method,
                                        m_inArchive.m_filterFlag,
                                        useFilter);
        if (hr != RC_S_OK)
        {
            return hr;
        }
    }

    RCByteBuffer byteBuf;
    const uint32_t kBufferLength = 1 << 11;
    byteBuf.SetCapacity(kBufferLength);
    byte_t* buffer = byteBuf.data();

    bool dataError = false;
    for (i = 0; i < numItems; i++, currentTotalSize += currentItemSize)
    {
        currentItemSize = 0;
        hr = extractCallback->SetCompleted(currentTotalSize);
        if (hr != RC_S_OK)
        {
            return hr;
        }

        ISequentialOutStreamPtr realOutStream;
        int32_t askMode;
        askMode = btestMode ?
            RC_ARCHIVE_EXTRACT_ASK_MODE_TEST :
        RC_ARCHIVE_EXTRACT_ASK_MODE_EXTRACT;

        uint32_t index = allFilesMode ? i : indices[i];

        hr = extractCallback->GetStream(index, realOutStream.GetAddress(), askMode);
        if (hr != RC_S_OK)
        {
            return hr;
        }

#ifdef NSIS_SCRIPT
        if (index >= (uint32_t)m_inArchive.m_items.Size())
        {
            currentItemSize = m_inArchive.m_script.Length();
            if(!btestMode && (!realOutStream))
            {
                continue;
            }
            hr = extractCallback->PrepareOperation(askMode);
            if (hr != RC_S_OK)
            {
                return hr;
            }

            if (!btestMode)
            {
                hr = WriteStream(realOutStream, (const char *)m_inArchive.m_script, (uint32_t)m_inArchive.m_script.Length());
                if (hr != RC_S_OK)
                {
                    return hr;
                }
            }
        }
        else
#endif
        {
            const RCNsisItem &item = m_inArchive.m_items[index];

            if (m_inArchive.m_isSolid)
            {
                GetUncompressedSize(index, currentItemSize);
            }
            else
            {
                GetCompressedSize(index, currentItemSize);
            }

            if(!btestMode && (!realOutStream))
            {
                continue;
            }

            hr = extractCallback->PrepareOperation(index,askMode);
            if (hr != RC_S_OK)
            {
                return hr;
            }

            if (!dataError)
            {
                bool needDecompress = false;
                bool sizeIsKnown = false;
                uint32_t fullSize = 0;

                if (m_inArchive.m_isSolid)
                {
                    uint64_t pos = m_inArchive.GetPosOfSolidItem(index);
                    while(streamPos < pos)
                    {
                        size_t processedSize = (uint32_t)MyMin(pos - streamPos, (uint64_t)kBufferLength);
                        hr = m_inArchive.m_decoder.Read(buffer, &processedSize);
                        if (hr != RC_S_OK)
                        {
                            if (hr != RC_S_FALSE)
                            {
                                return hr;
                            }
                            dataError = true;
                            break;
                        }

                        if (processedSize == 0)
                        {
                            dataError = true;
                            break;
                        }

                        streamPos += processedSize;
                    }
                    if (streamPos == pos)
                    {
                        byte_t buffer2[4];
                        size_t processedSize = 4;
                        hr = m_inArchive.m_decoder.Read(buffer2, &processedSize);
                        if (hr != RC_S_OK)
                        {
                            return hr;
                        }

                        if (processedSize != 4)
                        {
                            return RC_E_FAIL;
                        }
                        streamPos += processedSize;
                        fullSize = Get32(buffer2);
                        sizeIsKnown = true;
                        needDecompress = true;
                    }
                }
                else
                {
                    hr = m_inStream->Seek(m_inArchive.GetPosOfNonSolidItem(index) + 4, RC_STREAM_SEEK_SET, NULL);
                    if (item.m_isCompressed)
                    {
                        needDecompress = true;
                        bool useFilter;
                        hr = m_inArchive.m_decoder.Init(m_compressCodecsInfo.Get(),
                                                        m_inStream.Get(),
                                                        m_inArchive.m_method,
                                                        m_inArchive.m_filterFlag,
                                                        useFilter);
                        if (hr != RC_S_OK)
                        {
                            return hr;
                        }
                        fullSize = 0xFFFFFFFF;
                    }
                    else
                    {
                        fullSize = item.m_size;
                    }
                }
                if (!dataError)
                {
                    if (needDecompress)
                    {
                        uint64_t offset = 0;
                        while(!sizeIsKnown || fullSize > 0)
                        {
                            uint32_t curSize = kBufferLength;
                            if (sizeIsKnown && curSize > fullSize)
                            {
                                curSize = fullSize;
                            }

                            size_t processedSize = curSize;
                            hr = m_inArchive.m_decoder.Read(buffer, &processedSize);
                            if (hr != RC_S_OK)
                            {
                                if (hr != RC_S_FALSE)
                                {
                                    return hr;
                                }
                                dataError = true;
                                break;
                            }

                            if (processedSize == 0)
                            {
                                if (sizeIsKnown)
                                {
                                    dataError = true;
                                }
                                break;
                            }

                            fullSize -= (uint32_t)processedSize;
                            streamPos += processedSize;
                            offset += processedSize;

                            uint64_t completed;
                            if (m_inArchive.m_isSolid)
                            {
                                completed = currentTotalSize + offset;
                            }
                            else
                            {
                                completed = streamPos;
                            }

                            hr = extractCallback->SetCompleted(completed);
                            if (hr != RC_S_OK)
                            {
                                return hr;
                            }

                            if (!btestMode)
                            {
                                hr = RCStreamUtils::WriteStream(realOutStream.Get(), buffer, processedSize);
                                if (hr != RC_S_OK)
                                {
                                    return hr;
                                }
                            }
                        }
                    }
                    else
                    {
                        while(fullSize > 0)
                        {
                            uint32_t curSize = MyMin(fullSize, kBufferLength);
                            uint32_t processedSize;
                            hr = m_inStream->Read(buffer, curSize, &processedSize);
                            if (hr != RC_S_OK)
                            {
                                return hr;
                            }

                            if (processedSize == 0)
                            {
                                dataError = true;
                                break;
                            }

                            fullSize -= processedSize;
                            streamPos += processedSize;
                            if (!btestMode)
                            {
                                hr = RCStreamUtils::WriteStream(realOutStream.Get(), buffer, processedSize);
                                if (hr != RC_S_OK)
                                {
                                    return hr;
                                }
                            }
                        }
                    }
                }
            }
        }
        realOutStream.Release();

        hr = (extractCallback->SetOperationResult(index,dataError ?
                                                                    RC_ARCHIVE_EXTRACT_RESULT_DATA_ERROR :
                                                                    RC_ARCHIVE_EXTRACT_RESULT_OK));
        if (hr != RC_S_OK)
        {
            return hr;
        }
    }

    return RC_S_OK;
}

END_NAMESPACE_RCZIP
