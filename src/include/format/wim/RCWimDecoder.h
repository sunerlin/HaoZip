/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCWimDecoder_h_
#define __RCWimDecoder_h_ 1

#include "format/wim/RCWimBitStream.h"
#include "compress/lz/RCLZOutWindow.h"
#include "compress/huffman/RCHuffmanDecoder.h"

BEGIN_NAMESPACE_RCZIP

/** Wim 解码
*/
class RCWimDecoder
{
public:

    /** 默认构造函数
    */
    RCWimDecoder() ;
    
    /** 默认析构函数
    */
    ~RCWimDecoder() ;

public:

    /** 释放流
    */
    void ReleaseStreams() ;

    /** Flush数据
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult Flush() ;

    /** 解码
    @param [in] inStream 输入流
    @param [in] outStream 输出流
    @param [in] outSize 输出大小
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult Code(ISequentialInStream* inStream, ISequentialOutStream* outStream, uint32_t outSize) ;

private:

    /** 解码
    @param [in] size 解码大小
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult CodeSpec(uint32_t size) ;

    /** 解码
    @param [in] inStream 输入流
    @param [in] outStream 输出流
    @param [in] outSize 输出允许大小
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult CodeReal(ISequentialInStream* inStream, ISequentialOutStream* outStream, uint32_t outSize); 
       
private:

    /** Huffman Bits
    */
    static const int32_t s_kWimNumHuffmanBits = 16;

    /** Match Min Length
    */
    static const uint32_t s_kWimMatchMinLen = 3;

    /** Slots Length
    */
    static const uint32_t s_kWimNumLenSlots = 16;

    /** Pos Slots Number
    */
    static const uint32_t s_kWimNumPosSlots = 16;

    /** Pos Length Slots Number
    */
    static const uint32_t s_kWimNumPosLenSlots = s_kWimNumPosSlots * s_kWimNumLenSlots;

    /** Main Table Size
    */
    static const uint32_t s_kWimMainTableSize = 256 + s_kWimNumPosLenSlots;
    
    /** Dict Size
    */
    static const uint32_t s_kWimDictSize = (1 << s_kWimNumPosSlots);
    
private:

    /** 输入流
    */
    RCWimBitStream m_inBitStream ;

    /** 输出流
    */
    RCLZOutWindow  m_outWindowStream ;

    /** 解码器
    */
    RCHuffmanDecoder<s_kWimNumHuffmanBits, s_kWimMainTableSize> m_mainDecoder ;
};

END_NAMESPACE_RCZIP

#endif //__RCWimDecoder_h_
