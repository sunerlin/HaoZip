/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "crypto/hash/RCRandomGenerator.h"
#include "crypto/hash/RCSha1Context.h"
#include "thread/RCMutex.h"

#ifdef RCZIP_OS_WIN
    #include "base/RCWindowsDefs.h"
#else
    #include <unistd.h>
    #include <time.h>
    #include <sys/time.h>   
#endif

/////////////////////////////////////////////////////////////////
//RCRandomGeneratorImpl class implementation

// This is not very good random number generator.
// Please use it only for salt.
// First generated data block depends from timer and processID.
// Other generated data blocks depend from previous state
// Maybe it's possible to restore original timer value from generated value.

BEGIN_NAMESPACE_RCZIP

RCRandomGeneratorImpl::RCRandomGeneratorImpl():
    m_needInit(true)
{
#ifdef COMPRESS_MT
    m_lock = new RCMutex ;
#endif
}

RCRandomGeneratorImpl::~RCRandomGeneratorImpl()
{
#ifdef COMPRESS_MT
    if(m_lock)
    {
        delete m_lock ;
        m_lock = NULL ;
    }
#endif
}

void RCRandomGeneratorImpl::Init()
{
    RCSha1Context hash ;
    hash.Init() ;

#ifdef RCZIP_OS_WIN
    DWORD w = ::GetCurrentProcessId();
    hash.Update((const byte_t *)&w, sizeof(w));
    w = ::GetCurrentThreadId();
    hash.Update((const byte_t *)&w, sizeof(w));
#else
    pid_t pid = getpid();
    hash.Update((const byte_t *)&pid, sizeof(pid));
    pid = getppid();
    hash.Update((const byte_t *)&pid, sizeof(pid));
#endif

    for (int32_t i = 0; i < 1000; ++i)
    {
#ifdef RCZIP_OS_WIN
        LARGE_INTEGER v;
        if (::QueryPerformanceCounter(&v))
        {
            hash.Update((const byte_t *)&v.QuadPart, sizeof(v.QuadPart));
        }
        DWORD tickCount = ::GetTickCount();
        hash.Update((const byte_t *)&tickCount, sizeof(tickCount));
#else
        timeval v;
        if (gettimeofday(&v, 0) == 0)
        {
            hash.Update((const byte_t *)&v.tv_sec, sizeof(v.tv_sec));
            hash.Update((const byte_t *)&v.tv_usec, sizeof(v.tv_usec));
        }
        time_t v2 = time(NULL);
        hash.Update((const byte_t *)&v2, sizeof(v2));
#endif
        for (int32_t j = 0; j < 100; j++)
        {
            hash.Final(m_buff);
            hash.Init();
            hash.Update(m_buff, RCSha1Defs::kDigestSize);
        }
    }
    hash.Final(m_buff);
    m_needInit = false ;
}

void RCRandomGeneratorImpl::Generate(byte_t *data, uint32_t size)
{
#ifdef COMPRESS_MT
    RCMutexLock spLock(m_lock) ;
#endif
    if (m_needInit)
    {
        Init() ;
    }
    while (size > 0)
    {
        RCSha1Context hash ;
        hash.Init();
        hash.Update(m_buff, RCSha1Defs::kDigestSize);
        hash.Final(m_buff);
    
        hash.Init();
        uint32_t salt = 0xF672ABD1;
        hash.Update((const byte_t *)&salt, sizeof(salt));
        hash.Update(m_buff, RCSha1Defs::kDigestSize);
        byte_t buff[RCSha1Defs::kDigestSize];
        hash.Final(buff);
        for (unsigned int i = 0; i < RCSha1Defs::kDigestSize && size > 0; i++, size--)
        {
            *data++ = buff[i];
        }
    }
}

END_NAMESPACE_RCZIP
