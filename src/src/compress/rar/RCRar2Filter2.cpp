/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "compress/rar/RCRar2Filter2.h"

/////////////////////////////////////////////////////////////////
//RCRar2Filter2 class implementation

BEGIN_NAMESPACE_RCZIP

RCRar2Filter2::RCRar2Filter2()
{
}

RCRar2Filter2::~RCRar2Filter2()
{
}

void RCRar2Filter2::Init()
{
    memset(this, 0, sizeof(*this));
}

byte_t RCRar2Filter2::Decode(byte_t delta)
{
    return m_filters[m_currentChannel].Decode(m_channelDelta, delta);
}

END_NAMESPACE_RCZIP
