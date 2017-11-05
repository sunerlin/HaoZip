/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "compress/arj/RCArjDecoder1Info.h"
#include "compress/arj/RCArjDecoder1.h"
#include "interface/ICoder.h"
#include "interface/RCMethodDefs.h"

/////////////////////////////////////////////////////////////////
//RCArjDecoder1Info class implementation

BEGIN_NAMESPACE_RCZIP

void* ArjDecoder1CreateCodecFunc(void)
{
    return (void*)(ICompressCoder*)(new RCArjDecoder1);
}

RCArjDecoder1Info::RCArjDecoder1Info()
{
}

RCArjDecoder1Info::~RCArjDecoder1Info()
{
}

PFNCreateCodec RCArjDecoder1Info::GetCreateDecoderFunc(void) const
{
    return ArjDecoder1CreateCodecFunc ;
}

RC_IID RCArjDecoder1Info::GetDecoderIID(void) const
{
    return IID_ICompressCoder ;
}

PFNCreateCodec RCArjDecoder1Info::GetCreateEncoderFunc(void) const
{
    return NULL ;
}

RC_IID RCArjDecoder1Info::GetEncoderIID(void) const
{
    return (RC_IID)IID_INVALID ;
}

RCMethodID RCArjDecoder1Info::GetMethodID(void) const
{
    return RCMethod::ID_COMPRESS_ARJ_1 ;
}

RCString RCArjDecoder1Info::GetMethodName(void) const
{
    return RC_METHOD_NAME_COMPRESS_ARJ_1 ;
}

uint32_t RCArjDecoder1Info::GetNumInStreams(void) const
{
    return 1 ;
}

bool RCArjDecoder1Info::IsFilter(void) const 
{
    return false ;
}

uint32_t RCArjDecoder1Info::GetNumOutStreams(void) const
{
    return 1 ;
}
    
bool RCArjDecoder1Info::IsEncoderAssigned(void) const
{
    return false ;
}
    
bool RCArjDecoder1Info::IsDecoderAssigned(void) const
{
    return true ;
}
    
bool RCArjDecoder1Info::IsSimpleCodec(void) const
{
    return true ;
}

END_NAMESPACE_RCZIP
