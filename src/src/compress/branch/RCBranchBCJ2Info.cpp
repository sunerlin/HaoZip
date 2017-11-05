/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "compress/branch/RCBranchBCJ2Info.h"
#ifndef RC_STATIC_SFX
#include "compress/branch/RCBranchBCJ2Encoder.h"
#endif
#include "compress/branch/RCBranchBCJ2Decoder.h"
#include "interface/RCMethodDefs.h"

/////////////////////////////////////////////////////////////////
//RCBranchBCJ2Info class implementation

BEGIN_NAMESPACE_RCZIP

void* BranchBCJ2CreateEncoderFunc(void)
{
#ifndef RC_STATIC_SFX
    return (void*)(ICompressCoder2*)(new RCBranchBCJ2Encoder);
#else
    return 0;
#endif
}

void* BranchBCJ2CreateDecoderFunc(void)
{
    return (void*)(ICompressCoder2*)(new RCBranchBCJ2Decoder);
}

RCBranchBCJ2Info::RCBranchBCJ2Info()
{
}

RCBranchBCJ2Info::~RCBranchBCJ2Info()
{
}

PFNCreateCodec RCBranchBCJ2Info::GetCreateDecoderFunc(void) const
{
    return BranchBCJ2CreateDecoderFunc ;
}

RC_IID RCBranchBCJ2Info::GetDecoderIID(void) const
{
    return IID_ICompressCoder2 ;
}

PFNCreateCodec RCBranchBCJ2Info::GetCreateEncoderFunc(void) const
{
    return BranchBCJ2CreateEncoderFunc ;
}

RC_IID RCBranchBCJ2Info::GetEncoderIID(void) const
{
    return IID_ICompressCoder2 ;
}

RCMethodID RCBranchBCJ2Info::GetMethodID(void) const
{
    return RCMethod::ID_COMPRESS_BRANCH_BCJ_2 ;
}

RCString RCBranchBCJ2Info::GetMethodName(void) const
{
    return RC_METHOD_NAME_COMPRESS_BRANCH_BCJ_2 ;
}

uint32_t RCBranchBCJ2Info::GetNumInStreams(void) const
{
    return 4 ;
}

bool RCBranchBCJ2Info::IsFilter(void) const 
{
    return false ;
}

uint32_t RCBranchBCJ2Info::GetNumOutStreams(void) const
{
    return 1 ;
}
    
bool RCBranchBCJ2Info::IsEncoderAssigned(void) const
{
    return true ;
}
    
bool RCBranchBCJ2Info::IsDecoderAssigned(void) const
{
    return true ;
}
    
bool RCBranchBCJ2Info::IsSimpleCodec(void) const
{
    return false ;
}

END_NAMESPACE_RCZIP
