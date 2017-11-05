/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "compress/rar/RCRar2DecoderInfo.h"
#include "compress/rar/RCRar2Decoder.h"
#include "interface/RCMethodDefs.h"

/////////////////////////////////////////////////////////////////
//RCRar2DecoderInfo class implementation

BEGIN_NAMESPACE_RCZIP

void* Rar2DecoderCreateCodecFunc(void)
{
    return (void*)(ICompressCoder*)(new RCRar2Decoder);
}

RCRar2DecoderInfo::RCRar2DecoderInfo()
{
}

RCRar2DecoderInfo::~RCRar2DecoderInfo()
{
}

PFNCreateCodec RCRar2DecoderInfo::GetCreateDecoderFunc(void) const
{
    return Rar2DecoderCreateCodecFunc ;
}

RC_IID RCRar2DecoderInfo::GetDecoderIID(void) const
{
    return IID_ICompressCoder ;
}

PFNCreateCodec RCRar2DecoderInfo::GetCreateEncoderFunc(void) const
{
    return NULL ;
}

RC_IID RCRar2DecoderInfo::GetEncoderIID(void) const
{
    return (RC_IID)IID_INVALID ;
}

RCMethodID RCRar2DecoderInfo::GetMethodID(void) const
{
    return RCMethod::ID_COMPRESS_RAR_2 ;
}

RCString RCRar2DecoderInfo::GetMethodName(void) const
{
    return RC_METHOD_NAME_COMPRESS_RAR_2 ;
}

uint32_t RCRar2DecoderInfo::GetNumInStreams(void) const
{
    return 1 ;
}

bool RCRar2DecoderInfo::IsFilter(void) const 
{
    return false ;
}

uint32_t RCRar2DecoderInfo::GetNumOutStreams(void) const
{
    return 1 ;
}
    
bool RCRar2DecoderInfo::IsEncoderAssigned(void) const
{
    return false ;
}
    
bool RCRar2DecoderInfo::IsDecoderAssigned(void) const
{
    return true ;
}
    
bool RCRar2DecoderInfo::IsSimpleCodec(void) const
{
    return true ;
}

END_NAMESPACE_RCZIP
