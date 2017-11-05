/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __ICoder_h
#define __ICoder_h 1

#include "interface/IUnknown.h"
#include "interface/ICodecInfo.h"
#include "common/RCProperty.h"
#include <vector>
#include <utility>

BEGIN_NAMESPACE_RCZIP

/** 接口ID定义
*/
enum
{
    IID_ICompressProgressInfo     = IID_ICODER_BASE,
    IID_ICompressCoder,
    IID_ICompressCoder2,
    IID_ICompressSetCoderProperties,
    IID_ICompressSetDecoderProperties2,
    IID_ICompressWriteCoderProperties,
    IID_ICompressGetInStreamProcessedSize,
    IID_ICompressSetCoderMt,
    IID_ICompressGetSubStreamSize,
    IID_ICompressSetInStream,
    IID_ICompressSetOutStream,
    IID_ICompressSetInStreamSize,
    IID_ICompressSetOutStreamSize,
    IID_ICompressFilter,
    IID_ICompressCodecsInfo,
    IID_ISetCompressCodecsInfo,
    IID_ICryptoProperties,
    IID_ICryptoResetInitVector,
    IID_ICryptoSetPassword,
    IID_ICryptoSetCRC,
    IID_IMtCompressProgressInfo,
    IID_ISetMultiThreadMode,
    
    IID_IBZip2ResumeDecoder,
    IID_ILzmaResumeDecoder,
    IID_IGZipResumeDecoder 
};

/** 前置声明
*/
class ISequentialInStream ;
class ISequentialOutStream ;

/** 压缩进度接口 
*/
class ICompressProgressInfo:
    public IUnknown
{
public:
    
    /** 接口ID
    */
    enum { IID = IID_ICompressProgressInfo } ;
    
public:
    
    /** 设置压缩进度
    @param [in] inSize 输入数据大小
    @param [in] outSize 输出数据大小
    @return 成功返回RC_S_OK, 否则返回错误码
    */
    virtual HResult SetRatioInfo(uint64_t inSize, uint64_t outSize) = 0 ;
    
protected:
    
    /** 默认析构函数
    */
    ~ICompressProgressInfo(){} ;
};

/** 多线程压缩解压进度接口 
*/
class IMtCompressProgressInfo:
    public IUnknown
{
public:
    
    /** 接口ID
    */
    enum { IID = IID_IMtCompressProgressInfo } ;
    
public:
    
    /** 设置压缩进度
    @param [in] index 当前流的下标
    @param [in] inSize 输入数据大小
    @param [in] outSize 输出数据大小
    @return 成功返回RC_S_OK, 否则返回错误码
    */
    virtual HResult SetRatioInfo(uint32_t index, uint64_t inSize, uint64_t outSize) = 0 ;
    
protected:
    
    /** 默认析构函数
    */
    ~IMtCompressProgressInfo(){} ;
};

/** 多线程属性设置接口
*/
class ISetMultiThreadMode:
    public IUnknown
{
public:
    
    /** 接口ID
    */
    enum { IID = IID_ISetMultiThreadMode } ;
    
public:
    
    /** 设置压缩解压多线程模式
    @param [in] isMultThread 是否为多线程
    @return 成功返回RC_S_OK, 否则返回错误码
    */
    virtual HResult SetMultiThread(bool isMultThread) = 0 ;
    
protected:
    
    /** 默认析构函数
    */
    ~ISetMultiThreadMode(){} ;
};

/** 压缩数据接口 
*/
class ICompressCoder:
    public IUnknown
{
public:
    
    /** 接口ID
    */
    enum { IID = IID_ICompressCoder } ;
    
public:
    
    /** 压缩/解压数据
    @param [in] inStream 数据输入流
    @param [in] outStream 数据输出流
    @param [in] inSize 输入数据大小
    @param [in] outSize 输出数据大小 
    @param [in] progress 进度回调接口
    @return 成功返回RC_S_OK, 否则返回错误码
    */
    virtual HResult Code(ISequentialInStream* inStream,
                         ISequentialOutStream* outStream, 
                         const uint64_t* inSize, 
                         const uint64_t* outSize,
                         ICompressProgressInfo* progress) = 0 ;

protected:
    
    /** 默认析构函数
    */
    ~ICompressCoder(){} ;
};

