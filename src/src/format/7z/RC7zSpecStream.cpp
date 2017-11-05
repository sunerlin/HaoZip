/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "format/7z/RC7zSpecStream.h"

/////////////////////////////////////////////////////////////////
//RC7zSpecStream class implementation

BEGIN_NAMESPACE_RCZIP

RC7zSpecStream::RC7zSpecStream():
    m_size(0)
{
}

RC7zSpecStream::~RC7zSpecStream()
{
}

void RC7zSpecStream::Init(ISequentialInStream* stream)
{
    m_stream = stream;
    m_spGetSubStreamSize = 0 ;
    m_stream.QueryInterface(IID_ICompressGetSubStreamSize, m_spGetSubStreamSize.GetAddress());
    m_size = 0;
}

uint64_t RC7zSpecStream::GetSize() const
{
    return m_size;
}

HResult RC7zSpecStream::Read(void* data, uint32_t size, uint32_t* processedSize)
{
    uint32_t realProcessedSize = 0 ;
    HResult result = m_stream->Read(data, size, &realProcessedSize);
    m_size += realProcessedSize ;
    if (processedSize != NULL)
    {
        *processedSize = realProcessedSize;
    }
    return result ;
}

HResult RC7zSpecStream::GetSubStreamSize(uint64_t subStream, uint64_t& size)
{
    if (m_spGetSubStreamSize == NULL)
    {
        return RC_E_NOTIMPL ;
    }
    return  m_spGetSubStreamSize->GetSubStreamSize(subStream, size) ;
}

END_NAMESPACE_RCZIP
