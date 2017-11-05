/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCInStreamWithCRC_h_
#define __RCInStreamWithCRC_h_ 1

#include "interface/IStream.h"
#include "interface/IUnknownImpl.h"

BEGIN_NAMESPACE_RCZIP

class RCInStreamWithCRC:
    public IUnknownImpl<IInStream>
{
public:

    /** 默认构造函数
    */
    RCInStreamWithCRC() ;
    
    /** 默认析构函数
    */
    ~RCInStreamWithCRC() ;
    
public:

    /** 初始化
    */
    void Init() ;

    /** 设置输入流
    */
    void SetStream(IInStream *stream) ;

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
    @return 如果成功则返回RC_S_OK，否则返回错误号
    */
    virtual HResult Read(void* data, uint32_t size, uint32_t* processedSize) ;
    
    /** 调整指针位置
    @param [in] offset 需要调整的偏移量
    @param [in] seekOrigin 调整的基准位置
    @return 如果成功则返回RC_S_OK，否则返回错误号
    @param [out] newPosition 调整后的文件偏移
    */
    virtual HResult Seek(int64_t offset, RC_STREAM_SEEK seekOrigin, uint64_t* newPosition) ;

    /** 根据ID查询接口
    @param [in] iid 被查询的接口ID
    @param [out] outObject 如果查询成功，则保存结果接口指针，并增加引用计数
    @return 如果成功则返回RC_S_OK，否则返回错误号
    */
    virtual HResult QueryInterface(RC_IID iid, void** outObject) ;

private:

    /** 输入流
    */
    IInStreamPtr m_stream ;

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

#endif //__RCInStreamWithCRC_h_
