/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCSequentialOutStreamSizeCount_h_
#define __RCSequentialOutStreamSizeCount_h_ 1

#include "interface/IStream.h"
#include "interface/IUnknownImpl.h"

BEGIN_NAMESPACE_RCZIP

class RCSequentialOutStreamSizeCount:
    public IUnknownImpl<ISequentialOutStream>
{
public:

    /** 默认构造函数
    */
    RCSequentialOutStreamSizeCount() ;
    
    /** 默认析构函数
    */
    ~RCSequentialOutStreamSizeCount() ;
    
public:
    
    /** 设置输出流接口
    @param [in] stream 输出流接口指针
    */
    void SetStream(ISequentialOutStream* stream) ;
    
    /** 初始化
    */
    void Init() ;
    
    /** 获取数据大小
    @return 返回数据大小
    */
    uint64_t GetSize() const ;
    
    /** 写数据  
    @param [in] data 存放数据的缓冲区
    @param [in] size 缓冲区大小
    @param [out] processedSize 实际读取的大小
    @return 成功返回 RC_S_OK, 失败返回错误码
    */
    virtual HResult Write(const void* data, uint32_t size, uint32_t* processedSize) ;
    
private:
    
    /** 输出流接口
    */
    ISequentialOutStreamPtr m_spOutStream ;
    
    /** 数据大小
    */
    uint64_t m_size ;
};

END_NAMESPACE_RCZIP

#endif //__RCSequentialOutStreamSizeCount_h_
