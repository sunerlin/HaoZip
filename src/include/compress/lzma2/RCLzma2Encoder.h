/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCLzma2Encoder_h_
#define __RCLzma2Encoder_h_ 1

#include "interface/ICoder.h"
#include "interface/IStream.h"
#include "interface/IUnknownImpl.h"
#include "algorithm/Lzma2Enc.h"

BEGIN_NAMESPACE_RCZIP

/** Lzma2 编码器
*/
class RCLzma2Encoder:
    public IUnknownImpl4<ICompressCoder,
                         ICompressSetOutStream,
                         ICompressSetCoderProperties,
                         ICompressWriteCoderProperties>
{
public:

    /** 默认构造函数
    */
    RCLzma2Encoder() ;

    /** 默认析构函数
    */
    ~RCLzma2Encoder() ;

public:

    /** 压缩/解压数据
    @param [in] inStream 数据输入流
    @param [in] outStream 数据输出流
    @param [in] inSize 输入数据大小
    @param [in] outSize 输出数据大小 
    @param [in] progress 进度回调接口
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult Code(ISequentialInStream* inStream,
                         ISequentialOutStream* outStream, 
                         const uint64_t* inSize, 
                         const uint64_t* outSize,
                         ICompressProgressInfo* progress) ;

    /** 设置输出流
    @param [in] outStream 输出流接口指针
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult SetOutStream(ISequentialOutStream* outStream) ;

    /** 释放输出流
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult ReleaseOutStream() ;

    /** 设置压缩编码属性
    @param [in] propertyArray 压缩编码属性列表
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult SetCoderProperties(const RCPropertyIDPairArray& propertyArray) ;

    /** 将压缩编码属性写入输出流
    @param [in] outStream 输出流接口指针
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult WriteCoderProperties(ISequentialOutStream* outStream) ;

private:
    
    /** 读取数据
    @param [in] object 源地址
    @param [in] data 目标地址
    @param [in] size 读取大小
    @return 返回读取字节数
    */
    static int32_t ReadData(void* object, void* data, size_t* size) ;
    
    /** 写数据
    @param [in] object 目标地址
    @param [in] data 源地址
    @param [in] size 写入大小
    @return 返回写入字节数
    */
    static size_t  WriteData(void* object, const void* data, size_t size) ;
    
    /** 大内存申请
    @param [in] p 指针
    @param [in] size 申请大小
    @return 返回申请内存地址
    */
    static void* SzBigAlloc(void* p, size_t size) ;
    
    /** 大内存释放
    @param [in] p 指针
    @param [in] address 内存地址
    */
    static void SzBigFree(void* p, void *address) ;
    
    /** 内存申请
    @param [in] p 指针
    @param [in] size 申请大小
    @return 返回申请内存地址
    */
    static void* SzAlloc(void* p, size_t size) ;
    
    /** 内存释放
    @param [in] p 指针
    @param [in] address 内存地址
    */
    static void SzFree(void* p, void *address) ;
    
    /** Res转HResult
    @param [in] res Res值
    @return 返回HResult
    */
    static HResult SResToHRESULT(result_t res) ;
    
    /** 压缩进度
    @param [in] pp 数据指针
    @param [in] inSize 输入大小
    @param [in] outSize 输出大小
    @return 返回result_t
    */
    static result_t CompressProgress(void* pp, uint64_t inSize, uint64_t outSize) ;
    
    /** 转为大写字符
    @param [in] c 待转字符
    @return 返回大写字符
    */
    static RCString::value_type GetUpperChar(RCString::value_type c) ;
        
    /** 匹配分析
    @param [in] s 字符串
    @param [in] btMode 模式
    @param [in] numHashBytes 哈希字节数
    @return 返回匹配结果
    */
    static int32_t ParseMatchFinder(const RCString::value_type* s, int32_t* btMode, int32_t* numHashBytes) ;

private:
    
    /** 序列化输入流
    */
    struct CSeqInStream
    {
        /** 输入流接口
        */
        ISeqInStream m_seqInStream;
        
        /** 输入流指针 
        */ 
        ISequentialInStream* m_realStream;
    };

    /** 序列化输出流
    */
    struct CSeqOutStream
    {
        /** 输出流接口
        */
        ISeqOutStream m_seqOutStream ;
        
        /** 输出流指针
        */
        ISequentialOutStreamPtr m_realStream ;
        
        /** 结果
        */
        HResult m_result ;
    };

private:
    
    /** 大内存申请
    */
    static ISzAlloc s_bigAlloc ;
    
    /** 内存申请
    */
    static ISzAlloc s_alloc ;

private:
    
    /** 编码器
    */
    CLzma2EncHandle m_encoder ;
    
    /** 输入流
    */
    CSeqInStream m_seqInStream ;
    
    /** 输出流
    */
    CSeqOutStream m_seqOutStream ;
};

END_NAMESPACE_RCZIP

#endif //__RCLzma2Encoder_h_
