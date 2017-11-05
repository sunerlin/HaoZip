/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

/////////////////////////////////////////////////////////////////
//RCBuffer class implementation

BEGIN_NAMESPACE_RCZIP

template <typename T> 
RCBuffer<T>::RCBuffer():
    m_capacity(0),
    m_data(NULL),
    m_dataLen(0)
{
}

template <typename T> 
RCBuffer<T>::~RCBuffer()
{
    if(m_data)
    {
        delete [] m_data ;
        m_data = NULL ;
    }
}

template <typename T> 
RCBuffer<T>::RCBuffer(size_t size):
    m_capacity(0),
    m_data(NULL),
    m_dataLen(0)
{
    SetCapacity(size) ;
}

template <typename T> 
RCBuffer<T>::RCBuffer(const RCBuffer& from):
    m_capacity(0),
    m_data(NULL),
    m_dataLen(0)
{
    *this = from ;
}

template <typename T> 
RCBuffer<T>& RCBuffer<T>::operator= (const RCBuffer& from)
{
    if(&from != this)
    {
        Free();
        if (from.m_capacity > 0)
        {
          SetCapacity(from.m_capacity);
          memmove(m_data, from.m_data, from.m_capacity * sizeof(T));
        }
    }
    return *this ;
}

template <typename T>
size_t RCBuffer<T>::GetCapacity() const
{
    return m_capacity ;
}

template <typename T>
void RCBuffer<T>::SetCapacity(size_t newCapacity)
{
    if (newCapacity == m_capacity)
    {
      return ;
    }
    T * newBuffer = NULL ;
    if (newCapacity > 0)
    {
        newBuffer = new T[newCapacity] ;
        if( (m_capacity > 0) && (m_data != NULL) )
        {
            memmove(newBuffer, m_data, std::min(m_capacity, newCapacity) * sizeof(T)) ;
        }
    }
    if(m_data)
    {
        delete []m_data ;
    }
    m_data = newBuffer ;
    m_capacity = newCapacity ;
}

template <typename T>
void RCBuffer<T>::Free() 
{
    if(m_data)
    {
        delete []m_data ;
        m_data = NULL ;
    }
    m_capacity = 0 ;
}

template <typename T>
T* RCBuffer<T>::data()
{
    return m_data ;
}

template <typename T>
const T* RCBuffer<T>::data() const
{
    return m_data ;
}

template <typename T>
void RCBuffer<T>::SetDataLength(size_t dataLen)
{
    m_dataLen = (dataLen > m_capacity) ? m_capacity : dataLen ;
}

template <typename T>
size_t RCBuffer<T>::GetDataLength(void) const
{
    return m_dataLen ;
}

template <typename T>
const T RCBuffer<T>::operator [] (size_t index) const
{
    RCZIP_ASSERT(index < m_capacity) ;
    return m_data[index] ;
}

template <class T>
bool operator==(const RCBuffer<T>& b1, const RCBuffer<T>& b2)
{
    if (b1.GetCapacity() != b2.GetCapacity())
    {
        return false;
    }
    for (size_t i = 0; i < b1.GetCapacity(); i++)
    {
        if (b1[i] != b2[i])
        {
            return false;
        }
    }
    return true;
}

END_NAMESPACE_RCZIP
