/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "format/cpio/RCCpioHandler.h"
#include "format/cpio/RCCpioIn.h"
#include "format/common/RCPropData.h"
#include "format/common/RCItemNameUtils.h"
#include "common/RCStringConvert.h"
#include "filesystem/RCFileTime.h"
#include "common/RCLocalProgress.h"
#include "interface/RCMethodDefs.h"
#include "common/RCCreateCoder.h"
#include "filesystem/RCLimitedSequentialInStream.h"
#include "interface/RCArchiveType.h"

/////////////////////////////////////////////////////////////////
//RCCpioHandler class implementation

BEGIN_NAMESPACE_RCZIP

static RCPropData s_cpioProps[] =
{
    { RCString(_T("")), RCPropID::kpidPath,     RCVariantType::RC_VT_STRING },
    { RCString(_T("")), RCPropID::kpidIsDir,    RCVariantType::RC_VT_BOOL   },
    { RCString(_T("")), RCPropID::kpidSize,     RCVariantType::RC_VT_UINT64 },
    { RCString(_T("")), RCPropID::kpidPackSize, RCVariantType::RC_VT_UINT64 },
    { RCString(_T("")), RCPropID::kpidMTime,    RCVariantType::RC_VT_UINT64 },
    { RCString(_T("")), RCPropID::kpidPosixAttrib, RCVariantType::RC_VT_UINT64 },
};

static RCPropData s_cpioArcProps[] =
{
    { RCString(_T("")), RCPropID::kpidArchiveType,    RCVariantType::RC_VT_UINT64 }
};

HResult RCCpioHandler::GetNumberOfProperties(uint32_t& numProperties)
{ 
    numProperties = sizeof(s_cpioProps) / sizeof(s_cpioProps[0]);

    return RC_S_OK; 
}

HResult RCCpioHandler::GetPropertyInfo(uint32_t index, RCString& name, RCPropertyID& propID, RCVariantType::RCVariantID& varType)
{
    if (index >= sizeof(s_cpioProps) / sizeof(s_cpioProps[0])) 
    {
        return RC_E_INVALIDARG;
    }
    const RCPropData& srcItem = s_cpioProps[index];

    propID  = srcItem.m_propID; 
    varType = srcItem.m_varType; 
    name    = RCString(_T(""));

    return RC_S_OK;
}

HResult RCCpioHandler::GetNumberOfArchiveProperties(uint32_t& numProperties)
{
    numProperties = sizeof(s_cpioArcProps) / sizeof(s_cpioArcProps[0]);
    return RC_S_OK;
}

HResult RCCpioHandler::GetArchivePropertyInfo(uint32_t index, RCString& name, RCPropertyID& propID, RCVariantType::RCVariantID& varType)
{
    if (index >= (sizeof(s_cpioArcProps) / sizeof(s_cpioArcProps[0])))
    {
        return RC_E_INVALIDARG;
    }
    const RCPropData& srcItem = s_cpioArcProps[index];
    propID = srcItem.m_propID;
    varType = srcItem.m_varType;
    name = srcItem.m_propName;
    return RC_S_OK;
}

HResult RCCpioHandler::GetArchiveProperty(RCPropertyID propID, RCVariant& value)
{
    ClearVariantValue(value) ;
    switch(propID)
    {
        case RCPropID::kpidArchiveType:
        {
            value = uint64_t(RC_ARCHIVE_CPIO) ;
            break;
        }
    }
    return RC_S_OK;
}

HResult RCCpioHandler::GetNumberOfItems(uint32_t& numItems)
{
    numItems = (uint32_t)m_cpioItems.size();

    return RC_S_OK;
}

HResult RCCpioHandler::GetProperty(uint32_t index, RCPropertyID propID, RCVariant& value)
{
    ClearVariantValue(value) ;
    RCVariant& prop = value;
    const RCCpioItemEx& item = m_cpioItems[index];

    switch(propID)
    {
    case RCPropID::kpidPath:
        {
            prop = RCItemNameUtils::GetOSName(RCStringConvert::MultiByteToUnicodeString(item.m_name, RC_CP_OEMCP));
        }
        break;

    case RCPropID::kpidIsDir:
        {
            prop = item.IsDir();
        }
        break;

    case RCPropID::kpidSize:
    case RCPropID::kpidPackSize:
        {
            prop = (uint64_t)item.m_size;
        }
        break;

    case RCPropID::kpidMTime:
        {
            if (item.m_mTime != 0)
            {
                RC_FILE_TIME utcFileTime ;
                RCFileTime::UnixTimeToFileTime(item.m_mTime, utcFileTime);
                prop = RCFileTime::ConvertFromFileTime(utcFileTime);
            }
        }
        break;
    case RCPropID::kpidPosixAttrib:
        prop = (uint64_t)item.m_mode ; 
        break;
    }

    return RC_S_OK ;
}

