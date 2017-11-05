/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "compress/branch/RCBranchARMTInfo.h"
#include "compress/branch/RCBranchARMTEncoder.h"
#include "compress/branch/RCBranchARMTDecoder.h"
#include "interface/RCMethodDefs.h"

/////////////////////////////////////////////////////////////////
//RCBranchARMTInfo class implementation

BEGIN_NAMESPACE_RCZIP

void* BranchARMTCreateEncoderFunc(void)
{
#ifndef RC_STATIC_SFX
    return (void*)(ICompressFilter*)(new RCBranchARMTEncoder);
#else
    return NULL;
#endif 
}

void* BranchARMTCreateDecoderFunc(void)
{
    return (void*)(ICompressFilter*)(new RCBranchARMTDecoder);
}

RCBranchARMTInfo::RCBranchARMTInfo()
{
}

RCBranchARMTInfo::~RCBranchARMTInfo()
{
}

PFNCreateCodec RCBranchARMTInfo::GetCreateDecoderFunc(void) const
{
    return BranchARMTCreateDecoderFunc ;
}

RC_IID RCBranchARMTInfo::GetDecoderIID(void) const
{
    return IID_ICompressFilter ;
}

PFNCreateCodec RCBranchARMTInfo::GetCreateEncoderFunc(void) const
{
    return BranchARMTCreateEncoderFunc ;
}

RC_IID RCBranchARMTInfo::GetEncoderIID(void) const
{
    return IID_ICompressFilter ;
}

RCMethodID RCBranchARMTInfo::GetMethodID(void) const
{
    return RCMethod::ID_COMPRESS_BRANCH_ARMT ;
}

RCString RCBranchARMTInfo::GetMethodName(void) const
{
    return RC_METHOD_NAME_COMPRESS_BRANCH_ARMT ;
}

uint32_t RCBranchARMTInfo::GetNumInStreams(void) const
{
    return 1 ;
}

bool RCBranchARMTInfo::IsFilter(void) const 
{
    return true ;
}

uint32_t RCBranchARMTInfo::GetNumOutStreams(void) const
{
    return 1 ;
}
    
bool RCBranchARMTInfo::IsEncoderAssigned(void) const
{
    return true ;
}
    
bool RCBranchARMTInfo::IsDecoderAssigned(void) const
{
    return true ;
}
    
bool RCBranchARMTInfo::IsSimpleCodec(void) const
{
    return true ;
}

END_NAMESPACE_RCZIP
