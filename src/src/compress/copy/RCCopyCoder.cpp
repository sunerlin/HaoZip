/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "compress/copy/RCCopyCoder.h"
#include "common/RCAlloc.h"
#include "filesystem/RCStreamUtils.h"

/////////////////////////////////////////////////////////////////
//RCCopyCoder class implementation

BEGIN_NAMESPACE_RCZIP

#define BUFFER_SIZE (1024 * 32)

RCCopyCoder::RCCopyCoder():
    m_buffer(NULL),
    m_totalSize(0)
{
}

RCCopyCoder::~RCCopyCoder()
{
    RCAlloc::Instance().MidFree(m_buffer) ;
}

HResult RCCopyCoder::Code(ISequentialInStream* inStream,
                          ISequentialOutStream* outStream, 
                          const uint64_t* inSize, 
                          const uint64_t* outSize,
                          ICompressProgressInfo* progress)
{
    RCZIP_ASSERT(inStream != NULL) ;
   // RCZIP_ASSERT(outStream != NULL) ;
    if (m_buffer == NULL)
    {
        m_buffer = (byte_t *)RCAlloc::Instance().MidAlloc(BUFFER_SIZE);
        if (m_buffer == NULL)
        {
            return RC_E_OUTOFMEMORY ; 
        }
    }
    m_totalSize = 0 ;
    for (;;)
    {
        uint32_t size = BUFFER_SIZE ;
        if (outSize != 0)
        {
            if (size > *outSize - m_totalSize)
            {
                size = (uint32_t)(*outSize - m_totalSize);
            }
        }
        HResult hr = inStream->Read(m_buffer, size, &size) ;
        if(hr != RC_S_OK)
        {
            return hr ;
        }
        if (size == 0)
        {
            break;
        }
        if(outStream != NULL)
        {
            hr = RCStreamUtils::WriteStream(outStream, m_buffer, size) ;
            if(!IsSuccess(hr))
            {
                return hr ;
            }
        }
        m_totalSize += size ;
        if (progress != NULL)
        {
            hr = progress->SetRatioInfo(m_totalSize, m_totalSize) ;
            if(!IsSuccess(hr))
            {
                return hr ;
            }            
        }
    }
    return RC_S_OK ;
}

HResult RCCopyCoder::GetInStreamProcessedSize(uint64_t& size)
{
    size = m_totalSize;
    return RC_S_OK ;
}

uint64_t RCCopyCoder::GetTotalSize(void) const
{
    return m_totalSize ;
}

END_NAMESPACE_RCZIP
