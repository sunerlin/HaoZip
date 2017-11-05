/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#include "format/cab/RCCabBlockInStream.h"
#include "format/cab/RCCabTempCabInBuffer2.h"
#include "format/cab/RCCabCheckSum2.h"
#include "filesystem/RCStreamUtils.h"
#include "common/RCAlloc.h"

BEGIN_NAMESPACE_RCZIP

RCCabBlockInStream::RCCabBlockInStream() :
    m_buffer(0), 
    m_reservedSize(0), 
    m_msZip(false), 
    m_dataError(false),
    m_totalPackSize(0)
{
}

bool RCCabBlockInStream::Create()
{
    if (!m_buffer)
    {
        m_buffer = static_cast<byte_t*>(RCAlloc::Instance().MyAlloc(RCCabBlockInStream::s_blockSize));
    }
    return (m_buffer != 0);
}

void RCCabBlockInStream::SetStream(ISequentialInStream *stream) 
{  
    m_stream = stream; 
}

void RCCabBlockInStream::InitForNewFolder() 
{ 
    m_totalPackSize = 0; 
}

void RCCabBlockInStream::InitForNewBlock() 
{ 
    m_size = 0; 
}

RCCabBlockInStream::~RCCabBlockInStream()
{
    RCAlloc::Instance().MyFree(m_buffer);
}

HResult RCCabBlockInStream::Read(void *data, uint32_t size, uint32_t *processedSize)
{
    if (processedSize != 0)
    {
        *processedSize = 0;
    }
    if (size == 0)
    {
        return RC_S_OK;
    }

    if (m_size != 0)
    {
        size = MyMin(m_size, size);
        memmove(data, m_buffer + m_pos, size);
        m_pos += size;
        m_size -= size;
        if (processedSize != 0)
        {
            *processedSize = size;
        }
        return RC_S_OK;
    }
    return RC_S_OK;
}

HResult RCCabBlockInStream::PreRead(uint32_t& packSize, uint32_t& unpackSize)
{
    RCCabTempCabInBuffer2 inBuffer;
    inBuffer.m_pos = 0;
    HResult result;

    result = RCStreamUtils::ReadStream_FALSE(m_stream.Get(), inBuffer.m_buffer, RCCabTempCabInBuffer2::s_dataBlockHeaderSize);
    if (!IsSuccess(result))
    {
        return result;
    }

    uint32_t checkSum = inBuffer.ReadUInt32();
    packSize = inBuffer.ReadUInt16();
    unpackSize = inBuffer.ReadUInt16();
    if (m_reservedSize != 0)
    {
        result = RCStreamUtils::ReadStream_FALSE(m_stream.Get(), m_buffer, m_reservedSize);
        if (!IsSuccess(result))
        {
            return result;
        }
    }

    m_pos = 0;
    RCCabCheckSum2 checkSumCalc;
    checkSumCalc.Init();
    uint32_t packSize2 = packSize;
    if (m_msZip && m_size == 0)
    {
        if (packSize < 2)
        {
            return RC_S_FALSE; // bad block;
        }
        byte_t sig[2];
        result = RCStreamUtils::ReadStream_FALSE(m_stream.Get(), sig, 2);
        if (!IsSuccess(result))
        {
            return result;
        }

        if (sig[0] != 0x43 || sig[1] != 0x4B)
        {
            return RC_S_FALSE;
        }
        packSize2 -= 2;
        checkSumCalc.Update(sig, 2);
    }

    if (RCCabBlockInStream::s_blockSize - m_size < packSize2)
    {
        return RC_S_FALSE;
    }

    uint32_t curSize = packSize2;
    if (curSize != 0)
    {
        size_t processedSizeLoc = curSize;
        result = RCStreamUtils::ReadStream(m_stream.Get(), m_buffer + m_size, &processedSizeLoc);
        if (!IsSuccess(result))
        {
            return result;
        }

        checkSumCalc.Update(m_buffer + m_size, static_cast<uint32_t>(processedSizeLoc));
        m_size += static_cast<uint32_t>(processedSizeLoc);
        if (processedSizeLoc != curSize)
        {
            return RC_S_FALSE;
        }
    }
    m_totalPackSize = m_size;
    checkSumCalc.FinishDataUpdate();

    bool dataError;
    if (checkSum == 0)
    {
        dataError = false;
    }
    else
    {
        checkSumCalc.UpdateUInt32(packSize | (((uint32_t)unpackSize) << 16));
        dataError = (checkSumCalc.GetResult() != checkSum);
    }
    m_dataError |= dataError;
    return dataError ? RC_S_FALSE : RC_S_OK;
}

END_NAMESPACE_RCZIP
