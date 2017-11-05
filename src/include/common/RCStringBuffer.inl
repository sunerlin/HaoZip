/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

/////////////////////////////////////////////////////////////////
//RCStringBufferBase class implementation

BEGIN_NAMESPACE_RCZIP

template <class string_type>
RCStringBufferBase<string_type>::RCStringBufferBase(string_type* str):
    m_str(str),
    m_buffer(NULL),
    m_length(0)
{
}

template <class string_type>
RCStringBufferBase<string_type>::~RCStringBufferBase()
{
    if(m_buffer)
    {
        delete [] m_buffer ;
        m_buffer = NULL ;
    }
}

template <class string_type>
typename string_type::value_type* RCStringBufferBase<string_type>::GetBuffer(uint32_t minBufLength)
{
    if(m_buffer != NULL)
    {
        delete [] m_buffer ;
        m_buffer = NULL ;
    }
    if(minBufLength < 1)
    {
        return NULL ;
    }
    m_buffer = new string_type::value_type[minBufLength + 1] ;
    m_buffer[0] = 0 ;
    m_length = minBufLength ;
    return m_buffer ;
}

template <class string_type>
void RCStringBufferBase<string_type>::ReleaseBuffer()
{
    if(m_buffer != NULL)
    {
        ReleaseBuffer((uint32_t)RCStringUtil::StringLen(m_buffer)) ;
    }
}

template <class string_type>
void RCStringBufferBase<string_type>::ReleaseBuffer(uint32_t newLength) 
{
    if(m_buffer != NULL)
    {
        if(newLength > m_length)
        {
            newLength = m_length ;
        }
        if(newLength > 0)
        {
            m_buffer[newLength] = 0 ;
            if(m_str)
            {
                *m_str = m_buffer ;
            }
        }
        else
        {
            if(m_str)
            {
                m_str->clear() ;
            }
        }
        delete [] m_buffer ;
        m_buffer = NULL ;
    }
    m_length = 0 ;
}

END_NAMESPACE_RCZIP
