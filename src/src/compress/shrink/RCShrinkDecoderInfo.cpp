/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "compress/shrink/RCShrinkDecoderInfo.h"
#include "compress/shrink/RCShrinkDecoder.h"
#include "interface/ICoder.h"
#include "interface/RCMethodDefs.h"

/////////////////////////////////////////////////////////////////
//RCShrinkDecoderInfo class implementation

BEGIN_NAMESPACE_RCZIP

void* ShrinkDecoderCreateCodecFunc(void)
{
    return (void*)(ICompressCoder*)(new RCShrinkDecoder);
}

RCShrinkDecoderInfo::RCShrinkDecoderInfo()
{
}

RCShrinkDecoderInfo::~RCShrinkDecoderInfo()
{
}

PFNCreateCodec RCShrinkDecoderInfo::GetCreateDecoderFunc(void) const
{
    return ShrinkDecoderCreateCodecFunc ;
}

RC_IID RCShrinkDecoderInfo::GetDecoderIID(void) const
{
    return IID_ICompressCoder ;
}

PFNCreateCodec RCShrinkDecoderInfo::GetCreateEncoderFunc(void) const
{
    return NULL ;
}

RC_IID RCShrinkDecoderInfo::GetEncoderIID(void) const
{
    return (RC_IID)IID_INVALID ;
}

RCMethodID RCShrinkDecoderInfo::GetMethodID(void) const
{
    return RCMethod::ID_COMPRESS_SHRINK ;
}

RCString RCShrinkDecoderInfo::GetMethodName(void) const
{
    return RC_METHOD_NAME_COMPRESS_SHRINK ;
}

uint32_t RCShrinkDecoderInfo::GetNumInStreams(void) const
{
    return 1 ;
}

bool RCShrinkDecoderInfo::IsFilter(void) const 
{
    return false ;
}

uint32_t RCShrinkDecoderInfo::GetNumOutStreams(void) const
{
    return 1 ;
}
    
bool RCShrinkDecoderInfo::IsEncoderAssigned(void) const
{
    return false ;
}
    
bool RCShrinkDecoderInfo::IsDecoderAssigned(void) const
{
    return true ;
}
    
bool RCShrinkDecoderInfo::IsSimpleCodec(void) const
{
    return true ;
}

END_NAMESPACE_RCZIP
