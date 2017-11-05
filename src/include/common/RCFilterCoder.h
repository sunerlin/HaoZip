/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCFilterCoder_h_
#define __RCFilterCoder_h_ 1

#include "interface/ICoder.h"
#include "interface/IPassword.h"
#include "interface/IStream.h"
#include "interface/IUnknownImpl.h"

BEGIN_NAMESPACE_RCZIP

class RCFilterCoder:
    public IUnknownImpl11<
                          ICompressCoder,
                          ICompressSetInStream,
                          ISequentialInStream,
                          ICompressSetOutStream,
                          ISequentialOutStream,
                          IOutStreamFlush,
                          ICryptoSetPassword,
                          ICompressSetCoderProperties,
                          ICompressWriteCoderProperties,
                          ICryptoResetInitVector,
                          ICompressSetDecoderProperties2
                         >
{
public:

    /** 默认构造函数
    */
    RCFilterCoder() ;
    
    /** 默认析构函数
    */
    ~RCFilterCoder() ;
    
public:
    
    /** 初始化
    @return 成功返回RC_S_OK, 失败返回错误码
    */
    HResult Init() ;
    
    /** 有限写入数据
    @param [in] outStream 输出流接口
    @param [in] size 期望写入的字节数
    @return 成功返回RC_S_OK, 失败返回错误码
    */
    HResult WriteWithLimit(ISequentialOutStream* outStream, uint32_t size) ;
    
    /** 是否需要写入更多数据
    */
    bool NeedMore() const ;
    
    /** 设置编码过滤器
    @param [in] filter 过滤器接口
    */
    void SetFilter(ICompressFilter* filter) ;
    
    /** 压缩数据
    @param [in] inStream 数据输入流
    @param [in] outStream 数据输出流
    @param [in] inSize 输入数据大小
    @param [in] outSize 输出数据大小 
    @param [in] progress 进度回调接口
    @return 成功返回RC_S_OK, 失败返回错误码
    */
    virtual HResult Code(ISequentialInStream* inStream,
                         ISequentialOutStream* outStream, 
                         const uint64_t* inSize, 
                         const uint64_t* outSize,
                         ICompressProgressInfo* progress) ;

    /** 设置输入流
    @param [in] inStream 输入流接口指针
    @return 成功返回RC_S_OK, 失败返回错误码
    */
    virtual HResult SetInStream(ISequentialInStream* inStream) ;
    
    /** 释放输入流
    @return 成功返回RC_S_OK, 失败返回错误码
    */
    virtual HResult ReleaseInStream() ;
    
    /** 读取数据 
    @param [out] data 存放数据的缓冲区
    @param [in] size 缓冲区大小
    @param [out] processedSize 实际读取的大小
    @return 成功返回RC_S_OK, 失败返回错误码
    */
    virtual HResult Read(void* data, uint32_t size, uint32_t* processedSize) ;
    
    /** 设置输出流
    @param [in] outStream 输出流接口指针
    @return 成功返回RC_S_OK, 失败返回错误码
    */
    virtual HResult SetOutStream(ISequentialOutStream* outStream) ;
    
    /** 释放输出流
    @return 成功返回RC_S_OK, 失败返回错误码
    */
    virtual HResult ReleaseOutStream() ;
    
    /** 写数据  
    @param [in] data 存放数据的缓冲区
    @param [in] size 缓冲区大小
    @param [out] processedSize 实际读取的大小
    @return 成功返回RC_S_OK, 失败返回错误码
    */
    virtual HResult Write(const void* data, uint32_t size, uint32_t* processedSize) ;
    
    /** 清空缓冲区，并把缓存数据写入流
    @return 成功返回RC_S_OK, 失败返回错误码
    */
    virtual HResult Flush() ;
    
    /** 设置密码
    @param [in] data 数据缓冲区
    @param [in] size 数据长度
    @return 成功返回RC_S_OK，否则返回错误号 
    */
    virtual HResult CryptoSetPassword(const byte_t* data, uint32_t size) ;
    
    /** 设置压缩编码属性
    @param [in] propertyArray 压缩编码属性列表
    @return 成功返回RC_S_OK, 失败返回错误码
    */
    virtual HResult SetCoderProperties(const RCPropertyIDPairArray& propertyArray) ;
    
    /** 将压缩编码属性写入输出流
    @param [in] outStream 输出流接口指针
    @return 成功返回RC_S_OK, 失败返回错误码
    */
    virtual HResult WriteCoderProperties(ISequentialOutStream* outStream) ;
    
    /** 重置加密初始化数据
    @return 成功返回RC_S_OK，否则返回错误号 
    @return 成功返回RC_S_OK, 失败返回错误码
    */
    virtual HResult ResetInitVector() ;
    
    /** 设置解压属性
    @param [in] data 属性数据
    @param [in] size 数据长度
    @return 成功返回RC_S_OK, 失败返回错误码
    */
    virtual HResult SetDecoderProperties2(const byte_t* data, uint32_t size);
    
    /** 根据ID查询接口
    @param [in] iid 被查询的接口ID
    @param [out] outObject 如果查询成功，则保存结果接口指针，并增加引用计数
    @return 如果成功则返回RC_S_OK，否则返回错误号
    */
    virtual HResult QueryInterface(RC_IID iid, void** outObject) ;
   
protected:
    
    /** 缓冲区地址
    */
    byte_t* m_buffer ;
    
    /** 输入流接口
    */
    ISequentialInStreamPtr m_spInStream ;
    
    /** 输出流接口
    */
    ISequentialOutStreamPtr m_spOutStream ;
    
    /** 缓冲区数据偏移
    */
    uint32_t m_bufferPos ;
    
    /** 已转换数据起始偏移
    */
    uint32_t m_convertedPosBegin ;
    
    /** 已转换数据结束偏移
    */
    uint32_t m_convertedPosEnd ;
    
    /** 是否定义预输出大小
    */
    bool m_outSizeIsDefined ;
    
    /** 预输出大小
    */
    uint64_t m_outSize ;
    
    /** 当前数据偏移
    */
    uint64_t m_nowPos64 ;
    
    /** 密码查询接口
    */
    ICryptoSetPasswordPtr m_spSetPassword;
    
    /** 编码属性设置接口
    */
    ICompressSetCoderPropertiesPtr m_spSetCoderProperties;
    
    /** 编码属性写入接口
    */
    ICompressWriteCoderPropertiesPtr m_spWriteCoderProperties;
    
    /** 加密InitVector重置接口
    */
    ICryptoResetInitVectorPtr m_spCryptoResetInitVector;
    
    /** 解码属性设置接口
    */
    ICompressSetDecoderProperties2Ptr m_spSetDecoderProperties;
    
    /** 编解码过滤器接口接口
    */
    ICompressFilterPtr m_spFilter ;
};

