/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "compress/branch/RCBranchConverter.h"

/////////////////////////////////////////////////////////////////
//RCBranchConverter class implementation

BEGIN_NAMESPACE_RCZIP

RCBranchConverter::RCBranchConverter():
    m_bufferPos(0)
{
}

RCBranchConverter::~RCBranchConverter()
{
}

void RCBranchConverter::SubInit()
{
}

HResult RCBranchConverter::Init()
{
    m_bufferPos = 0 ;
    SubInit() ;
    return RC_S_OK ;
}

uint32_t RCBranchConverter::Filter(byte_t* data, uint32_t size)
{
    uint32_t processedSize = SubFilter(data, size) ;
    m_bufferPos += processedSize ;
    return processedSize ;
}

END_NAMESPACE_RCZIP
