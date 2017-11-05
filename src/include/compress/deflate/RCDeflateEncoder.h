/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCDeflateEncoder_h_
#define __RCDeflateEncoder_h_ 1

#include "base/RCNonCopyable.h"
#include "common/RCBitlEncoder.h"
#include "compress/deflate/RCDeflateEncoderValue.h"
#include "compress/deflate/RCDeflateDefs.h"
#include "compress/deflate/RCDeflateLevels.h"
#include "compress/deflate/RCDeflateEncoderTables.h"
#include "compress/deflate/RCDeflateEncoderOptimal.h"
#include "interface/ICoder.h"

#include "algorithm/LzFind.h"

BEGIN_NAMESPACE_RCZIP

/** Deflate 编码器
*/
class RCDeflateEncoder:
    private RCNonCopyable
{
protected:

    typedef struct _CSeqInStream
    {
        ISeqInStream SeqInStream ;
        ISequentialInStreamPtr RealStream ;
    }CSeqInStream;

public:

    /** 默认构造函数
    */
    explicit RCDeflateEncoder(bool deflate64Mode = false) ;
    
    /** 默认析构函数
    */
    virtual ~RCDeflateEncoder() ;
    
public:

    /** 返回匹配
    */
    void GetMatches() ;

    /** 移动位置
    @param [in] num 位移
    */
    void MovePos(uint32_t num) ;

    /** 后退
    @param [in] backRes 后退位移
    @param [in] cur 起始位置
    @return 返回移动的字节数
    */
    uint32_t Backward(uint32_t& backRes, uint32_t cur) ;

    /** 取得优化
    @param [out] backRes 后退结果
    @return 返回移动字节数
    */
    uint32_t GetOptimal(uint32_t& backRes);

    /** 取得快速优化
    @param [out] backRes 后退结果
    @return 返回移动字节数
    */
    uint32_t GetOptimalFast(uint32_t& backRes) ;

    /** 级别表
    @param [in] levels 数据 
    @param [in] numLevels 级别
    @param [in] freqs 频度
    */
    void LevelTableDummy(const byte_t* levels, int32_t numLevels, uint32_t* freqs) ;

    /** 写入数据位
    @param [in] value 数据
    @param [in] numBits 位长度
    */
    void WriteBits(uint32_t value, int32_t numBits);

    /** 级别编码
    @param [in] levels 数据
    @param [in] numLevels 级别
    @param [in] lens 长度
    @param [in] codes 编码数据
    */
    void LevelTableCode(const byte_t* levels, int32_t numLevels, const byte_t* lens, const uint32_t* codes);

    /** 生成表
    @param [in] maxHuffLen 最大长度 
    */
    void MakeTables(uint32_t maxHuffLen);

    /** 返回块权重
    @return 返回块权重
    */
    uint32_t GetLzBlockPrice() const ;

    /** 尝试块
    */
    void TryBlock() ;

    /** 尝试块
    @param [in] tableIndex 表编号
    @param [in] numPasses 跳过值
    @return 返回成功字节数
    */
    uint32_t TryDynBlock(int32_t tableIndex, uint32_t numPasses) ;

    /** 尝试修复块
    @param [in] tableIndex 表编号
    @return 返回成功字节数
    */
    uint32_t TryFixedBlock(int32_t tableIndex) ;

    /** 设置权重
    @param [in] levels 级别
    */
    void SetPrices(const RCDeflateLevels& levels);

    /** 写入块
    */
    void WriteBlock() ;

    /** 创建
    @return 成功返回RC_S_OK,否则返回错误号
    */
    HResult Create() ;

    /** 释放
    */
    void Free() ;

    /** 写入存储块
    @param [in] blockSize 块大小
    @param [in] additionalOffset 额外偏移量
    @param [in] finalBlock 是否最后一块
    */
    void WriteStoreBlock(uint32_t blockSize, uint32_t additionalOffset, bool finalBlock) ;

    /** 释放流
    */
    void ReleaseStreams() ;

    /** 返回块权重
    @param [in] tableIndex 表编号
    @param [in] numDivPasses
    @return 返回权重
    */
    uint32_t GetBlockPrice(int32_t tableIndex, int32_t numDivPasses) ;

    /** 块编码
    @param [in] tableIndex 表编号
    @param [in] finalBlock 释放最后块
    */
    void CodeBlock(int32_t tableIndex, bool finalBlock) ;

public:
    
    /** 编码函数
    @param [in] inStream 输入流
    @param [in] outStream 输出流
    @param [in] inSize 输入大小
    @param [in] outSize 输出大小
    @param [in] progress 进度
    @return 成功返回RC_S_OK,否则返回错误号
    */
    HResult CodeReal(ISequentialInStream* inStream,
                     ISequentialOutStream* outStream, 
                     const uint64_t* inSize, 
                     const uint64_t* outSize,
                     ICompressProgressInfo* progress) ;

    /** 基础编码
    @param [in] inStream 输入流
    @param [in] outStream 输出流
    @param [in] inSize 输入大小
    @param [in] outSize 输出大小
    @param [in] progress 进度
    @return 成功返回RC_S_OK,否则返回错误号
    */
    HResult BaseCode(ISequentialInStream* inStream,
                     ISequentialOutStream* outStream, 
                     const uint64_t* inSize, 
                     const uint64_t* outSize,
                     ICompressProgressInfo *progress) ;
    
    /** 设置编码属性
    @param [in] propertyArray 属性列表
    @return 成功返回RC_S_OK,否则返回错误号
    */
    HResult BaseSetEncoderProperties2(const RCPropertyIDPairArray& propertyArray) ;
            
private:
    
    /** 编码值
    */
    RCDeflateEncoderValue* m_values;
    
    /** 匹配
    */
    uint16_t* m_matchDistances;
    
    /** 快速字节
    */
    uint32_t m_numFastBytes;
    
    /** 快速模式
    */
    bool m_fastMode;
    
    /** bt模式
    */
    bool m_btMode;
    
    /** 缓存
    */
    uint16_t* m_onePosMatchesMemory;
    
    /** 缓存
    */
    uint16_t* m_distanceMemory;
    
    /** 位置
    */
    uint32_t m_pos;
    
    /** 忽略字节数
    */
    int32_t m_numPasses;
    
    /** 忽略字节数
    */
    int32_t m_numDivPasses;
    
    /** 检查统计
    */
    bool m_checkStatic;
    
    /** Multi Pass
    */
    bool m_isMultiPass;
    
    /** 块大小
    */
    uint32_t m_valueBlockSize;
    
    /** 组合长度
    */
    uint32_t m_numLenCombinations;
    
    /** 最大匹配长度
    */
    uint32_t m_matchMaxLen;
    
    /** 开始位置
    */
    const byte_t* m_lenStart;
    
    /** Direct Bit
    */
    const byte_t* m_lenDirectBits;
    
    /** 是否创建
    */
    bool m_created;
    
    /** Deflate 64模式
    */
    bool m_deflate64Mode;
    
    /** 级别
    */
    byte_t m_levelLevels[RCDeflateDefs::s_kLevelTableSize];
        
    /** 级别数
    */
    int32_t m_numLitLenLevels;
    
    /** 级别数
    */
    int32_t m_numDistLevels;
    
    /** 编码个数
    */
    uint32_t m_numLevelCodes;
    
    /** 值编号
    */
    uint32_t m_valueIndex;
    
    /** Second Pass
    */
    bool m_secondPass;
    
    /** 附加偏移
    */
    uint32_t m_additionalOffset;
    
    /** 优化编号
    */
    uint32_t m_optimumEndIndex;
    
    /** 优化当前编号
    */
    uint32_t m_optimumCurrentIndex;
    
    /** 字符权重
    */
    byte_t  m_literalPrices[256];
    
    /** 长度权重
    */
    byte_t  m_lenPrices[RCDeflateDefs::s_kNumLenSymbolsMax];
        
    /** 位权重
    */
    byte_t  m_posPrices[RCDeflateDefs::s_kDistTableSize64];
    
    /** 级别
    */
    RCDeflateLevels m_newLevels;
    
    /** 频度
    */
    uint32_t m_mainFreqs[RCDeflateDefs::s_kFixedMainTableSize];
        
    /** 频度
    */
    uint32_t m_distFreqs[RCDeflateDefs::s_kDistTableSize64];
        
    /** 主编码
    */
    uint32_t m_mainCodes[RCDeflateDefs::s_kFixedMainTableSize];
    
    /** 摘要编码
    */
    uint32_t m_distCodes[RCDeflateDefs::s_kDistTableSize64];
    
    /** Level 编码
    */
    uint32_t m_levelCodes[RCDeflateDefs::s_kLevelTableSize];
    
    /** Level 长度
    */
    byte_t m_levelLens[RCDeflateDefs::s_kLevelTableSize];
    
    /** 块大小
    */
    uint32_t m_blockSizeRes;
    
    /** 编码表
    */
    RCDeflateEncoderTables* m_tables;
    
    /** 优化表
    */
    RCDeflateEncoderOptimal m_optimum[RCDeflateDefs::s_kNumOpts];
    
    /** 匹配循环
    */
    uint32_t m_matchFinderCycles;
    
private:
    
    /** 读取
    @param [in] object 数据
    @param [in] data 数据
    @param [in] size 数据大小
    @return 返回读取字节数
    */
    static result_t Read(void* object, void* data, size_t* size) ;
    
    /** 取得位置Slot
    @param [in] pos 位置
    @return 返回Slot
    */
    static uint32_t GetPosSlot(uint32_t pos) ;
    
    /** 内存申请
    @param [out] p 指针
    @param [in] size 内存大小
    @return 返回申请的内存地址
    */
    static void* SzAlloc(void* p, size_t size) ;
    
    /** 释放内存
    @param [in] p 指针
    @param [in] address 指针
    */
    static void SzFree(void* p, void* address) ;
    
    /** 
    @param [in] freqs
    @param [in] lens
    @param [in] num
    @return 返回权重
    */
    static uint32_t HuffmanGetPrice(const uint32_t* freqs, const byte_t* lens, uint32_t num) ;
    
    /**
    @param [in] freqs
    @param [in] lens
    @param [in] num
    @param [in] extraBits
    @param [in] extraBase
    @return 返回权重
    */
    static uint32_t HuffmanGetPriceSpec(const uint32_t* freqs, 
                                          const byte_t* lens, 
                                          uint32_t num, 
                                          const byte_t* extraBits, 
                                          uint32_t extraBase) ;
    
    /**
    @param [in] codes
    @param [in] lens
    @param [in] num
    */
    static void HuffmanReverseBits(uint32_t* codes, const byte_t* lens, uint32_t num) ;
    
    /**
    @param [in] blockSize
    @param [in] bitPosition
    @return 
    */
    static uint32_t GetStorePrice(uint32_t blockSize, int32_t bitPosition) ;
    
private:
    
    /** 内存管理
    */
    static ISzAlloc s_alloc ;
        
private:
    
    /** 匹配查找
    */
    CMatchFinder m_lzInWindow ;
    
    /** 位编码
    */
    RCBitlEncoder m_outStream ;
    
    /** 序列输入流
    */
    CSeqInStream m_seqInStream ;
};

END_NAMESPACE_RCZIP

#endif //__RCDeflateEncoder_h_
