/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RC7zCompressionMode_h_
#define __RC7zCompressionMode_h_ 1

#include "base/RCDefs.h"
#include "format/common/RCMethodProps.h"

BEGIN_NAMESPACE_RCZIP

struct RC7zArchiveMethodFull: 
    public RCArchiveMethod
{
    /** 输入流的个数
    */
    uint32_t m_numInStreams;

    /** 输出流的个数
    */
    uint32_t m_numOutStreams;

    /** 是否是简单编码
    */
    bool IsSimpleCoder() const
    {
        return (m_numInStreams == 1) && (m_numOutStreams == 1);
    }
};

struct RC7zBind
{
    /** 输入编码
    */
    uint32_t m_inCoder;

    /** 输入流
    */
    uint32_t m_inStream;

    /** 输出编码
    */
    uint32_t m_outCoder;

    /** 输出流
    */
    uint32_t m_outStream;
};

class RC7zCompressionMethodMode
{
public:

    /** 默认构造函数
    */
    RC7zCompressionMethodMode():
      m_passwordIsDefined(false),
      m_numThreads(1)
    {
    }

public:

    /** 是否是空
    */
    bool IsEmpty() const
    {
        return (m_methods.empty() && !m_passwordIsDefined);
    }

public:

    /** 压缩模式
    */
    RCVector<RC7zArchiveMethodFull> m_methods;

    /** 绑定信息
    */
    RCVector<RC7zBind> m_binds;

    /** 线程个数
    */
    uint32_t m_numThreads;

    /** 是否有密码
    */
    bool m_passwordIsDefined;

    /** 密码
    */
    RCString m_password;
};

END_NAMESPACE_RCZIP

#endif //__RC7zCompressionMode_h_
