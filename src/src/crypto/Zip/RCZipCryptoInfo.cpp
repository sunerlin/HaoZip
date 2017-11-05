/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "crypto/Zip/RCZipCryptoInfo.h"
#include "crypto/Zip/RCZipDecoder.h"
#include "crypto/Zip/RCZipEncoder.h"
#include "interface/ICoder.h"
#include "interface/RCMethodDefs.h"

/////////////////////////////////////////////////////////////////
//RCZipCryptoInfo class implementation

BEGIN_NAMESPACE_RCZIP

/** 创建解码器的函数
*/
void* RCZipCreateDecoder()
{
    return (ICompressFilter*)(new RCZipDecoder) ;
}

/** 创建编码器的函数
*/
void* RCZipCreateEncoder()
{
    return (ICompressFilter*)(new RCZipEncoder) ;
}

RCZipCryptoInfo::RCZipCryptoInfo()
{
}

RCZipCryptoInfo::~RCZipCryptoInfo()
{
}

PFNCreateCodec RCZipCryptoInfo::GetCreateDecoderFunc(void) const
{
    return RCZipCreateDecoder ;
}

RC_IID RCZipCryptoInfo::GetDecoderIID(void) const
{
    return IID_ICompressFilter ;
}

PFNCreateCodec RCZipCryptoInfo::GetCreateEncoderFunc(void) const
{
    return RCZipCreateEncoder ;
}

RC_IID RCZipCryptoInfo::GetEncoderIID(void) const
{
    return IID_ICompressFilter ;
}

RCMethodID RCZipCryptoInfo::GetMethodID(void) const
{
    return RCMethod::ID_CRYPTO_ZIP ;
}

RCString RCZipCryptoInfo::GetMethodName(void) const
{
    return RC_METHOD_NAME_CRYPTO_ZIP ;
}

uint32_t RCZipCryptoInfo::GetNumInStreams(void) const
{
    return 1 ;
}

bool RCZipCryptoInfo::IsFilter(void) const 
{
    return true ;
}

uint32_t RCZipCryptoInfo::GetNumOutStreams(void) const
{
    return 1 ;
}
    
bool RCZipCryptoInfo::IsEncoderAssigned(void) const
{
    return true ;
}
    
bool RCZipCryptoInfo::IsDecoderAssigned(void) const
{
    return true ;
}
    
bool RCZipCryptoInfo::IsSimpleCodec(void) const
{
    return true ;
}

END_NAMESPACE_RCZIP