HResult RCCpioHandler::Open(IInStream* stream,
                           uint64_t maxCheckStartPosition,
                           IArchiveOpenCallback* openArchiveCallback)
{
    HResult  hr;
    RCCpioIn inArchive;
    uint64_t endPos = 0;
    bool needSetTotal = true;

    if (openArchiveCallback != NULL)
    {
        hr = stream->Seek(0, RC_STREAM_SEEK_END, &endPos);
        if (hr != RC_S_OK)
        {
            return hr;
        }

        hr = stream->Seek(0, RC_STREAM_SEEK_SET, NULL);
        if (hr != RC_S_OK)
        {
            return hr;
        }
    }

    hr = inArchive.Open(stream);
    if (hr != RC_S_OK)
    {
        return hr;
    }

    m_cpioItems.clear();

    for (;;)
    {
        RCCpioItemEx item;
        bool filled;
        HResult result = inArchive.GetNextItem(filled, item);
        if (result == RC_S_FALSE)
        {
            return RC_S_FALSE;
        }

        if (result != RC_S_OK)
        {
            return RC_S_FALSE;
        }

        if (!filled)
        {
            break;
        }

        m_cpioItems.push_back(item);

        inArchive.SkipDataRecords(item.m_size, item.m_align);

        if (openArchiveCallback != NULL)
        {
            if (needSetTotal)
            {
                hr = openArchiveCallback->SetTotal(NULL, endPos);
                if (hr != RC_S_OK)
                {
                    return hr;
                }

                needSetTotal = false;
            }

            if (m_cpioItems.size() % 100 == 0)
            {
                uint64_t numFiles = m_cpioItems.size();
                uint64_t numBytes = item.m_headerPosition;
                hr = openArchiveCallback->SetCompleted(numFiles, numBytes);
                if (hr != RC_S_OK)
                {
                    return hr;
                }
            }
        }
    }

    if (m_cpioItems.size() == 0)
    {
        return RC_S_FALSE;
    }

    m_inStream = stream;

    return RC_S_OK;
}

HResult RCCpioHandler::Close()
{
    m_cpioItems.clear();
    m_inStream.Release();

    return RC_S_OK;
}

HResult RCCpioHandler::Extract(const std::vector<uint32_t>& indices,
                               int32_t testMode,
                               IArchiveExtractCallback* extractCallback)
{
    uint32_t numItems = (uint32_t)indices.size();
    bool allFilesMode = (numItems == uint32_t(0));
    if (allFilesMode)
    {
        numItems = (uint32_t)m_cpioItems.size();
    }

    if (numItems == 0)
    {
        return RC_S_OK;
    }

    HResult hr;
    uint64_t totalSize = 0;
    uint32_t i;
    for (i = 0; i < numItems; i++)
    {
        totalSize += m_cpioItems[allFilesMode ? i : indices[i]].m_size;
    }

    extractCallback->SetTotal(totalSize);

    uint64_t currentTotalSize = 0;

    ICompressCoderPtr copyCoder;

    hr = RCCreateCoder::CreateCoder(m_compressCodecsInfo.Get(),
                                    RCMethod::ID_COMPRESS_COPY,
                                    copyCoder,
                                    false);
    if (hr != RC_S_OK)
    {
        return hr;
    }

    ICompressGetInStreamProcessedSizePtr copyCoderSize;
    hr = copyCoder.QueryInterface(ICompressGetInStreamProcessedSize::IID, copyCoderSize.GetAddress());
    if (hr != RC_S_OK)
    {
        return hr;
    }

    RCLocalProgress *lps = new RCLocalProgress;
    ICompressProgressInfoPtr progress = lps;
    lps->Init(extractCallback, false);

    RCLimitedSequentialInStream *streamSpec = new RCLimitedSequentialInStream;
    ISequentialInStreamPtr inStream(streamSpec);
    streamSpec->SetStream(m_inStream.Get());

    for (i = 0; i < numItems; ++i)
    {
        lps->SetInSize(currentTotalSize);
        lps->SetOutSize(currentTotalSize);

        hr = lps->SetCur();
        if (hr != RC_S_OK)
        {
            return hr;
        }

        ISequentialOutStreamPtr outStream ;
        int32_t askMode = testMode ? RC_ARCHIVE_EXTRACT_ASK_MODE_TEST :
                                     RC_ARCHIVE_EXTRACT_ASK_MODE_EXTRACT;

        int32_t index = allFilesMode ? i : indices[i];
        const RCCpioItemEx& item = m_cpioItems[index];

        hr = extractCallback->GetStream(index, outStream.GetAddress(), askMode);
        if (hr != RC_S_OK)
        {
            return hr;
        }

        currentTotalSize += item.m_size ;
        if (item.IsDir())
        {
            hr = extractCallback->PrepareOperation(index,askMode);
            if (hr != RC_S_OK)
            {
                return hr;
            }

            hr = extractCallback->SetOperationResult(index,RC_ARCHIVE_EXTRACT_RESULT_OK);
            if (hr != RC_S_OK)
            {
                return hr;
            }
            continue;
        }

        if (!testMode && !outStream)
        {
            continue;
        }

        hr = extractCallback->PrepareOperation(index,askMode);
        if (hr != RC_S_OK)
        {
            return hr;
        }

        if (testMode)
        {
            hr = extractCallback->SetOperationResult(index,RC_ARCHIVE_EXTRACT_RESULT_OK);
            if (hr != RC_S_OK)
            {
                return hr;
            }
            continue;
        }

        hr = m_inStream->Seek(item.GetDataPosition(), RC_STREAM_SEEK_SET, NULL);
        if (hr != RC_S_OK)
        {
            return hr;
        }

        streamSpec->Init(item.m_size);
        hr = copyCoder->Code(inStream.Get(), outStream.Get(), NULL, NULL, progress.Get());
        if (hr != RC_S_OK)
        {
            return hr;
        }

        outStream.Release();

        uint64_t u64size;
        copyCoderSize.Get()->GetInStreamProcessedSize(u64size);

        hr = extractCallback->SetOperationResult(index,(u64size == item.m_size) ?
            RC_ARCHIVE_EXTRACT_RESULT_OK:
        RC_ARCHIVE_EXTRACT_RESULT_DATA_ERROR);
        if (hr != RC_S_OK)
        {
            return hr;
        }
    }
    return RC_S_OK;
}

END_NAMESPACE_RCZIP
