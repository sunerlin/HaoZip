/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/
#include "format/udf/RCUdfExtentsStream.h"

BEGIN_NAMESPACE_RCZIP

RCUdfExtentsStream::RCUdfExtentsStream():
    m_phyPos(0),
    m_virtPos(0),
    m_needStartSeek(true)
{
}

void RCUdfExtentsStream::Init()
{
    m_virtPos = 0;
    m_phyPos = 0;
    m_needStartSeek = true;
}
void RCUdfExtentsStream::ReleaseStream() 
{ 
    m_stream.Release(); 
}

HResult RCUdfExtentsStream::SeekToPhys()
{
    if(m_stream != NULL)
    {
        return m_stream->Seek(m_phyPos, RC_STREAM_SEEK_SET, NULL) ;
    }
    else
    {
        return RC_E_FAIL ;
    }
}

HResult RCUdfExtentsStream::Read(void* data, uint32_t size, uint32_t* processedSize)
{
    if (processedSize)
    {
        *processedSize = 0;
    }
    if (size > 0)
    {
        if(m_extents.empty())
        {
            return RC_E_FAIL ;
        }
        uint64_t totalSize = m_extents.back().m_virt;
        if (m_virtPos >= totalSize)
        {
            return (m_virtPos == totalSize) ? RC_S_OK : RC_E_FAIL;
        }
        int32_t left = 0 ;
        int32_t right = (int32_t)m_extents.size() - 1;
        for (;;)
        {
            int mid = (left + right) / 2;
            if (mid == left)
            {
                break;
            }
            if (m_virtPos < m_extents[mid].m_virt)
            {
                right = mid;
            }
            else
            {
                left = mid;
            }
        }

        const RCUdfSeekExtent& extent = m_extents[left];
        uint64_t phyPos = extent.m_phy + (m_virtPos - extent.m_virt);
        if (m_needStartSeek || m_phyPos != phyPos)
        {
            m_needStartSeek = false;
            m_phyPos = phyPos;
            HResult hr = SeekToPhys() ;
            if(!IsSuccess(hr))
            {
                return hr ;
            }
        }

        uint64_t rem = m_extents[left + 1].m_virt - m_virtPos;
        if (size > rem)
        {
            size = (uint32_t)rem ;
        }

        HResult res = m_stream->Read(data, size, &size);
        m_phyPos += size;
        m_virtPos += size;
        if (processedSize)
        {
            *processedSize = size ;
        }
        return res;
    }
    return RC_S_OK ;
}

HResult RCUdfExtentsStream::Seek(int64_t offset, RC_STREAM_SEEK seekOrigin, uint64_t* newPosition)
{
    switch(seekOrigin)
    {
    case RC_STREAM_SEEK_SET: 
        m_virtPos = offset ; 
        break;
    case RC_STREAM_SEEK_CUR: 
        m_virtPos += offset; 
        break;
    case RC_STREAM_SEEK_END: 
        if(m_extents.empty())
        {
            return RC_E_FAIL ;
        }
        m_virtPos = m_extents.back().m_virt + offset; 
        break ;
    default: 
        return RC_E_FAIL ;
    }
    if (newPosition)
    {
        *newPosition = m_virtPos ;
    }
    return RC_S_OK ;
}

END_NAMESPACE_RCZIP
