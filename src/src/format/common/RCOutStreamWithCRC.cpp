/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "format/common/RCOutStreamWithCRC.h"
#include "algorithm/7zCrc.h"

/////////////////////////////////////////////////////////////////
//RCOutStreamWithCRC class implementation

BEGIN_NAMESPACE_RCZIP

RCOutStreamWithCRC::RCOutStreamWithCRC():
    m_size(0),
    m_crc(0),
    m_calculate(false)
{
}

RCOutStreamWithCRC::~RCOutStreamWithCRC()
{
}

void RCOutStreamWithCRC::SetStream(ISequentialOutStream *stream)
{
    m_stream = stream ;
}

void RCOutStreamWithCRC::ReleaseStream()
{
    m_stream.Release();
}

void RCOutStreamWithCRC::Init(bool calculate)
{
    m_size = 0;
    m_calculate = calculate;
    m_crc = CRC_INIT_VAL ;
}

void RCOutStreamWithCRC::InitCRC()
{
    m_crc = CRC_INIT_VAL;
}

uint64_t RCOutStreamWithCRC::GetSize() const
{
    return m_size;
}

uint32_t RCOutStreamWithCRC::GetCRC() const
{
    return CRC_GET_DIGEST(m_crc) ;
}

HResult RCOutStreamWithCRC::Write(const void* data, uint32_t size, uint32_t* processedSize) 
{
    HResult result = RC_S_OK ;
    if (m_stream)
    {
        result = m_stream->Write(data, size, &size);
    }

    if (m_calculate)
    {
        m_crc = CrcUpdate(m_crc, data, size);
    }

    m_size += size ;
    if (processedSize)
    {
        *processedSize = size;
    }
    return result;
}

END_NAMESPACE_RCZIP
