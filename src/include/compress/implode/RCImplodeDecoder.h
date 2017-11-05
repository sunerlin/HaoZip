/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCImplodeDecoder_h_
#define __RCImplodeDecoder_h_ 1

#include "interface/ICoder.h"
#include "interface/IUnknownImpl.h"
#include "compress/implode/RCImplodeHuffmanDecoder.h"
#include "compress/lz/RCLZOutWindow.h"

BEGIN_NAMESPACE_RCZIP

/** Implode 解码
*/
class RCImplodeDecoder:
    public IUnknownImpl3<ICompressCoder,
                         ICompressSetDecoderProperties2,
                         IOutStreamFlush>
{
public:

    /** 默认构造函数
    */
    RCImplodeDecoder() ;
    
    /** 默认析构函数
    */
    ~RCImplodeDecoder() ;

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
    
    /** 设置解压属性
    @param [in] data 属性数据
    @param [in] size 数据长度
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult SetDecoderProperties2(const byte_t* data, uint32_t size) ;
    
    /** 清空缓冲区，并把缓存数据写入流
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult Flush() ;
    
    /** 释放流
    */
    void ReleaseStreams() ;
    
private:
    
    /** ReadLevelItems
    @param [in] decoder
    @param [in] levels
    @param [in] numLevelItems
    @return 
    */
    bool ReadLevelItems(RCImplodeHuffmanDecoder& decoder, byte_t* levels, int32_t numLevelItems);
    
    /** ReadTables
    @return 
    */
    bool ReadTables();
    
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
    
    /** Bit流
    */
    RCBitlDecoder<RCInBuffer> m_inBitStream ;
      
    /** 解码器
    */
    RCImplodeHuffmanDecoder m_literalDecoder ;
    
    /** 解码器
    */
    RCImplodeHuffmanDecoder m_lengthDecoder ;
    
    /** 解码器
    */
    RCImplodeHuffmanDecoder m_distanceDecoder ;
    
    /** 大字典是否启用
    */
    bool m_bigDictionaryOn ;
    
    /** 字符是否启用
    */
    bool m_literalsOn ;
    
    /** 字典位数
    */
    int32_t m_numDistanceLowDirectBits ;
    
    /** 最小匹配长度
    */
    uint32_t m_minMatchLength ;
};

END_NAMESPACE_RCZIP

#endif //__RCImplodeDecoder_h_
