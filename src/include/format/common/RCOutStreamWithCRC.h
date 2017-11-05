/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCOutStreamWithCRC_h_
#define __RCOutStreamWithCRC_h_ 1

#include "interface/IStream.h"
#include "interface/IUnknownImpl.h"

BEGIN_NAMESPACE_RCZIP

class RCOutStreamWithCRC:
    public IUnknownImpl<ISequentialOutStream>
{
public:

    /** 默认构造函数
    */
    RCOutStreamWithCRC() ;
    
    /** 默认析构函数
    */
    ~RCOutStreamWithCRC() ;
    
public:

    /** 设置输入流
    @param [in] stream 输入流
    */
    void SetStream(ISequentialOutStream* stream) ;

    /** 释放流
    */
    void ReleaseStream() ;

    /** 初始化
    @param [in] calculate 是否计算crc
    */
    void Init(bool calculate = true) ;

    /** 初始化crc
    */
    void InitCRC() ;

    /** 获取大小
    @return 返回大小
    */
    uint64_t GetSize() const ;

    /** 获取crc
    @return 返回crc
    */
    uint32_t GetCRC() const ;
    
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
    ISequentialOutStreamPtr m_stream;

    /** 大小
    */
    uint64_t m_size;

    /** crc
    */
    uint32_t m_crc;

    /** 是否计算crc
    */
    bool m_calculate;
};

END_NAMESPACE_RCZIP

#endif //__RCOutStreamWithCRC_h_
