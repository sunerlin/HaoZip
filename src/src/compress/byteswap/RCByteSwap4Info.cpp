/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "compress/byteswap/RCByteSwap4Info.h"
#include "compress/byteswap/RCByteSwap4.h"
#include "interface/ICoder.h"
#include "interface/RCMethodDefs.h"

/////////////////////////////////////////////////////////////////
//RCByteSwap4Info class implementation

BEGIN_NAMESPACE_RCZIP

void* ByteSwap4CreateCodecFunc(void)
{
    return (void*)(ICompressFilter*)(new RCByteSwap4);
}

RCByteSwap4Info::RCByteSwap4Info()
{
}

RCByteSwap4Info::~RCByteSwap4Info()
{
}

PFNCreateCodec RCByteSwap4Info::GetCreateDecoderFunc(void) const
{
    return ByteSwap4CreateCodecFunc ;
}

RC_IID RCByteSwap4Info::GetDecoderIID(void) const
{
    return IID_ICompressFilter ;
}

PFNCreateCodec RCByteSwap4Info::GetCreateEncoderFunc(void) const
{
    return ByteSwap4CreateCodecFunc ;
}

RC_IID RCByteSwap4Info::GetEncoderIID(void) const
{
    return IID_ICompressFilter ;
}

RCMethodID RCByteSwap4Info::GetMethodID(void) const
{
    return RCMethod::ID_COMPRESS_BYTE_SWAP_4 ;
}

RCString RCByteSwap4Info::GetMethodName(void) const
{
    return RC_METHOD_NAME_COMPRESS_BYTE_SWAP_4 ;
}

uint32_t RCByteSwap4Info::GetNumInStreams(void) const
{
    return 1 ;
}

bool RCByteSwap4Info::IsFilter(void) const 
{
    return true ;
}

uint32_t RCByteSwap4Info::GetNumOutStreams(void) const
{
    return 1 ;
}
    
bool RCByteSwap4Info::IsEncoderAssigned(void) const
{
    return true ;
}
    
bool RCByteSwap4Info::IsDecoderAssigned(void) const
{
    return true ;
}
    
bool RCByteSwap4Info::IsSimpleCodec(void) const
{
    return true ;
}

END_NAMESPACE_RCZIP
