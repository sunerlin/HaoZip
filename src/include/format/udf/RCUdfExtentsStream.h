/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCUdfExtentsStream_h_
#define __RCUdfExtentsStream_h_ 1

#include "base/RCTypes.h"
#include "interface/IStream.h"
#include "interface/IUnknownImpl.h"
#include "format/udf/RCUdfIn.h"
#include "common/RCVector.h"

BEGIN_NAMESPACE_RCZIP

/** Udf Seek
*/
struct RCUdfSeekExtent
{
    /** 物理
    */
    uint64_t m_phy ;

    /** 虚拟
    */
    uint64_t m_virt ;
};

/** Udf Extents Stream
*/
class RCUdfExtentsStream:
    public IUnknownImpl<IInStream>
{
public:
    
    /** 默认构造函数
    */
    RCUdfExtentsStream() ;
    
    /** 初始化
    */
    void Init();
    
    /** 释放流
    */
    void ReleaseStream() ;

public:
    
    /** 读取数据 
    @param [out] data 存放数据的缓冲区
    @param [in] size 缓冲区大小
    @param [out] processedSize 实际读取的大小
    @return 成功返回RC_S_OK，否则返回错误号
    */
    virtual HResult Read(void* data, uint32_t size, uint32_t* processedSize) ;
    
    /** 调整指针位置
    @param [in] offset 需要调整的偏移量
    @param [in] seekOrigin 调整的基准位置
    @param [out] newPosition 调整后的文件偏移
    @return 成功返回RC_S_OK，否则返回错误号
    */
    virtual HResult Seek(int64_t offset, RC_STREAM_SEEK seekOrigin, uint64_t* newPosition) ;

public:
    
    /** 对应文件的输入流
    */
    IInStreamPtr m_stream;
    
    /** 虚拟位置与文件中真实位置的映射表
    */
    RCVector<RCUdfSeekExtent> m_extents ;
    
private:

    /** 定位到物理偏移位置
    */
    HResult SeekToPhys() ;
       
private:

    /** 物理位置
    */
    uint64_t m_phyPos ;

    /** 虚拟位置
    */
    uint64_t m_virtPos ;

    /** 是否Seek
    */
    bool m_needStartSeek ;
};

END_NAMESPACE_RCZIP

#endif //__RCUdfExtentsStream_h_
