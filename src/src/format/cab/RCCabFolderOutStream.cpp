/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#include "format/cab/RCCabFolderOutStream.h"
#include "filesystem/RCStreamUtils.h"

BEGIN_NAMESPACE_RCZIP

#define MAX_TEMP_BUF_SIZE uint32_t(1 << 20) 

uint64_t RCCabFolderOutStream::GetRemain() const 
{ 
    return m_folderSize - m_posInFolder; 
}

uint64_t RCCabFolderOutStream::GetPosInFolder() const 
{ 
    return m_posInFolder; 
}

HResult RCCabFolderOutStream::Write(const void* data, uint32_t size, uint32_t* processedSize)
{
    return Write2(data, size, processedSize, true);
}

HResult RCCabFolderOutStream::WriteEmptyFiles()
{
    if (m_fileIsOpen)
    {
        return RC_S_OK;
    }
    for(;m_currentIndex < static_cast<int32_t>(m_extractStatuses->size()); m_currentIndex++)
    {
        const RCCabMvItem &mvItem = m_database->m_items[m_startIndex + m_currentIndex];
        const RCCabItem &item = m_database->m_volumes[mvItem.m_volumeIndex].m_items[mvItem.m_itemIndex];
        uint64_t fileSize = item.m_size;
        if (fileSize != 0)
        {
            return RC_S_OK;
        }
        HResult result = OpenFile();
        m_realOutStream.Release();
        if(!IsSuccess(result))
        {
            return result;
        }
        result = m_extractCallback->SetOperationResult(m_startIndex + m_currentIndex,RC_ARCHIVE_EXTRACT_RESULT_OK);
        if(!IsSuccess(result))
        {
            return result;
        }
    }
    return RC_S_OK;
}

void RCCabFolderOutStream::Init(const RCCabMvDatabaseEx *database, 
                                const RCVector<bool> *extractStatuses, 
                                int32_t startIndex,
                                uint64_t folderSize,
                                IArchiveExtractCallback *extractCallback,
                                bool testMode)
{
    m_database = database;
    m_extractStatuses = extractStatuses;
    m_startIndex = startIndex;
    m_folderSize = folderSize;
    m_extractCallback = extractCallback;
    m_testMode = testMode;

    m_currentIndex = 0;
    m_posInFolder = 0;
    m_fileIsOpen = false;
    m_isOk = true;
    m_tempBufMode = false ;
    m_isSupported = false ;
    m_bufStartFolderOffset = 0 ;
}

HResult RCCabFolderOutStream::CloseFile()
{
    m_realOutStream.Release() ;
    HResult res = m_extractCallback->SetOperationResult(m_startIndex + m_currentIndex,
                                                        m_isOk ? RC_ARCHIVE_EXTRACT_RESULT_OK:
                                                                 RC_ARCHIVE_EXTRACT_RESULT_DATA_ERROR) ;
    m_fileIsOpen = false ;
    return res ;
}

HResult RCCabFolderOutStream::FlushCorrupted()
{
    const uint32_t kBufferSize = (1 << 10);
    byte_t buffer[kBufferSize];
    for (uint32_t i = 0; i < kBufferSize; i++)
    {
        buffer[i] = 0;
    }
    for (;;)
    {
        uint64_t remain = GetRemain();
        if (remain == 0)
        {
            return RC_S_OK;
        }
        uint32_t size = static_cast<uint32_t>(MyMin(remain, (uint64_t)kBufferSize));
        uint32_t processedSizeLocal = 0;
        HResult rs = Write2(buffer, size, &processedSizeLocal, false);
        if (!IsSuccess(rs))
        {
            return rs;
        }
    }
}

HResult RCCabFolderOutStream::Unsupported()
{
    while(m_currentIndex < static_cast<int32_t>(m_extractStatuses->size()))
    {
        HResult result = OpenFile();
        if (result != RC_S_FALSE && result != RC_S_OK)
        {
            return result;
        }
        m_realOutStream.Release();
        result = m_extractCallback->SetOperationResult(m_startIndex + m_currentIndex,RC_ARCHIVE_EXTRACT_RESULT_UNSUPPORTED_METHOD);
        if (!IsSuccess(result))
        {
            return result;
        }
        m_currentIndex++;
    }
    return RC_S_OK;
}

HResult RCCabFolderOutStream::OpenFile()
{
    int32_t askMode = (*m_extractStatuses)[m_currentIndex] ? 
                            (m_testMode ? RC_ARCHIVE_EXTRACT_ASK_MODE_TEST :
                            RC_ARCHIVE_EXTRACT_ASK_MODE_EXTRACT) :
                        RC_ARCHIVE_EXTRACT_ASK_MODE_SKIP ;

    if (!m_tempBufMode)
    {
        const RCCabMvItem& mvItem = m_database->m_items[m_startIndex + m_currentIndex];
        const RCCabItem& item = m_database->m_volumes[mvItem.m_volumeIndex].m_items[mvItem.m_itemIndex];
        int32_t curIndex = m_currentIndex + 1 ;
        for (; curIndex < (int32_t)m_extractStatuses->size(); curIndex++)
        {
            if ((*m_extractStatuses)[curIndex])
            {
                const RCCabMvItem& mvItem2 = m_database->m_items[m_startIndex + curIndex];
                const RCCabItem& item2 = m_database->m_volumes[mvItem2.m_volumeIndex].m_items[mvItem2.m_itemIndex];
                if (item.m_offset != item2.m_offset ||
                    item.m_size != item2.m_size ||
                    item.m_size == 0)
                {
                    break;
                }
            }
            if (curIndex > m_currentIndex + 1)
            {
                size_t oldCapacity = m_tempBuf.GetCapacity();
                m_isSupported = (item.m_size <= MAX_TEMP_BUF_SIZE);
                if (item.m_size > oldCapacity && m_isSupported)
                {
                    m_tempBuf.SetCapacity(0);
                    m_tempBuf.SetCapacity(item.m_size);
                }
                m_tempBufMode = true;
                m_bufStartFolderOffset = item.m_offset;
            }
        }
    }
    HResult rs = m_extractCallback->GetStream(m_startIndex + m_currentIndex, m_realOutStream.GetAddress(), askMode);
    if (!IsSuccess(rs))
    {
        return rs;
    }
    if (!m_realOutStream && !m_testMode)
    askMode = RC_ARCHIVE_EXTRACT_ASK_MODE_SKIP;
    return m_extractCallback->PrepareOperation(m_startIndex + m_currentIndex,askMode);
}

