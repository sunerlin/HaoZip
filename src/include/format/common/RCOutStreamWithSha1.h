/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCOutStreamWithSha1_h_
#define __RCOutStreamWithSha1_h_ 1

#include "interface/IStream.h"
#include "interface/IUnknownImpl.h"

BEGIN_NAMESPACE_RCZIP

class RCSha1Context ;

class RCOutStreamWithSha1:
    public IUnknownImpl<ISequentialOutStream>
{
public:

    /** 默认构造函数
    */
    RCOutStreamWithSha1() ;
    
    /** 默认析构函数
    */
    ~RCOutStreamWithSha1() ;
    
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

    /** 初始化sha1
    */
    void InitSha1() ;

    /** 获取大小
    @return 返回大小
    */
    uint64_t GetSize() const ;

    /** 完成
    */
    void Final(byte_t* digest) ;
    
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
    ISequentialOutStreamPtr m_spStream ;

    /** 大小
    */
    uint64_t m_size ;

    /** sha
    */
    RCSha1Context& m_sha ;

    /** 是否计算crc
    */
    bool m_calculate ;
};

END_NAMESPACE_RCZIP

#endif //__RCOutStreamWithSha1_h_
