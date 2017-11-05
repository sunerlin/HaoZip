/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "compress/deflate/RCZlibOutStreamWithAdler.h"

/////////////////////////////////////////////////////////////////
//RCZlibOutStreamWithAdler class implementation

BEGIN_NAMESPACE_RCZIP

#define ADLER_INIT_VAL  1
#define ADLER_MOD       65521
#define ADLER_LOOP_MAX  5550

RCZlibOutStreamWithAdler::RCZlibOutStreamWithAdler()
{
}

RCZlibOutStreamWithAdler::~RCZlibOutStreamWithAdler()
{
}

void RCZlibOutStreamWithAdler::SetStream(ISequentialOutStream *stream)
{
    m_stream = stream;
}

void RCZlibOutStreamWithAdler::ReleaseStream()
{
    m_stream.Release();
}

void RCZlibOutStreamWithAdler::Init()
{
    m_adler = ADLER_INIT_VAL;
}

uint32_t RCZlibOutStreamWithAdler::GetAdler() const
{
    return m_adler;
}

HResult RCZlibOutStreamWithAdler::Write(const void* data, uint32_t size, uint32_t* processedSize)
{
    HResult result = m_stream->Write(data, size, &size);
    m_adler = Adler32Update(m_adler, (const byte_t *)data, size);
    if (processedSize != NULL)
    {
        *processedSize = size;
    }
    return result;
}

uint32_t RCZlibOutStreamWithAdler::Adler32Update(uint32_t adler, const byte_t *buf, size_t size)
{
    uint32_t a = adler & 0xFFFF;
    uint32_t b = (adler >> 16) & 0xFFFF;
    while (size > 0)
    {
        uint32_t curSize = (size > ADLER_LOOP_MAX) ? ADLER_LOOP_MAX : (unsigned )size;
        uint32_t i = 0 ;
        for (i = 0; i < curSize; i++)
        {
            a += buf[i];
            b += a;
        }
        buf += curSize;
        size -= curSize;
        a %= ADLER_MOD;
        b %= ADLER_MOD;
    }
    return (b << 16) + a;
}

END_NAMESPACE_RCZIP
