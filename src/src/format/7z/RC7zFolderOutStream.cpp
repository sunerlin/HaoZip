/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "format/7z/RC7zFolderOutStream.h"
#include "format/common/RCOutStreamWithCRC.h"
#include "format/7z/RC7zItem.h"
#include "format/7z/RC7zArchiveDatabaseEx.h"

/////////////////////////////////////////////////////////////////
//RC7zFolderOutStream class implementation

BEGIN_NAMESPACE_RCZIP

RC7zFolderOutStream::RC7zFolderOutStream():
    m_crcStreamSpec(NULL),
    m_db(NULL),
    m_extractStatuses(NULL),
    m_startIndex(0),
    m_ref2Offset(0),
    m_testMode(false),
    m_fileIsOpen(false),
    m_checkCrc(false),
    m_rem(0)
{
    m_crcStreamSpec = new RCOutStreamWithCRC;
    m_crcStream = m_crcStreamSpec;
}

RC7zFolderOutStream::~RC7zFolderOutStream()
{
}

HResult RC7zFolderOutStream::OpenFile()
{    
    int32_t askMode = ((*m_extractStatuses)[m_currentIndex]) ? (m_testMode ? RC_ARCHIVE_EXTRACT_ASK_MODE_TEST :
                                                                             RC_ARCHIVE_EXTRACT_ASK_MODE_EXTRACT) :
                                                                RC_ARCHIVE_EXTRACT_ASK_MODE_SKIP ;
    
    ISequentialOutStreamPtr realOutStream ;

    uint32_t index = m_startIndex + m_currentIndex;
    HResult hr = m_extractCallback->GetStream(m_ref2Offset + index, realOutStream.GetAddress(), askMode) ;
    if(!IsSuccess(hr))
    {
        return hr ;
    }
    m_crcStreamSpec->SetStream(realOutStream.Get());
    m_crcStreamSpec->Init(m_checkCrc);
    m_fileIsOpen = true ;
    
    const RC7zFileItemPtr& fi = m_db->m_files[index];
    m_rem = fi->m_size ;
    if ( (askMode == RC_ARCHIVE_EXTRACT_ASK_MODE_EXTRACT) && 
         !realOutStream &&
         !m_db->IsItemAnti(index) && 
         !fi->m_isDir)
    {
        askMode = RC_ARCHIVE_EXTRACT_ASK_MODE_SKIP ;
    }        
    return m_extractCallback->PrepareOperation(m_ref2Offset + index, askMode) ;
}

HResult RC7zFolderOutStream::CloseFileAndSetResult(uint32_t index, int32_t res)
{
    m_crcStreamSpec->ReleaseStream();
    m_fileIsOpen = false;
    m_currentIndex++;
    return m_extractCallback->SetOperationResult(index, res) ;
}

HResult RC7zFolderOutStream::CloseFileAndSetResult()
{
    uint32_t index = m_startIndex + m_currentIndex ;
    const RC7zFileItemPtr& fi = m_db->m_files[index] ;
    return CloseFileAndSetResult(index, 
                                (fi->m_isDir || !fi->m_crcDefined || !m_checkCrc || fi->m_crc == m_crcStreamSpec->GetCRC()) ?
                                        RC_ARCHIVE_EXTRACT_RESULT_OK :
                                        RC_ARCHIVE_EXTRACT_RESULT_CRC_ERROR) ;
}

HResult RC7zFolderOutStream::ProcessEmptyFiles()
{
    HResult hr = RC_S_OK ;
    while (m_currentIndex < (int32_t)m_extractStatuses->size() && m_db->m_files[m_startIndex + m_currentIndex]->m_size == 0)
    {
        hr = OpenFile() ;
        if(!IsSuccess(hr))
        {
            return hr ;
        }
        hr = CloseFileAndSetResult() ;
        if(!IsSuccess(hr))
        {
            return hr ;
        }
    }
    return RC_S_OK ;
}

HResult RC7zFolderOutStream::Init(  const RC7zArchiveDatabaseEx* archiveDatabase,
                                    uint32_t ref2Offset,
                                    uint32_t startIndex,
                                    const RCBoolVector* extractStatuses,
                                    IArchiveExtractCallback* extractCallback,
                                    bool testMode,
                                    bool checkCrc)
{
    m_db = archiveDatabase;
    m_ref2Offset = ref2Offset;
    m_startIndex = startIndex;
    
    m_extractStatuses = extractStatuses;
    m_extractCallback = extractCallback;
    m_testMode = testMode;
    
    m_checkCrc = checkCrc;
    
    m_currentIndex = 0;
    m_fileIsOpen = false;
    return ProcessEmptyFiles() ;
}

HResult RC7zFolderOutStream::FlushCorrupted(int32_t resultEOperationResult)
{
    HResult hr = RC_S_OK ;
    while (m_currentIndex < (int32_t)m_extractStatuses->size())
    {
        if (m_fileIsOpen)
        {
            hr = CloseFileAndSetResult(m_startIndex + m_currentIndex, resultEOperationResult) ;
            if(!IsSuccess(hr))
            {
                return hr ;
            }
        }
        else
        {
            hr = OpenFile() ;
            if(!IsSuccess(hr))
            {
                return hr ;
            }
        }
    }
    return RC_S_OK ;
}

HResult RC7zFolderOutStream::WasWritingFinished() const
{
    if (m_currentIndex == (int32_t)m_extractStatuses->size())
    {
        return RC_S_OK ;
    }
    return RC_E_FAIL ;
}

HResult RC7zFolderOutStream::Write(const void* data, uint32_t size, uint32_t* processedSize)
{
    if (processedSize)
    {
        *processedSize = 0 ;
    }
    while (size != 0)
    {
        if (m_fileIsOpen)
        {
            uint32_t cur = size < m_rem ? size : (uint32_t)m_rem ;
            HResult hr = m_crcStream->Write(data, cur, &cur) ;
            if(!IsSuccess(hr))
            {
                return hr ;
            }
            if (cur == 0)
            {
                break ;
            }
            data = (const byte_t*)data + cur ;
            size -= cur ;
            m_rem -= cur ;
            if (processedSize)
            {
                *processedSize += cur ;
            }
            if (m_rem == 0)
            {
                hr = CloseFileAndSetResult() ;
                if(!IsSuccess(hr))
                {
                    return hr ;
                }
                hr = ProcessEmptyFiles() ;
                if(!IsSuccess(hr))
                {
                    return hr ;
                }
                continue ;
            }
        }
        else
        {
            HResult hr = ProcessEmptyFiles() ;
            if(!IsSuccess(hr))
            {
                return hr ;
            }
            if (m_currentIndex == (int32_t)m_extractStatuses->size())
            {
                // we support partial extracting
                if (processedSize)
                {
                    *processedSize += size ;
                }
                break ;
            }
            hr = OpenFile() ;
            if(!IsSuccess(hr))
            {
                return hr ;
            }
        }
    }
    return RC_S_OK ;
}


END_NAMESPACE_RCZIP
