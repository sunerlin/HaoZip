/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "compress/delta/RCDeltaCoderInfo.h"
#include "compress/delta/RCDeltaEncoder.h"
#include "compress/delta/RCDeltaDecoder.h"
#include "interface/ICoder.h"
#include "interface/RCMethodDefs.h"

/////////////////////////////////////////////////////////////////
//RCDeltaCoderInfo class implementation

BEGIN_NAMESPACE_RCZIP

void* DeltaCreateEncoderFunc(void)
{
#ifndef RC_STATIC_SFX
    return (void*)(ICompressFilter*)(new RCDeltaEncoder);
#else
    return 0;
#endif
}

void* DeltaCreateDecoderFunc(void)
{
    return (void*)(ICompressFilter*)(new RCDeltaDecoder);
}

RCDeltaCoderInfo::RCDeltaCoderInfo()
{
}

RCDeltaCoderInfo::~RCDeltaCoderInfo()
{
}

PFNCreateCodec RCDeltaCoderInfo::GetCreateDecoderFunc(void) const
{
    return DeltaCreateDecoderFunc ;
}

RC_IID RCDeltaCoderInfo::GetDecoderIID(void) const
{
    return IID_ICompressFilter ;
}

PFNCreateCodec RCDeltaCoderInfo::GetCreateEncoderFunc(void) const
{
    return DeltaCreateEncoderFunc ;
}

RC_IID RCDeltaCoderInfo::GetEncoderIID(void) const
{
    return IID_ICompressFilter ;
}

RCMethodID RCDeltaCoderInfo::GetMethodID(void) const
{
    return RCMethod::ID_COMPRESS_DELTA ;
}

RCString RCDeltaCoderInfo::GetMethodName(void) const
{
    return RC_METHOD_NAME_COMPRESS_DELTA ;
}

uint32_t RCDeltaCoderInfo::GetNumInStreams(void) const
{
    return 1 ;
}

bool RCDeltaCoderInfo::IsFilter(void) const 
{
    return true ;
}

uint32_t RCDeltaCoderInfo::GetNumOutStreams(void) const
{
    return 1 ;
}
    
bool RCDeltaCoderInfo::IsEncoderAssigned(void) const
{
    return true ;
}
    
bool RCDeltaCoderInfo::IsDecoderAssigned(void) const
{
    return true ;
}
    
bool RCDeltaCoderInfo::IsSimpleCodec(void) const
{
    return true ;
}

END_NAMESPACE_RCZIP
