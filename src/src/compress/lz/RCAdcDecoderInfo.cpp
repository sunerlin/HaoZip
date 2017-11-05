/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "compress/lz/RCAdcDecoderInfo.h"
#include "compress/lz/RCAdcDecoder.h"
#include "interface/ICoder.h"
#include "interface/RCMethodDefs.h"

/////////////////////////////////////////////////////////////////
//RCAdcDecoderInfo class implementation

BEGIN_NAMESPACE_RCZIP

void* AdcCoderCreateCodecFunc(void)
{
    return (void*)(ICompressCoder*)(new RCAdcDecoder);
}

RCAdcDecoderInfo::RCAdcDecoderInfo()
{
}

RCAdcDecoderInfo::~RCAdcDecoderInfo()
{
}

PFNCreateCodec RCAdcDecoderInfo::GetCreateDecoderFunc(void) const
{
    return AdcCoderCreateCodecFunc ;
}

RC_IID RCAdcDecoderInfo::GetDecoderIID(void) const
{
    return IID_ICompressCoder ;
}

PFNCreateCodec RCAdcDecoderInfo::GetCreateEncoderFunc(void) const
{
    return NULL ;
}

RC_IID RCAdcDecoderInfo::GetEncoderIID(void) const
{
    return IID_ICompressCoder ;
}

RCMethodID RCAdcDecoderInfo::GetMethodID(void) const
{
    return RCMethod::ID_COMPRESS_ADC ;
}

RCString RCAdcDecoderInfo::GetMethodName(void) const
{
    return RC_METHOD_NAME_COMPRESS_ADC ;
}

uint32_t RCAdcDecoderInfo::GetNumInStreams(void) const
{
    return 1 ;
}

bool RCAdcDecoderInfo::IsFilter(void) const 
{
    return false ;
}

uint32_t RCAdcDecoderInfo::GetNumOutStreams(void) const
{
    return 1 ;
}
    
bool RCAdcDecoderInfo::IsEncoderAssigned(void) const
{
    return false ;
}
    
bool RCAdcDecoderInfo::IsDecoderAssigned(void) const
{
    return true ;
}
    
bool RCAdcDecoderInfo::IsSimpleCodec(void) const
{
    return true ;
}

END_NAMESPACE_RCZIP
