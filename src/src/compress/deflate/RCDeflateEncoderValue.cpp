/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "compress/deflate/RCDeflateEncoderValue.h"

/////////////////////////////////////////////////////////////////
//RCDeflateEncoderValue class implementation

BEGIN_NAMESPACE_RCZIP

RCDeflateEncoderValue::RCDeflateEncoderValue():
    m_len(0),
    m_pos(0)
{
}

RCDeflateEncoderValue::~RCDeflateEncoderValue()
{
}

void RCDeflateEncoderValue::SetAsLiteral()
{
    m_len = (1 << 15);
}

bool RCDeflateEncoderValue::IsLiteral() const
{
    return (m_len >= (1 << 15));
}

END_NAMESPACE_RCZIP
