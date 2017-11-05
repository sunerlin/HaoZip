/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCDeflateDecoder_h_
#define __RCDeflateDecoder_h_ 1

#include "interface/ICoder.h"
#include "interface/IUnknownImpl.h"
#include "compress/lz/RCLZOutWindow.h"
#include "common/RCInBuffer.h"
#include "common/RCBitlDecoder.h"
#include "compress/huffman/RCHuffmanDecoder.h"
#include "compress/deflate/RCDeflateDefs.h"

BEGIN_NAMESPACE_RCZIP

/** Deflate 解码器
*/
class RCDeflateDecoder:
    public IUnknownImpl7<ICompressCoder,
                         ICompressGetInStreamProcessedSize,
                         ICompressSetInStream,
                         ICompressSetOutStreamSize,
                         ISequentialInStream,
                         ICompressSetCoderProperties,
                         IGZipResumeDecoder
                        >
{
public:

    /** 默认构造函数
    */
    explicit RCDeflateDecoder(bool deflate64Mode, bool deflateNSIS = false) ;
    
    /** 默认析构函数
    */
    virtual ~RCDeflateDecoder() ;
    
public:
    
    /** 释放输出流
    */
    void ReleaseOutStream() ;
    
    /** 刷新缓存数据
    @return 成功返回RC_S_OK,否则返回错误号
    */
    HResult Flush() ;
        
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

    /** 获取输入流读取的数据长度
    @param [out] size 返回数据长度
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult GetInStreamProcessedSize(uint64_t& size) ;

    /** 设置输入流
    @param [in] inStream 输入流接口指针
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult SetInStream(ISequentialInStream* inStream) ;
    
    /** 释放输入流
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult ReleaseInStream() ;
    
    /** 设置输出流大小
    @param [in] outSize 输出流大小
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult SetOutStreamSize(const uint64_t* outSize) ;
    
    /** 读取数据 
    @param [out] data 存放数据的缓冲区
    @param [in] size 缓冲区大小
    @param [out] processedSize 实际读取的大小
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult Read(void* data, uint32_t size, uint32_t* processedSize) ;
    
    /** 设置压缩编码属性
    @param [in] propertyArray 压缩编码属性列表
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult SetCoderProperties(const RCPropertyIDPairArray& propertyArray) ;

public:
    
    /** 解压编码
    @param [in] outStream  输出流
    @param [in] outSize 输出流大小
    @param [in] progress 进度接口
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult CodeResume(ISequentialOutStream* outStream, const uint64_t* outSize, ICompressProgressInfo* progress);
    
    /** 初始化输入流
    @param [in] needInit 是否需要初始化
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult InitInStream(bool needInit) ;
    
    /** 对其到字节
    */
    virtual void AlignToByte() ;
    
    /** 读取字节
    @return 返回字节
    */
    virtual byte_t ReadByte() ;
    
    /** 是否已经读取到输出流的结束位置
    @return 返回是否已经读取到输出流的结束位置,是返回true,否则返回false
    */
    virtual bool InputEofError() const ;
    
    /** 获取输入流已处理的大小
    @return 返回输入流已处理的大小
    */
    virtual uint64_t GetInputProcessedSize() const ;
               
public:
    
    /** 是否zlib模式
    */
    bool m_zlibMode ;
    
    /** zlib Footer
    */
    byte_t m_zlibFooter[4] ;
    
private:
    
    /** 设置保存历史
    @param [in] keepHistory 保存历史
    */
    void SetKeepHistory(bool keepHistory) ;
    
    /** 读取数据位
    @param [in] numBits 位数
    @return 返回读取位数
    */
    uint32_t ReadBits(int32_t numBits) ;
    
    /** 解码表
    @param [in] values 数据指针
    @param [in] numSymbols 符号数
    @return 成功返回true,否则返回false
    */
    bool DeCodeLevelTable(byte_t* values, int32_t numSymbols);
    
    /** 取得解码表
    @return 成功返回true,否则返回false
    */
    bool ReadTables() ;    
    
    /** 解码
    @param [in] curSize 当前大小
    @return 成功返回RC_S_OK,否则返回错误号
    */
    HResult CodeSpec(uint32_t curSize) ;
    
    /** 解码实现函数
    @param [in] outStream 输出流
    @param [in] outSize 输出大小
    @param [in] progress 进度接口
    @return 成功返回RC_S_OK,否则返回错误号
    */
    HResult CodeRealImpl(ISequentialOutStream* outStream, 
                         const uint64_t* outSize,
                         ICompressProgressInfo* progress) ;
    
    /** 解码实现函数
    @param [in] outStream 输出流
    @param [in] outSize 输出大小
    @param [in] progress 进度接口
    @return 成功返回RC_S_OK,否则返回错误号
    */                 
    HResult CodeReal(ISequentialOutStream* outStream, 
                     const uint64_t* outSize,
                     ICompressProgressInfo* progress) ;
    
private:
    
    /** 输出数据流
    */
    RCLZOutWindow m_outWindowStream;
    
    /** 输入数据流
    */
    RCBitlDecoder<RCInBuffer> m_inBitStream;
    
    /** Huffman 解码器
    */
    RCHuffmanDecoder<RCDeflateDefs::s_kNumHuffmanBits, RCDeflateDefs::s_kFixedMainTableSize> m_mainDecoder;
        
    /** Huffman 解码器
    */
    RCHuffmanDecoder<RCDeflateDefs::s_kNumHuffmanBits, RCDeflateDefs::s_kFixedDistTableSize> m_distDecoder;
        
    /** Huffman 解码器
    */
    RCHuffmanDecoder<RCDeflateDefs::s_kNumHuffmanBits, RCDeflateDefs::s_kLevelTableSize> m_levelDecoder;
    
    /** 保存块大小
    */
    uint32_t m_storedBlockSize;
    
    /** 是否最终块
    */
    bool m_finalBlock;
    
    /** 是否存储模式
    */
    bool m_storedMode;
    
    /** 级别
    */
    uint32_t m_numDistLevels;
    
    /** 是否Deflate NSIS
    */
    bool m_deflateNSIS;
    
    /** 是否Deflate 64
    */
    bool m_deflate64Mode;
    
    /** 是否保存历史
    */
    bool m_keepHistory;
    
    /** 是否初始化输入流
    */
    bool m_needInitInStream ;
    
    /** 剩余长度
    */
    int32_t m_remainLen;
    
    /** rep0
    */
    uint32_t m_rep0;
    
    /** 是否需要读取表
    */
    bool m_needReadTable;
};

END_NAMESPACE_RCZIP

#endif //__RCDeflateDecoder_h_
