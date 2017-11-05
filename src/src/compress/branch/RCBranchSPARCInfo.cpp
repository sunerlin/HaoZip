/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "compress/branch/RCBranchSPARCInfo.h"
#include "compress/branch/RCBranchSPARCEncoder.h"
#include "compress/branch/RCBranchSPARCDecoder.h"
#include "interface/RCMethodDefs.h"

/////////////////////////////////////////////////////////////////
//RCBranchSPARCInfo class implementation

BEGIN_NAMESPACE_RCZIP

void* BranchSPARCCreateEncoderFunc(void)
{
#ifndef RC_STATIC_SFX
    return (void*)(ICompressFilter*)(new RCBranchSPARCEncoder);
#else
    return NULL;
#endif
}

void* BranchSPARCCreateDecoderFunc(void)
{
    return (void*)(ICompressFilter*)(new RCBranchSPARCDecoder);
}

RCBranchSPARCInfo::RCBranchSPARCInfo()
{
}

RCBranchSPARCInfo::~RCBranchSPARCInfo()
{
}

PFNCreateCodec RCBranchSPARCInfo::GetCreateDecoderFunc(void) const
{
    return BranchSPARCCreateDecoderFunc ;
}

RC_IID RCBranchSPARCInfo::GetDecoderIID(void) const
{
    return IID_ICompressFilter ;
}

PFNCreateCodec RCBranchSPARCInfo::GetCreateEncoderFunc(void) const
{
    return BranchSPARCCreateEncoderFunc ;
}

RC_IID RCBranchSPARCInfo::GetEncoderIID(void) const
{
    return IID_ICompressFilter ;
}

RCMethodID RCBranchSPARCInfo::GetMethodID(void) const
{
    return RCMethod::ID_COMPRESS_BRANCH_SPARC ;
}

RCString RCBranchSPARCInfo::GetMethodName(void) const
{
    return RC_METHOD_NAME_COMPRESS_BRANCH_SPARC ;
}

uint32_t RCBranchSPARCInfo::GetNumInStreams(void) const
{
    return 1 ;
}

bool RCBranchSPARCInfo::IsFilter(void) const 
{
    return true ;
}

uint32_t RCBranchSPARCInfo::GetNumOutStreams(void) const
{
    return 1 ;
}
    
bool RCBranchSPARCInfo::IsEncoderAssigned(void) const
{
    return true ;
}
    
bool RCBranchSPARCInfo::IsDecoderAssigned(void) const
{
    return true ;
}
    
bool RCBranchSPARCInfo::IsSimpleCodec(void) const
{
    return true ;
}

END_NAMESPACE_RCZIP
