/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "compress/branch/RCBranchPPCInfo.h"
#include "compress/branch/RCBranchPPCEncoder.h"
#include "compress/branch/RCBranchPPCDecoder.h"
#include "interface/RCMethodDefs.h"

/////////////////////////////////////////////////////////////////
//RCBranchPPCInfo class implementation

BEGIN_NAMESPACE_RCZIP

void* BranchPPCCreateEncoderFunc(void)
{
#ifndef RC_STATIC_SFX
    return (void*)(ICompressFilter*)(new RCBranchPPCEncoder);
#else
    return NULL;
#endif
}

void* BranchPPCCreateDecoderFunc(void)
{
    return (void*)(ICompressFilter*)(new RCBranchPPCDecoder);
}

RCBranchPPCInfo::RCBranchPPCInfo()
{
}

RCBranchPPCInfo::~RCBranchPPCInfo()
{
}

PFNCreateCodec RCBranchPPCInfo::GetCreateDecoderFunc(void) const
{
    return BranchPPCCreateDecoderFunc ;
}

RC_IID RCBranchPPCInfo::GetDecoderIID(void) const
{
    return IID_ICompressFilter ;
}

PFNCreateCodec RCBranchPPCInfo::GetCreateEncoderFunc(void) const
{
    return BranchPPCCreateEncoderFunc ;
}

RC_IID RCBranchPPCInfo::GetEncoderIID(void) const
{
    return IID_ICompressFilter ;
}

RCMethodID RCBranchPPCInfo::GetMethodID(void) const
{
    return RCMethod::ID_COMPRESS_BRANCH_PPC ;
}

RCString RCBranchPPCInfo::GetMethodName(void) const
{
    return RC_METHOD_NAME_COMPRESS_BRANCH_PPC ;
}

uint32_t RCBranchPPCInfo::GetNumInStreams(void) const
{
    return 1 ;
}

bool RCBranchPPCInfo::IsFilter(void) const 
{
    return true ;
}

uint32_t RCBranchPPCInfo::GetNumOutStreams(void) const
{
    return 1 ;
}
    
bool RCBranchPPCInfo::IsEncoderAssigned(void) const
{
    return true ;
}
    
bool RCBranchPPCInfo::IsDecoderAssigned(void) const
{
    return true ;
}
    
bool RCBranchPPCInfo::IsSimpleCodec(void) const
{
    return true ;
}

END_NAMESPACE_RCZIP
