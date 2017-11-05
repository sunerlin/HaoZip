/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCImplodeHuffmanDecoder_h_
#define __RCImplodeHuffmanDecoder_h_ 1

#include "base/RCNonCopyable.h"
#include "common/RCBitlDecoder.h"
#include "common/RCInBuffer.h"

BEGIN_NAMESPACE_RCZIP

/** Implode Huffman 解码
*/
class RCImplodeHuffmanDecoder:
    private RCNonCopyable
{
public:

    /** 构造函数
    @param [in] numSymbols 符号数
    */
    explicit RCImplodeHuffmanDecoder(uint32_t numSymbols) ;
    
    /** 默认析构函数
    */
    ~RCImplodeHuffmanDecoder() ;

public:
    
    /** 最长位数
    */
    static const int32_t s_kNumBitsInLongestCode = 16 ;
    
    /** RCInBit
    */
    typedef RCBitlDecoder<RCInBuffer> RCInBit ;
    
public:
    
    /** 设置编码长度
    @param [in] codeLengths 编码长度
    @return 成功返回true,否则返回false
    */
    bool SetCodeLengths(const byte_t* codeLengths);
    
    /** 解码
    @param [in] inStream 输入数据流
    @return 返回字节数
    */
    uint32_t DecodeSymbol(RCInBit* inStream);
    
private:
    
    /** m_limitits[i] = value limit for symbols with length = i
    */
    uint32_t m_limitits[s_kNumBitsInLongestCode + 2];
    
    /** m_positions[i] = index in m_symbols[] of first symbol with length = i
    */
    uint32_t m_positions[s_kNumBitsInLongestCode + 2]; 
    
    /** number of symbols in m_symbols
    */
    uint32_t m_numSymbols;
    
    /** symbols: at first with len=1 then 2, ... 15.
    */
    uint32_t* m_symbols;
};

END_NAMESPACE_RCZIP

#endif //__RCImplodeHuffmanDecoder_h_
