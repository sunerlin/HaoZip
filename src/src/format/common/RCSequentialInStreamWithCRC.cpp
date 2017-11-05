/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "format/common/RCSequentialInStreamWithCRC.h"
#include "algorithm/7zCrc.h"

/////////////////////////////////////////////////////////////////
//RCSequentialInStreamWithCRC class implementation

BEGIN_NAMESPACE_RCZIP

RCSequentialInStreamWithCRC::RCSequentialInStreamWithCRC():
    m_size(0),
    m_crc(0),
    m_wasFinished(false)
{
}

RCSequentialInStreamWithCRC::~RCSequentialInStreamWithCRC()
{
}

void RCSequentialInStreamWithCRC::Init()
{
    m_size = 0;
    m_wasFinished = false;
    m_crc = CRC_INIT_VAL;
}

void RCSequentialInStreamWithCRC::SetStream(ISequentialInStream *stream)
{
    m_stream = stream ; 
}

void RCSequentialInStreamWithCRC::ReleaseStream()
{
    m_stream.Release();
}

uint32_t RCSequentialInStreamWithCRC::GetCRC() const
{
    return CRC_GET_DIGEST(m_crc);
}

uint64_t RCSequentialInStreamWithCRC::GetSize() const
{
    return m_size;
}

bool RCSequentialInStreamWithCRC::WasFinished() const
{
    return m_wasFinished;
}

HResult RCSequentialInStreamWithCRC::Read(void* data, uint32_t size, uint32_t* processedSize)
{
    uint32_t realProcessedSize = 0 ;
    HResult result = m_stream->Read(data, size, &realProcessedSize);

    m_size += realProcessedSize;
    if (size > 0 && realProcessedSize == 0)
    {
        m_wasFinished = true;
    }

    m_crc = CrcUpdate(m_crc, data, realProcessedSize);
    if (processedSize != NULL)
    {
        *processedSize = realProcessedSize;
    }
    return result;
}

END_NAMESPACE_RCZIP
