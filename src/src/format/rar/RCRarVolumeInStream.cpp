/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "format/rar/RCRarVolumeInStream.h"
#include "algorithm/7zCrc.h"

/////////////////////////////////////////////////////////////////
//RCRarVolumeInStream class implementation

BEGIN_NAMESPACE_RCZIP

void RCRarFolderInStream::Init(RCVector<RCRarIn>* archives,
                               const RCVector<RCRarItemExPtr>* items,
                               const RCRarRefItemPtr& refItem)
{
    m_archives = archives;
    m_items    = items;
    m_refItem  = refItem;
    m_curIndex = 0;
    m_crcs.clear();
    m_fileIsOpen = false;
}

HResult RCRarFolderInStream::OpenStream()
{
    while (m_curIndex < m_refItem->m_numItems)
    {
        const RCRarItemExPtr& item = (*m_items)[m_refItem->m_itemIndex + m_curIndex];
        m_stream.Attach((*m_archives)[m_refItem->m_volumeIndex + m_curIndex].
            CreateLimitedStream(item->GetDataPosition(), item->m_packSize));
        m_curIndex++;
        m_fileIsOpen = true;
        m_crc = CRC_INIT_VAL;
        return RC_S_OK;
    }
    return RC_S_OK;
}

HResult RCRarFolderInStream::CloseStream()
{
    m_crcs.push_back(CRC_GET_DIGEST(m_crc));
    m_stream.Release();
    m_fileIsOpen = false;
    return RC_S_OK;
}

HResult RCRarFolderInStream::Read(void* data, uint32_t size, uint32_t* processedSize)
{
    uint32_t realProcessedSize = 0;
    while ((m_curIndex < m_refItem->m_numItems || m_fileIsOpen) && size > 0)
    {
        HResult hr;
        if (m_fileIsOpen)
        {
            uint32_t localProcessedSize;
            hr = m_stream->Read(((byte_t *)data) + realProcessedSize, size, &localProcessedSize);
            if (hr != RC_S_OK)
            {
                return hr;
            }

            m_crc = CrcUpdate(m_crc, ((byte_t *)data) + realProcessedSize, localProcessedSize);
            if (localProcessedSize == 0)
            {
                hr = CloseStream();
                if (hr != RC_S_OK)
                {
                    return hr;
                }
                continue;
            }
            realProcessedSize += localProcessedSize;
            size -= localProcessedSize;
            break;
        }
        else
        {
            hr = OpenStream();
            if (hr != RC_S_OK)
            {
                return hr;
            }
        }
    }

    if (processedSize != 0)
    {
        *processedSize = realProcessedSize;
    }

    return RC_S_OK;
}

END_NAMESPACE_RCZIP