/** 高级压缩数据接口 
*/
class ICompressCoder2:
    public IUnknown
{
public:
    
    /** 接口ID
    */
    enum { IID = IID_ICompressCoder2 } ;
    
public:
    
    /** 输入流信息 
    */
    typedef std::pair<ISequentialInStream*,uint64_t> in_stream_data ;

    /** 输出流信息 
    */
    typedef std::pair<ISequentialOutStream*,uint64_t> out_stream_data ;

public:
    
    /** 压缩/解压数据
    @param [in] inStreams 输入流信息
    @param [in] outStreams 输出流信息
    @param [in] progress 进度回调接口
    @return 成功返回RC_S_OK, 否则返回错误码
    */
    virtual HResult Code(const std::vector<in_stream_data>& inStreams,
                         const std::vector<out_stream_data>& outStreams, 
                         ICompressProgressInfo* progress) = 0 ;
protected:
    
    /** 默认析构函数
    */
    ~ICompressCoder2(){} ;
};

/** 设置压缩编码属性 
*/
class ICompressSetCoderProperties:
    public IUnknown
{
public:
    
    /** 接口ID
    */
    enum { IID = IID_ICompressSetCoderProperties } ;
    
public:
    
    /** 设置压缩编码属性
    @param [in] propertyArray 压缩编码属性列表
    @return 成功返回RC_S_OK, 否则返回错误码
    */
    virtual HResult SetCoderProperties(const RCPropertyIDPairArray& propertyArray) = 0 ;
    
protected:
    
    /** 默认析构函数
    */
    ~ICompressSetCoderProperties(){} ;
};

/** 设置解压编码属性 
*/
class ICompressSetDecoderProperties2:
    public IUnknown
{
public:
    
    /** 接口ID
    */
    enum { IID = IID_ICompressSetDecoderProperties2 } ;
    
public:
    
    /** 设置解压属性
    @param [in] data 属性数据
    @param [in] size 数据长度
    @return 成功返回RC_S_OK, 否则返回错误码
    */
    virtual HResult SetDecoderProperties2(const byte_t* data, uint32_t size) = 0 ;
    
protected:
    
    /** 默认析构函数
    */
    ~ICompressSetDecoderProperties2(){} ;
};

/** 输出压缩编码属性 
*/
class ICompressWriteCoderProperties:
    public IUnknown
{
public:
    
    /** 接口ID
    */
    enum { IID = IID_ICompressWriteCoderProperties } ;
    
public:
    
    /** 将压缩编码属性写入输出流
    @param [in] outStream 输出流接口指针
    @return 成功返回RC_S_OK, 否则返回错误码
    */
    virtual HResult WriteCoderProperties(ISequentialOutStream* outStream) = 0 ;
    
protected:
    
    /** 默认析构函数
    */
    ~ICompressWriteCoderProperties(){} ;
};

/** 获取输入流读取的数据 
*/
class ICompressGetInStreamProcessedSize:
    public IUnknown
{
public:
    
    /** 接口ID
    */
    enum { IID = IID_ICompressGetInStreamProcessedSize } ;
    
public:
    
    /** 获取输入流读取的数据长度
    @param [out] size 返回数据长度
    @return 成功返回RC_S_OK, 否则返回错误码
    */
    virtual HResult GetInStreamProcessedSize(uint64_t& size) = 0 ;
    
protected:
    
    /** 默认析构函数
    */
    ~ICompressGetInStreamProcessedSize(){} ;
};

