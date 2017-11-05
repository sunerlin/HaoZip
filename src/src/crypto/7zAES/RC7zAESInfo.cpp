/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "crypto/7zAES/RC7zAESInfo.h"
#include "interface/ICoder.h"
#include "interface/RCMethodDefs.h"
#include "crypto/7zAES/RC7zAESDecoder.h"
#ifndef RC_STATIC_SFX
#include "crypto/7zAES/RC7zAESEncoder.h"
#endif

/////////////////////////////////////////////////////////////////
//RC7zAESInfo class implementation

BEGIN_NAMESPACE_RCZIP

/** 创建编码器的函数
*/
void* RC7zAESCreateEncoder()
{
#ifndef RC_STATIC_SFX
    return (ICompressFilter*)(new RC7zAESEncoder) ;
#else
    return 0;
#endif
}

/** 创建解码器的函数
*/
void* RC7zAESCreateDecoder()
{
    return (ICompressFilter*)(new RC7zAESDecoder) ;
}

RC7zAESInfo::RC7zAESInfo()
{
}

RC7zAESInfo::~RC7zAESInfo()
{
}

PFNCreateCodec RC7zAESInfo::GetCreateDecoderFunc(void) const
{
    return RC7zAESCreateDecoder ;
}

RC_IID RC7zAESInfo::GetDecoderIID(void) const
{
    return IID_ICompressFilter ;
}

PFNCreateCodec RC7zAESInfo::GetCreateEncoderFunc(void) const
{
    return RC7zAESCreateEncoder ;
}

RC_IID RC7zAESInfo::GetEncoderIID(void) const
{
    return IID_ICompressFilter ;
}

RCMethodID RC7zAESInfo::GetMethodID(void) const
{
    return RCMethod::ID_CRYPTO_7Z_AES ;
}

RCString RC7zAESInfo::GetMethodName(void) const
{
    return RC_METHOD_NAME_CRYPTO_7Z_AES ;
}

uint32_t RC7zAESInfo::GetNumInStreams(void) const
{
    return 1 ;
}

bool RC7zAESInfo::IsFilter(void) const 
{
    return true ;
}

uint32_t RC7zAESInfo::GetNumOutStreams(void) const
{
    return 1 ;
}
    
bool RC7zAESInfo::IsEncoderAssigned(void) const
{
    return true ;
}
    
bool RC7zAESInfo::IsDecoderAssigned(void) const
{
    return true ;
}
    
bool RC7zAESInfo::IsSimpleCodec(void) const
{
    return true ;
}

END_NAMESPACE_RCZIP
