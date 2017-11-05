/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCRar2Decoder_h_
#define __RCRar2Decoder_h_ 1

#include "interface/ICoder.h"
#include "interface/IUnknownImpl.h"
#include "common/RCBitmDecoder.h"
#include "common/RCInBuffer.h"
#include "compress/lz/RCLZOutWindow.h"
#include "compress/huffman/RCHuffmanDecoder.h"
#include "compress/rar/RCRar2Filter2.h"

BEGIN_NAMESPACE_RCZIP

/** RAR2 解码器
*/
class RCRar2Decoder:
    public IUnknownImpl2<ICompressCoder,
                         ICompressSetDecoderProperties2
                        >
{
public:

    /** 默认构造函数
    */
    RCRar2Decoder() ;
    
    /** 默认析构函数
    */
    ~RCRar2Decoder() ;
    
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

    /** 设置解压属性
    @param [in] data 属性数据
    @param [in] size 数据长度
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult SetDecoderProperties2(const byte_t* data, uint32_t size) ;
    
private:

    /** 初始化
    */
    void InitStructures() ; 

    /** 位读取
    @param [in] numBits 位数
    @return 返回读取结果
    */
    uint32_t ReadBits(int32_t numBits) ;

    /** 读取表
    @return 成功返回true,否则返回false
    */
    bool ReadTables();

    /** 读取表
    @return 成功返回true,否则返回false
    */
    bool ReadLastTables() ;

    /** 解码
    @param [in] pos 位置
    @return 成功返回true,否则返回false
    */
    bool DecodeMm(uint32_t pos) ;

    /** lz 解码
    @param [in] pos 位置
    @return 成功返回true,否则返回false
    */
    bool DecodeLz(int32_t pos) ;
    
    /** 压缩/解压数据
    @param [in] inStream 数据输入流
    @param [in] outStream 数据输出流
    @param [in] inSize 输入数据大小
    @param [in] outSize 输出数据大小 
    @param [in] progress 进度回调接口
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult CodeReal(ISequentialInStream* inStream,
                             ISequentialOutStream* outStream, 
                             const uint64_t* inSize, 
                             const uint64_t* outSize,
                             ICompressProgressInfo* progress) ;
    
private:

    /** 输出流
    */
    RCLZOutWindow m_outWindowStream ;

    /** 输入流
    */
    RCBitmDecoder<RCInBuffer> m_inBitStream ;

    /** 解码器
    */
    RCHuffmanDecoder<RCRar2Defs::s_kNumHuffmanBits, RCRar2Defs::s_kMainTableSize> m_mainDecoder;

    /** 解码器
    */
    RCHuffmanDecoder<RCRar2Defs::s_kNumHuffmanBits, RCRar2Defs::s_kDistTableSize> m_distDecoder;

    /** 解码器
    */
    RCHuffmanDecoder<RCRar2Defs::s_kNumHuffmanBits, RCRar2Defs::s_kLenTableSize> m_lenDecoder;

    /** 解码器
    */
    RCHuffmanDecoder<RCRar2Defs::s_kNumHuffmanBits, RCRar2Defs::s_kMMTableSize> m_mmDecoders[RCRar2Defs::s_kNumChanelsMax];

    /** 解码器
    */
    RCHuffmanDecoder<RCRar2Defs::s_kNumHuffmanBits, RCRar2Defs::s_kLevelTableSize> m_levelDecoder;

    /** mm 过滤器
    */
    RCRar2Filter2 m_mmFilter;

    /** 是否audio模式
    */
    bool m_audioMode ;    

    /** 通道数
    */
    int32_t m_numChannels;

    /** rep Dists
    */
    uint32_t m_repDists[RCRar2Defs::s_kNumRepDists] ;

    /** repDistPtr
    */
    uint32_t m_repDistPtr;

    /** 最后长度
    */
    uint32_t m_lastLength;  

    /** 最后级别
    */
    byte_t m_lastLevels[RCRar2Defs::s_kMaxTableSize] ;

    /** 压缩后大小
    */
    uint64_t m_packSize ;

    /** 是否固实
    */
    bool m_isSolid ;
};

END_NAMESPACE_RCZIP

#endif //__RCRar2Decoder_h_