/** 多线程属性 
*/
class ICompressSetCoderMt:
    public IUnknown
{
public:
    
    /** 接口ID
    */
    enum { IID = IID_ICompressSetCoderMt } ;
    
public:
    
    /** 设置多线程个数
    @param [in] numThreads 线程个数
    @return 成功返回RC_S_OK, 否则返回错误码
    */
    virtual HResult SetNumberOfThreads(uint32_t numThreads) = 0 ;
    
protected:
    
    /** 默认析构函数
    */
    ~ICompressSetCoderMt(){} ;
};

/** 获取子流的大小
*/
class ICompressGetSubStreamSize:
    public IUnknown
{
public:
    
    /** 接口ID
    */
    enum { IID = IID_ICompressGetSubStreamSize } ;
    
public:
    
    /** 获取流大小
    @param [in] subStream 流的下标
    @param [out] size 流的大小
    @return 成功返回RC_S_OK, 否则返回错误码
    */
    virtual HResult GetSubStreamSize(uint64_t subStream, uint64_t& size) = 0 ;
    
protected:
    
    /** 默认析构函数
    */
    ~ICompressGetSubStreamSize(){} ;
};

/** 设置输入流
*/
class ICompressSetInStream:
    public IUnknown
{
public:
    
    /** 接口ID
    */
    enum { IID = IID_ICompressSetInStream } ;
    
public:
    
    /** 设置输入流
    @param [in] inStream 输入流接口指针
    @return 成功返回RC_S_OK, 否则返回错误码
    */
    virtual HResult SetInStream(ISequentialInStream* inStream) = 0 ;
    
    /** 释放输入流
    @return 成功返回RC_S_OK, 否则返回错误码
    */
    virtual HResult ReleaseInStream() = 0;
    
protected:
    
    /** 默认析构函数
    */
    ~ICompressSetInStream(){} ;
};

/** 设置输出流
*/
class ICompressSetOutStream:
    public IUnknown
{
public:
    
    /** 接口ID
    */
    enum { IID = IID_ICompressSetOutStream } ;
    
public:
    
    /** 设置输出流
    @param [in] outStream 输出流接口指针
    @return 成功返回RC_S_OK, 否则返回错误码
    */
    virtual HResult SetOutStream(ISequentialOutStream* outStream) = 0 ;
    
    /** 释放输出流
    @return 成功返回RC_S_OK, 否则返回错误码
    */
    virtual HResult ReleaseOutStream() = 0 ;
    
protected:
    
    /** 默认构造函数
    */
    ~ICompressSetOutStream(){} ;
};

/** 设置输入流的大小 
*/
class ICompressSetInStreamSize:
    public IUnknown
{
public:
    
    /** 接口ID
    */
    enum { IID = IID_ICompressSetInStreamSize } ;
    
public:
    
    /** 设置输入流大小
    @param [in] inSize 输入流大小
    @return 成功返回RC_S_OK, 否则返回错误码
    */
    virtual HResult SetInStreamSize(uint64_t inSize) = 0 ;
    
protected:
    
    /** 默认析构函数
    */
    ~ICompressSetInStreamSize(){} ;
};

/** 设置输出流的大小 
*/
class ICompressSetOutStreamSize:
    public IUnknown
{
public:
    
    /** 接口ID
    */
    enum { IID = IID_ICompressSetOutStreamSize } ;
    
public:
    
    /** 设置输出流大小
    @param [in] outSize 输出流大小
    @return 成功返回RC_S_OK, 否则返回错误码
    */
    virtual HResult SetOutStreamSize(const uint64_t* outSize) = 0 ;
    
protected:
    
    /** 默认析构函数
    */
    ~ICompressSetOutStreamSize(){} ;
};

