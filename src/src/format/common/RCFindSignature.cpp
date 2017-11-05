/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "format/common/RCFindSignature.h"
#include "common/RCBuffer.h"
#include "filesystem/RCStreamUtils.h"

/////////////////////////////////////////////////////////////////
//RCFindSignature class implementation

BEGIN_NAMESPACE_RCZIP

HResult RCFindSignature::FindSignatureInStream(ISequentialInStream* stream,
                                               const byte_t* signature, 
                                               uint32_t signatureSize,
                                               const uint64_t* limit, 
                                               uint64_t& resPos)
{
    if( (signature == NULL) || (signatureSize == 0) )
    {
        return RC_E_INVALIDARG ;
    }

    resPos = 0;
    RCByteBuffer byteBuffer2 ;
    byteBuffer2.SetCapacity(signatureSize) ;
    HResult hr = RCStreamUtils::ReadStream_FALSE(stream, byteBuffer2.data(), signatureSize) ;
    if (!IsSuccess(hr))
    {
        return hr ;
    }
    if (memcmp(byteBuffer2.data(), signature, signatureSize) == 0)
    {
        return RC_S_OK;
    }

    const uint32_t kBufferSize = (1 << 16) ;
    RCByteBuffer byteBuffer;
    byteBuffer.SetCapacity(kBufferSize) ;
    byte_t* buffer = byteBuffer.data() ;
    uint32_t numPrevBytes = signatureSize - 1;
    memcpy(buffer, (const byte_t*)byteBuffer2.data() + 1, numPrevBytes) ;
    resPos = 1;

    for (;;)
    {
        if (limit != NULL)
        {
            if (resPos > *limit)
            {
                return RC_S_FALSE;
            }
        }

        do
        {
            uint32_t numReadBytes = kBufferSize - numPrevBytes;
            uint32_t processedSize;

            HResult hr = stream->Read(buffer + numPrevBytes, numReadBytes, &processedSize) ;
            if (!IsSuccess(hr))
            {
                return hr ;
            }

            numPrevBytes += processedSize;
            if (processedSize == 0)
            {
                return RC_S_FALSE;
            }
        } while(numPrevBytes < signatureSize) ;

        uint32_t numTests = numPrevBytes - signatureSize + 1;

        for (uint32_t pos = 0; pos < numTests; pos++)
        {
            byte_t b = signature[0] ;
            for (; buffer[pos] != b && pos < numTests; pos++)
            {
            }

            if (pos == numTests)
            {
                break;
            }

            if (memcmp(buffer + pos, signature, signatureSize) == 0)
            {
                resPos += pos;
                return RC_S_OK;
            }
        }

        resPos += numTests;
        numPrevBytes -= numTests;
        memmove(buffer, buffer + numTests, numPrevBytes);
    }
}

END_NAMESPACE_RCZIP
