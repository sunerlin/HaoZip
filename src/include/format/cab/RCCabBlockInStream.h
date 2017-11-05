/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCCabBlockInStream_h_
#define __RCCabBlockInStream_h_ 1

#include "base/RCTypes.h"
#include "interface/IStream.h"
#include "interface/IUnknownImpl.h"

BEGIN_NAMESPACE_RCZIP

class RCCabBlockInStream : 
    public IUnknownImpl<ISequentialInStream>
{
public:

    /** 默认构造函数
    */
    RCCabBlockInStream();

    /** 默认析构函数
    */
    ~RCCabBlockInStream();

public:

    /** 创建
    */
    bool Create() ;

    /** 设置流
    @param [in] stream 输入流
    */
    void SetStream(ISequentialInStream* stream) ;

    /** 为新的文件夹初始化
    */
    void InitForNewFolder() ;

    /** 为新的块初始化
    */
    void InitForNewBlock() ;

    /** 读取数据 
    @param [out] data 存放数据的缓冲区
    @param [in] size 缓冲区大小
    @param [out] processedSize 实际读取的大小
    @return 成功返回RC_S_OK, 失败返回错误码
    */
    virtual HResult Read(void* data, uint32_t size, uint32_t* processedSize);

    /** 读之前的操作
    @param [in] packSize 解压前大小
    @param [in] unpackSize 解压后大小
    @return 成功返回RC_S_OK, 失败返回错误码
    */
    HResult PreRead(uint32_t& packSize, uint32_t& unpackSize);

public:
    
    /** 解压前总大小
    */
    uint32_t m_totalPackSize;

    /** 保留大小
    */
    uint32_t m_reservedSize;

    /** 是否数据错误
    */
    bool m_dataError;

    /** 是否是mszip
    */
    bool m_msZip;

private:

    /** 输入流
    */
    ISequentialInStreamPtr m_stream;

    /** 缓冲内存
    */
    byte_t* m_buffer;

    /** 位置
    */
    uint32_t m_pos;

    /** 大小
    */
    uint32_t m_size;

    /** 块大小
    */
    static const uint32_t s_blockSize = (1 << 16);
};

END_NAMESPACE_RCZIP

#endif //__RCCabBlockInStream_h_