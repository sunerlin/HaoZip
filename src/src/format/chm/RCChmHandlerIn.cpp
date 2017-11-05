/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "format/chm/RCChmHandler.h"
#include "format/chm/RCChmIn.h"
#include "format/chm/RCChmFolderOutStream.h"
#include "common/RCStringConvert.h"
#include "format/common/RCItemNameUtils.h"
#include "common/RCLocalProgress.h"
#include "interface/RCMethodDefs.h"
#include "common/RCCreateCoder.h"
#include "compress/copy/RCCopyCoder.h"
#include "compress/lzx/RCLzxDecoder.h"
#include "filesystem/RCLimitedSequentialInStream.h"
#include "filesystem/RCStreamUtils.h"
#include "format/common/RCPropData.h"
#include "interface/RCArchiveType.h"

/////////////////////////////////////////////////////////////////
//RCChmHandler class implementation

BEGIN_NAMESPACE_RCZIP

#ifdef _CHM_DETAILS
enum
{
    kpidSection = kpidUserDefined
};
#endif

static RCPropData s_chmProps[] =
{
    { RCString(_T("")), RCPropID::kpidPath,   RCVariantType::RC_VT_STRING },
    { RCString(_T("")), RCPropID::kpidSize,   RCVariantType::RC_VT_UINT64 },
    { RCString(_T("")), RCPropID::kpidMethod, RCVariantType::RC_VT_STRING },
    { RCString(_T("")), RCPropID::kpidBlock,  RCVariantType::RC_VT_UINT64 }

#ifdef _CHM_DETAILS
    ,
    { RCString(_T("Section")), RCPropID::kpidSection, RCVariantType::RC_VT_UINT64 },
    { RCString(_T("")), RCPropID::kpidOffset,         RCVariantType::RC_VT_UINT64 }
#endif
};

static RCPropData s_chmArcProps[] =
{
    { RCString(_T("")), RCPropID::kpidArchiveType, RCVariantType::RC_VT_UINT64 }
};

HResult RCChmHandler::GetNumberOfProperties(uint32_t& numProperties)
{ 
    numProperties = sizeof(s_chmProps) / sizeof(s_chmProps[0]);

    return RC_S_OK; 
}

HResult RCChmHandler::GetPropertyInfo(uint32_t index, RCString& name, RCPropertyID& propID, RCVariantType::RCVariantID& varType)
{
    if (index >= sizeof(s_chmProps) / sizeof(s_chmProps[0])) 
    {
        return RC_E_INVALIDARG;
    }
    const RCPropData& srcItem = s_chmProps[index];

    propID  = srcItem.m_propID ; 
    varType = srcItem.m_varType ; 
    name    = srcItem.m_propName ;

    return RC_S_OK;
}

HResult RCChmHandler::GetNumberOfArchiveProperties(uint32_t& numProperties)
{
    numProperties = sizeof(s_chmArcProps) / sizeof(s_chmArcProps[0]);
    return RC_S_OK;
}

HResult RCChmHandler::GetArchivePropertyInfo(uint32_t index, RCString& name, RCPropertyID& propID, RCVariantType::RCVariantID& varType)
{
    if (index >= (sizeof(s_chmArcProps) / sizeof(s_chmArcProps[0])))
    {
        return RC_E_INVALIDARG;
    }
    const RCPropData& srcItem = s_chmArcProps[index];
    propID = srcItem.m_propID;
    varType = srcItem.m_varType;
    name = srcItem.m_propName;
    return RC_S_OK;
}

HResult RCChmHandler::GetArchiveProperty(RCPropertyID propID, RCVariant& value)
{
    ClearVariantValue(value) ;
    RCVariant& prop = value ;
    switch(propID)
    {
        case RCPropID::kpidArchiveType:
        {
            prop = uint64_t(RC_ARCHIVE_CHM) ;
            break;
        }
    }
    return RC_S_OK;
}

HResult RCChmHandler::GetNumberOfItems(uint32_t& numItems)
{
    numItems = (uint32_t)(m_filesDatabase.m_newFormat ? 1:
        (m_filesDatabase.m_lowLevel ?
         m_filesDatabase.m_items.size():
         m_filesDatabase.m_indices.size()));

    return RC_S_OK;
}

