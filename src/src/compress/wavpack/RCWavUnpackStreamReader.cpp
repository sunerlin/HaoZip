/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#include "compress/wavpack/RCWavUnpackStreamReader.h"

BEGIN_NAMESPACE_RCZIP

/** 实现文件流操作
*/
static int32_t rc_read_bytes(void *id, void *data, int32_t bcount)
{
    RCWavPackBufferStream* pstream = static_cast<RCWavPackBufferStream*>(id);
    return pstream->ReadByte(data, bcount);
}

static uint32_t rc_get_pos(void *id)
{
    RCWavPackBufferStream* pstream = static_cast<RCWavPackBufferStream*>(id);
    return pstream->GetPos();
}

static int rc_set_pos_abs(void *id, uint32_t pos)
{
    return -1;
}

static int rc_set_pos_rel(void *id, int32_t delta, int mode)
{
    return -1;
}

static int rc_push_back_byte(void *id, int c)
{
    RCWavPackBufferStream* pstream = static_cast<RCWavPackBufferStream*>(id);
    return pstream->PushBackByte(c);
}

static uint32_t rc_get_length(void *id)
{
    return 0;
}

static int rc_can_seek(void *id)
{
    return 0;
}

static int32_t rc_write_bytes(void *id, void *data, int32_t bcount)
{
    return -1;
}

WavpackStreamReader rcWavStreamReader = {
    rc_read_bytes, rc_get_pos, rc_set_pos_abs, rc_set_pos_rel, rc_push_back_byte, rc_get_length, rc_can_seek,
    rc_write_bytes
};

RCWavPackBufferStream::RCWavPackBufferStream(ISequentialInStream* inStream) :
    m_inStream(inStream),
    m_buffer(RCWavPackBufferStream::ENUM_BUFFER_SIZE),
    m_dataPos(0),
    m_totalReadSize(0)
{
}

int32_t RCWavPackBufferStream::ReadByte(void* data, int32_t bcount)
{
    if (m_inStream == NULL)
    {
        return -1;
    }

    /** 如果rc_push_back_byte 过数据
    *   先从 缓冲中取数据
    */
    if (bcount <= (int32_t)m_dataPos)
    {
        memcpy(data, m_buffer.data(), bcount);
        RCByteBuffer temp_buffer(ENUM_BUFFER_SIZE);
        memcpy(temp_buffer.data(), m_buffer.data() + bcount, m_dataPos - bcount);
        memcpy(m_buffer.data(), temp_buffer.data(), m_dataPos - bcount);
        m_dataPos -= bcount;
        m_totalReadSize += bcount;
        return bcount;
    }

    uint32_t read_size = bcount;
    uint32_t get_size = 0;
    uint32_t start_pos = 0;
    RCByteBuffer local_buffer(bcount);
    if (m_dataPos > 0)
    {
        memcpy(local_buffer.data(), m_buffer.data(), m_dataPos);
        read_size = bcount - m_dataPos;
        start_pos = m_dataPos;
        m_dataPos = 0;
    }
    HResult ret = m_inStream->Read(local_buffer.data() + start_pos, read_size, &get_size);

    if (!IsSuccess(ret))
    {
        return -1;
    }

    /** 对于加密文件流可能出现读取字节数少于期望
    *   反复读取直到满足期望值或者到达文件结尾
    */
    while(get_size && (bcount > (int32_t)(start_pos + get_size)))
    {
        read_size = bcount - start_pos - get_size;
        start_pos += get_size;
        get_size = 0;

        ret = m_inStream->Read(local_buffer.data() + start_pos, read_size , &get_size);

        if (!IsSuccess(ret))
        {
            return -1;
        }
    }

    memcpy(data, local_buffer.data(), start_pos + get_size);
    m_totalReadSize += (start_pos + get_size);

    return start_pos + get_size;
}

uint32_t RCWavPackBufferStream::GetPos(void)
{
    return m_totalReadSize;
}

int RCWavPackBufferStream::PushBackByte(int c)
{
    byte_t val = static_cast<byte_t>(c & 0xFF);

    if (m_dataPos < (ENUM_BUFFER_SIZE - 1))
    {
        m_buffer.data()[m_dataPos++] = val;
        m_totalReadSize--;
    }  
    return c;
}

WavpackContext* RcWavpackOpenInputStream(void* inStream, char* error, int flags, int norm_offset)
{
    WavpackContext* wpc = NULL;
    wpc = WavpackOpenFileInputEx(&rcWavStreamReader, inStream , NULL, error, flags, norm_offset);
    return wpc;
}

END_NAMESPACE_RCZIP