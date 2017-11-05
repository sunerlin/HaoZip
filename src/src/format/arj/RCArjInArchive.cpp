/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#include "format/arj/RCArjInArchive.h"
#include "filesystem/RCStreamUtils.h"
#include "algorithm/CpuArch.h"
#include "algorithm/7zCrc.h"

#define Get16(p) GetUi16(p)
#define Get32(p) GetUi32(p)

BEGIN_NAMESPACE_RCZIP

HResult RCArjInArchive::GetNextItem(bool &filled, RCArjItem &item)
{
    HResult result;
    result = ReadSignatureAndBlock(filled);
    if (!IsSuccess(result))
    {
        return result;
    }
    if (!filled)
    {
        return RC_S_OK;
    }
    filled = false;
    result = item.Parse(m_block, m_blockSize);
    if (!IsSuccess(result))
    {
        return result;
    }
    result = SkipExtendedHeaders();
    if (!IsSuccess(result))
    {
        return result;
    }
    filled = true;
    return RC_S_OK;
}

HResult RCArjInArchive::SkipExtendedHeaders()
{
    HResult result;
    for (uint32_t i = 0;; i++)
    {
        bool filled;
        result = ReadBlock(filled);
        if (!IsSuccess(result))
        {
            return result;
        }
        if (!filled)
        {
            return RC_S_OK;
        }
        if (m_openArchiveCallback && (i & 0xFF) == 0)
        {
            result = m_openArchiveCallback->SetCompleted(m_numFiles, m_numBytes);
            if (!IsSuccess(result))
            {
                return result;
            }
        }
    }
}

HResult RCArjInArchive::Open(const uint64_t *searchHeaderSizeLimit)
{
    HResult result;
    uint64_t position = 0;
    result = RCArjDefs::FindAndReadMarker(m_stream, searchHeaderSizeLimit, position);
    if (!IsSuccess(result))
    {
        return result;
    }
    result = m_stream->Seek(position, RC_STREAM_SEEK_SET, NULL);
    if (!IsSuccess(result))
    {
        return result;
    }
    bool filled;
    result = ReadSignatureAndBlock(filled);
    if (!IsSuccess(result))
    {
        return result;
    }
    if (!filled)
    {
        return RC_S_FALSE;
    }
    result = m_header.Parse(m_block, m_blockSize);
    if (!IsSuccess(result))
    {
        return result;
    }
    return SkipExtendedHeaders();
}

HResult RCArjInArchive::ReadBlock(bool &filled)
{
    HResult result;
    filled = false;
    byte_t buf[2];
    result = SafeReadBytes(buf, 2);
    if (!IsSuccess(result))
    {
        return result;
    }
    m_blockSize = Get16(buf);
    if (m_blockSize == 0)
    {
        return RC_S_OK;
    }
    if (m_blockSize > RCArjDefs::kBlockSizeMax)
    {
        _ThrowCode(RC_E_IncorrectArchive) ;
    }
    result = SafeReadBytes(m_block, m_blockSize + 4);
    if (!IsSuccess(result))
    {
        return result;
    }
    m_numBytes += m_blockSize + 6;
    if (Get32(m_block + m_blockSize) != CrcCalc(m_block, m_blockSize))
    {
        _ThrowCode(RC_E_DataCRCError) ;
    }
    filled = true;
    return RC_S_OK;
}

HResult RCArjInArchive::ReadSignatureAndBlock(bool &filled)
{
    HResult result;
    byte_t id[2];
    result = SafeReadBytes(id, 2);
    if (!IsSuccess(result))
    {
        return result;
    }
    if (id[0] != RCArjDefs::kSig0 || id[1] != RCArjDefs::kSig1)
    {
        _ThrowCode(RC_E_IncorrectArchive) ;
    }
    return ReadBlock(filled);
}

HResult RCArjInArchive::SafeReadBytes(void *data, uint32_t size)
{
    HResult result;
    size_t processed = size;
    result = RCStreamUtils::ReadStream(m_stream, data, &processed);
    if (!IsSuccess(result))
    {
        return result;
    }
    if (processed != size)
    {
        _ThrowCode(RC_E_UnexpectedEndOfArchive) ;
    }
    return RC_S_OK;
}

END_NAMESPACE_RCZIP
