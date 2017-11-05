/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "format/common/RCOutStreamWithSha1.h"
#include "crypto/hash/RCSha1Context.h"

/////////////////////////////////////////////////////////////////
//RCOutStreamWithSha1 class implementation

BEGIN_NAMESPACE_RCZIP

RCOutStreamWithSha1::RCOutStreamWithSha1():
    m_size(0),
    m_sha( *(new RCSha1Context)),
    m_calculate(false)
{
}

RCOutStreamWithSha1::~RCOutStreamWithSha1()
{
    delete &m_sha ;
}

void RCOutStreamWithSha1::SetStream(ISequentialOutStream *stream)
{
    m_spStream = stream ;
}

void RCOutStreamWithSha1::ReleaseStream()
{
    m_spStream.Release() ;
}

void RCOutStreamWithSha1::Init(bool calculate)
{
    m_size = 0 ;
    m_calculate = calculate ;
    m_sha.Init() ;
}

void RCOutStreamWithSha1::InitSha1()
{
    m_sha.Init() ;
}

uint64_t RCOutStreamWithSha1::GetSize() const
{
    return m_size ;
}

void RCOutStreamWithSha1::Final(byte_t *digest)
{
    m_sha.Final(digest) ;
}

HResult RCOutStreamWithSha1::Write(const void* data, uint32_t size, uint32_t* processedSize)
{
    HResult result = RC_S_OK ;
    if (m_spStream)
    {
        result = m_spStream->Write(data, size, &size) ;
    }

    if (m_calculate)
    {
        m_sha.Update((const byte_t *)data, size);
    }

    m_size += size ;
    if (processedSize)
    {
        *processedSize = size ;
    }
    return result;
}

END_NAMESPACE_RCZIP
