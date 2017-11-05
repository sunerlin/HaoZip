/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCLzhHuffmanDecoderImpl_h_
#define __RCLzhHuffmanDecoderImpl_h_ 1

#include "compress/huffman/RCHuffmanDecoder.h"
#include "compress/lzh/RCLzhDecoderDefs.h"

BEGIN_NAMESPACE_RCZIP

/** Lzh Huffman 解码器实现
*/
template <uint32_t m_NumSymbols>
class RCLzhHuffmanDecoderImpl:
    public RCHuffmanDecoder<RCLzhDecoderDefs::s_kMaxHuffmanLen, m_NumSymbols>
{
public:

    /** 解码数据
    @param [in] bitStream 位数据流 
    */
    template <class TBitDecoder>
    uint32_t Decode(TBitDecoder *bitStream)
    {
        if (m_symbol >= 0)
        {
            return (uint32_t)m_symbol;
        }
        return DecodeSymbol(bitStream);
    }
    
public:

    /** 设置 symbol
    @param [in] symbol 符号 
    */
    void SetSymbol(int32_t symbol)
    {
        m_symbol = symbol ;
    }
    
    /** 获取 symbol
    @return 返回符号
    */
    int32_t GetSymbol() const
    {
        return m_symbol ;
    }

private:

    /** symbol
    */
    int32_t m_symbol ;
};

END_NAMESPACE_RCZIP

#endif //__RCLzhHuffmanDecoderImpl_h_

