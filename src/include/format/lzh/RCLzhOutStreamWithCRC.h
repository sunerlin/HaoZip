/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCLzhOutStreamWithCRC_h_
#define __RCLzhOutStreamWithCRC_h_ 1

#include "interface/IStream.h"
#include "interface/IUnknownImpl.h"
#include "RCLzhCRC.h"

BEGIN_NAMESPACE_RCZIP

class RCLzhOutStreamWithCRC:
    public IUnknownImpl<ISequentialOutStream>
{
public:

    /** 写数据  
    @param [in] data 存放数据的缓冲区
    @param [in] size 缓冲区大小
    @param [out] processedSize 实际写入的大小
    @return 成功返回RC_S_OK，否则返回错误号
    */
    virtual HResult Write(const void* data, uint32_t size, uint32_t* processedSize) ;

public:

    /** 初始化
    @param [in] stream 输入流
    */
    void Init(ISequentialOutStream *stream)
    {
        m_stream = stream;
        m_crc.Init();
    }

    /** 释放流
    */
    void ReleaseStream()
    {
        m_stream.Release();
    }

    /** 获取crc
    @return 返回crc
    */
    uint32_t GetCRC() const
    {
        return m_crc.GetDigest();
    }

    /** 初始化crc
    */
    void InitCRC()
    {
        m_crc.Init();
    }

private:

    /** crc
    */
    RCLzhCRC m_crc;

    /** 输出流
    */
    ISequentialOutStreamPtr m_stream;
};

END_NAMESPACE_RCZIP

#endif //__RCLzhOutStreamWithCRC_h_