HResult RCCabFolderOutStream::Write2(const void *data, uint32_t size, uint32_t *processedSize, bool isOK)
{
    HResult rs;
    uint32_t realProcessed = 0;
    if (processedSize != NULL)
    {
        *processedSize = 0;
    }
    while(size != 0)
    {
        if (m_fileIsOpen)
        {
            uint32_t numBytesToWrite = (uint32_t)MyMin(m_remainFileSize, size);
            HResult res = RC_S_OK;
            if (numBytesToWrite > 0)
            {
                if (!isOK)
                {
                    m_isOk = false;
                }
                if (m_realOutStream)
                {
                    uint32_t processedSizeLocal = 0;
                    res = m_realOutStream->Write((const byte_t *)data, numBytesToWrite, &processedSizeLocal);
                    numBytesToWrite = processedSizeLocal;
                }
                if (m_tempBufMode && m_isSupported)
                {
                    memcpy(m_tempBuf.data() + (m_posInFolder - m_bufStartFolderOffset), data, numBytesToWrite) ;
                }
            }
            realProcessed += numBytesToWrite;
            if (processedSize != NULL)
            {
                *processedSize = realProcessed;
            }
            data = (const void *)((const byte_t *)data + numBytesToWrite);
            size -= numBytesToWrite;
            m_remainFileSize -= numBytesToWrite;
            m_posInFolder += numBytesToWrite;
            if (res != RC_S_OK)
            {
                return res;
            }
            if (m_remainFileSize == 0)
            {
                m_realOutStream.Release();
                HResult hr = CloseFile() ;
                if(!IsSuccess(hr))
                {
                    return hr ;
                }
        
                if (m_tempBufMode)
                {
                    while (m_currentIndex < (int32_t)m_extractStatuses->size())
                    {
                        const RCCabMvItem& mvItem = m_database->m_items[m_startIndex + m_currentIndex];
                        const RCCabItem& item = m_database->m_volumes[mvItem.m_volumeIndex].m_items[mvItem.m_itemIndex];
                        if (item.m_offset != m_bufStartFolderOffset)
                        {
                            break ;
                        }
                        HResult result = OpenFile();
                        m_fileIsOpen = true;
                        m_currentIndex++;
                        m_isOk = true;
                        if (result == RC_S_OK && m_realOutStream && m_isSupported)
                        {
                            result = RCStreamUtils::WriteStream(m_realOutStream.Get(), m_tempBuf.data(), item.m_size);
                        }
        
                        if (m_isSupported)
                        {
                            hr = CloseFile() ;
                            if(!IsSuccess(hr))
                            {
                                return hr ;
                            }
                            if(!IsSuccess(result))
                            {
                                return result ;
                            }
                        }
                        else
                        {
                            m_realOutStream.Release();
                            hr = m_extractCallback->SetOperationResult(m_startIndex + m_currentIndex, RC_ARCHIVE_EXTRACT_RESULT_UNSUPPORTED_METHOD) ;
                            if(!IsSuccess(hr))
                            {
                                return hr ;
                            }
                            m_fileIsOpen = false;
                        }
                    }
                    m_tempBufMode = false;
                }
            }
            if (realProcessed > 0)
            {
                break; // with this break this function works as Write-Part
            }
        }
        else
        {
            if (m_currentIndex >= static_cast<int32_t>(m_extractStatuses->size()))
            {
                return RC_E_FAIL;
            }

            const RCCabMvItem &mvItem = m_database->m_items[m_startIndex + m_currentIndex];
            const RCCabItem &item = m_database->m_volumes[mvItem.m_volumeIndex].m_items[mvItem.m_itemIndex];
            m_remainFileSize = item.m_size;
            uint32_t fileOffset = item.m_offset;

            if (fileOffset < m_posInFolder)
            {
                return RC_E_FAIL;
            }
            if (fileOffset > m_posInFolder)
            {
                uint32_t numBytesToWrite = MyMin(fileOffset - (uint32_t)m_posInFolder, size);
                realProcessed += numBytesToWrite;
                if (processedSize != NULL)
                *processedSize = realProcessed;
                data = (const void *)((const byte_t *)data + numBytesToWrite);
                size -= numBytesToWrite;
                m_posInFolder += numBytesToWrite;
            }
            if (fileOffset == m_posInFolder)
            {
                rs = OpenFile();
                if (!IsSuccess(rs))
                {
                    return rs;
                }
                m_fileIsOpen = true;
                m_currentIndex++;
                m_isOk = true;
            }
        }
    }
    return WriteEmptyFiles();
}

END_NAMESPACE_RCZIP
