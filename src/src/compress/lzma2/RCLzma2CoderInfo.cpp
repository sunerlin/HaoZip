/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "compress/lzma2/RCLzma2CoderInfo.h"
#include "compress/lzma2/RCLzma2Decoder.h"
#ifndef RC_STATIC_SFX
#include "compress/lzma2/RCLzma2Encoder.h"
#endif
#include "interface/ICoder.h"
#include "interface/RCMethodDefs.h"

/////////////////////////////////////////////////////////////////
//RCLzma2CoderInfo class implementation

BEGIN_NAMESPACE_RCZIP

void* Lzma2CoderCreateEncoderFunc(void)
{
#ifndef RC_STATIC_SFX
    return (void*)(ICompressCoder*)(new RCLzma2Encoder);
#else
    return 0;
#endif
}

void* Lzma2CoderCreateDecoderFunc(void)
{
    return (void*)(ICompressCoder*)(new RCLzma2Decoder);
}

RCLzma2CoderInfo::RCLzma2CoderInfo()
{
}

RCLzma2CoderInfo::~RCLzma2CoderInfo()
{
}

PFNCreateCodec RCLzma2CoderInfo::GetCreateDecoderFunc(void) const
{
    return Lzma2CoderCreateDecoderFunc ;
}

RC_IID RCLzma2CoderInfo::GetDecoderIID(void) const
{
    return IID_ICompressCoder ;
}

PFNCreateCodec RCLzma2CoderInfo::GetCreateEncoderFunc(void) const
{
    return Lzma2CoderCreateEncoderFunc ;
}

RC_IID RCLzma2CoderInfo::GetEncoderIID(void) const
{
    return IID_ICompressCoder ;
}

RCMethodID RCLzma2CoderInfo::GetMethodID(void) const
{
    return RCMethod::ID_COMPRESS_LZMA2 ;
}

RCString RCLzma2CoderInfo::GetMethodName(void) const
{
    return RC_METHOD_NAME_COMPRESS_LZMA2 ;
}

uint32_t RCLzma2CoderInfo::GetNumInStreams(void) const
{
    return 1 ;
}

bool RCLzma2CoderInfo::IsFilter(void) const 
{
    return false ;
}

uint32_t RCLzma2CoderInfo::GetNumOutStreams(void) const
{
    return 1 ;
}
    
bool RCLzma2CoderInfo::IsEncoderAssigned(void) const
{
    return true ;
}
    
bool RCLzma2CoderInfo::IsDecoderAssigned(void) const
{
    return true ;
}
    
bool RCLzma2CoderInfo::IsSimpleCodec(void) const
{
    return true ;
}

END_NAMESPACE_RCZIP
