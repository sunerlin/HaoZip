/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#include "compress/ppmdi1/RCPPMDI1Decoder.h"
#include "RCInOutStream.h"
#include "PPMdType.h"
#include "PPMd.h"
//#include "SubAlloc.hpp"

BEGIN_NAMESPACE_RCZIP

RCPPMDI1Decoder::RCPPMDI1Decoder()
{
}

RCPPMDI1Decoder::~RCPPMDI1Decoder()
{
    StopSubAllocator();
}

HResult RCPPMDI1Decoder::Code(ISequentialInStream* inStream,
                             ISequentialOutStream* outStream, 
                             const uint64_t* inSize, 
                             const uint64_t* outSize,
                             ICompressProgressInfo* progress)
{
    RCInOutStream iofile(inStream, outStream, progress);
    if (!StartSubAllocator(m_memorySize))
    {
        return RC_E_OUTOFMEMORY;
    }
    MR_METHOD rmethod = (MR_METHOD)(m_restoreMethod);
    if (DecodeFile(&iofile, &iofile, m_order, rmethod) == 0)
    {
        return RC_S_OK;
    }
    else
    {
        return RC_E_ABORT;
    }
}
    
HResult RCPPMDI1Decoder::SetDecoderProperties2(const byte_t* data, uint32_t size)
{
    if (size < 4)
    {
        return RC_E_NOTIMPL;
    }
    m_order = data[0];
    m_memorySize = (uint32_t)(data[2] << 8) + data[1];
    m_restoreMethod = data[3];

    return RC_S_OK;
}

END_NAMESPACE_RCZIP
