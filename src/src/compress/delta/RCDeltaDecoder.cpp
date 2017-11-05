/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "compress/delta/RCDeltaDecoder.h"

/////////////////////////////////////////////////////////////////
//RCDeltaDecoder class implementation

BEGIN_NAMESPACE_RCZIP

HResult RCDeltaDecoder::Init()
{
    DeltaInit();
    return RC_S_OK;
}

uint32_t RCDeltaDecoder::Filter(byte_t* data, uint32_t size)
{
    Delta_Decode(m_state, m_delta, data, size);
    return size;
}

HResult RCDeltaDecoder::SetDecoderProperties2(const byte_t* data, uint32_t size)
{
    if (size != 1)
    {
        return RC_E_INVALIDARG;
    }
    m_delta = (unsigned)data[0] + 1;
    return RC_S_OK;
}

END_NAMESPACE_RCZIP
