/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "format/7z/RC7zFolderInStream.h"
#include "format/common/RCSequentialInStreamWithCRC.h"

/////////////////////////////////////////////////////////////////
//RC7zFolderInStream class implementation

BEGIN_NAMESPACE_RCZIP

RC7zFolderInStream::RC7zFolderInStream():
    m_inStreamWithHashSpec(NULL),
    m_currentSizeIsDefined(false),
    m_currentSize(0),
    m_fileIsOpen(false),
    m_filePos(0),
    m_fileIndices(NULL),
    m_numFiles(0),
    m_fileIndex(0)
{
    m_inStreamWithHashSpec = new RCSequentialInStreamWithCRC ;
    m_inStreamWithHash = m_inStreamWithHashSpec;
}

RC7zFolderInStream::~RC7zFolderInStream()
{
}

HResult RC7zFolderInStream::OpenStream()
{
    m_filePos = 0;
    while (m_fileIndex < m_numFiles)
    {
        m_currentSizeIsDefined = false;
        ISequentialInStreamPtr stream;
        HResult result = m_updateCallback->GetStream(m_fileIndices[m_fileIndex], stream.GetAddress());
        if (result != RC_S_OK && result != RC_S_FALSE)
        {
            return result;
        }
        m_fileIndex++;
        m_inStreamWithHashSpec->SetStream(stream.Get());
        m_inStreamWithHashSpec->Init();
        if (!stream)
        {
            HResult hr = m_updateCallback->SetOperationResult(m_fileIndices[m_fileIndex],RC_ARCHIVE_UPDATE_RESULT_OK) ;
            if(!IsSuccess(hr))
            {
                return hr ;
            }
            m_sizes.push_back(0);
            m_processed.push_back(result == RC_S_OK && stream != NULL);
            AddDigest();
            continue;
        }
        IStreamGetSizePtr streamGetSize;
        if (stream.QueryInterface(IID_IStreamGetSize, streamGetSize.GetAddress()) == RC_S_OK)
        {
            if(streamGetSize)
            {
                m_currentSizeIsDefined = true;
                HResult hr = streamGetSize->GetSize(&m_currentSize) ;
                if(!IsSuccess(hr))
                {
                    return hr ;
                }
            }
        }
        m_fileIsOpen = true;
        return RC_S_OK;
    }
    return RC_S_OK;
}

HResult RC7zFolderInStream::CloseStream()
{
    HResult hr = m_updateCallback->SetOperationResult(m_fileIndices[m_fileIndex],RC_ARCHIVE_UPDATE_RESULT_OK) ;
    if(!IsSuccess(hr))
    {
        return hr ;
    }
    m_inStreamWithHashSpec->ReleaseStream();
    m_fileIsOpen = false;
    m_processed.push_back(true);
    m_sizes.push_back(m_filePos);
    AddDigest();
    return RC_S_OK;
}

void RC7zFolderInStream::AddDigest()
{
    m_crcs.push_back(m_inStreamWithHashSpec->GetCRC());
}

void RC7zFolderInStream::Init(IArchiveUpdateCallback* updateCallback,
                              const uint32_t* fileIndices, 
                              uint32_t numFiles)
{
    m_updateCallback = updateCallback;
    m_numFiles = numFiles;
    m_fileIndex = 0;
    m_fileIndices = fileIndices;
    m_processed.clear();
    m_crcs.clear();
    m_sizes.clear();
    m_fileIsOpen = false;
    m_currentSizeIsDefined = false;
}

uint64_t RC7zFolderInStream::GetFullSize() const
{
    uint64_t size = 0;
    int32_t count = (int32_t)m_sizes.size() ;
    for (int32_t i = 0; i < count ; i++)
    {
        size += m_sizes[i];
    }
    return size;
}

HResult RC7zFolderInStream::Read(void* data, uint32_t size, uint32_t* processedSize)
{
    uint32_t realProcessedSize = 0;
    while ((m_fileIndex < m_numFiles || m_fileIsOpen) && size > 0)
    {
        if (m_fileIsOpen)
        {
            uint32_t localProcessedSize = 0 ;
            HResult hr = m_inStreamWithHash->Read( ((byte_t *)data) + realProcessedSize, size, &localProcessedSize) ;
            if(!IsSuccess(hr))
            {
                return hr ;
            }
            if (localProcessedSize == 0)
            {
                hr = CloseStream() ;
                if(!IsSuccess(hr))
                {
                    return hr ;
                }
                continue;
            }
            realProcessedSize += localProcessedSize;
            m_filePos += localProcessedSize;
            size -= localProcessedSize;
            break;
        }
        else
        {
            HResult hr = OpenStream() ;
            if(!IsSuccess(hr))
            {
                return hr ;
            }
        }
    }
    if (processedSize != 0)
    {
        *processedSize = realProcessedSize;
    }
    return RC_S_OK;
}

HResult RC7zFolderInStream::GetSubStreamSize(uint64_t subStream, uint64_t& size)
{
    size = 0 ;
    int32_t subStreamIndex = (int32_t)subStream;
    if (subStreamIndex < 0 || subStream > m_sizes.size())
    {
        return RC_E_FAIL;
    }
    if (subStreamIndex < (int32_t)m_sizes.size())
    {
        size = m_sizes[subStreamIndex];
        return RC_S_OK;
    }
    if (!m_currentSizeIsDefined)
    {
        return RC_S_FALSE;
    }
    size = m_currentSize;
    return RC_S_OK;
}

END_NAMESPACE_RCZIP
