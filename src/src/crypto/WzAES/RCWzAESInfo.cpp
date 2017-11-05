/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "crypto/WzAES/RCWzAESInfo.h"
#include "crypto/WzAES/RCWzAESDecoder.h"
#include "crypto/WzAES/RCWzAESEncoder.h"
#include "interface/ICoder.h"
#include "interface/RCMethodDefs.h"

/////////////////////////////////////////////////////////////////
//RCWzAESInfo class implementation

BEGIN_NAMESPACE_RCZIP

/** 创建解码器的函数
*/
void* RCWzAESCreateDecoder()
{
    return (ICompressFilter*)(new RCWzAESDecoder) ;
}

/** 创建编码器的函数
*/
void* RCWzAESCreateEncoder()
{
    return (ICompressFilter*)(new RCWzAESEncoder) ;
}

RCWzAESInfo::RCWzAESInfo()
{
}

RCWzAESInfo::~RCWzAESInfo()
{
}

PFNCreateCodec RCWzAESInfo::GetCreateDecoderFunc(void) const
{
    return RCWzAESCreateDecoder ;
}

RC_IID RCWzAESInfo::GetDecoderIID(void) const
{
    return IID_ICompressFilter ;
}

PFNCreateCodec RCWzAESInfo::GetCreateEncoderFunc(void) const
{
    return RCWzAESCreateEncoder ;
}

RC_IID RCWzAESInfo::GetEncoderIID(void) const
{
    return IID_ICompressFilter ;
}

RCMethodID RCWzAESInfo::GetMethodID(void) const
{
    return RCMethod::ID_CRYPTO_WZ_AES ;
}

RCString RCWzAESInfo::GetMethodName(void) const
{
    return RC_METHOD_NAME_CRYPTO_WZ_AES ;
}

uint32_t RCWzAESInfo::GetNumInStreams(void) const
{
    return 1 ;
}

bool RCWzAESInfo::IsFilter(void) const 
{
    return true ;
}

uint32_t RCWzAESInfo::GetNumOutStreams(void) const
{
    return 1 ;
}
    
bool RCWzAESInfo::IsEncoderAssigned(void) const
{
    return true ;
}
    
bool RCWzAESInfo::IsDecoderAssigned(void) const
{
    return true ;
}
    
bool RCWzAESInfo::IsSimpleCodec(void) const
{
    return true ;
}

END_NAMESPACE_RCZIP
