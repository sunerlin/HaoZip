/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RC7zSpecStream_h_
#define __RC7zSpecStream_h_ 1

#include "interface/IStream.h"
#include "interface/ICoder.h"
#include "interface/IUnknownImpl.h"

BEGIN_NAMESPACE_RCZIP

class RC7zSpecStream:
    public IUnknownImpl2<ISequentialInStream,
                         ICompressGetSubStreamSize
                        >
{
public:

    /** 默认构造函数
    */
    RC7zSpecStream() ;
    
    /** 默认析构函数
    */
    ~RC7zSpecStream() ;
    
public:

    /** 初始化
    @param [in] stream 输入流
    */
    void Init(ISequentialInStream* stream) ;

    /** 获取大小
    @return 返回大小
    */
    uint64_t GetSize() const ;
    
    /** 读取数据 
    @param [out] data 存放数据的缓冲区
    @param [in] size 缓冲区大小
    @param [out] processedSize 实际读取的大小
    @return 成功返回RC_S_OK，否则返回错误号
    */
    virtual HResult Read(void* data, uint32_t size, uint32_t* processedSize) ;

    /** 获取流大小
    @param [in] subStream 流的下标
    @param [out] size 流的大小
    @return 成功返回RC_S_OK，否则返回错误号
    */
    virtual HResult GetSubStreamSize(uint64_t subStream, uint64_t& size) ;
    
private:

    /** 输入流
    */
    ISequentialInStreamPtr m_stream ;

    /** 获取流大小的接口
    */
    ICompressGetSubStreamSizePtr m_spGetSubStreamSize ;

    /** 大小
    */
    uint64_t m_size ;
};

END_NAMESPACE_RCZIP

#endif //__RC7zSpecStream_h_
