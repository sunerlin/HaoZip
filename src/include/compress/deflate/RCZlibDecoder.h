/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCZlibDecoder_h_
#define __RCZlibDecoder_h_ 1

#include "interface/ICoder.h"
#include "interface/IStream.h"
#include "interface/IUnknownImpl.h"

BEGIN_NAMESPACE_RCZIP

class RCZlibOutStreamWithAdler ;
class RCDeflateCOMDecoder ;

/** Zlib 解码器
*/
class RCZlibDecoder:
    public IUnknownImpl<ICompressCoder>
{
public:

    /** 默认构造函数
    */
    RCZlibDecoder() ;
    
    /** 默认析构函数
    */
    ~RCZlibDecoder() ;
    
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

    /** 获取输入处理的字节大小
    @return 返回处理的字节大小
    */
    uint64_t GetInputProcessedSize() const ;
    
private:
    
    /** Zlib 输出流
    */
    RCZlibOutStreamWithAdler* m_adlerSpec;
    
    /** 序列化输出流
    */
    ISequentialOutStreamPtr m_spAdlerStream;  
    
    /** Deflate 解码器
    */
    RCDeflateCOMDecoder* m_deflateDecoderSpec;
    
    /** Deflate 解码器
    */
    ICompressCoderPtr m_spDeflateDecoder;
};

END_NAMESPACE_RCZIP

#endif //__RCZlibDecoder_h_
