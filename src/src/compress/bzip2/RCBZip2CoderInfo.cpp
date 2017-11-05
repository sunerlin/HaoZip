/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "compress/bzip2/RCBZip2CoderInfo.h"
#include "compress/bzip2/RCBZip2Decoder.h"
#include "compress/bzip2/RCBZip2Encoder.h"
#include "interface/RCMethodDefs.h"

/////////////////////////////////////////////////////////////////
//RCBZip2CoderInfo class implementation

BEGIN_NAMESPACE_RCZIP

void* BZip2CoderCreateEncoderFunc(void)
{
#ifndef RC_STATIC_SFX
    return (void*)(ICompressCoder*)(new RCBZip2Encoder);
#else
    return NULL;
#endif
}

void* BZip2CoderCreateDecoderFunc(void)
{
    return (void*)(ICompressCoder*)(new RCBZip2Decoder);
}

RCBZip2CoderInfo::RCBZip2CoderInfo()
{
}

RCBZip2CoderInfo::~RCBZip2CoderInfo()
{
}

PFNCreateCodec RCBZip2CoderInfo::GetCreateDecoderFunc(void) const
{
    return BZip2CoderCreateDecoderFunc ;
}

RC_IID RCBZip2CoderInfo::GetDecoderIID(void) const
{
    return IID_ICompressCoder ;
}

PFNCreateCodec RCBZip2CoderInfo::GetCreateEncoderFunc(void) const
{
    return BZip2CoderCreateEncoderFunc ;
}

RC_IID RCBZip2CoderInfo::GetEncoderIID(void) const
{
    return IID_ICompressCoder ;
}

RCMethodID RCBZip2CoderInfo::GetMethodID(void) const
{
    return RCMethod::ID_COMPRESS_BZIP2 ;
}

RCString RCBZip2CoderInfo::GetMethodName(void) const
{
    return RC_METHOD_NAME_COMPRESS_BZIP2 ;
}

uint32_t RCBZip2CoderInfo::GetNumInStreams(void) const
{
    return 1 ;
}

bool RCBZip2CoderInfo::IsFilter(void) const 
{
    return false ;
}

uint32_t RCBZip2CoderInfo::GetNumOutStreams(void) const
{
    return 1 ;
}
    
bool RCBZip2CoderInfo::IsEncoderAssigned(void) const
{
    return true ;
}
    
bool RCBZip2CoderInfo::IsDecoderAssigned(void) const
{
    return true ;
}
    
bool RCBZip2CoderInfo::IsSimpleCodec(void) const
{
    return true ;
}

END_NAMESPACE_RCZIP
