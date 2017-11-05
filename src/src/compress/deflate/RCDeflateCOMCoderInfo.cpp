/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "compress/deflate/RCDeflateCOMCoderInfo.h"
#include "compress/deflate/RCDeflateCOMDecoder.h"
#include "compress/deflate/RCDeflateCOMEncoder.h"
#include "interface/RCMethodDefs.h"

/////////////////////////////////////////////////////////////////
//RCDeflateCOMCoderInfo class implementation

BEGIN_NAMESPACE_RCZIP

void* DeflateCOMCoderCreateEncoderFunc(void)
{
#ifndef RC_STATIC_SFX
    return (void*)(ICompressCoder*)(new RCDeflateCOMEncoder);
#else
    return 0;
#endif
}

void* DeflateCOMCoderCreateDecoderFunc(void)
{
    return (void*)(ICompressCoder*)(new RCDeflateCOMDecoder);
}

RCDeflateCOMCoderInfo::RCDeflateCOMCoderInfo()
{
}

RCDeflateCOMCoderInfo::~RCDeflateCOMCoderInfo()
{
}

PFNCreateCodec RCDeflateCOMCoderInfo::GetCreateDecoderFunc(void) const
{
    return DeflateCOMCoderCreateDecoderFunc ;
}

RC_IID RCDeflateCOMCoderInfo::GetDecoderIID(void) const
{
    return IID_ICompressCoder ;
}

PFNCreateCodec RCDeflateCOMCoderInfo::GetCreateEncoderFunc(void) const
{
    return DeflateCOMCoderCreateEncoderFunc ;
}

RC_IID RCDeflateCOMCoderInfo::GetEncoderIID(void) const
{
    return IID_ICompressCoder ;
}

RCMethodID RCDeflateCOMCoderInfo::GetMethodID(void) const
{
    return RCMethod::ID_COMPRESS_DEFLATE_COM ;
}

RCString RCDeflateCOMCoderInfo::GetMethodName(void) const
{
    return RC_METHOD_NAME_COMPRESS_DEFLATE_COM ;
}

uint32_t RCDeflateCOMCoderInfo::GetNumInStreams(void) const
{
    return 1 ;
}

bool RCDeflateCOMCoderInfo::IsFilter(void) const 
{
    return false ;
}

uint32_t RCDeflateCOMCoderInfo::GetNumOutStreams(void) const
{
    return 1 ;
}
    
bool RCDeflateCOMCoderInfo::IsEncoderAssigned(void) const
{
    return true ;
}
    
bool RCDeflateCOMCoderInfo::IsDecoderAssigned(void) const
{
    return true ;
}
    
bool RCDeflateCOMCoderInfo::IsSimpleCodec(void) const
{
    return true ;
}

END_NAMESPACE_RCZIP
