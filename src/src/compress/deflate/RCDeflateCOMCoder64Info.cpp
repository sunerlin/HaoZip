/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "compress/deflate/RCDeflateCOMCoder64Info.h"
#include "compress/deflate/RCDeflateCOMDecoder64.h"
#include "compress/deflate/RCDeflateCOMEncoder64.h"
#include "interface/RCMethodDefs.h"

/////////////////////////////////////////////////////////////////
//RCDeflateCOMCoder64Info class implementation

BEGIN_NAMESPACE_RCZIP

void* Deflate64COMCoderCreateEncoderFunc(void)
{
#ifndef RC_STATIC_SFX
    return (void*)(ICompressCoder*)(new RCDeflateCOMEncoder64);
#else
    return 0;
#endif
}

void* Deflate64COMCoderCreateDecoderFunc(void)
{
    return (void*)(ICompressCoder*)(new RCDeflateCOMDecoder64);
}

RCDeflateCOMCoder64Info::RCDeflateCOMCoder64Info()
{
}

RCDeflateCOMCoder64Info::~RCDeflateCOMCoder64Info()
{
}

PFNCreateCodec RCDeflateCOMCoder64Info::GetCreateDecoderFunc(void) const
{
    return Deflate64COMCoderCreateDecoderFunc ;
}

RC_IID RCDeflateCOMCoder64Info::GetDecoderIID(void) const
{
    return IID_ICompressCoder ;
}

PFNCreateCodec RCDeflateCOMCoder64Info::GetCreateEncoderFunc(void) const
{
    return Deflate64COMCoderCreateEncoderFunc ;
}

RC_IID RCDeflateCOMCoder64Info::GetEncoderIID(void) const
{
    return IID_ICompressCoder ;
}

RCMethodID RCDeflateCOMCoder64Info::GetMethodID(void) const
{
    return RCMethod::ID_COMPRESS_DEFLATE_COM_64 ;
}

RCString RCDeflateCOMCoder64Info::GetMethodName(void) const
{
    return RC_METHOD_NAME_COMPRESS_DEFLATE_COM_64 ;
}

uint32_t RCDeflateCOMCoder64Info::GetNumInStreams(void) const
{
    return 1 ;
}

bool RCDeflateCOMCoder64Info::IsFilter(void) const 
{
    return false ;
}

uint32_t RCDeflateCOMCoder64Info::GetNumOutStreams(void) const
{
    return 1 ;
}
    
bool RCDeflateCOMCoder64Info::IsEncoderAssigned(void) const
{
    return true ;
}
    
bool RCDeflateCOMCoder64Info::IsDecoderAssigned(void) const
{
    return true ;
}
    
bool RCDeflateCOMCoder64Info::IsSimpleCodec(void) const
{
    return true ;
}

END_NAMESPACE_RCZIP
