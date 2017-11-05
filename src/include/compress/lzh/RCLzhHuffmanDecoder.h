/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCLzhHuffmanDecoder_h_
#define __RCLzhHuffmanDecoder_h_ 1

#include "compress/lzh/RCLzhHuffmanDecoderImpl.h"
#include "compress/lz/RCLZOutWindow.h"
#include "interface/ICoder.h"
#include "interface/IUnknownImpl.h"
#include "common/RCBitmDecoder.h"
#include "common/RCInBuffer.h"

BEGIN_NAMESPACE_RCZIP

/** Lzh Huffman 解码器
*/
class RCLzhHuffmanDecoder:
    public IUnknownImpl2<ICompressCoder,
                         ICompressSetCoderProperties>
{
public:

    /** 默认构造函数
    */
    RCLzhHuffmanDecoder() ;
    
    /** 默认析构函数
    */
    ~RCLzhHuffmanDecoder() ;
    
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
    
    /** 设置压缩编码属性
    @param [in] propertyArray 压缩编码属性列表
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult SetCoderProperties(const RCPropertyIDPairArray& propertyArray) ;
    
private:

    /** 释放流
    */
    void ReleaseStreams() ;

    /** 读取位数据
    @param [in] numBits 位数
    @return 返回读取位数
    */
    uint32_t ReadBits(int32_t numBits) ;

    /** 读取级别表
    @return 成功返回RC_S_OK,否则返回错误号
    */
    HResult ReadLevelTable() ;

    /** 读取P表
    @param [in] numBits 位数 
    @return 成功返回RC_S_OK,否则返回错误号
    */
    HResult ReadPTable(int32_t numBits) ;

    /** 读取C表
    @return 成功返回RC_S_OK,否则返回错误号
    */
    HResult ReadCTable() ;
    
    /** 设置字典大小
    @param [in] numDictBits 字典位数 
    */
    void SetDictionary(int32_t numDictBits) ;

private:
    
    /** 压缩/解压数据
    @param [in] inStream 数据输入流
    @param [in] outStream 数据输出流
    @param [in] inSize 输入数据大小
    @param [in] outSize 输出数据大小 
    @param [in] progress 进度回调接口
    @throws 失败抛异常
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult CodeReal(ISequentialInStream* inStream,
                             ISequentialOutStream* outStream, 
                             const uint64_t* inSize, 
                             const uint64_t* outSize,
                             ICompressProgressInfo* progress) ;

private:

    /** 解码器释放
    */
    class CCoderReleaser
    {
    public:

        /** 是否需要Flush
        */
        bool m_needFlush ;

        /** 构造函数
        @param [in] coder 解码器指针 
        */
        CCoderReleaser(RCLzhHuffmanDecoder* coder) ;

        /** 析构函数
        */
        ~CCoderReleaser() ;

    private:

        /** 解码器指针
        */
        RCLzhHuffmanDecoder* m_coder;
    }; 

    friend class CCoderReleaser ;
    
private:

    /** 字典大小
    */
    int32_t m_numDictBits ;

    /** 输出流
    */
    RCLZOutWindow m_outWindowStream ;

    /** 位输入流
    */
    RCBitmDecoder<RCInBuffer> m_inBitStream ;    

    /** 解码器实现
    */
    RCLzhHuffmanDecoderImpl<RCLzhDecoderDefs::s_kNumLevelSymbols> m_levelHuffman ;

    /** 解码器实现
    */
    RCLzhHuffmanDecoderImpl<RCLzhDecoderDefs::s_kNumDistanceSymbols> m_pHuffmanDecoder ;

    /** 解码器实现
    */
    RCLzhHuffmanDecoderImpl<RCLzhDecoderDefs::s_kNumCSymbols> m_cHuffmanDecoder ;
};

END_NAMESPACE_RCZIP

#endif //__RCLzhHuffmanDecoder_h_

