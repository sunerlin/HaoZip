/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "compress/deflate/RCDeflateEncoderOptimal.h"

/////////////////////////////////////////////////////////////////
//RCDeflateEncoderOptimal class implementation

BEGIN_NAMESPACE_RCZIP

RCDeflateEncoderOptimal::RCDeflateEncoderOptimal():
    m_price(0),
    m_posPrev(0),
    m_backPrev(0)
{
}

RCDeflateEncoderOptimal::~RCDeflateEncoderOptimal()
{
}

END_NAMESPACE_RCZIP
