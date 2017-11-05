/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "compress/deflate/RCDeflateCOMEncoder.h"

/////////////////////////////////////////////////////////////////
//RCDeflateCOMEncoder class implementation

BEGIN_NAMESPACE_RCZIP

RCDeflateCOMEncoder::RCDeflateCOMEncoder():
    RCDeflateEncoder(false)
{
}

RCDeflateCOMEncoder::~RCDeflateCOMEncoder()
{
}

HResult RCDeflateCOMEncoder::Code(ISequentialInStream* inStream,
                                  ISequentialOutStream* outStream, 
                                  const uint64_t* inSize, 
                                  const uint64_t* outSize,
                                  ICompressProgressInfo* progress)
{
    return BaseCode(inStream, outStream, inSize, outSize, progress) ;
}

HResult RCDeflateCOMEncoder::SetCoderProperties(const RCPropertyIDPairArray& propertyArray)
{
    return BaseSetEncoderProperties2(propertyArray);
}

END_NAMESPACE_RCZIP
