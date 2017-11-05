/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCSequentialInStreamWithCRC_h_
#define __RCSequentialInStreamWithCRC_h_ 1

#include "interface/IStream.h"
#include "interface/IUnknownImpl.h"

BEGIN_NAMESPACE_RCZIP

class RCSequentialInStreamWithCRC:
    public IUnknownImpl<ISequentialInStream>
{
public:

    /** 默认构造函数
    */
    RCSequentialInStreamWithCRC() ;
    
    /** 默认析构函数
    */
    ~RCSequentialInStreamWithCRC() ;
    
public:

    /** 初始化
    */
    void Init() ;

    /** 设置流
    @param [in] stream 输入流
    */
    void SetStream(ISequentialInStream* stream) ;

    /** 释放流
    */
    void ReleaseStream() ;

    /** 获取crc
    @return 返回crc
    */
    uint32_t GetCRC() const ;

    /** 获取大小
    @return 返回大小
    */
    uint64_t GetSize() const ;

    /** 是否完成
    @return 是返回true，否则返回false
    */
    bool WasFinished() const ;
    
    /** 读取数据 
    @param [out] data 存放数据的缓冲区
    @param [in] size 缓冲区大小
    @param [out] processedSize 实际读取的大小
    @return 成功返回RC_S_OK，否则返回错误号
    */
    virtual HResult Read(void* data, uint32_t size, uint32_t* processedSize) ;
    
private:

    /** 输入流
    */
    ISequentialInStreamPtr m_stream ;

    /** 大小
    */
    uint64_t m_size ;

    /** crc
    */
    uint32_t m_crc ;

    /** 是否完成
    */
    bool m_wasFinished ;
};

END_NAMESPACE_RCZIP

#endif //__RCSequentialInStreamWithCRC_h_
