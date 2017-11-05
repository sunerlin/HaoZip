/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCLimitedSequentialOutStream_h_
#define __RCLimitedSequentialOutStream_h_ 1

#include "interface/IStream.h"
#include "interface/IUnknownImpl.h"

BEGIN_NAMESPACE_RCZIP

class RCLimitedSequentialOutStream:
    public IUnknownImpl<ISequentialOutStream>
{
public:

    /** 默认构造函数
    */
    RCLimitedSequentialOutStream() ;
    
    /** 默认析构函数
    */
    ~RCLimitedSequentialOutStream() ;

public:
    
    /** 设置输出流接口
    @param [in] stream 输出流接口指针
    */
    void SetStream(ISequentialOutStream* stream) ;
    
    /** 释放输出流
    */
    void ReleaseStream() ;
    
    /** 初始化
    @param [in] size 输出流大小
    @param [in] overflowIsAllowed 是否允许数据溢出
    */
    void Init(uint64_t size, bool overflowIsAllowed = false) ;
    
    /** 是否已经完成
    @return 如果已经完成返回true, 否则返回false
    */
    bool IsFinishedOK() const ;
    
    /** 获取数据大小
    @return 返回数据大小
    */
    uint64_t GetRem() const ;
    
    /** 写数据  
    @param [in] data 存放数据的缓冲区
    @param [in] size 缓冲区大小
    @param [out] processedSize 实际读取的大小
    @return 成功返回RC_S_OK, 失败则返回错误码
    */
    virtual HResult Write(const void* data, uint32_t size, uint32_t* processedSize) ;
    
private:
    
    /** 输出流接口
    */
    ISequentialOutStreamPtr m_spOutStream ;
    
    /** 数据大小
    */
    uint64_t m_size ;
    
    /** 是否溢出
    */
    bool m_overflow ;
    
    /** 是否允许溢出
    */
    bool m_overflowIsAllowed ;
};

typedef RCComPtr<RCLimitedSequentialOutStream> RCLimitedSequentialOutStreamPtr ;

END_NAMESPACE_RCZIP

#endif //__RCLimitedSequentialOutStream_h_