class RCInStreamReleaser
{
public:
    
    /** 默认构造函数
    */
    RCInStreamReleaser():
        m_filterCoder(NULL)
    {
    }
    
    /** 默认析构函数
    */
    ~RCInStreamReleaser()
    {
        if (m_filterCoder)
        {
            m_filterCoder->ReleaseInStream();
        }
    }
    
    /** 设置编码过滤器接口
    @param [in] filterCoder 编码过滤器接口
    */
    void SetFilterCoder(RCFilterCoder* filterCoder)
    {
        if (m_filterCoder && (m_filterCoder != filterCoder))
        {
            m_filterCoder->ReleaseInStream();
        }
        m_filterCoder = filterCoder ;
    }

private:
    
    /** 编码过滤器接口
    */
    RCFilterCoder* m_filterCoder ;
};

class RCOutStreamReleaser
{
public:
    
    /** 默认构造函数
    */
    RCOutStreamReleaser():
        m_filterCoder(NULL)
    {
    }
    
    /** 默认析构函数
    */
    ~RCOutStreamReleaser()
    {
        if (m_filterCoder)
        {
            m_filterCoder->ReleaseOutStream();
        }
    }
    
    /** 设置编码过滤器接口
    @param [in] filterCoder 编码过滤器接口
    */
    void SetFilterCoder(RCFilterCoder* filterCoder)
    {
        if (m_filterCoder && (filterCoder != m_filterCoder))
        {
            m_filterCoder->ReleaseOutStream();
        }
        m_filterCoder = filterCoder ;
    }

private:
    
    /** 编码过滤器接口
    */
    RCFilterCoder* m_filterCoder ;
};

END_NAMESPACE_RCZIP

#endif //__RCFilterCoder_h_