/** 压缩数据过滤器接口 
*/
class ICompressFilter:
    public IUnknown
{
public:
    
    /** 接口ID
    */
    enum { IID = IID_ICompressFilter } ;
    
public:
    
    /** 初始化
    @return 成功返回RC_S_OK, 否则返回错误码
    */
    virtual HResult Init() = 0 ;
    
    /** 过滤数据
    @param [in,out] data 数据缓冲区
    @param [in] size 数据长度
    @return 实际处理数据的长度
    */
    virtual uint32_t Filter(byte_t* data, uint32_t size) = 0 ;
    
protected:
    
    /** 默认析构函数
    */
    ~ICompressFilter(){} ;
};

/** 编码解码器信息 
*/
class ICompressCodecsInfo:
    public IUnknown
{
public:
    
    /** 接口ID
    */
    enum { IID = IID_ICompressCodecsInfo } ;
    
public:
    /** 获取压缩编码解码器个数
    @param [out] numMethods 返回编码解码器个数
    @return 成功返回RC_S_OK, 否则返回错误码
    */
    virtual HResult GetNumberOfMethods(uint32_t& numMethods) const = 0 ;
    
    /** 获取编码解码信息
    @param [in] index 编码解码器下标，从0开始
    @param [out] spCodecInfo 编码解码器信息
    @return 成功返回RC_S_OK，否则返回错误号 
    */
    virtual HResult GetMethod(uint32_t index, ICodecInfoPtr& spCodecInfo) const = 0 ;
    
    /** 创建解码器
    @param [in] index 编码器下标，从0开始
    @param [in] iid 解码器的接口ID
    @param [out] coder 解码器的接口指针，引用计数加1
    @return 成功返回RC_S_OK，否则返回错误号 
    */
    virtual HResult CreateDecoder(uint32_t index, RC_IID iid, void** coder) const = 0 ;
    
    /** 创建编码器
    @param [in] index 编码器下标，从0开始
    @param [in] iid 编码器的接口ID
    @param [out] coder 编码器的接口指针，引用计数加1
    @return 成功返回RC_S_OK，否则返回错误号 
    */
    virtual HResult CreateEncoder(uint32_t index, RC_IID iid, void** coder) const = 0 ;
    
protected:
    
    /** 默认析构函数
    */
    ~ICompressCodecsInfo(){} ;
};

/** 设置编码解码器接口 
*/
class ISetCompressCodecsInfo:
    public IUnknown
{
public:
    
    /** 接口ID
    */
    enum { IID = IID_ISetCompressCodecsInfo } ;
    
public:
    /** 设置编码解码器管理接口指针
    @param [in] compressCodecsInfo 编码解码器指针
    @return 成功返回RC_S_OK，否则返回错误号 
    */
    virtual HResult SetCompressCodecsInfo(ICompressCodecsInfo* compressCodecsInfo) = 0 ;
    
protected:
    
    /** 默认析构函数
    */
    ~ISetCompressCodecsInfo(){} ;
};

/** 加密属性
*/
class ICryptoProperties:
    public IUnknown
{
public:
    
    /** 接口ID
    */
    enum { IID = IID_ICryptoProperties } ;
    
public:
    
    /** 设置加密的键数据
    @param [in] data 数据缓冲区
    @param [in] size 数据长度
    @return 成功返回RC_S_OK, 否则返回错误码
    */
    virtual HResult SetKey(const byte_t* data, uint32_t size) = 0 ;
    
    /** 设置加密初始化数据
    @param [in] data 数据缓冲区
    @param [in] size 数据长度
    @return 成功返回RC_S_OK, 否则返回错误码
    */
    virtual HResult SetInitVector(const byte_t *data, uint32_t size) = 0 ;
    
protected:
    
    /** 默认析构函数
    */
    ~ICryptoProperties(){} ;
};

/** 重置加密初始化数据 
*/
class ICryptoResetInitVector:
    public IUnknown
{
public:
    
    /** 接口ID
    */
    enum { IID = IID_ICryptoResetInitVector } ;
    
public:
    
    /** 重置加密初始化数据
    @return 成功返回RC_S_OK，否则返回错误号 
    */
    virtual HResult ResetInitVector() = 0 ;
    
protected:
    
    /** 默认析构函数
    */
    ~ICryptoResetInitVector(){} ;
};

