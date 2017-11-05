/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "compress/byteswap/RCByteSwap2Info.h"
#include "compress/byteswap/RCByteSwap2.h"
#include "interface/ICoder.h"
#include "interface/RCMethodDefs.h"

/////////////////////////////////////////////////////////////////
//RCByteSwap2Info class implementation

BEGIN_NAMESPACE_RCZIP

void* ByteSwap2CreateCodecFunc(void)
{
    return (void*)(ICompressFilter*)(new RCByteSwap2);
}

RCByteSwap2Info::RCByteSwap2Info()
{
}

RCByteSwap2Info::~RCByteSwap2Info()
{
}

PFNCreateCodec RCByteSwap2Info::GetCreateDecoderFunc(void) const
{
    return ByteSwap2CreateCodecFunc ;
}

RC_IID RCByteSwap2Info::GetDecoderIID(void) const
{
    return IID_ICompressFilter ;
}

PFNCreateCodec RCByteSwap2Info::GetCreateEncoderFunc(void) const
{
    return ByteSwap2CreateCodecFunc ;
}

RC_IID RCByteSwap2Info::GetEncoderIID(void) const
{
    return IID_ICompressFilter ;
}

RCMethodID RCByteSwap2Info::GetMethodID(void) const
{
    return RCMethod::ID_COMPRESS_BYTE_SWAP_2 ;
}

RCString RCByteSwap2Info::GetMethodName(void) const
{
    return RC_METHOD_NAME_COMPRESS_BYTE_SWAP_2 ;
}

uint32_t RCByteSwap2Info::GetNumInStreams(void) const
{
    return 1 ;
}

bool RCByteSwap2Info::IsFilter(void) const 
{
    return true ;
}

uint32_t RCByteSwap2Info::GetNumOutStreams(void) const
{
    return 1 ;
}
    
bool RCByteSwap2Info::IsEncoderAssigned(void) const
{
    return true ;
}
    
bool RCByteSwap2Info::IsDecoderAssigned(void) const
{
    return true ;
}
    
bool RCByteSwap2Info::IsSimpleCodec(void) const
{
    return true ;
}

END_NAMESPACE_RCZIP
