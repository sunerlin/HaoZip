/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCRar3Decoder_h_
#define __RCRar3Decoder_h_ 1

#include "interface/ICoder.h"
#include "interface/IUnknownImpl.h"
#include "compress/rar/RCRar3RangeDecoder.h"
#include "compress/huffman/RCHuffmanDecoder.h"
#include "compress/rar/RCRar3Defs.h"
#include "compress/ppmd/RCPPMDDecodeInfo.h"
#include "common/RCVector.h"

BEGIN_NAMESPACE_RCZIP

class RCRar3Filter ;
class RCRar3TempFilter ;
class RCRar3Vm ;
class RCRar3BlockRef ;

/** RAR3 解码器
*/
class RCRar3Decoder:
    public IUnknownImpl2<ICompressCoder,
                         ICompressSetDecoderProperties2
                        >
{
public:

    /** Huffman 位数
    */
    static const int32_t s_kNumHuffmanBits = 15;
    
public:

    /** 默认构造函数
    */
    RCRar3Decoder() ;
    
    /** 默认析构函数
    */
    ~RCRar3Decoder() ;
    
public:

    /** 释放流
    */
    void ReleaseStreams() ;

    /** 复制块
    @param [in] distance 距离
    @param [in] len 长度
    */
    void CopyBlock(uint32_t distance, uint32_t len) ;

    /** 输入字节
    @param [in] b 待输入字节
    */
    void PutByte(byte_t b) ;
    
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

    /** 数据写入流
    @param [in] data 数据
    @param [in] size 数据大小
    @return 成功返回RC_S_OK,否则返回错误号
    */
    HResult WriteDataToStream(const byte_t* data, uint32_t size) ;

    /** 写数据
    @param [in] data 数据
    @param [in] size 数据大小
    @return 成功返回RC_S_OK,否则返回错误号
    */
    HResult WriteData(const byte_t* data, uint32_t size);

    /** 写区域
    @param [in] startPtr 起始指针
    @param [in] endPtr 结束指针
    @return 成功返回RC_S_OK,否则返回错误号
    */
    HResult WriteArea(uint32_t startPtr, uint32_t endPtr) ;

    /** 执行过滤
    @param [in] tempFilterIndex 编号
    @param [out] outBlockRef 输出块
    */
    void ExecuteFilter(int32_t tempFilterIndex, RCRar3BlockRef& outBlockRef);

    /** 写缓存
    @return 成功返回RC_S_OK,否则返回错误号
    */
    HResult WriteBuf() ;

    /** 初始化过滤器
    */
    void InitFilters() ;

    /** AddVmCode
    @param [in] firstByte 开始字节
    @param [in] codeSize 大小
    @return 成功返回true,否则返回false
    */
    bool AddVmCode(uint32_t firstByte, uint32_t codeSize) ;

    /** ReadVmCodeLZ
    @return 成功返回true,否则返回false
    */
    bool ReadVmCodeLZ();

    /** ReadVmCodePPM
    @return 成功返回true,否则返回false
    */
    bool ReadVmCodePPM() ;

    /** 读取位
    @param [in] numBits 位数
    @return 返回读取结果
    */
    uint32_t ReadBits(int32_t numBits) ;

    /** 初始化PPM
    @return 成功返回RC_S_OK,否则返回错误号
    */
    HResult InitPPM() ;

    /** 解码Ppm符号
    @return 返回解码结果
    */
    int32_t DecodePpmSymbol() ;

    /** 解码PPM
    @param [in] num 数量
    @param [out] keepDecompressing 保持解码
    @return 成功返回RC_S_OK,否则返回错误号
    */
    HResult DecodePPM(int32_t num, bool& keepDecompressing) ;

    /** 读取表
    @param [out] keepDecompressing 保持解码
    @return 成功返回RC_S_OK,否则返回错误号
    */
    HResult ReadTables(bool& keepDecompressing) ;

    /** 读取块结尾
    @param [out] keepDecompressing 保持解码
    @return 成功返回RC_S_OK,否则返回错误号
    */
    HResult ReadEndOfBlock(bool& keepDecompressing);

    /** LZ 解码
    @param [out] keepDecompressing 保持解码
    @return 成功返回RC_S_OK,否则返回错误号
    */
    HResult DecodeLZ(bool& keepDecompressing) ;

    /** 解码
    @param [in] progress 进度回调接口
    @return 成功返回RC_S_OK,否则返回错误号
    */
    HResult CodeReal(ICompressProgressInfo* progress);
    
private:

    /** 输入流
    */
    RCRar3RangeDecoder m_inBitStream;

    /** window
    */
    byte_t* m_window;

    /** winPos
    */
    uint32_t m_winPos;

    /** wrPtr
    */
    uint32_t m_wrPtr;

    /** lzSize
    */
    uint64_t m_lzSize;

    /** 解压后大小
    */
    uint64_t m_unpackSize;

    /** 写文件大小
    */
    uint64_t m_writtenFileSize; // if it's > m_unpackSize, then m_unpackSize only written

    /** 输出流
    */
    ISequentialOutStreamPtr m_outStream;

    /** 解码器
    */
    RCHuffmanDecoder<s_kNumHuffmanBits, RCRar3Defs::s_kMainTableSize> m_mainDecoder;

    /** 解码器
    */
    RCHuffmanDecoder<s_kNumHuffmanBits, RCRar3Defs::s_kDistTableSize> m_distDecoder;

    /** 解码器
    */
    RCHuffmanDecoder<s_kNumHuffmanBits, RCRar3Defs::s_kAlignTableSize> m_alignDecoder;
 
    /** 解码器
    */
    RCHuffmanDecoder<s_kNumHuffmanBits, RCRar3Defs::s_kLenTableSize> m_lenDecoder;

    /** 解码器
    */
    RCHuffmanDecoder<s_kNumHuffmanBits, RCRar3Defs::s_kLevelTableSize> m_levelDecoder;

    /** reps
    */
    uint32_t m_reps[RCRar3Defs::s_kNumReps];

    /** 最后长度
    */
    uint32_t m_lastLength;

    /** 最后级别
    */
    byte_t m_lastLevels[RCRar3Defs::s_kTablesSizesSum];

    /** vmData
    */
    byte_t* m_vmData;

    /** vmCode
    */
    byte_t* m_vmCode;

    /** vm
    */
    RCRar3Vm& m_vm ;

    /** 过滤列表
    */
    RCVector<RCRar3Filter*> m_filters;

    /** 过滤列表
    */
    RCVector<RCRar3TempFilter*> m_tempFilters;

    /** 最后过滤编号
    */
    uint32_t m_lastFilter;

    /** 是否固实
    */
    bool m_isSolid ;

    /** lz模式
    */
    bool m_lzMode ;

    /** 前对齐位
    */
    uint32_t m_prevAlignBits ;

    /** 前对齐数
    */
    uint32_t m_prevAlignCount ;

    /** 是否读取表
    */
    bool m_isTablesRead ;

    /** ppm
    */
    RCPPMDDecodeInfo m_ppm ;

    /** ppm Esc Char
    */
    int32_t m_ppmEscChar ;
};

END_NAMESPACE_RCZIP

#endif //__RCRar3Decoder_h_
