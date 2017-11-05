/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#include "compress/wavpack/RCWavPackCoderInfo.h"
#include "compress/wavpack/RCWavPackDecoder.h"
#include "interface/RCMethodDefs.h"

BEGIN_NAMESPACE_RCZIP

static void* WavPackCoderCreateEncoderFunc(void)
{
    return NULL;
}

static void* WavPackCoderCreateDecoderFunc(void)
{
    return (void*)(ICompressCoder*)(new RCWavPackDecoder);
}

RCWavPackCoderInfo::RCWavPackCoderInfo()
{
}

RCWavPackCoderInfo::~RCWavPackCoderInfo()
{
}

PFNCreateCodec RCWavPackCoderInfo::GetCreateDecoderFunc(void) const 
{
    return WavPackCoderCreateDecoderFunc;
}
    
RC_IID RCWavPackCoderInfo::GetDecoderIID(void) const 
{
    return IID_ICompressCoder;
}
    
PFNCreateCodec RCWavPackCoderInfo::GetCreateEncoderFunc(void) const 
{
    return WavPackCoderCreateEncoderFunc;
}
    
RC_IID RCWavPackCoderInfo::GetEncoderIID(void) const 
{
    return IID_ICompressCoder;
}
    
RCMethodID RCWavPackCoderInfo::GetMethodID(void) const 
{
    return RCMethod::ID_COMPRESS_WAVPACK;
}
    
RCString RCWavPackCoderInfo::GetMethodName(void) const 
{
    return RC_METHOD_NAME_COMPRESS_WAVPACK;
}
    
uint32_t RCWavPackCoderInfo::GetNumInStreams(void) const 
{
    return 1u;
}
    
uint32_t RCWavPackCoderInfo::GetNumOutStreams(void) const 
{
    return 1u;
}
    
bool RCWavPackCoderInfo::IsEncoderAssigned(void) const 
{
    return false;
}
    
bool RCWavPackCoderInfo::IsDecoderAssigned(void) const 
{
    return true;
}
    
bool RCWavPackCoderInfo::IsSimpleCodec(void) const 
{
    return true;
}
    
bool RCWavPackCoderInfo::IsFilter(void) const 
{
    return false;
}

END_NAMESPACE_RCZIP
