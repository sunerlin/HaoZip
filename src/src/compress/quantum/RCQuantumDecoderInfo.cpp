/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "compress/quantum/RCQuantumDecoderInfo.h"
#include "compress/quantum/RCQuantumDecoder.h"
#include "interface/RCMethodDefs.h"

/////////////////////////////////////////////////////////////////
//RCQuantumDecoderInfo class implementation

BEGIN_NAMESPACE_RCZIP

void* QuantumDecoderCreateCodecFunc(void)
{
    return (void*)(ICompressCoder*)(new RCQuantumDecoder);
}

RCQuantumDecoderInfo::RCQuantumDecoderInfo()
{
}

RCQuantumDecoderInfo::~RCQuantumDecoderInfo()
{
}

PFNCreateCodec RCQuantumDecoderInfo::GetCreateDecoderFunc(void) const
{
    return QuantumDecoderCreateCodecFunc ;
}

RC_IID RCQuantumDecoderInfo::GetDecoderIID(void) const
{
    return IID_ICompressCoder ;
}

PFNCreateCodec RCQuantumDecoderInfo::GetCreateEncoderFunc(void) const
{
    return NULL ;
}

RC_IID RCQuantumDecoderInfo::GetEncoderIID(void) const
{
    return (RC_IID)IID_INVALID ;
}

RCMethodID RCQuantumDecoderInfo::GetMethodID(void) const
{
    return RCMethod::ID_COMPRESS_QUANTUM ;
}

RCString RCQuantumDecoderInfo::GetMethodName(void) const
{
    return RC_METHOD_NAME_COMPRESS_QUANTUM ;
}

uint32_t RCQuantumDecoderInfo::GetNumInStreams(void) const
{
    return 1 ;
}

bool RCQuantumDecoderInfo::IsFilter(void) const 
{
    return false ;
}

uint32_t RCQuantumDecoderInfo::GetNumOutStreams(void) const
{
    return 1 ;
}
    
bool RCQuantumDecoderInfo::IsEncoderAssigned(void) const
{
    return false ;
}
    
bool RCQuantumDecoderInfo::IsDecoderAssigned(void) const
{
    return true ;
}
    
bool RCQuantumDecoderInfo::IsSimpleCodec(void) const
{
    return true ;
}

END_NAMESPACE_RCZIP