/** 设置密码接口 
*/
class ICryptoSetPassword:
    public IUnknown
{
public:
    
    /** 接口ID
    */
    enum { IID = IID_ICryptoSetPassword } ;
    
public:
    
    /** 设置密码
    @param [in] data 数据缓冲区
    @param [in] size 数据长度
    @return 成功返回RC_S_OK，否则返回错误号 
    */
    virtual HResult CryptoSetPassword(const byte_t* data, uint32_t size) = 0 ;
    
protected:
    
    /** 默认析构函数
    */
    ~ICryptoSetPassword(){} ;
};

/** 设置CRC数据校验接口 
*/
class ICryptoSetCRC:
    public IUnknown
{
public:
    
    /** 接口ID
    */
    enum { IID = IID_ICryptoSetCRC } ;
    
public:
    
    /** 设置CRC值
    @param [in] crc CRC数值
    @return 成功返回RC_S_OK，否则返回错误号 
    */
    virtual HResult CryptoSetCRC(uint32_t crc) = 0 ;
    
protected:
    
    /** 默认析构函数
    */
    ~ICryptoSetCRC(){} ;
};

/** 压缩数据接口 
*/
class IBZip2ResumeDecoder:
    public IUnknown
{
public:
    
    /** 接口ID
    */
    enum { IID = IID_IBZip2ResumeDecoder } ;
    
public:
    
    /** 设置输入流
    @param [in] inStream 数据输入流
    @return 成功返回RC_S_OK, 否则返回错误码
    */
    virtual HResult SetInStream(ISequentialInStream* inStream) = 0 ;
    
    /** 释放输入流
    @return 成功返回RC_S_OK, 否则返回错误码
    */
    virtual HResult ReleaseInStream() = 0 ;
    
    /** 编码
    @param [in] outStream 数据输出流
    @param [out] isBZ 是否为BZip2编码
    @param [in]  progress 进度回调接口
    @return 成功返回RC_S_OK, 否则返回错误码
    */
    virtual HResult CodeResume(ISequentialOutStream* outStream, bool& isBZ, ICompressProgressInfo* progress) = 0 ;
    
    /** 获取输入流数据已处理长度
    @return 返回输入流处理的字节数
    */
    virtual uint64_t GetInputProcessedSize() = 0 ;
    
protected:
    
    /** 默认析构函数
    */
    ~IBZip2ResumeDecoder(){} ;
};

/** 压缩数据接口 
*/
class ILzmaResumeDecoder:
    public IUnknown
{
public:
    
    /** 接口ID
    */
    enum { IID = IID_ILzmaResumeDecoder } ;
    
public:
    
    /** 解码接口
    @param [in] outStream 输出流接口
    @param [in] outSize 输出流的数据大小
    @param [in] progress 进度回调接口
    @return 成功返回RC_S_OK, 否则返回错误码
    */
    virtual HResult CodeResume(ISequentialOutStream* outStream, 
                               const uint64_t* outSize, 
                               ICompressProgressInfo* progress) = 0 ;
                       
    
    /** 从输入流中读取数据
    @param [in] data 数据缓冲区
    @param [in] size 缓冲区的长度
    @param [out] processedSize 实际读取的长度
    @return 成功返回RC_S_OK, 否则返回错误码
    */
    virtual HResult ReadFromInputStream(void* data, uint32_t size, uint32_t* processedSize) = 0 ;
    
protected:
    
    /** 默认析构函数
    */
    ~ILzmaResumeDecoder(){} ;
};

