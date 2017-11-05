/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "compress/lzh/RCLzhHuffmanDecoderInfo.h"
#include "compress/lzh/RCLzhHuffmanDecoder.h"
#include "interface/RCMethodDefs.h"

/////////////////////////////////////////////////////////////////
//RCLzhHuffmanDecoderInfo class implementation

BEGIN_NAMESPACE_RCZIP

void* LzhHuffmanDecoderCreateCodecFunc(void)
{
    return (void*)(ICompressCoder*)(new RCLzhHuffmanDecoder);
}

RCLzhHuffmanDecoderInfo::RCLzhHuffmanDecoderInfo()
{
}

RCLzhHuffmanDecoderInfo::~RCLzhHuffmanDecoderInfo()
{
}

PFNCreateCodec RCLzhHuffmanDecoderInfo::GetCreateDecoderFunc(void) const
{
    return LzhHuffmanDecoderCreateCodecFunc ;
}

RC_IID RCLzhHuffmanDecoderInfo::GetDecoderIID(void) const
{
    return IID_ICompressCoder ;
}

PFNCreateCodec RCLzhHuffmanDecoderInfo::GetCreateEncoderFunc(void) const
{
    return NULL ;
}

RC_IID RCLzhHuffmanDecoderInfo::GetEncoderIID(void) const
{
    return (RC_IID)IID_INVALID ;
}

RCMethodID RCLzhHuffmanDecoderInfo::GetMethodID(void) const
{
    return RCMethod::ID_COMPRESS_LZH ;
}

RCString RCLzhHuffmanDecoderInfo::GetMethodName(void) const
{
    return RC_METHOD_NAME_COMPRESS_LZH ;
}

uint32_t RCLzhHuffmanDecoderInfo::GetNumInStreams(void) const
{
    return 1 ;
}

bool RCLzhHuffmanDecoderInfo::IsFilter(void) const 
{
    return false ;
}

uint32_t RCLzhHuffmanDecoderInfo::GetNumOutStreams(void) const
{
    return 1 ;
}
    
bool RCLzhHuffmanDecoderInfo::IsEncoderAssigned(void) const
{
    return false ;
}
    
bool RCLzhHuffmanDecoderInfo::IsDecoderAssigned(void) const
{
    return true ;
}
    
bool RCLzhHuffmanDecoderInfo::IsSimpleCodec(void) const
{
    return true ;
}

END_NAMESPACE_RCZIP
