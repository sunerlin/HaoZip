/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "compress/deflate/RCZlibCoderInfo.h"
#include "compress/deflate/RCZlibDecoder.h"
#include "interface/RCMethodDefs.h"

/////////////////////////////////////////////////////////////////
//RCZlibCoderInfo class implementation

BEGIN_NAMESPACE_RCZIP

void* RCZlibCreateDecoderFunc(void)
{
    return (void*)(ICompressCoder*)(new RCZlibDecoder);
}

RCZlibCoderInfo::RCZlibCoderInfo()
{
}

RCZlibCoderInfo::~RCZlibCoderInfo()
{
}

PFNCreateCodec RCZlibCoderInfo::GetCreateDecoderFunc(void) const
{
    return RCZlibCreateDecoderFunc ;
}

RC_IID RCZlibCoderInfo::GetDecoderIID(void) const
{
    return IID_ICompressCoder ;
}

PFNCreateCodec RCZlibCoderInfo::GetCreateEncoderFunc(void) const
{
    return NULL ;
}

RC_IID RCZlibCoderInfo::GetEncoderIID(void) const
{
    return (RC_IID)IID_INVALID ;
}

RCMethodID RCZlibCoderInfo::GetMethodID(void) const
{
    return RCMethod::ID_COMPRESS_ZLIB ;
}

RCString RCZlibCoderInfo::GetMethodName(void) const
{
    return RC_METHOD_NAME_COMPRESS_ZLIB ;
}

uint32_t RCZlibCoderInfo::GetNumInStreams(void) const
{
    return 1 ;
}

bool RCZlibCoderInfo::IsFilter(void) const 
{
    return false ;
}

uint32_t RCZlibCoderInfo::GetNumOutStreams(void) const
{
    return 1 ;
}
    
bool RCZlibCoderInfo::IsEncoderAssigned(void) const
{
    return false ;
}
    
bool RCZlibCoderInfo::IsDecoderAssigned(void) const
{
    return true ;
}
    
bool RCZlibCoderInfo::IsSimpleCodec(void) const
{
    return true ;
}

END_NAMESPACE_RCZIP
