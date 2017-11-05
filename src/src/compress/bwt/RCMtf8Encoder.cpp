/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "compress/bwt/RCMtf8Encoder.h"

/////////////////////////////////////////////////////////////////
//RCMtf8Encoder class implementation

BEGIN_NAMESPACE_RCZIP

RCMtf8Encoder::RCMtf8Encoder()
{
}

RCMtf8Encoder::~RCMtf8Encoder()
{
}

int32_t RCMtf8Encoder::FindAndMove(byte_t value)
{
    int32_t pos = 0 ;
    int32_t resPos = -1 ;
    for(pos = 0; pos < BUFFER_LEN ; ++pos)
    {
        if(m_buffer[pos] == value)
        {
            resPos = pos ;
            break ;
        }
    }
    if(resPos < 0)
    {
        return resPos ;
    }
    
    for (; pos >= 8; pos -= 8)
    {
      m_buffer[pos] = m_buffer[pos - 1];
      m_buffer[pos - 1] = m_buffer[pos - 2];
      m_buffer[pos - 2] = m_buffer[pos - 3];
      m_buffer[pos - 3] = m_buffer[pos - 4];
      m_buffer[pos - 4] = m_buffer[pos - 5];
      m_buffer[pos - 5] = m_buffer[pos - 6];
      m_buffer[pos - 6] = m_buffer[pos - 7];
      m_buffer[pos - 7] = m_buffer[pos - 8];
    }
    for (; pos > 0; --pos)
    {
        m_buffer[pos] = m_buffer[pos - 1] ;
    }
    m_buffer[0] = value ;
    return resPos ;
}

byte_t& RCMtf8Encoder::operator[](uint32_t index)
{
    RCZIP_ASSERT(index < BUFFER_LEN ) ;
    return m_buffer[index] ;
}

const byte_t RCMtf8Encoder::operator[](uint32_t index) const
{
    RCZIP_ASSERT(index < BUFFER_LEN ) ;
    return m_buffer[index] ;
}

END_NAMESPACE_RCZIP
