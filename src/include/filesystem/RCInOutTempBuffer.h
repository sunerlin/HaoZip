/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCInOutTempBuffer_h_
#define __RCInOutTempBuffer_h_ 1

#include "base/RCNonCopyable.h"
#include "base/RCString.h"
#include "filesystem/RCOutFile.h"
#include "filesystem/RCTempFile.h"

BEGIN_NAMESPACE_RCZIP

class ISequentialOutStream ;

class RCInOutTempBuffer:
    private RCNonCopyable
{
public:

    /** 默认构造函数
    */
    RCInOutTempBuffer() ;
    
    /** 默认析构函数
    */
    ~RCInOutTempBuffer() ;

public:
    
    /** 创建缓冲区
    */
    void Create() ;
    
    /** 写操作初始化
    */
    void InitWriting() ;
    
    /** 写入数据
    @param [in] data 数据地址
    @param [in] size 数据长度
    @return 成功返回true, 否则返回false
    */
    bool Write(const void* data, uint32_t size) ;
    
    /** 获取数据长度
    @return 返回数据长度
    */
    uint64_t GetDataSize() const ;
    
    /** 将缓冲区中的数据写入输出流
    @param [in] stream 输出流接口
    @return 成功返回RC_S_OK, 失败则返回错误码
    */
    HResult WriteToStream(ISequentialOutStream* stream) ;
    
private:
    
    /** 释放缓冲区
    */
    void Free(void) ;
    
    /** 数据写入文件
    @param [in] data 数据地址
    @param [in] size 数据长度
    @return 成功返回true, 否则返回false
    */
    bool WriteToFile(const void* data, uint32_t size) ;
    
private:
    
    /** 缓冲区地址
    */
    byte_t* m_buf ;
    
    /** 当前数据指针偏移
    */
    uint32_t m_bufPos ;
    
    /** 临时文件名
    */
    RCString m_tempFileName ;
    
    /** 临时文件是否已被创建
    */
    bool m_tempFileCreated ;
    
    /** 数据长度
    */
    uint64_t m_size ;
    
    /** 数据CRC校验值
    */
    uint32_t m_crc ;
    
    /** 临时文件管理器
    */
    RCTempFile m_tempFile ;
    
    /** 输出文件管理器
    */
    RCOutFile m_outFile ;
};

END_NAMESPACE_RCZIP

#endif //__RCInOutTempBuffer_h_
