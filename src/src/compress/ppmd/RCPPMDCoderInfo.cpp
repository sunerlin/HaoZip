/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "compress/ppmd/RCPPMDCoderInfo.h"
#include "compress/ppmd/RCPPMDDecoder.h"
#ifndef RC_STATIC_SFX
#include "compress/ppmd/RCPPMDEncoder.h"
#endif
#include "interface/RCMethodDefs.h"

/////////////////////////////////////////////////////////////////
//RCPPMDCoderInfo class implementation

BEGIN_NAMESPACE_RCZIP

void* PPMDCoderCreateEncoderFunc(void)
{
#ifndef RC_STATIC_SFX
    return (void*)(ICompressCoder*)(new RCPPMDEncoder);
#else
    return 0;
#endif
}

void* PPMDCoderCreateDecoderFunc(void)
{
    return (void*)(ICompressCoder*)(new RCPPMDDecoder);
}

RCPPMDCoderInfo::RCPPMDCoderInfo()
{
}

RCPPMDCoderInfo::~RCPPMDCoderInfo()
{
}

PFNCreateCodec RCPPMDCoderInfo::GetCreateDecoderFunc(void) const
{
    return PPMDCoderCreateDecoderFunc ;
}

RC_IID RCPPMDCoderInfo::GetDecoderIID(void) const
{
    return IID_ICompressCoder ;
}

PFNCreateCodec RCPPMDCoderInfo::GetCreateEncoderFunc(void) const
{
    return PPMDCoderCreateEncoderFunc ;
}

RC_IID RCPPMDCoderInfo::GetEncoderIID(void) const
{
    return IID_ICompressCoder ;
}

RCMethodID RCPPMDCoderInfo::GetMethodID(void) const
{
    return RCMethod::ID_COMPRESS_PPMD ;
}

RCString RCPPMDCoderInfo::GetMethodName(void) const
{
    return RC_METHOD_NAME_COMPRESS_PPMD ;
}

uint32_t RCPPMDCoderInfo::GetNumInStreams(void) const
{
    return 1 ;
}

bool RCPPMDCoderInfo::IsFilter(void) const 
{
    return false ;
}

uint32_t RCPPMDCoderInfo::GetNumOutStreams(void) const
{
    return 1 ;
}
    
bool RCPPMDCoderInfo::IsEncoderAssigned(void) const
{
    return true ;
}
    
bool RCPPMDCoderInfo::IsDecoderAssigned(void) const
{
    return true ;
}
    
bool RCPPMDCoderInfo::IsSimpleCodec(void) const
{
    return true ;
}

END_NAMESPACE_RCZIP
