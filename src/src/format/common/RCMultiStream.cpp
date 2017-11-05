/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "format/common/RCMultiStream.h"

/////////////////////////////////////////////////////////////////
//RCMultiStream class implementation

BEGIN_NAMESPACE_RCZIP

RCMultiStream::RCMultiStream():
    m_streamIndex(0),
    m_pos(0),
    m_seekPos(0),
    m_totalLength(0)
{
}

RCMultiStream::~RCMultiStream()
{
}

void RCMultiStream::Init()
{
    m_streamIndex = 0;
    m_pos = 0;
    m_seekPos = 0;
    m_totalLength = 0;

    for (int32_t i = 0; i < (int32_t)m_streams.size(); i++)
    {
        m_totalLength += m_streams[i].m_size ;
    }
}

HResult RCMultiStream::Read(void* data, uint32_t size, uint32_t* processedSize)
{
    if (processedSize != NULL)
    {
        *processedSize = 0;
    }

    while (m_streamIndex < (int32_t)m_streams.size() && size > 0)
    {
        CSubStreamInfo& s = m_streams[m_streamIndex];
        if (m_pos == s.m_size)
        {
            m_streamIndex++;
            m_pos = 0;
            continue;
        }

        HResult hr = s.m_spStream->Seek(s.m_pos + m_pos, RC_STREAM_SEEK_SET, 0) ;
        if (!IsSuccess(hr))
        {
            return hr ;
        }
        uint32_t sizeToRead = uint32_t(MyMin((uint64_t)size, s.m_size - m_pos));
        uint32_t realProcessed = 0 ;

        HResult result = s.m_spStream->Read(data, sizeToRead,&realProcessed);
        data = (void *)((byte_t *)data + realProcessed) ;
        size -= realProcessed;
        if (processedSize != NULL)
        {
            *processedSize += realProcessed;
        }

        m_pos += realProcessed;
        m_seekPos += realProcessed;
        if (!IsSuccess(result))
        {
            return result ;
        }
        break;
    }
    return RC_S_OK;
}

HResult RCMultiStream::Seek(int64_t offset, RC_STREAM_SEEK seekOrigin, uint64_t* newPosition) 
{
    uint64_t newPos = 0 ;
    switch(seekOrigin)
    {
    case RC_STREAM_SEEK_SET:
        newPos = offset;
        break;
    case RC_STREAM_SEEK_CUR:
        newPos = m_seekPos + offset;
        break;
    case RC_STREAM_SEEK_END:
        newPos = m_totalLength + offset;
        break;
    default:
        return RC_E_INVALIDARG;
    }

    m_seekPos = 0 ;

    for (m_streamIndex = 0; m_streamIndex < (int32_t)m_streams.size(); m_streamIndex++)
    {
        uint64_t size = m_streams[m_streamIndex].m_size;
        if (newPos < m_seekPos + size)
        {
            m_pos = newPos - m_seekPos;
            m_seekPos += m_pos;
            if (newPosition != 0)
            {
                *newPosition = newPos;
            }
            return RC_S_OK;
        }
        m_seekPos += size ;
    }

    if (newPos == m_seekPos)
    {
        if (newPosition != 0)
        {
            *newPosition = newPos;
        }
        return RC_S_OK;
    }
    return RC_E_FAIL;
}

IUnknown* RCMultiStream::Clone(void)
{
    RCMultiStream* multiStream = new RCMultiStream ;
    multiStream->m_streamIndex  = m_streamIndex ;
    multiStream->m_pos          = m_pos ;
    multiStream->m_seekPos      = m_seekPos ;
    multiStream->m_totalLength  = m_totalLength ;
    multiStream->m_streams      = m_streams ;

    bool result = true ;

    for (size_t index = 0 ; index < m_streams.size(); ++index)
    {
        CSubStreamInfo& streamInfo = m_streams[index] ;
        IInStreamPtr spStream = streamInfo.m_spStream ;
        if (spStream != NULL)
        {
            //Clone new stream 
            ICloneStreamPtr spCloneStream ;
            HResult hr = spStream->QueryInterface(IID_ICloneStream,(void**)spCloneStream.GetAddress()) ;
            if (!IsSuccess(hr))
            {
                result = false ;
                break ;
            }

            if (!spCloneStream)
            {
                result = false ;
                break ;
            }

            IUnknown* cloneStream = spCloneStream->Clone() ;
            if (NULL == cloneStream)
            {
                result = false ;
                break ;
            }

            IInStreamPtr spNewInStream((IInStream*)cloneStream) ;
            multiStream->m_streams[index].m_spStream = spNewInStream ;
        }
    }

    if (!result)
    {
        delete multiStream ;
        return NULL ;
    }
    else
    {
        return (IUnknown*)(IInStream*)multiStream ;
    }
}

HResult RCMultiStream::GetVolumeSize(uint32_t index, uint64_t& volumeSize)
{
    if (index >= (uint32_t)m_streams.size())
    {
        return RC_S_FALSE;
    }
    volumeSize = m_streams[index].m_size;

    return RC_S_OK;
}

HResult RCMultiStream::GetVolumeTotalSize(uint32_t index, uint64_t& volumeTotalSize)
{
    if (index >= (uint32_t)m_streams.size())
    {
        return RC_S_FALSE;
    }

    uint64_t totalSize = 0;
    uint32_t i = 0;
    for (i = 0; i < index; i++)
    {
        totalSize += m_streams[i].m_size;
    }
    volumeTotalSize = totalSize;

    return RC_S_OK;
}

END_NAMESPACE_RCZIP
