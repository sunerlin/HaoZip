/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "compress/deflate/RCDeflateNSISCoderInfo.h"
#include "compress/deflate/RCDeflateNsisCOMDecoder.h"
#include "interface/RCMethodDefs.h"

/////////////////////////////////////////////////////////////////
//RCDeflateNSISCoderInfo class implementation

BEGIN_NAMESPACE_RCZIP

void* DeflateNSISCreateCodecFunc(void)
{
    return (void*)(ICompressCoder*)(new RCDeflateNsisCOMDecoder);
}

RCDeflateNSISCoderInfo::RCDeflateNSISCoderInfo()
{
}

RCDeflateNSISCoderInfo::~RCDeflateNSISCoderInfo()
{
}

PFNCreateCodec RCDeflateNSISCoderInfo::GetCreateDecoderFunc(void) const
{
    return DeflateNSISCreateCodecFunc ;
}

RC_IID RCDeflateNSISCoderInfo::GetDecoderIID(void) const
{
    return IID_ICompressCoder ;
}

PFNCreateCodec RCDeflateNSISCoderInfo::GetCreateEncoderFunc(void) const
{
    return NULL ;
}

RC_IID RCDeflateNSISCoderInfo::GetEncoderIID(void) const
{
    return (RC_IID)IID_INVALID ;
}

RCMethodID RCDeflateNSISCoderInfo::GetMethodID(void) const
{
    return RCMethod::ID_COMPRESS_DEFLATE_NSIS ;
}

RCString RCDeflateNSISCoderInfo::GetMethodName(void) const
{
    return RC_METHOD_NAME_COMPRESS_DEFLATE_NSIS ;
}

uint32_t RCDeflateNSISCoderInfo::GetNumInStreams(void) const
{
    return 1 ;
}

bool RCDeflateNSISCoderInfo::IsFilter(void) const 
{
    return false ;
}

uint32_t RCDeflateNSISCoderInfo::GetNumOutStreams(void) const
{
    return 1 ;
}
    
bool RCDeflateNSISCoderInfo::IsEncoderAssigned(void) const
{
    return false ;
}
    
bool RCDeflateNSISCoderInfo::IsDecoderAssigned(void) const
{
    return true ;
}
    
bool RCDeflateNSISCoderInfo::IsSimpleCodec(void) const
{
    return true ;
}

END_NAMESPACE_RCZIP