/** 压缩数据接口 
*/
class IGZipResumeDecoder:
    public IUnknown
{
public:
    
    /** 接口ID
    */
    enum { IID = IID_IGZipResumeDecoder } ;
    
public:
    
    /** 解压编码
    @param [in] outStream  输出流
    @param [in] outSize 输出流大小
    @param [in] progress 进度接口
    @return 成功返回RC_S_OK, 否则返回错误码
    */
    virtual HResult CodeResume(ISequentialOutStream* outStream, const uint64_t* outSize, ICompressProgressInfo* progress) = 0 ;
    
    /** 初始化输入流
    @return 成功返回RC_S_OK, 否则返回错误码
    */
    virtual HResult InitInStream(bool needInit) = 0 ;
    
    /** 对其到字节
    */
    virtual void AlignToByte() = 0 ;
    
    /** 读取字节
    @return 返回读取到的字节值
    */
    virtual byte_t ReadByte() = 0 ;
    
    /** 是否已经读取到输出流的结束位置
    @return 如果独到流的结束返回true，否则返回false
    */
    virtual bool InputEofError() const = 0 ;
    
    /** 获取输入流已处理的大小
    @return 返回输入流处理的数据字节数
    */
    virtual uint64_t GetInputProcessedSize() const = 0 ;
    
protected:
    
    /** 默认析构函数
    */
    ~IGZipResumeDecoder(){} ;
};

/** 智能指针定义
*/
typedef RCComPtr<ICompressProgressInfo>             ICompressProgressInfoPtr ;
typedef RCComPtr<ICompressCoder>                    ICompressCoderPtr ;
typedef RCComPtr<ICompressCoder2>                   ICompressCoder2Ptr ;
typedef RCComPtr<ICompressSetCoderProperties>       ICompressSetCoderPropertiesPtr ;
typedef RCComPtr<ICompressSetDecoderProperties2>    ICompressSetDecoderProperties2Ptr ;
typedef RCComPtr<ICompressWriteCoderProperties>     ICompressWriteCoderPropertiesPtr ;
typedef RCComPtr<ICompressGetInStreamProcessedSize> ICompressGetInStreamProcessedSizePtr ;
typedef RCComPtr<ICompressSetCoderMt>               ICompressSetCoderMtPtr ;
typedef RCComPtr<ICompressGetSubStreamSize>         ICompressGetSubStreamSizePtr ;
typedef RCComPtr<ICompressSetInStream>              ICompressSetInStreamPtr ;
typedef RCComPtr<ICompressSetOutStream>             ICompressSetOutStreamPtr ;
typedef RCComPtr<ICompressSetInStreamSize>          ICompressSetInStreamSizePtr ;
typedef RCComPtr<ICompressSetOutStreamSize>         ICompressSetOutStreamSizePtr ;
typedef RCComPtr<ICompressFilter>                   ICompressFilterPtr ;
typedef RCComPtr<ICompressCodecsInfo>               ICompressCodecsInfoPtr ;
typedef RCComPtr<ISetCompressCodecsInfo>            ISetCompressCodecsInfoPtr ;
typedef RCComPtr<ICryptoProperties>                 ICryptoPropertiesPtr ;
typedef RCComPtr<ICryptoResetInitVector>            ICryptoResetInitVectorPtr ;
typedef RCComPtr<ICryptoSetPassword>                ICryptoSetPasswordPtr ;
typedef RCComPtr<ICryptoSetCRC>                     ICryptoSetCRCPtr ;
typedef RCComPtr<IMtCompressProgressInfo>           IMtCompressProgressInfoPtr ;
typedef RCComPtr<ISetMultiThreadMode>               ISetMultiThreadModePtr ;

typedef RCComPtr<IBZip2ResumeDecoder>               IBZip2ResumeDecoderPtr ;
typedef RCComPtr<ILzmaResumeDecoder>                ILzmaResumeDecoderPtr ;
typedef RCComPtr<IGZipResumeDecoder>                IGZipResumeDecoderPtr ;

END_NAMESPACE_RCZIP

#endif //__ICoder_h
