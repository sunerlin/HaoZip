/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCLimitedInStream_h_
#define __RCLimitedInStream_h_ 1

#include "interface/IStream.h"
#include "interface/IUnknownImpl.h"

BEGIN_NAMESPACE_RCZIP

class RCLimitedInStream:
    public IUnknownImpl<IInStream>
{
public:

    /** 默认构造函数
    */
    RCLimitedInStream() ;
    
    /** 默认析构函数
    */
    ~RCLimitedInStream() ;
    
public:
    
    /** 读取数据 
    @param [out] data 存放数据的缓冲区
    @param [in] size 缓冲区大小
    @param [out] processedSize 实际读取的大小
    @return 成功返回RC_S_OK, 失败则返回错误码
    */
    virtual HResult Read(void* data, uint32_t size, uint32_t* processedSize) ;
    
    /** 调整指针位置
    @param [in] offset 需要调整的偏移量
    @param [in] seekOrigin 调整的基准位置
    @param [out] newPosition 调整后的文件偏移
    @return 成功返回RC_S_OK, 失败则返回错误码
    */
    virtual HResult Seek(int64_t offset, RC_STREAM_SEEK seekOrigin, uint64_t* newPosition) ;

public:
    
    /** 设置输入流接口
    @param [in] stream 输入流接口指针
    */
    void SetStream(IInStream* stream) ;
    
    /** 初始化并移动到指定偏移
    @param [in] startOffset 起始偏移
    @param [in] size 流数据大小
    @return 成功返回RC_S_OK, 失败则返回错误码
    */
    HResult InitAndSeek(uint64_t startOffset, uint64_t size) ;
    
    /** 移动到起始位置
    @return 成功返回RC_S_OK, 失败则返回错误码
    */
    HResult SeekToStart() ;
    
private:
    
    /** 移动到当前位置对应的物理偏移位置
    @return 成功返回RC_S_OK, 失败则返回错误码
    */
    HResult SeekToPhys() ;
        
private:
    
    /** 输出流接口
    */
    IInStreamPtr m_spStream ;
    
    /** 逻辑位置
    */
    uint64_t m_virtPos;
    
    /** 物理位置
    */
    uint64_t m_physPos;
    
    /** 数据长度
    */
    uint64_t m_size;
    
    /** 起始偏移
    */
    uint64_t m_startOffset;
};

/** 智能指针定义
*/
typedef RCComPtr<RCLimitedInStream>   RCLimitedInStreamPtr ;

END_NAMESPACE_RCZIP

#endif //__RCLimitedInStream_h_
