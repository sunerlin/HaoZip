/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

BEGIN_NAMESPACE_RCZIP

template <class T>
RCComPtr<T>::RCComPtr():
    m_ptr(NULL)
{
}

template <class T>
RCComPtr<T>::RCComPtr(T* p):
    m_ptr(p)
{
    if(m_ptr != NULL)
    {
        p->AddRef() ;
    }
}

template <class T>
RCComPtr<T>::RCComPtr(const RCComPtr<T>& rhs):
    m_ptr(rhs.m_ptr)
{
    if(m_ptr != NULL)
    {
        m_ptr->AddRef() ;
    }
}

template <class T>
RCComPtr<T>::~RCComPtr()
{
    if(m_ptr)
    {
        m_ptr->Release() ;
    }
}

template <class T>
void RCComPtr<T>::Release()
{
    if(m_ptr)
    {
        m_ptr->Release() ;
        m_ptr = NULL ;
    }
}

template <class T>
T** RCComPtr<T>::GetAddress()
{
    return &m_ptr ;
}

template <class T>
T* RCComPtr<T>::operator->() const 
{
    return m_ptr ;
}

template <class T>
T& RCComPtr<T>::operator *() const
{
    return *m_ptr ;
}

template <class T>
RCComPtr<T>& RCComPtr<T>::operator = (T* p)
{
    if(m_ptr != p)
    {
        if(p != NULL)
        {
            p->AddRef() ;
        }
        if (m_ptr)
        {
          m_ptr->Release() ;
        }
        m_ptr = p ;
    }
    return *this ;
}

template <class T>
RCComPtr<T>& RCComPtr<T>::operator=(const RCComPtr<T>& rhs)
{
    return (*this = rhs.m_ptr);
}

template <class T>
bool RCComPtr<T>::operator!() const
{
    return (m_ptr == NULL) ;
}

template <class T>
RCComPtr<T>::operator bool () const
{
    return (m_ptr != NULL) ;
}

template <class T>
void RCComPtr<T>::Attach(T* p)
{
    Release() ;
    m_ptr = p ;
}

template <class T>
T* RCComPtr<T>::Detach()
{
    T* pt = m_ptr ;
    m_ptr = NULL ;
    return pt ;
}

template <class T>
T* RCComPtr<T>::Get() const
{
    return m_ptr ;
}

END_NAMESPACE_RCZIP
