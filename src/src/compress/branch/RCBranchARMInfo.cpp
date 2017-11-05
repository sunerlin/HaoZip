/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "compress/branch/RCBranchARMInfo.h"
#include "compress/branch/RCBranchARMEncoder.h"
#include "compress/branch/RCBranchARMDecoder.h"
#include "interface/RCMethodDefs.h"

/////////////////////////////////////////////////////////////////
//RCBranchARMInfo class implementation

BEGIN_NAMESPACE_RCZIP

void* BranchARMCreateEncoderFunc(void)
{
#ifndef RC_STATIC_SFX
    return (void*)(ICompressFilter*)(new RCBranchARMEncoder);
#else
    return NULL;
#endif
}

void* BranchARMCreateDecoderFunc(void)
{
    return (void*)(ICompressFilter*)(new RCBranchARMDecoder);
}

RCBranchARMInfo::RCBranchARMInfo()
{
}

RCBranchARMInfo::~RCBranchARMInfo()
{
}

PFNCreateCodec RCBranchARMInfo::GetCreateDecoderFunc(void) const
{
    return BranchARMCreateDecoderFunc ;
}

RC_IID RCBranchARMInfo::GetDecoderIID(void) const
{
    return IID_ICompressFilter ;
}

PFNCreateCodec RCBranchARMInfo::GetCreateEncoderFunc(void) const
{
    return BranchARMCreateEncoderFunc ;
}

RC_IID RCBranchARMInfo::GetEncoderIID(void) const
{
    return IID_ICompressFilter ;
}

RCMethodID RCBranchARMInfo::GetMethodID(void) const
{
    return RCMethod::ID_COMPRESS_BRANCH_ARM ;
}

RCString RCBranchARMInfo::GetMethodName(void) const
{
    return RC_METHOD_NAME_COMPRESS_BRANCH_ARM ;
}

uint32_t RCBranchARMInfo::GetNumInStreams(void) const
{
    return 1 ;
}

bool RCBranchARMInfo::IsFilter(void) const 
{
    return true ;
}

uint32_t RCBranchARMInfo::GetNumOutStreams(void) const
{
    return 1 ;
}
    
bool RCBranchARMInfo::IsEncoderAssigned(void) const
{
    return true ;
}
    
bool RCBranchARMInfo::IsDecoderAssigned(void) const
{
    return true ;
}
    
bool RCBranchARMInfo::IsSimpleCodec(void) const
{
    return true ;
}

END_NAMESPACE_RCZIP
