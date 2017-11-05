/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCLzxDecoder_h_
#define __RCLzxDecoder_h_ 1

#include "interface/ICoder.h"
#include "interface/IUnknownImpl.h"
#include "interface/IStream.h"
#include "compress/lz/RCLZOutWindow.h"
#include "compress/huffman/RCHuffmanDecoder.h"
#include "compress/lzx/RCLzxBitStreamDecoder.h"
#include "compress/lzx/RCLzxX86ConvertOutStream.h"
#include "compress/lzx/RCLzxDefs.h"

BEGIN_NAMESPACE_RCZIP

/** Lzx 解码器
*/
class RCLzxDecoder:
    public IUnknownImpl5<ICompressCoder,
                         IOutStreamFlush,
                         ICompressSetInStream,
                         ICompressSetOutStreamSize,
                         ICompressSetCoderProperties>
{
public:

    /** 构造函数
    @param [in] wimMode wim模式
    */
    explicit RCLzxDecoder(bool wimMode = false) ;
    
    /** 默认析构函数
    */
    ~RCLzxDecoder() ;

public:
    
    /** 释放流
    */
    void ReleaseStreams() ;    
        
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
    
    /** 清空缓冲区，并把缓存数据写入流
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult Flush() ;
    
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
    
    /** 设置压缩编码属性
    @param [in] propertyArray 压缩编码属性列表
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult SetCoderProperties(const RCPropertyIDPairArray& propertyArray) ;
    
private:
    
    /** 设置参数
    @param [in] numDictBits 字典位数
    @return 成功返回RC_S_OK,否则返回错误号
    */
    HResult SetParams(int32_t numDictBits) ;
    
    /** 设置是否保留历史
    @param [in] keepHistory  是否保留历史
    */
    void SetKeepHistory(bool keepHistory) ;
    
    /** 读取数据
    @param [in] numBits 数据位数
    @return 返回读取结果
    */
    uint32_t ReadBits(int32_t numBits) ;
    
    /** 读取表
    @param [in] lastLevels 最后级别
    @param [in] newLevels 新级别
    @param [in] numSymbols 符号数
    @return 成功返回true,否则返回false
    */
    bool ReadTable(byte_t* lastLevels, byte_t* newLevels, uint32_t numSymbols);
    
    /** 读取表
    @return 成功返回true,否则返回false
    */
    bool ReadTables();
    
    /** 清除先前级别
    */
    void ClearPrevLevels();
    
    /** 编码
    @param [in] size 大小
    @return 成功返回true,否则返回false
    */
    HResult CodeSpec(uint32_t size) ;
    
    /** 编码函数
    @param [in] inStream 输入流
    @param [in] outStream 输出流
    @param [in] inSize 输入大小
    @param [in] outSize 输出代销哦啊
    @param [in] progress 进度回调接口
    @return 成功返回true,否则返回false
    */
    HResult CodeReal(ISequentialInStream* inStream,
                     ISequentialOutStream* outStream, 
                     const uint64_t* inSize, 
                     const uint64_t* outSize,
                     ICompressProgressInfo* progress) ;
    
private:
    
    /** 输入流
    */
    RCLzxBitStreamDecoder m_inBitStream;
    
    /** 输出流
    */
    RCLZOutWindow m_outWindowStream;

    /** 重复长度
    */
    uint32_t m_repDistances[RCLzxDefs::s_kNumRepDistances];
        
    /** 位置长度
    */
    uint32_t m_numPosLenSlots;

    /** 是否是解压块
    */
    bool m_isUncompressedBlock;
    
    /** 是否对齐
    */
    bool m_alignIsUsed;

    /** 解码器
    */
    RCHuffmanDecoder<RCLzxDefs::s_kNumHuffmanBits, RCLzxDefs::s_kMainTableSize> m_mainDecoder;
        
    /** 解码器
    */
    RCHuffmanDecoder<RCLzxDefs::s_kNumHuffmanBits, RCLzxDefs::s_kNumLenSymbols> m_lenDecoder;
        
    /** 解码器
    */
    RCHuffmanDecoder<RCLzxDefs::s_kNumHuffmanBits, RCLzxDefs::s_kAlignTableSize> m_alignDecoder;
    
    /** 解码器
    */
    RCHuffmanDecoder<RCLzxDefs::s_kNumHuffmanBits, RCLzxDefs::s_kLevelTableSize> m_levelDecoder;

    /** 主级别
    */
    byte_t m_lastMainLevels[RCLzxDefs::s_kMainTableSize];
        
    /** 级别
    */
    byte_t m_lastLenLevels[RCLzxDefs::s_kNumLenSymbols];

    /** 输出流
    */
    RCLzxX86ConvertOutStream* m_x86ConvertOutStreamSpec;
    
    /** 输出流
    */
    ISequentialOutStreamPtr m_x86ConvertOutStream;

    /** 解压块大小
    */
    uint32_t m_unCompressedBlockSize;

    /** 是否保存历史
    */
    bool m_keepHistory;
    
    /** 保留长度
    */
    int32_t m_remainLen;
    
    /** 是否跳过字节
    */
    bool m_skipByte;
    
    /** wim模式
    */
    bool m_wimMode;
};

END_NAMESPACE_RCZIP

#endif //__RCLzxDecoder_h_
