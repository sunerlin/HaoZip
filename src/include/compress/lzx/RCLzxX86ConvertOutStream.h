/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCLzxX86ConvertOutStream_h_
#define __RCLzxX86ConvertOutStream_h_ 1

#include "interface/IStream.h"
#include "interface/IUnknownImpl.h"

BEGIN_NAMESPACE_RCZIP

/** Lzx 输出流
*/
class RCLzxX86ConvertOutStream:
    public IUnknownImpl<ISequentialOutStream>
{
public:

    /** 默认构造函数
    */
    RCLzxX86ConvertOutStream() ;
    
    /** 默认析构函数
    */
    ~RCLzxX86ConvertOutStream() ;

public:
    
    /** 设置输出流
    @param [in] outStream 输出流
    */
    void SetStream(ISequentialOutStream *outStream) ;
    
    /** 释放流
    */
    void ReleaseStream() ;
    
    /** 初始化
    @param [in] translationMode 转换模式
    @param [in] translationSize 转换大小
    */
    void Init(bool translationMode, uint32_t translationSize) ;
    
    /** Flush数据
    @return 成功返回RC_S_OK,否则返回错误号
    */
    HResult Flush() ;
    
    /** 写数据  
    @param [in] data 存放数据的缓冲区
    @param [in] size 缓冲区大小
    @param [out] processedSize 实际读取的大小
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult Write(const void* data, uint32_t size, uint32_t* processedSize) ;
    
private:
    
    /** 转换
    */
    void MakeTranslation() ;

private:
    
    /** 解压块大小
    */
    static const int s_kUncompressedBlockSize = 1 << 15 ;
    
private:
    
    /** 输出流
    */
    ISequentialOutStreamPtr m_stream ;
    
    /** 已经处理大小
    */
    uint32_t m_processedSize ;
    
    /** 位置
    */
    uint32_t m_pos ;
    
    /** 转换大小
    */
    uint32_t m_translationSize ;
    
    /** 释放转换模式
    */
    bool m_translationMode ;
    
    /** 缓存
    */
    byte_t m_buffer[s_kUncompressedBlockSize] ;
};

END_NAMESPACE_RCZIP

#endif //__RCLzxX86ConvertOutStream_h_
