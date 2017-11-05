/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "compress/rar/RCRar3DecoderInfo.h"
#include "compress/rar/RCRar3Decoder.h"
#include "interface/RCMethodDefs.h"

/////////////////////////////////////////////////////////////////
//RCRar3DecoderInfo class implementation

BEGIN_NAMESPACE_RCZIP

void* Rar3DecoderCreateCodecFunc(void)
{
    return (void*)(ICompressCoder*)(new RCRar3Decoder);
}

RCRar3DecoderInfo::RCRar3DecoderInfo()
{
}

RCRar3DecoderInfo::~RCRar3DecoderInfo()
{
}

PFNCreateCodec RCRar3DecoderInfo::GetCreateDecoderFunc(void) const
{
    return Rar3DecoderCreateCodecFunc ;
}

RC_IID RCRar3DecoderInfo::GetDecoderIID(void) const
{
    return IID_ICompressCoder ;
}

PFNCreateCodec RCRar3DecoderInfo::GetCreateEncoderFunc(void) const
{
    return NULL ;
}

RC_IID RCRar3DecoderInfo::GetEncoderIID(void) const
{
    return (RC_IID)IID_INVALID ;
}

RCMethodID RCRar3DecoderInfo::GetMethodID(void) const
{
    return RCMethod::ID_COMPRESS_RAR_3 ;
}

RCString RCRar3DecoderInfo::GetMethodName(void) const
{
    return RC_METHOD_NAME_COMPRESS_RAR_3 ;
}

uint32_t RCRar3DecoderInfo::GetNumInStreams(void) const
{
    return 1 ;
}

bool RCRar3DecoderInfo::IsFilter(void) const 
{
    return false ;
}

uint32_t RCRar3DecoderInfo::GetNumOutStreams(void) const
{
    return 1 ;
}
    
bool RCRar3DecoderInfo::IsEncoderAssigned(void) const
{
    return false ;
}
    
bool RCRar3DecoderInfo::IsDecoderAssigned(void) const
{
    return true ;
}
    
bool RCRar3DecoderInfo::IsSimpleCodec(void) const
{
    return true ;
}

END_NAMESPACE_RCZIP
