/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "crypto/RarAES/RCRarAESDecoderInfo.h"
#include "crypto/RarAES/RCRarAESDecoder.h"
#include "interface/ICoder.h"
#include "interface/RCMethodDefs.h"

/////////////////////////////////////////////////////////////////
//RCRarAESDecoderInfo class implementation

BEGIN_NAMESPACE_RCZIP

/** 创建解码器的函数
*/
void* RCRarAESCreateDecoder()
{
    return (ICompressFilter*)(new RCRarAESDecoder) ;
}

RCRarAESDecoderInfo::RCRarAESDecoderInfo()
{
}

RCRarAESDecoderInfo::~RCRarAESDecoderInfo()
{
}

PFNCreateCodec RCRarAESDecoderInfo::GetCreateDecoderFunc(void) const
{
    return RCRarAESCreateDecoder ;
}

RC_IID RCRarAESDecoderInfo::GetDecoderIID(void) const
{
    return IID_ICompressFilter ;
}

PFNCreateCodec RCRarAESDecoderInfo::GetCreateEncoderFunc(void) const
{
    return NULL ;
}

RC_IID RCRarAESDecoderInfo::GetEncoderIID(void) const
{
    return (RC_IID)IID_INVALID ;
}

RCMethodID RCRarAESDecoderInfo::GetMethodID(void) const
{
    return RCMethod::ID_CRYPTO_RAR_29 ;
}

RCString RCRarAESDecoderInfo::GetMethodName(void) const
{
    return RC_METHOD_NAME_CRYPTO_RAR_29 ;
}

uint32_t RCRarAESDecoderInfo::GetNumInStreams(void) const
{
    return 1 ;
}

bool RCRarAESDecoderInfo::IsFilter(void) const 
{
    return true ;
}

uint32_t RCRarAESDecoderInfo::GetNumOutStreams(void) const
{
    return 1 ;
}
    
bool RCRarAESDecoderInfo::IsEncoderAssigned(void) const
{
    return false ;
}
    
bool RCRarAESDecoderInfo::IsDecoderAssigned(void) const
{
    return true ;
}
    
bool RCRarAESDecoderInfo::IsSimpleCodec(void) const
{
    return true ;
}

END_NAMESPACE_RCZIP
