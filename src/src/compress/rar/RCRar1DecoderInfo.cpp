/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "compress/rar/RCRar1DecoderInfo.h"
#include "compress/rar/RCRar1Decoder.h"
#include "interface/RCMethodDefs.h"

/////////////////////////////////////////////////////////////////
//RCRar1DecoderInfo class implementation

BEGIN_NAMESPACE_RCZIP

void* Rar1DecoderCreateCodecFunc(void)
{
    return (void*)(ICompressCoder*)(new RCRar1Decoder);
}

RCRar1DecoderInfo::RCRar1DecoderInfo()
{
}

RCRar1DecoderInfo::~RCRar1DecoderInfo()
{
}

PFNCreateCodec RCRar1DecoderInfo::GetCreateDecoderFunc(void) const
{
    return Rar1DecoderCreateCodecFunc ;
}

RC_IID RCRar1DecoderInfo::GetDecoderIID(void) const
{
    return IID_ICompressCoder ;
}

PFNCreateCodec RCRar1DecoderInfo::GetCreateEncoderFunc(void) const
{
    return NULL ;
}

RC_IID RCRar1DecoderInfo::GetEncoderIID(void) const
{
    return (RC_IID)IID_INVALID ;
}

RCMethodID RCRar1DecoderInfo::GetMethodID(void) const
{
    return RCMethod::ID_COMPRESS_RAR_1 ;
}

RCString RCRar1DecoderInfo::GetMethodName(void) const
{
    return RC_METHOD_NAME_COMPRESS_RAR_1 ;
}

uint32_t RCRar1DecoderInfo::GetNumInStreams(void) const
{
    return 1 ;
}

bool RCRar1DecoderInfo::IsFilter(void) const 
{
    return false ;
}

uint32_t RCRar1DecoderInfo::GetNumOutStreams(void) const
{
    return 1 ;
}
    
bool RCRar1DecoderInfo::IsEncoderAssigned(void) const
{
    return false ;
}
    
bool RCRar1DecoderInfo::IsDecoderAssigned(void) const
{
    return true ;
}
    
bool RCRar1DecoderInfo::IsSimpleCodec(void) const
{
    return true ;
}

END_NAMESPACE_RCZIP
