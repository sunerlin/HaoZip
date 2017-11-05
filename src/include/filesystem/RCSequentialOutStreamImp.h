/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCSequentialOutStreamImp_h_
#define __RCSequentialOutStreamImp_h_ 1

#include "interface/IStream.h"
#include "interface/IUnknownImpl.h"
#include "common/RCDynamicBuffer.h"

BEGIN_NAMESPACE_RCZIP

class RCWriteBuffer ;

class RCSequentialOutStreamImp:
    public IUnknownImpl<ISequentialOutStream>
{
public:

    /** 默认构造函数
    */
    RCSequentialOutStreamImp() ;
    
    /** 默认析构函数
    */
    ~RCSequentialOutStreamImp() ;
    
public:
    
    /** 初始化
    */
    void Init() ;
    
    /** 获取大小
    @return 返回数据大小
    */
    size_t GetSize() const ;
    
    /** 获取缓冲区
    @return 返回缓冲区引用
    */
    const RCDynamicByteBuffer& GetBuffer() const ;
    
    /** 写数据  
    @param [in] data 存放数据的缓冲区
    @param [in] size 缓冲区大小
    @param [out] processedSize 实际读取的大小
    @return 成功返回 RC_S_OK, 失败返回错误码
    */
    virtual HResult Write(const void* data, uint32_t size, uint32_t* processedSize) ;
    
private:
    
    /** 写入操作数据缓冲区
    */
    RCWriteBuffer& m_writeBuffer ;
};

END_NAMESPACE_RCZIP

#endif //__RCSequentialOutStreamImp_h_