HResult RCChmHandler::GetProperty(uint32_t index, RCPropertyID propID, RCVariant& value)
{
    ClearVariantValue(value) ;
    RCVariant& prop = value;

    if (m_filesDatabase.m_newFormat)
    {
        switch(propID)
        {
        case RCPropID::kpidSize:
            {
                prop = (uint64_t)m_filesDatabase.m_newFormatString.length();
            }
            break;
        }
        return RC_S_OK;
    }

    int32_t entryIndex;
    if (m_filesDatabase.m_lowLevel)
    {
        entryIndex = index;
    }
    else
    {
        entryIndex = m_filesDatabase.m_indices[index];
    }

    const RCChmItem& item = m_filesDatabase.m_items[entryIndex];

    switch(propID)
    {
    case RCPropID::kpidPath:
        {
            RCString us;
            if (RCStringConvert::ConvertUTF8ToUnicode(item.m_name.c_str(), item.m_name.length() ,us))
            {
                if (!m_filesDatabase.m_lowLevel)
                {
                    if (us.length() > 1)
                    {      
                        if (us[0] == _T('/'))
                        {
                            us.erase(us.begin());
                        }
                    }
                }
                prop = RCItemNameUtils::GetOSName2(us);
            }
        }
        break;

    case RCPropID::kpidIsDir:
        {
            prop = item.IsDir();
        }
        break;

    case RCPropID::kpidSize:
        {
            prop = item.m_size;
        }
        break;

    case RCPropID::kpidMethod:
        {
            if (!item.IsDir())
            {
                if (item.m_section == 0)
                {
                    prop = RCString(_T("Copy"));
                }
                else if (item.m_section < m_filesDatabase.m_sections.size())
                {
                    prop = m_filesDatabase.m_sections[(int32_t)item.m_section].GetMethodName();
                }
            }
        }
        break;

    case RCPropID::kpidBlock:
        {
            if (m_filesDatabase.m_lowLevel)
            {
                prop = item.m_section;
            }
            else if (item.m_section != 0)
            {
                prop = m_filesDatabase.GetFolder(index);
            }
        }
        break;

#ifdef _CHM_DETAILS
    case RCPropID::kpidSection:
        {
            prop = (uint32_t)item.Section;
        }
        break;

    case RCPropID::kpidOffset:
        {
            prop = (uint32_t)item.Offset;
        }
        break;
#endif
    }

    return RC_S_OK;
}

class RCChmProgressImp:
    public RCChmProgressVirt
{
public:

    /** 默认函数
    @param [in] callback 打开文档回调接口指针
    */
    RCChmProgressImp(IArchiveOpenCallback* callback): 
      m_callback(callback)
    {
    }

public:

    /** 总体数量
    @param [in] numFiles 文件数
    @return 成功返回RC_S_OK, 否则返回错误码
    */
    virtual HResult SetTotal(uint64_t numFiles) ;

    /** 当前完成
    @param [in] numFiles 文件数
    @return 成功返回RC_S_OK, 否则返回错误码
    */
    virtual HResult SetCompleted(uint64_t numFiles) ;

private:

    /** 打开文档回调接口指针
    */
    IArchiveOpenCallbackPtr m_callback ;
};

HResult RCChmProgressImp::SetTotal(uint64_t numFiles)
{
    if (m_callback)
    {
        return m_callback->SetTotal(numFiles, NULL);
    }
    return RC_S_OK;
}

HResult RCChmProgressImp::SetCompleted(uint64_t numFiles)
{
    if (m_callback)
    {
        return m_callback->SetCompleted(numFiles, NULL);
    }
    return RC_S_OK;
}

HResult RCChmHandler::Open(IInStream* stream,
                           uint64_t maxCheckStartPosition,
                           IArchiveOpenCallback* openArchiveCallback)
{
    m_inStream.Release();

    RCChmIn inArchive;

    //RCChmProgressImp progressImp(openArchiveCallback);

    HResult hr = inArchive.Open(stream, &maxCheckStartPosition, m_filesDatabase);
    if (hr != RC_S_OK)
    {
        return hr;
    }

    /*
    if (m_filesDatabase.LowLevel)
    return RC_S_FALSE;
    */
    
    m_inStream = stream;

    return RC_S_OK;
}

HResult RCChmHandler::Close()
{
    m_filesDatabase.Clear();
    m_inStream.Release();

    return RC_S_OK;
}

