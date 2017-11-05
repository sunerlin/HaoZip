/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "compress/branch/RCBranchBCJInfo.h"
#ifndef RC_STATIC_SFX
#include "compress/branch/RCBranchBCJx86Encoder.h"
#endif
#include "compress/branch/RCBranchBCJx86Decoder.h"
#include "interface/RCMethodDefs.h"

/////////////////////////////////////////////////////////////////
//RCBranchBCJInfo class implementation

BEGIN_NAMESPACE_RCZIP

void* BranchBCJCreateEncoderFunc(void)
{
#ifndef RC_STATIC_SFX
    return (void*)(ICompressFilter*)(new RCBranchBCJx86Encoder);
#else
    return 0;
#endif
}

void* BranchBCJCreateDecoderFunc(void)
{
    return (void*)(ICompressFilter*)(new RCBranchBCJx86Decoder);
}

RCBranchBCJInfo::RCBranchBCJInfo()
{
}

RCBranchBCJInfo::~RCBranchBCJInfo()
{
}

PFNCreateCodec RCBranchBCJInfo::GetCreateDecoderFunc(void) const
{
    return BranchBCJCreateDecoderFunc ;
}

RC_IID RCBranchBCJInfo::GetDecoderIID(void) const
{
    return IID_ICompressFilter ;
}

PFNCreateCodec RCBranchBCJInfo::GetCreateEncoderFunc(void) const
{
    return BranchBCJCreateEncoderFunc ;
}

RC_IID RCBranchBCJInfo::GetEncoderIID(void) const
{
    return IID_ICompressFilter ;
}

RCMethodID RCBranchBCJInfo::GetMethodID(void) const
{
    return RCMethod::ID_COMPRESS_BRANCH_BCJ ;
}

RCString RCBranchBCJInfo::GetMethodName(void) const
{
    return RC_METHOD_NAME_COMPRESS_BRANCH_BCJ ;
}

uint32_t RCBranchBCJInfo::GetNumInStreams(void) const
{
    return 1 ;
}

bool RCBranchBCJInfo::IsFilter(void) const 
{
    return true ;
}

uint32_t RCBranchBCJInfo::GetNumOutStreams(void) const
{
    return 1 ;
}
    
bool RCBranchBCJInfo::IsEncoderAssigned(void) const
{
    return true ;
}
    
bool RCBranchBCJInfo::IsDecoderAssigned(void) const
{
    return true ;
}
    
bool RCBranchBCJInfo::IsSimpleCodec(void) const
{
    return true ;
}

END_NAMESPACE_RCZIP
