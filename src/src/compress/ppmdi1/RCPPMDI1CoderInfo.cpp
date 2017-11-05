/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#include "compress/ppmdi1/RCPPMDI1CoderInfo.h"
#include "compress/ppmdi1/RCPPMDI1Decoder.h"
#include "interface/RCMethodDefs.h"

BEGIN_NAMESPACE_RCZIP

void* PPMDI1CoderCreateEncoderFunc(void)
{
    return NULL;
}

void* PPMDI1CoderCreateDecoderFunc(void)
{
    return (void*)(ICompressCoder*)(new RCPPMDI1Decoder);
}

RCPPMDI1CoderInfo::RCPPMDI1CoderInfo()
{
}

RCPPMDI1CoderInfo::~RCPPMDI1CoderInfo()
{
}

PFNCreateCodec RCPPMDI1CoderInfo::GetCreateDecoderFunc(void) const
{
    return PPMDI1CoderCreateDecoderFunc;
}
    
RC_IID RCPPMDI1CoderInfo::GetDecoderIID(void) const 
{
    return IID_ICompressCoder;
}
    
PFNCreateCodec RCPPMDI1CoderInfo::GetCreateEncoderFunc(void) const 
{
    return PPMDI1CoderCreateEncoderFunc;
}
    
RC_IID RCPPMDI1CoderInfo::GetEncoderIID(void) const 
{
    return IID_ICompressCoder ;
}
    
RCMethodID RCPPMDI1CoderInfo::GetMethodID(void) const 
{
    return RCMethod::ID_COMPRESS_PPMDI1 ;
}
    
RCString RCPPMDI1CoderInfo::GetMethodName(void) const 
{
    return RC_METHOD_NAME_COMPRESS_PPMDI1;
}
    
uint32_t RCPPMDI1CoderInfo::GetNumInStreams(void) const 
{
    return 1;
}
    
uint32_t RCPPMDI1CoderInfo::GetNumOutStreams(void) const 
{
    return 1;
}
    
bool RCPPMDI1CoderInfo::IsEncoderAssigned(void) const 
{
    return false;
}
    
bool RCPPMDI1CoderInfo::IsDecoderAssigned(void) const 
{
    return true;
}
    
bool RCPPMDI1CoderInfo::IsSimpleCodec(void) const 
{
    return true;
}
    
bool RCPPMDI1CoderInfo::IsFilter(void) const 
{
    return false;
}

END_NAMESPACE_RCZIP
