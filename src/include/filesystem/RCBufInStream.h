/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCBufInStream_h_
#define __RCBufInStream_h_ 1

#include "interface/IStream.h"
#include "interface/IUnknownImpl.h"
#include "common/RCBuffer.h"

BEGIN_NAMESPACE_RCZIP

struct RCReferenceBuf:
    public IUnknown,
    public RCRefCounted
{
public:
    
    /** 数据缓冲区
    */
    RCByteBuffer m_buf ;

public:
    /** 根据ID查询接口
    @param [in] iid 被查询的接口ID
    @param [out] outObject 如果查询成功，则保存结果接口指针，并增加引用计数
    @return 如果成功则返回RC_S_OK，否则返回错误号
    */
    virtual HResult QueryInterface(RC_IID iid, void** outObject)
    {
        if(outObject == NULL)
        {
            return RC_E_INVALIDARG ;
        }
        switch(iid)
        {
            case IUnknown::IID:
                {
                    *outObject = (IUnknown*)this ;
                }
                break ;
            default:
                return RC_E_NOINTERFACE ;
                break ;                
        }
        this->AddRef() ;
        return RC_S_OK ;
    }
    
    /** 增加引用计数
    */
    virtual void AddRef(void)
    {
        RCRefCounted::Increase() ;
    }
    
    /** 减少引用计数
    */
    virtual void Release(void)
    {
        RCRefCounted::Decrease() ;
    }    
};

class RCBufInStream:
    public IUnknownImpl<IInStream>
{
public:

    /** 默认构造函数
    */
    RCBufInStream() ;
    
    /** 默认析构函数
    */
    ~RCBufInStream() ;
    
    /** 初始化数据缓冲区
    @param [in] data 缓冲区起始地址
    @param [in] size 数据长度
    */
    void Init(const byte_t* data, uint64_t size) ;
    
    /** 初始化数据缓冲区
    @param [in] ref 含引用计数的缓冲区指针
    */
    void Init(RCReferenceBuf* ref) ;
    
    /** 读取数据 
    @param [out] data 存放数据的缓冲区
    @param [in] size 缓冲区大小
    @param [out] processedSize 实际读取的大小
    @return 成功返回RC_S_OK, 失败返回错误码
    */
    virtual HResult Read(void* data, uint32_t size, uint32_t* processedSize) ;
    
    /** 调整指针位置
    @param [in] offset 需要调整的偏移量
    @param [in] seekOrigin 调整的基准位置
    @param [out] newPosition 调整后的文件偏移
    @return 成功返回RC_S_OK, 失败返回错误码
    */
    virtual HResult Seek(int64_t offset, RC_STREAM_SEEK seekOrigin, uint64_t* newPosition) ;
    
private:
    
    /** 缓冲区地址
    */
    const byte_t* _data ;
    
    /** 缓冲区大小
    */
    uint64_t _size ;
    
    /** 当前位置偏移
    */
    uint64_t _pos ;
    
    /** 含引用计数的缓冲区接口指针
    */
    IUnknownPtr _ref ;
};

END_NAMESPACE_RCZIP

#endif //__RCBufInStream_h_