HResult RCChmHandler::Extract(const std::vector<uint32_t>& indices,
                              int32_t testMode,
                              IArchiveExtractCallback* extractCallback)
{    
    uint32_t numItems = (uint32_t)indices.size();

    bool allFilesMode = (numItems == uint32_t(0));

    if (allFilesMode)
    {
        numItems = (uint32_t)(m_filesDatabase.m_newFormat ? 1:
            (m_filesDatabase.m_lowLevel ?
             m_filesDatabase.m_items.size():
             m_filesDatabase.m_indices.size()));
    }

    if (numItems == 0)
    {
        return RC_S_OK;
    }

    bool btestMode = (testMode != 0);

    HResult hr;
    uint64_t currentTotalSize = 0;

    RCCopyCoder* copyCoderSpec;
    ICompressCoderPtr copyCoder;

    hr = RCCreateCoder::CreateCoder(m_compressCodecsInfo.Get(),
                                    RCMethod::ID_COMPRESS_COPY,
                                    copyCoder,
                                    false);
    if (hr != RC_S_OK)
    {
        return hr;
    }
    copyCoderSpec = dynamic_cast<RCCopyCoder*>(copyCoder.Get());

    ICompressGetInStreamProcessedSizePtr copyCoderSize;
    hr = copyCoder.QueryInterface(ICompressGetInStreamProcessedSize::IID, copyCoderSize.GetAddress());
    if (hr != RC_S_OK)
    {
        return hr;
    }

    uint32_t i;

    RCLocalProgress *lps = new RCLocalProgress;
    ICompressProgressInfoPtr progress = lps;
    lps->Init(extractCallback, false);

    RCLimitedSequentialInStream *streamSpec = new RCLimitedSequentialInStream;
    ISequentialInStreamPtr inStream(streamSpec);
    streamSpec->SetStream(m_inStream.Get());

    if (m_filesDatabase.m_lowLevel)
    {
        uint64_t currentItemSize = 0;
        uint64_t totalSize = 0;
        if (m_filesDatabase.m_newFormat)
        {
            totalSize = m_filesDatabase.m_newFormatString.length();
        }
        else
        {
            for (i = 0; i < numItems; i++)
            {
                totalSize += m_filesDatabase.m_items[allFilesMode ? i : indices[i]].m_size;
            }
        }

        extractCallback->SetTotal(totalSize);

        for (i = 0; i < numItems; i++, currentTotalSize += currentItemSize)
        {
            currentItemSize = 0;
            lps->SetInSize(currentTotalSize);
            lps->SetOutSize(currentTotalSize);

            hr = lps->SetCur();
            if (hr != RC_S_OK)
            {
                return hr;
            }

            ISequentialOutStreamPtr realOutStream;
            int32_t askMode= btestMode ?
                RC_ARCHIVE_EXTRACT_ASK_MODE_TEST :
            RC_ARCHIVE_EXTRACT_ASK_MODE_EXTRACT;
            int32_t index = allFilesMode ? i : indices[i];

            hr = extractCallback->GetStream(index, realOutStream.GetAddress(), askMode);
            if (hr != RC_S_OK)
            {
                return hr;
            }

            if (m_filesDatabase.m_newFormat)
            {
                if (index != 0)
                {
                    return RC_E_FAIL;
                }

                if (!btestMode && (!realOutStream))
                {
                    continue;
                }

                if (!btestMode)
                {
                    uint32_t size = (uint32_t)m_filesDatabase.m_newFormatString.length();
                    hr = RCStreamUtils::WriteStream(realOutStream.Get(), (const char *)m_filesDatabase.m_newFormatString.c_str(), size);
                    if (hr != RC_S_OK)
                    {
                        return hr;
                    }
                }

                hr = extractCallback->SetOperationResult(index,RC_ARCHIVE_EXTRACT_RESULT_OK);
                if (hr != RC_S_OK)
                {
                    return hr;
                }

                continue;
            }

            const RCChmItem& item = m_filesDatabase.m_items[index];

            currentItemSize = item.m_size;

            if (!btestMode && (!realOutStream))
            {
                continue;
            }

            hr = extractCallback->PrepareOperation(index,askMode);
            if (hr != RC_S_OK)
            {
                return hr;
            }

            if (item.m_section != 0)
            {
                hr = extractCallback->SetOperationResult(index,RC_ARCHIVE_EXTRACT_RESULT_UNSUPPORTED_METHOD);
                if (hr != RC_S_OK)
                {
                    return hr;
                }
                continue;
            }

            if (btestMode)
            {
                hr = extractCallback->SetOperationResult(index,RC_ARCHIVE_EXTRACT_RESULT_OK);
                if (hr != RC_S_OK)
                {
                    return hr;
                }
                continue;
            }

            hr = m_inStream->Seek(m_filesDatabase.m_contentOffset + item.m_offset, RC_STREAM_SEEK_SET, NULL);
            if (hr != RC_S_OK)
            {
                return hr;
            }

            streamSpec->Init(item.m_size);

            hr = copyCoder->Code(inStream.Get(), realOutStream.Get(), NULL, NULL, progress.Get());
            if (hr != RC_S_OK)
            {
                return hr;
            }

            realOutStream.Release();

            uint64_t u64size;
            copyCoderSize.Get()->GetInStreamProcessedSize(u64size);

            hr = extractCallback->SetOperationResult(index, (u64size == item.m_size) ?
                RC_ARCHIVE_EXTRACT_RESULT_OK:
                RC_ARCHIVE_EXTRACT_RESULT_DATA_ERROR);
            if (hr != RC_S_OK)
            {
                return hr;
            }
        }
        return RC_S_OK;
    }

    uint64_t lastFolderIndex = ((uint64_t)0 - 1);
    for (i = 0; i < numItems; i++)
    {
        uint32_t index = allFilesMode ? i : indices[i];
        int32_t entryIndex = m_filesDatabase.m_indices[index];
        const RCChmItem &item = m_filesDatabase.m_items[entryIndex];
        uint64_t sectionIndex = item.m_section;
        if (item.IsDir() || item.m_size == 0)
        {
            continue;
        }

        if (sectionIndex == 0)
        {
            currentTotalSize += item.m_size;
            continue;
        }

        const RCChmSectionInfo &section = m_filesDatabase.m_sections[(int32_t)item.m_section];
        if (section.IsLzx())
        {
            const RCChmLzxInfo &lzxInfo = section.m_methods[0].m_lzxInfo;
            uint64_t folderIndex = m_filesDatabase.GetFolder(index);
            if (lastFolderIndex == folderIndex)
            {
                folderIndex++;
            }

            lastFolderIndex = m_filesDatabase.GetLastFolder(index);
            for (; folderIndex <= lastFolderIndex; folderIndex++)
            {
                currentTotalSize += lzxInfo.GetFolderSize();
            }
        }
    }

    hr = extractCallback->SetTotal(currentTotalSize);
    if (hr != RC_S_OK)
    {
        return hr;
    }

    RCLzxDecoder* lzxDecoderSpec = 0;
    ICompressCoderPtr lzxDecoder;

    RCChmFolderOutStream* chmFolderOutStream = 0;
    ISequentialOutStreamPtr outStream;

    currentTotalSize = 0;

    RCVector<bool> extractStatuses;
    for (i = 0; i < numItems;)
    {
        hr = extractCallback->SetCompleted(currentTotalSize);
        if (hr != RC_S_OK)
        {
            return hr;
        }

        uint32_t index = allFilesMode ? i : indices[i];
        i++;
        int entryIndex = m_filesDatabase.m_indices[index];
        const RCChmItem &item = m_filesDatabase.m_items[entryIndex];
        uint64_t sectionIndex = item.m_section;
        int32_t askMode= btestMode ?
            RC_ARCHIVE_EXTRACT_ASK_MODE_TEST :
        RC_ARCHIVE_EXTRACT_ASK_MODE_EXTRACT;

        if (item.IsDir())
        {
            ISequentialOutStreamPtr realOutStream;
            hr = extractCallback->GetStream(index, realOutStream.GetAddress(), askMode);
            if (hr != RC_S_OK)
            {
                return hr;
            }

            hr = extractCallback->PrepareOperation(index,askMode);
            if (hr != RC_S_OK)
            {
                return hr;
            }

            realOutStream.Release();
            hr = extractCallback->SetOperationResult(index,RC_ARCHIVE_EXTRACT_RESULT_OK);
            if (hr != RC_S_OK)
            {
                return hr;
            }
            continue;
        }

        lps->SetInSize(currentTotalSize);
        lps->SetOutSize(currentTotalSize);

        if (item.m_size == 0 || sectionIndex == 0)
        {
            ISequentialOutStreamPtr realOutStream;
            hr = extractCallback->GetStream(index, realOutStream.GetAddress(), askMode);
            if (hr != RC_S_OK)
            {
                return hr;
            }

            if (!btestMode && (!realOutStream))
            {
                continue;
            }

            hr = extractCallback->PrepareOperation(index,askMode);
            if (hr != RC_S_OK)
            {
                return hr;
            }

            int32_t opRes = RC_ARCHIVE_EXTRACT_RESULT_OK;
            if (!btestMode && item.m_size != 0)
            {
                hr = m_inStream->Seek(m_filesDatabase.m_contentOffset + item.m_offset, RC_STREAM_SEEK_SET, NULL);
                if (hr != RC_S_OK)
                {
                    return hr;
                }

                streamSpec->Init(item.m_size);
                hr = copyCoder->Code(inStream.Get(), realOutStream.Get(), NULL, NULL, progress.Get());
                if (hr != RC_S_OK)
                {
                    return hr;
                }

                uint64_t u64size;
                copyCoderSize.Get()->GetInStreamProcessedSize(u64size);

                if (u64size != item.m_size)
                {
                    opRes = RC_ARCHIVE_EXTRACT_RESULT_DATA_ERROR;
                }
            }

            realOutStream.Release();
            hr = extractCallback->SetOperationResult(index, opRes);
            if (hr != RC_S_OK)
            {
                return hr;
            }

            currentTotalSize += item.m_size;
            continue;
        }

        const RCChmSectionInfo &section = m_filesDatabase.m_sections[(int)sectionIndex];

        if (!section.IsLzx())
        {
            ISequentialOutStreamPtr realOutStream;
            hr = extractCallback->GetStream(index, realOutStream.GetAddress(), askMode);
            if (hr != RC_S_OK)
            {
                return hr;
            }

            if(!btestMode && (!realOutStream))
            {
                continue;
            }

            hr = (extractCallback->PrepareOperation(index,askMode));
            if (hr != RC_S_OK)
            {
                return hr;
            }

            hr = extractCallback->SetOperationResult(index,RC_ARCHIVE_EXTRACT_RESULT_UNSUPPORTED_METHOD);
            if (hr != RC_S_OK)
            {
                return hr;
            }

            continue;
        }

        const RCChmLzxInfo& lzxInfo = section.m_methods[0].m_lzxInfo;

        if (chmFolderOutStream == 0)
        {
            chmFolderOutStream = new RCChmFolderOutStream;
            outStream = chmFolderOutStream;
        }

        chmFolderOutStream->Init(&m_filesDatabase, extractCallback, btestMode);

        if(lzxDecoderSpec == NULL)
        {            
            hr = RCCreateCoder::CreateCoder(m_compressCodecsInfo.Get(),
                                            RCMethod::ID_COMPRESS_LZX,
                                            lzxDecoder,
                                            false);
            if (hr != RC_S_OK)
            {
                return hr;
            }
            lzxDecoderSpec = dynamic_cast<RCLzxDecoder*>(lzxDecoder.Get());
        }

        uint64_t folderIndex = m_filesDatabase.GetFolder(index);

        uint64_t compressedPos = m_filesDatabase.m_contentOffset + section.m_offset;
        uint32_t numDictBits = lzxInfo.GetNumDictBits();
     
        //hr = lzxDecoderSpec->SetParams(numDictBits);
        //if (hr != RC_S_OK)
        //{
        //    return hr;
        //}
        {
            ICompressSetCoderPropertiesPtr propset;
            RCPropertyIDPairArray proparray;

            hr = lzxDecoder.QueryInterface(IID_ICompressSetCoderProperties, propset.GetAddress());
            if (hr != RC_S_OK)
            {
                return hr;
            }

            proparray.clear();
            proparray.push_back(RCPropertyIDPairArray::value_type(RCMethodProperty::ID_SET_PARAMS, (uint64_t)numDictBits));
            hr = propset->SetCoderProperties(proparray);
            if (hr != RC_S_OK)
            {
                return hr;
            }
        }

        const RCChmItem* lastItem = &item;
        extractStatuses.clear();
        extractStatuses.push_back(true);

        for (;; folderIndex++)
        {
            hr = extractCallback->SetCompleted(currentTotalSize);
            if (hr != RC_S_OK)
            {
                return hr;
            }

            uint64_t startPos = lzxInfo.GetFolderPos(folderIndex);
            uint64_t finishPos = lastItem->m_offset + lastItem->m_size;
            uint64_t limitFolderIndex = lzxInfo.GetFolder(finishPos);

            lastFolderIndex = m_filesDatabase.GetLastFolder(index);
            uint64_t folderSize = lzxInfo.GetFolderSize();
            uint64_t unPackSize = folderSize;
            if (extractStatuses.empty())
            {
                chmFolderOutStream->m_startIndex = index + 1;
            }
            else
            {
                chmFolderOutStream->m_startIndex = index;
            }

            if (limitFolderIndex == folderIndex)
            {
                for (; i < numItems; i++)
                {
                    uint32_t nextIndex = allFilesMode ? i : indices[i];
                    int entryIndex = m_filesDatabase.m_indices[nextIndex];
                    const RCChmItem &nextItem = m_filesDatabase.m_items[entryIndex];
                    if (nextItem.m_section != sectionIndex)
                    {
                        break;
                    }

                    uint64_t nextFolderIndex = m_filesDatabase.GetFolder(nextIndex);
                    if (nextFolderIndex != folderIndex)
                    {
                        break;
                    }
                    for (index++; index < nextIndex; index++)
                    {
                        extractStatuses.push_back(false);
                    }

                    extractStatuses.push_back(true);
                    index = nextIndex;
                    lastItem = &nextItem;
                    if (nextItem.m_size != 0)
                        finishPos = nextItem.m_offset + nextItem.m_size;
                    lastFolderIndex = m_filesDatabase.GetLastFolder(index);
                }
            }
            unPackSize = MyMin(finishPos - startPos, unPackSize);

            chmFolderOutStream->m_folderSize = folderSize;
            chmFolderOutStream->m_posInFolder = 0;
            chmFolderOutStream->m_posInSection = startPos;
            chmFolderOutStream->m_extractStatuses = &extractStatuses;
            chmFolderOutStream->m_numFiles = (int32_t)extractStatuses.size();
            chmFolderOutStream->m_currentIndex = 0;


            uint64_t startBlock = lzxInfo.GetBlockIndexFromFolderIndex(folderIndex);
            const RCChmResetTable& rt = lzxInfo.m_resetTable;
            uint32_t numBlocks = (uint32_t)rt.GetNumBlocks(unPackSize);

            for (uint32_t b = 0; b < numBlocks; b++)
            {
                uint64_t completedSize = currentTotalSize + chmFolderOutStream->m_posInSection - startPos;
                hr = extractCallback->SetCompleted(completedSize);
                if (hr != RC_S_OK)
                {
                    return hr;
                }

                uint64_t bCur = startBlock + b;
                if (bCur >= rt.m_resetOffsets.size())
                {
                    return RC_E_FAIL;
                }

                uint64_t offset = rt.m_resetOffsets[(int)bCur];
                uint64_t compressedSize;
                rt.GetCompressedSizeOfBlock(bCur, compressedSize);
                uint64_t rem = finishPos - chmFolderOutStream->m_posInSection;
                if (rem > rt.m_blockSize)
                {
                    rem = rt.m_blockSize;
                }

                hr = m_inStream->Seek(compressedPos + offset, RC_STREAM_SEEK_SET, NULL);
                if (hr != RC_S_OK)
                {
                    return hr;
                }

                streamSpec->SetStream(m_inStream.Get());
                streamSpec->Init(compressedSize);
               
                //lzxDecoderSpec->SetKeepHistory(b > 0);
                {
                    ICompressSetCoderPropertiesPtr propset;
                    RCPropertyIDPairArray proparray;

                    hr = lzxDecoder.QueryInterface(IID_ICompressSetCoderProperties, propset.GetAddress());
                    if (hr != RC_S_OK)
                    {
                        return hr;
                    }

                    proparray.clear();
                    proparray.push_back(RCPropertyIDPairArray::value_type(RCMethodProperty::ID_SET_KEEP_HISTORY, b > 0));
                    hr = propset->SetCoderProperties(proparray);
                    if (hr != RC_S_OK)
                    {
                        return hr;
                    }
                }

                hr = lzxDecoder->Code(inStream.Get(), outStream.Get(), NULL, &rem, NULL);
                if (hr != RC_S_OK)
                {
                    if (hr != RC_S_FALSE)
                    {
                        return hr;
                    }
                }
            }

            currentTotalSize += folderSize;
            if (folderIndex == lastFolderIndex)
            {
                break;
            }
            extractStatuses.clear();
        }
    }
    return RC_S_OK;
}

END_NAMESPACE_RCZIP
