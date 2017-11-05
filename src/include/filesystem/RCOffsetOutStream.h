/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCOffsetOutStream_h_
#define __RCOffsetOutStream_h_ 1

#include "interface/IStream.h"
#include "interface/IUnknownImpl.h"

BEGIN_NAMESPACE_RCZIP

class RCOffsetOutStream:
    public IUnknownImpl<IOutStream>
{
public:

    /** 默认构造函数
    */
    RCOffsetOutStream() ;
    
    /** 默认析构函数
    */
    ~RCOffsetOutStream() ;
    
    /** 初始化
    @param [in] stream 输出流接口指针
    @param [in] offset 输出流的偏移量
    @return 成功返回RC_S_OK, 失败返回错误号
    */
    HResult Init(IOutStream* stream, uint64_t offset);
    
    /** 写数据  
    @param [in] data 存放数据的缓冲区
    @param [in] size 缓冲区大小
    @param [out] processedSize 实际读取的大小
    @return 成功返回RC_S_OK, 失败返回错误号
    */
    virtual HResult Write(const void* data, uint32_t size, uint32_t* processedSize) ;
    
    /** 调整指针位置
    @param [in] offset 需要调整的偏移量
    @param [in] seekOrigin 调整的基准位置
    @param [out] newPosition 调整后的文件偏移
    @return 成功返回RC_S_OK, 失败返回错误号
    */
    virtual HResult Seek(int64_t offset, RC_STREAM_SEEK seekOrigin, uint64_t* newPosition) ;
    
    /** 调整大小
    @param [in] newSize 新的大小 
    @return 成功返回RC_S_OK, 失败返回错误号
    */
    virtual HResult SetSize(uint64_t newSize) ;
    
private:
    
    /** 偏移值
    */
    uint64_t m_offset;
    
    /** 输出流接口
    */
    IOutStreamPtr m_spOutStream;
};

END_NAMESPACE_RCZIP

#endif //__RCOffsetOutStream_h_
