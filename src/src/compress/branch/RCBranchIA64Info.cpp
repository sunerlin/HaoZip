/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "compress/branch/RCBranchIA64Info.h"
#include "compress/branch/RCBranchIA64Encoder.h"
#include "compress/branch/RCBranchIA64Decoder.h"
#include "interface/RCMethodDefs.h"

/////////////////////////////////////////////////////////////////
//RCBranchIA64Info class implementation

BEGIN_NAMESPACE_RCZIP

void* BranchIA64CreateEncoderFunc(void)
{
#ifndef RC_STATIC_SFX
    return (void*)(ICompressFilter*)(new RCBranchIA64Encoder);
#else
    return NULL;
#endif
}

void* BranchIA64CreateDecoderFunc(void)
{
    return (void*)(ICompressFilter*)(new RCBranchIA64Decoder);
}

RCBranchIA64Info::RCBranchIA64Info()
{
}

RCBranchIA64Info::~RCBranchIA64Info()
{
}

PFNCreateCodec RCBranchIA64Info::GetCreateDecoderFunc(void) const
{
    return BranchIA64CreateDecoderFunc ;
}

RC_IID RCBranchIA64Info::GetDecoderIID(void) const
{
    return IID_ICompressFilter ;
}

PFNCreateCodec RCBranchIA64Info::GetCreateEncoderFunc(void) const
{
    return BranchIA64CreateEncoderFunc ;
}

RC_IID RCBranchIA64Info::GetEncoderIID(void) const
{
    return IID_ICompressFilter ;
}

RCMethodID RCBranchIA64Info::GetMethodID(void) const
{
    return RCMethod::ID_COMPRESS_BRANCH_IA64 ;
}

RCString RCBranchIA64Info::GetMethodName(void) const
{
    return RC_METHOD_NAME_COMPRESS_BRANCH_IA64 ;
}

uint32_t RCBranchIA64Info::GetNumInStreams(void) const
{
    return 1 ;
}

bool RCBranchIA64Info::IsFilter(void) const 
{
    return true ;
}

uint32_t RCBranchIA64Info::GetNumOutStreams(void) const
{
    return 1 ;
}
    
bool RCBranchIA64Info::IsEncoderAssigned(void) const
{
    return true ;
}
    
bool RCBranchIA64Info::IsDecoderAssigned(void) const
{
    return true ;
}
    
bool RCBranchIA64Info::IsSimpleCodec(void) const
{
    return true ;
}

END_NAMESPACE_RCZIP
