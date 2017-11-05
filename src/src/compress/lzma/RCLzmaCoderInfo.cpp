/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "compress/lzma/RCLzmaCoderInfo.h"
#include "compress/lzma/RCLzmaDecoder.h"
#ifndef RC_STATIC_SFX
#include "compress/lzma/RCLzmaEncoder.h"
#endif
#include "interface/ICoder.h"
#include "interface/RCMethodDefs.h"

/////////////////////////////////////////////////////////////////
//RCLzmaCoderInfo class implementation

BEGIN_NAMESPACE_RCZIP

void* LzmaCoderCreateEncoderFunc(void)
{
#ifndef RC_STATIC_SFX
    return (void*)(ICompressCoder*)(new RCLzmaEncoder);
#else
    return 0;
#endif
}

void* LzmaCoderCreateDecoderFunc(void)
{
    return (void*)(ICompressCoder*)(new RCLzmaDecoder);
}

RCLzmaCoderInfo::RCLzmaCoderInfo()
{
}

RCLzmaCoderInfo::~RCLzmaCoderInfo()
{
}

PFNCreateCodec RCLzmaCoderInfo::GetCreateDecoderFunc(void) const
{
    return LzmaCoderCreateDecoderFunc ;
}

RC_IID RCLzmaCoderInfo::GetDecoderIID(void) const
{
    return IID_ICompressCoder ;
}

PFNCreateCodec RCLzmaCoderInfo::GetCreateEncoderFunc(void) const
{
    return LzmaCoderCreateEncoderFunc ;
}

RC_IID RCLzmaCoderInfo::GetEncoderIID(void) const
{
    return IID_ICompressCoder ;
}

RCMethodID RCLzmaCoderInfo::GetMethodID(void) const
{
    return RCMethod::ID_COMPRESS_LZMA ;
}

RCString RCLzmaCoderInfo::GetMethodName(void) const
{
    return RC_METHOD_NAME_COMPRESS_LZMA ;
}

uint32_t RCLzmaCoderInfo::GetNumInStreams(void) const
{
    return 1 ;
}

bool RCLzmaCoderInfo::IsFilter(void) const 
{
    return false ;
}

uint32_t RCLzmaCoderInfo::GetNumOutStreams(void) const
{
    return 1 ;
}
    
bool RCLzmaCoderInfo::IsEncoderAssigned(void) const
{
    return true ;
}
    
bool RCLzmaCoderInfo::IsDecoderAssigned(void) const
{
    return true ;
}
    
bool RCLzmaCoderInfo::IsSimpleCodec(void) const
{
    return true ;
}

END_NAMESPACE_RCZIP
