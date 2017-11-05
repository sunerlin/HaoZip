/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "compress/arj/RCArjDecoder2Info.h"
#include "compress/arj/RCArjDecoder2.h"
#include "interface/ICoder.h"
#include "interface/RCMethodDefs.h"

/////////////////////////////////////////////////////////////////
//RCArjDecoder2Info class implementation

BEGIN_NAMESPACE_RCZIP

void* ArjDecoder2CreateCodecFunc(void)
{
    return (void*)(ICompressCoder*)(new RCArjDecoder2);
}

RCArjDecoder2Info::RCArjDecoder2Info()
{
}

RCArjDecoder2Info::~RCArjDecoder2Info()
{
}

PFNCreateCodec RCArjDecoder2Info::GetCreateDecoderFunc(void) const
{
    return ArjDecoder2CreateCodecFunc ;
}

RC_IID RCArjDecoder2Info::GetDecoderIID(void) const
{
    return IID_ICompressCoder ;
}

PFNCreateCodec RCArjDecoder2Info::GetCreateEncoderFunc(void) const
{
    return NULL ;
}

RC_IID RCArjDecoder2Info::GetEncoderIID(void) const
{
    return (RC_IID)IID_INVALID ;
}

RCMethodID RCArjDecoder2Info::GetMethodID(void) const
{
    return RCMethod::ID_COMPRESS_ARJ_2 ;
}

RCString RCArjDecoder2Info::GetMethodName(void) const
{
    return RC_METHOD_NAME_COMPRESS_ARJ_2 ;
}

uint32_t RCArjDecoder2Info::GetNumInStreams(void) const
{
    return 1 ;
}

bool RCArjDecoder2Info::IsFilter(void) const 
{
    return false ;
}

uint32_t RCArjDecoder2Info::GetNumOutStreams(void) const
{
    return 1 ;
}
    
bool RCArjDecoder2Info::IsEncoderAssigned(void) const
{
    return false ;
}
    
bool RCArjDecoder2Info::IsDecoderAssigned(void) const
{
    return true ;
}
    
bool RCArjDecoder2Info::IsSimpleCodec(void) const
{
    return true ;
}

END_NAMESPACE_RCZIP
