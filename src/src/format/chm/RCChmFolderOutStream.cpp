/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "format/chm/RCChmFolderOutStream.h"

/////////////////////////////////////////////////////////////////
//RCChmFolderOutStream class implementation

BEGIN_NAMESPACE_RCZIP

void RCChmFolderOutStream::Init(const RCChmFilesDatabase* database,
                                IArchiveExtractCallback* extractCallback,
                                bool testMode)
{
    m_database = database;
    m_extractCallback = extractCallback;
    m_testMode = testMode;

    m_currentIndex = 0;
    m_fileIsOpen = false;
}

HResult RCChmFolderOutStream::OpenFile()
{
    int32_t askMode = (*m_extractStatuses)[m_currentIndex] ? (m_testMode ?
        RC_ARCHIVE_EXTRACT_ASK_MODE_TEST :
    RC_ARCHIVE_EXTRACT_ASK_MODE_EXTRACT) :
    RC_ARCHIVE_EXTRACT_ASK_MODE_SKIP;

    m_realOutStream.Release();

    HResult hr = m_extractCallback->GetStream(m_startIndex + m_currentIndex, m_realOutStream.GetAddress(), askMode);
    if (hr != RC_S_OK)
    {
        return hr;
    }

    if (!m_realOutStream && !m_testMode)
    {
        askMode = RC_ARCHIVE_EXTRACT_ASK_MODE_SKIP;
    }

    return m_extractCallback->PrepareOperation(m_startIndex + m_currentIndex,askMode);
}

HResult RCChmFolderOutStream::WriteEmptyFiles()
{
    if (m_fileIsOpen)
    {
        return RC_S_OK;
    }

    for (;m_currentIndex < m_numFiles; m_currentIndex++)
    {
        uint64_t fileSize = m_database->GetFileSize(m_startIndex + m_currentIndex);
        if (fileSize != 0)
        {
            return RC_S_OK;
        }

        HResult hr = OpenFile();
        m_realOutStream.Release();
        if (hr != RC_S_OK)
        {
            return hr;
        }

        hr = m_extractCallback->SetOperationResult(m_startIndex + m_currentIndex, RC_ARCHIVE_EXTRACT_RESULT_OK);
        if (hr != RC_S_OK)
        {
            return hr;
        }
    }
    return RC_S_OK;
}

// This is WritePart function
HResult RCChmFolderOutStream::Write2(const void* data, uint32_t size, uint32_t* processedSize, bool isOK)
{
    uint32_t realProcessed = 0;
    if (processedSize != NULL)
    {
        *processedSize = 0;
    }
    while(size != 0)
    {
        if (m_fileIsOpen)
        {
            uint32_t numBytesToWrite = (uint32_t)MyMin(m_remainFileSize, (uint64_t)(size));
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
            }
            realProcessed += numBytesToWrite;
            if (processedSize != NULL)
            {
                *processedSize = realProcessed;
            }
            data = (const void *)((const byte_t *)data + numBytesToWrite);
            size -= numBytesToWrite;
            m_remainFileSize -= numBytesToWrite;
            m_posInSection += numBytesToWrite;
            m_posInFolder += numBytesToWrite;
            if (res != RC_S_OK)
            {
                return res;
            }
            if (m_remainFileSize == 0)
            {
                m_realOutStream.Release();
                HResult hr = m_extractCallback->SetOperationResult(m_startIndex + m_currentIndex, m_isOk ?
                    RC_ARCHIVE_EXTRACT_RESULT_OK:
                    RC_ARCHIVE_EXTRACT_RESULT_DATA_ERROR);
                if (hr != RC_S_OK)
                {
                    return hr;
                }

                m_fileIsOpen = false;
            }

            if (realProcessed > 0)
            {
                break; // with this break this function works as write part
            }
        }
        else
        {
            if (m_currentIndex >= m_numFiles)
            {
                return RC_E_FAIL;
            }

            int fullIndex = m_startIndex + m_currentIndex;
            m_remainFileSize = m_database->GetFileSize(fullIndex);
            uint64_t fileOffset = m_database->GetFileOffset(fullIndex);
            if (fileOffset < m_posInSection)
            {
                return RC_E_FAIL;
            }

            if (fileOffset > m_posInSection)
            {
                uint32_t numBytesToWrite = (uint32_t)MyMin(fileOffset - m_posInSection, uint64_t(size));
                realProcessed += numBytesToWrite;
                if (processedSize != NULL)
                {
                    *processedSize = realProcessed;
                }

                data = (const void *)((const byte_t *)data + numBytesToWrite);
                size -= numBytesToWrite;
                m_posInSection += numBytesToWrite;
                m_posInFolder += numBytesToWrite;
            }

            if (fileOffset == m_posInSection)
            {
                HResult hr = OpenFile();
                if (hr != RC_S_OK)
                {
                    return hr;
                }

                m_fileIsOpen = true;
                m_currentIndex++;
                m_isOk = true;
            }
        }
    }
    return WriteEmptyFiles();
}

HResult RCChmFolderOutStream::Write(const void *data, uint32_t size, uint32_t *processedSize)
{
    return Write2(data, size, processedSize, true);
}

HResult RCChmFolderOutStream::FlushCorrupted(uint64_t maxSize)
{
    const uint32_t kBufferSize = (1 << 10);
    byte_t buffer[kBufferSize];
    for (int i = 0; i < kBufferSize; i++)
    {
        buffer[i] = 0;
    }

    if (maxSize > m_folderSize)
    {
        maxSize = m_folderSize;
    }

    while (m_posInFolder < maxSize)
    {
        uint32_t size = (uint32_t)MyMin(maxSize - m_posInFolder, (uint64_t)kBufferSize);
        uint32_t processedSizeLocal = 0;
        HResult hr = Write2(buffer, size, &processedSizeLocal, false);
        if (hr != RC_S_OK)
        {
            return hr;
        }

        if (processedSizeLocal == 0)
        {
            return RC_S_OK;
        }
    }

    return RC_S_OK;
}

END_NAMESPACE_RCZIP
