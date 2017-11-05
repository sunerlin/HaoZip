/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "format/7z/RC7zArchiveDatabaseEx.h"

/////////////////////////////////////////////////////////////////
//RC7zArchiveDatabaseEx class implementation

BEGIN_NAMESPACE_RCZIP

RC7zArchiveDatabaseEx::RC7zArchiveDatabaseEx()
{
}

RC7zArchiveDatabaseEx::~RC7zArchiveDatabaseEx()
{
}

void RC7zArchiveDatabaseEx::Clear()
{
    RC7zArchiveDatabase::Clear();
    m_archiveInfo.Clear();
    m_packStreamStartPositions.clear();
    m_folderStartPackStreamIndex.clear();
    m_folderStartFileIndex.clear();
    m_fileIndexToFolderIndexMap.clear();

    m_headersSize = 0;
    m_phySize = 0;

}

void RC7zArchiveDatabaseEx::Fill()
{
    FillFolderStartPackStream();
    FillStartPos();
    FillFolderStartFileIndex();
}

uint64_t RC7zArchiveDatabaseEx::GetFolderStreamPos(int32_t folderIndex, int32_t indexInFolder) const
{
    return m_archiveInfo.m_dataStartPosition +
           m_packStreamStartPositions[m_folderStartPackStreamIndex[folderIndex] + indexInFolder];
}

uint64_t RC7zArchiveDatabaseEx::GetFolderFullPackSize(int32_t folderIndex) const
{
    RC7zNum packStreamIndex = m_folderStartPackStreamIndex[folderIndex];
    const RC7zFolder &folder = m_folders[folderIndex];
    uint64_t size = 0;
    int32_t count = (int32_t)folder.m_packStreams.size() ;
    for (int32_t i = 0; i < count ; i++)
    {
        size += m_packSizes[packStreamIndex + i];
    }
    return size;
}

uint64_t RC7zArchiveDatabaseEx::GetFolderPackStreamSize(int32_t folderIndex, int32_t streamIndex) const
{
    return m_packSizes[m_folderStartPackStreamIndex[folderIndex] + streamIndex];
}

uint64_t RC7zArchiveDatabaseEx::GetFilePackSize(RC7zNum fileIndex) const
{
    RC7zNum folderIndex = m_fileIndexToFolderIndexMap[fileIndex];
    if (folderIndex != RC7zDefs::s_7zNumNoIndex)
    {
        if (m_folderStartFileIndex[folderIndex] == fileIndex)
        {
            return GetFolderFullPackSize(folderIndex);
        }
    }
    return 0;
}

void RC7zArchiveDatabaseEx::FillFolderStartPackStream()
{
    m_folderStartPackStreamIndex.clear();
    m_folderStartPackStreamIndex.reserve(m_folders.size());
    RC7zNum startPos = 0;
    int32_t count = (int32_t)m_folders.size() ;
    for (int32_t i = 0; i < count; i++)
    {
        m_folderStartPackStreamIndex.push_back(startPos);
        startPos += (RC7zNum)m_folders[i].m_packStreams.size();
    }
}

void RC7zArchiveDatabaseEx::FillStartPos()
{
    m_packStreamStartPositions.clear();
    m_packStreamStartPositions.reserve(m_packSizes.size());
    uint64_t startPos = 0;
    int32_t count = (int32_t)m_packSizes.size() ;
    for (int32_t i = 0; i < count; i++)
    {
        m_packStreamStartPositions.push_back(startPos);
        startPos += m_packSizes[i];
    }
}

void RC7zArchiveDatabaseEx::FillFolderStartFileIndex()
{
    m_folderStartFileIndex.clear();
    m_folderStartFileIndex.reserve(m_folders.size());
    m_fileIndexToFolderIndexMap.clear();
    m_fileIndexToFolderIndexMap.reserve(m_files.size());

    int32_t folderIndex = 0;
    RC7zNum indexInFolder = 0;
    int32_t count = (int32_t)m_files.size() ;
    for (int32_t i = 0; i < count; i++)
    {
        const RC7zFileItemPtr &file = m_files[i];
        bool emptyStream = !file->m_hasStream;
        if (emptyStream && indexInFolder == 0)
        {
            m_fileIndexToFolderIndexMap.push_back(RC7zDefs::s_7zNumNoIndex);
            continue;
        }
        if (indexInFolder == 0)
        {
            // v3.13 incorrectly worked with empty folders
            // v4.07: Loop for skipping empty folders
            for (;;)
            {
                if (folderIndex >= (int32_t)m_folders.size())
                {
                    _ThrowCode(RC_E_DataError) ;
                }
                m_folderStartFileIndex.push_back(i); // check it
                if (m_numUnpackStreamsVector[folderIndex] != 0)
                {
                    break;
                }
                folderIndex++;
            }
        }
        m_fileIndexToFolderIndexMap.push_back(folderIndex);
        if (emptyStream)
        {
            continue;
        }
        indexInFolder++;
        if (indexInFolder >= m_numUnpackStreamsVector[folderIndex])
        {
            folderIndex++;
            indexInFolder = 0;
        }
    }
}
    
END_NAMESPACE_RCZIP
