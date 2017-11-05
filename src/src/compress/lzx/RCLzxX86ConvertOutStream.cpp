/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "compress/lzx/RCLzxX86ConvertOutStream.h"
#include <algorithm>

/////////////////////////////////////////////////////////////////
//RCLzxX86ConvertOutStream class implementation

BEGIN_NAMESPACE_RCZIP

#define RESIDUE (6 + 4)

RCLzxX86ConvertOutStream::RCLzxX86ConvertOutStream():
    m_processedSize(0),
    m_pos(0),
    m_translationSize(0),
    m_translationMode(false)
{
}

RCLzxX86ConvertOutStream::~RCLzxX86ConvertOutStream()
{
}

void RCLzxX86ConvertOutStream::MakeTranslation()
{
    if (m_pos <= RESIDUE)
    {
        return;
    }
    uint32_t numBytes = m_pos - RESIDUE;
    byte_t* buffer = m_buffer;
    for (uint32_t i = 0; i < numBytes;)
    {
        if (buffer[i++] == 0xE8)
        {
            int32_t absValue = 0;
            int32_t j = 0 ;
            for(j = 0; j < 4; j++)
            {
                absValue += (uint32_t)buffer[i + j] << (j * 8);
            }
            int32_t pos = (int32_t)(m_processedSize + i - 1);
            if (absValue >= -pos && absValue < (int32_t)m_translationSize)
            {
                uint32_t offset = (absValue >= 0) ? absValue - pos : absValue + m_translationSize;
                for(j = 0; j < 4; j++)
                {
                    buffer[i + j] = (byte_t)(offset & 0xFF);
                    offset >>= 8;
                }
            }
            i += 4;
        }
    }
}

void RCLzxX86ConvertOutStream::SetStream(ISequentialOutStream *outStream)
{
    m_stream = outStream;
}

void RCLzxX86ConvertOutStream::ReleaseStream()
{
    m_stream.Release();
}

HResult RCLzxX86ConvertOutStream::Write(const void* data, uint32_t size, uint32_t* processedSize) 
{
    if (processedSize != NULL)
    {
        *processedSize = 0;
    }
    if (!m_translationMode)
    {
        return m_stream->Write(data, size, processedSize);
    }
    uint32_t realProcessedSize = 0;
    while (realProcessedSize < size)
    {
        uint32_t writeSize = (uint32_t)std::min(size - realProcessedSize, s_kUncompressedBlockSize - m_pos);
        memmove(m_buffer + m_pos, (const byte_t *)data + realProcessedSize, writeSize);
        m_pos += writeSize;
        realProcessedSize += writeSize;
        if (m_pos == s_kUncompressedBlockSize)
        {
            HResult hr = Flush() ;
            if(!IsSuccess(hr))
            {
                return hr ;
            }
        }
    }
    if (processedSize != NULL)
    {
        *processedSize = realProcessedSize;
    }
    return RC_S_OK;
}

void RCLzxX86ConvertOutStream::Init(bool translationMode, uint32_t translationSize)
{
    m_translationMode = translationMode;
    m_translationSize = translationSize;
    m_processedSize = 0;
    m_pos = 0;
}

HResult RCLzxX86ConvertOutStream::Flush()
{
    if (m_pos == 0)
    {
        return RC_S_OK;
    }
    if (m_translationMode)
    {
        MakeTranslation();
    }
    uint32_t pos = 0;
    do
    {
        uint32_t processed;
        HResult hr = m_stream->Write(m_buffer + pos, m_pos - pos, &processed) ;
        if(!IsSuccess(hr))
        {
            return hr ;
        }
        if (processed == 0)
        {
            return RC_E_FAIL;
        }
        pos += processed;
    }while(pos < m_pos) ;
    m_processedSize += m_pos;
    m_pos = 0;
    m_translationMode = (m_translationMode && (m_processedSize < (1 << 30)));
    return RC_S_OK;
}

END_NAMESPACE_RCZIP
