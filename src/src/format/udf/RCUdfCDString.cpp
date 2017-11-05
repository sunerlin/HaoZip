/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#include "format/udf/RCUdfCDString.h"
#include "common/RCStringBuffer.h"

BEGIN_NAMESPACE_RCZIP

struct RCUdfParseStringImpl
{
    static RCString ParseDString(const byte_t* data, int32_t size)
    {
        RCString res;
        RCString::value_type* p = NULL ;
        RCStringBuffer lbuffer(&res);
        if ( (size > 0) && (data != NULL) )
        {
            byte_t type = data[0];
            if (type == 8)
            {
                p = lbuffer.GetBuffer((int32_t)size + 1);
                for (int32_t i = 1; i < size; i++)
                {
                    RCString::value_type c = data[i];
                    if (c == 0)
                    {
                        break;
                    }
                    *p++ = c;
                }
            }
            else if (type == 16)
            {
                p = lbuffer.GetBuffer((int32_t)size / 2 + 1);
                for (int32_t i = 1; i + 2 <= size; i += 2)
                {
                    RCString::value_type c = ((RCString::value_type)data[i] << 8) | data[i + 1];
                    if (c == 0)
                    {
                        break;
                    }
                    *p++ = c;
                }
            }
            else
            {
                return _T("[unknow]");
            }
            *p++ = 0;
            lbuffer.ReleaseBuffer();
        }
        return res;
    }
};

void RCUdfCDString128::Parse(const byte_t *buf)
{
    memcpy(m_data, buf, sizeof(m_data));
}

RCString RCUdfCDString128::GetString() const
{
    int32_t size = m_data[sizeof(m_data) - 1];
    return RCUdfParseStringImpl::ParseDString(m_data, MyMin(size, (int32_t)(sizeof(m_data) - 1)));
}

void RCUdfCDString::Parse(const byte_t *p, uint32_t size)
{
    m_data.SetCapacity(size);
    memcpy(m_data.data(), p, size);
}

RCString RCUdfCDString::GetString() const
{ 
    return RCUdfParseStringImpl::ParseDString(m_data.data(), (int32_t)m_data.GetCapacity()); 
}

END_NAMESPACE_RCZIP
