/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCClusterInStream_h_
#define __RCClusterInStream_h_ 1

#include "interface/IStream.h"
#include "interface/IUnknownImpl.h"
#include "common/RCVector.h"

BEGIN_NAMESPACE_RCZIP

class RCClusterInStream:
    public IUnknownImpl<IInStream>
{
public:

    /** 默认构造函数
    */
    RCClusterInStream() ;
    
    /** 默认析构函数
    */
    ~RCClusterInStream() ;
    
public:
    
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
    @param [out] newPosition 调整后的文件偏移
    @return 如果成功则返回RC_S_OK，否则返回错误号
    */
    virtual HResult Seek(int64_t offset, RC_STREAM_SEEK seekOrigin, uint64_t* newPosition) ;

public:
    
    /** 移动到指定物理位置
    @return 如果成功则返回RC_S_OK，否则返回错误号
    */
    HResult SeekToPhys() ;
    
    /** 初始化并定位文件偏移
    @return 如果成功则返回RC_S_OK，否则返回错误号
    */
    HResult InitAndSeek() ;
    
public:
    
    /** 输入流接口
    */
    IInStreamPtr m_stream ;
    
    /** 起始偏移位置
    */
    uint64_t m_startOffset ;
    
    /** 大小
    */
    uint64_t m_size ;
    
    /** 逻辑块地址
    */
    int32_t m_blockSizeLog ;
    
    /** 位置向量
    */
    RCVector<uint32_t> m_vector ;
    
private:
    
    /** 虚拟位置
    */
    uint64_t m_virtPos;
    
    /** 物理位置
    */
    uint64_t m_physPos;
    
    /** 当前位置
    */
    uint32_t m_curRem;
};

/** 智能指针定义
*/
typedef RCComPtr<RCClusterInStream>   RCClusterInStreamPtr ;

END_NAMESPACE_RCZIP

#endif //__RCClusterInStream_h_
