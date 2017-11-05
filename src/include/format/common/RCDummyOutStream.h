/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCDummyOutStream_h_
#define __RCDummyOutStream_h_ 1

#include "interface/IStream.h"
#include "interface/IUnknownImpl.h"

BEGIN_NAMESPACE_RCZIP

class RCDummyOutStream:
    public IUnknownImpl<ISequentialOutStream>
{
public:

    /** 默认构造函数
    */
    RCDummyOutStream() ;
    
    /** 默认析构函数
    */
    ~RCDummyOutStream() ;

public:

    /** 设置流
    @param [in] outStream 输出流
    */
    void SetStream(ISequentialOutStream* outStream) ;

    /** 释放流
    */
    void ReleaseStream() ;

    /** 初始化
    */
    void Init() ;

    /** 获取大小
    @return 返回大小
    */
    uint64_t GetSize() const ;
    
    /** 写数据  
    @param [in] data 存放数据的缓冲区
    @param [in] size 缓冲区大小
    @param [out] processedSize 实际读取的大小
    @return 成功返回RC_S_OK，否则返回错误号
    */
    virtual HResult Write(const void* data, uint32_t size, uint32_t* processedSize) ;
    
private:

    /** 输出流
    */
    ISequentialOutStreamPtr m_stream ;

    /** 大小
    */
    uint64_t m_size ;
};

/** RCDummyOutStream智能指针
*/
typedef RCComPtr<RCDummyOutStream>  RCDummyOutStreamPtr ;

END_NAMESPACE_RCZIP

#endif //__RCDummyOutStream_h_
