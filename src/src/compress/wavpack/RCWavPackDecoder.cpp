/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#include "compress/wavpack/RCWavPackDecoder.h"
#include "compress/wavpack/RCWavUnpack.h"

BEGIN_NAMESPACE_RCZIP

RCWavPackDecoder::RCWavPackDecoder()
{
}

RCWavPackDecoder::~RCWavPackDecoder()
{
}

HResult RCWavPackDecoder::Code(ISequentialInStream* inStream,
                               ISequentialOutStream* outStream, 
                               const uint64_t* inSize, 
                               const uint64_t* outSize,
                               ICompressProgressInfo* progress)
{
    if (inStream == NULL || outStream == NULL)
    {
        return RC_S_FALSE;
    }

    int ret = RcWavUnpackFile(inStream, outStream, progress);

    if (ret == NO_ERROR)
    {
        return RC_S_OK;
    }
    else if (ret == USER_ABORT)
    {
        return RC_E_ABORT;
    }
    else
    {
        return RC_S_FALSE;
    }
}

HResult RCWavPackDecoder::SetDecoderProperties2(const byte_t* data, uint32_t size) 
{
    return RC_S_OK;
}
    
END_NAMESPACE_RCZIP
