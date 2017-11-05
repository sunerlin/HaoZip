/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

/////////////////////////////////////////////////////////////////
//RCDynamicBuffer class implementation

BEGIN_NAMESPACE_RCZIP

template <typename T> 
RCDynamicBuffer<T>::RCDynamicBuffer():
    RCBuffer<T>()
{
}

template <typename T> 
RCDynamicBuffer<T>::~RCDynamicBuffer()
{

}

template <typename T> 
RCDynamicBuffer<T>::RCDynamicBuffer(size_t size):
    RCBuffer<T>(size)
{
}

template <typename T> 
RCDynamicBuffer<T>::RCDynamicBuffer(const RCDynamicBuffer& from):
    RCBuffer<T>(from)
{
}

template <typename T> 
RCDynamicBuffer<T>& RCDynamicBuffer<T>::operator= (const RCDynamicBuffer& from)
{
    if(&from != this)
    {
        RCBuffer<T>::operator=(from) ;
    }
    return *this ;
}

template <typename T> 
void RCDynamicBuffer<T>::GrowLength(size_t size)
{
    size_t delta = 0 ;
    if(this->m_capacity > 64)
    {
        delta = this->m_capacity / 4 ;
    }
    else if (this->m_capacity > 8)
    {
        delta = 16 ;
    }
    else
    {
        delta = 4 ;
    }
    delta = std::max(delta, size) ;
    SetCapacity(this->m_capacity + delta) ;
}

template <typename T> 
void RCDynamicBuffer<T>::EnsureCapacity(size_t capacity)
{
    if (this->m_capacity < capacity)
    {
        GrowLength(capacity - this->m_capacity) ;
    }
}

END_NAMESPACE_RCZIP
