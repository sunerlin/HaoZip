/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCRar1Decoder_h_
#define __RCRar1Decoder_h_ 1

#include "interface/ICoder.h"
#include "interface/IUnknownImpl.h"
#include "common/RCBitmDecoder.h"
#include "common/RCInBuffer.h"
#include "compress/lz/RCLZOutWindow.h"
#include "compress/huffman/RCHuffmanDecoder.h"

BEGIN_NAMESPACE_RCZIP

/** Rar1 解码器
*/
class RCRar1Decoder:
    public IUnknownImpl2<ICompressCoder,
                         ICompressSetDecoderProperties2
                        >
{
public:

    /** 默认构造函数
    */
    RCRar1Decoder() ;
    
    /** 默认析构函数
    */
    ~RCRar1Decoder() ;

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

    /** 读取位
    @param [in] numBits 位数
    @return 返回读取结果
    */
    uint32_t ReadBits(int32_t numBits) ;

    /** 复制块
    @param [in] distance 距离
    @param [in] len 长度
    @return 成功返回RC_S_OK,否则返回错误号
    */
    HResult CopyBlock(uint32_t distance, uint32_t len) ;

    /** 解码
    @param [in] posTab 
    @return 返回解码结果
    */
    uint32_t DecodeNum(const uint32_t* posTab) ;

    /** LZ 解码
    @return 成功返回RC_S_OK,否则返回错误号
    */
    HResult ShortLZ() ;

    /** LZ 解码
    @return 成功返回RC_S_OK,否则返回错误号
    */
    HResult LongLZ() ;

    /** Huffman 解码
    @return 成功返回RC_S_OK,否则返回错误号
    */
    HResult HuffDecode();

    /** 取得标志缓存
    */
    void GetFlagsBuf() ;

    /** 初始化数据
    */
    void InitData() ;

    /** 初始化Huffman
    */
    void InitHuff() ;

    /** CorrHuff
    @param [in] CharSet 
    @param [in] NumToPlace
    */
    void CorrHuff(uint32_t* CharSet, uint32_t* NumToPlace) ;

    /** 初始化
    */
    void InitStructures();
    
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

    /** s_kNumRepDists
    */
    static const uint32_t s_kNumRepDists = 4 ;
    
private:

    /** 输出流
    */
    RCLZOutWindow m_outWindowStream;

    /** 输入流
    */
    RCBitmDecoder<RCInBuffer> m_inBitStream;

    /** 摘要
    */
    uint32_t m_repDists[s_kNumRepDists];

    /** rep指针
    */
    uint32_t m_repDistPtr;

    /** lastDist
    */
    uint32_t m_lastDist;

    /** lastLength
    */
    uint32_t m_lastLength;

    /** 解压后大小
    */
    int64_t m_unpackSize;

    /** 是否固实
    */
    bool m_isSolid;

    /** chSet
    */
    uint32_t m_chSet[256] ;

    /** chSetA
    */
    uint32_t m_chSetA[256] ;

    /** chSetB
    */
    uint32_t m_chSetB[256] ;

    /** chSetC
    */
    uint32_t m_chSetC[256] ;

    /** place
    */
    uint32_t m_place[256] ;

    /** placeA
    */
    uint32_t m_placeA[256] ;

    /** placeB
    */
    uint32_t m_placeB[256] ;

    /** placeC
    */
    uint32_t m_placeC[256] ;

    /** NToPl
    */
    uint32_t m_NToPl[256] ;

    /** NToPlB
    */
    uint32_t m_NToPlB[256] ;

    /** NToPlC
    */
    uint32_t m_NToPlC[256] ;

    /** 标志缓存
    */
    uint32_t m_flagBuf ;

    /** avrPlc
    */
    uint32_t m_avrPlc ;

    /** avrPlcB
    */
    uint32_t m_avrPlcB ;

    /** avrLn1
    */
    uint32_t m_avrLn1 ;

    /** avrLn2
    */
    uint32_t m_avrLn2 ;

    /** avrLn3
    */
    uint32_t m_avrLn3 ;

    /** buf60
    */
    int32_t m_buf60 ;

    /** numHuf
    */
    int32_t m_numHuf ;

    /** stMode
    */
    int32_t m_stMode ;

    /** lCount
    */
    int32_t m_lCount ;

    /** flagsCnt
    */
    int32_t m_flagsCnt;

    /** nhfb
    */
    uint32_t m_nhfb ;

    /** nlzb
    */
    uint32_t m_nlzb ;

    /** maxDist3
    */
    uint32_t m_maxDist3 ;
};

END_NAMESPACE_RCZIP

#endif //__RCRar1Decoder_h_
