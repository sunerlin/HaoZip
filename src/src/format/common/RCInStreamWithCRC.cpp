/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "format/common/RCInStreamWithCRC.h"
#include "algorithm/7zCrc.h"

/////////////////////////////////////////////////////////////////
//RCInStreamWithCRC class implementation

BEGIN_NAMESPACE_RCZIP

RCInStreamWithCRC::RCInStreamWithCRC():
    m_size(0),
    m_crc(0),
    m_wasFinished(false)
{
}

RCInStreamWithCRC::~RCInStreamWithCRC()
{
}

void RCInStreamWithCRC::Init()
{
    m_size = 0 ;
    m_wasFinished = false ;
    m_crc = CRC_INIT_VAL ;
}

void RCInStreamWithCRC::SetStream(IInStream *stream)
{
    m_stream = stream ;
}

void RCInStreamWithCRC::ReleaseStream()
{
    m_stream.Release();
}

uint32_t RCInStreamWithCRC::GetCRC() const
{
    return CRC_GET_DIGEST(m_crc);
}

uint64_t RCInStreamWithCRC::GetSize() const
{
    return m_size;
}

bool RCInStreamWithCRC::WasFinished() const
{
    return m_wasFinished;
}

HResult RCInStreamWithCRC::Read(void* data, uint32_t size, uint32_t* processedSize)
{
    uint32_t realProcessedSize;
    HResult result = m_stream->Read(data, size, &realProcessedSize);
    if (size > 0 && realProcessedSize == 0)
    {
        m_wasFinished = true;
    }

    m_size += realProcessedSize;
    m_crc = CrcUpdate(m_crc, data, realProcessedSize);
    if (processedSize != NULL)
    {
        *processedSize = realProcessedSize;
    }
    return result;
}

HResult RCInStreamWithCRC::Seek(int64_t offset, RC_STREAM_SEEK seekOrigin, uint64_t* newPosition)
{
    if (seekOrigin != RC_STREAM_SEEK_SET || offset != 0)
    {
        return RC_E_FAIL ;
    }
    m_size = 0;
    m_crc = CRC_INIT_VAL;
    return m_stream->Seek(offset, seekOrigin, newPosition);
}

HResult RCInStreamWithCRC::QueryInterface(RC_IID iid, void** outObject)
{
    if (outObject == NULL)
    {
        return RC_E_INVALIDARG ;
    }

    if (IUnknownImpl<IInStream>::QueryInterface(iid,outObject) == RC_S_OK)
    {
        return RC_S_OK ;
    }
    else if(iid == ISequentialInStream::IID)
    {
        *outObject = (ISequentialInStream*)this ;
        this->AddRef() ;
        return RC_S_OK ;
    }
    return RC_E_NOINTERFACE ;
}

END_NAMESPACE_RCZIP
