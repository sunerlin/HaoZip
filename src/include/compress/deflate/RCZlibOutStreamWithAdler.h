/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCZlibOutStreamWithAdler_h_
#define __RCZlibOutStreamWithAdler_h_ 1

#include "interface/IStream.h"
#include "interface/IUnknownImpl.h"

BEGIN_NAMESPACE_RCZIP

/** Zlib 输出流
*/
class RCZlibOutStreamWithAdler:
    public IUnknownImpl<ISequentialOutStream>
{
public:

    /** 默认构造函数
    */
    RCZlibOutStreamWithAdler() ;
    
    /** 默认析构函数
    */
    ~RCZlibOutStreamWithAdler() ;
    
public:
    
    /** 设置输出流
    @param [in] stream 输出流
    */
    void SetStream(ISequentialOutStream *stream) ;
    
    /** 释放流
    */
    void ReleaseStream() ;
    
    /** 初始化
    */
    void Init() ;
    
    /** 取得Adler
    @return 返回Adler
    */
    uint32_t GetAdler() const ;
    
    /** 写数据  
    @param [in] data 存放数据的缓冲区
    @param [in] size 缓冲区大小
    @param [out] processedSize 实际读取的大小
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult Write(const void* data, uint32_t size, uint32_t* processedSize) ;

private:
    
    /** Adler更新
    @param [in] adler
    @param [in] buf 数据缓存
    @param [in] size 大小
    @return 返回更新字节数
    */
    static uint32_t Adler32Update(uint32_t adler, const byte_t* buf, size_t size) ;    
    
private:
    
    /** 输出流
    */
    ISequentialOutStreamPtr m_stream ;
    
    /** Adler
    */
    uint32_t m_adler ;
};

END_NAMESPACE_RCZIP

#endif //__RCZlibOutStreamWithAdler_h_
