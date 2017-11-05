/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCLzxDefs_h_
#define __RCLzxDefs_h_ 1

#include "base/RCDefs.h"

BEGIN_NAMESPACE_RCZIP

/** Lzx 常量定义
*/
class RCLzxDefs
{
public:

    static const int32_t  s_kNumHuffmanBits = 16 ;
    static const uint32_t s_kNumRepDistances = 3 ;
    
    static const uint32_t s_kNumLenSlots = 8;
    static const uint32_t s_kMatchMinLen = 2;
    static const uint32_t s_kNumLenSymbols = 249;
    static const uint32_t s_kMatchMaxLen = s_kMatchMinLen + (s_kNumLenSlots - 1) + s_kNumLenSymbols - 1;
    
    static const int32_t  s_kNumAlignBits = 3;
    static const uint32_t s_kAlignTableSize = 1 << s_kNumAlignBits;
    
    static const uint32_t s_kNumPosSlots = 50;
    static const uint32_t s_kNumPosLenSlots = s_kNumPosSlots * s_kNumLenSlots;
    
    static const uint32_t s_kMainTableSize = 256 + s_kNumPosLenSlots;
    static const uint32_t s_kLevelTableSize = 20;
    static const uint32_t s_kMaxTableSize = s_kMainTableSize;
    
    static const int32_t  s_kNumBlockTypeBits = 3;
    static const int32_t  s_kBlockTypeVerbatim = 1;
    static const int32_t  s_kBlockTypeAligned = 2;
    static const int32_t  s_kBlockTypeUncompressed = 3;
    
    static const int32_t  s_kUncompressedBlockSizeNumBits = 24;
    
    static const int32_t  s_kNumBitsForPreTreeLevel = 4;
    
    static const int32_t  s_kLevelSymbolZeros = 17;
    static const int32_t  s_kLevelSymbolZerosBig = 18;
    static const int32_t  s_kLevelSymbolSame = 19;
    
    static const int32_t  s_kLevelSymbolZerosStartValue = 4;
    static const int32_t  s_kLevelSymbolZerosNumBits = 4;
    
    static const int32_t  s_kLevelSymbolZerosBigStartValue = s_kLevelSymbolZerosStartValue +
                                                         (1 << s_kLevelSymbolZerosNumBits) ;
    static const int32_t  s_kLevelSymbolZerosBigNumBits = 5;
    
    static const int32_t  s_kLevelSymbolSameNumBits = 1;
    static const int32_t  s_kLevelSymbolSameStartValue = 4;
    
    static const int32_t  s_kNumBitsForAlignLevel = 3;
      
    static const int32_t  s_kNumDictionaryBitsMin = 15;
    static const int32_t  s_kNumDictionaryBitsMax = 21;
    static const uint32_t s_kDictionarySizeMax = (1 << s_kNumDictionaryBitsMax);
    
    static const int32_t  s_kNumLinearPosSlotBits = 17;
    static const uint32_t s_kNumPowerPosSlots = 0x26;
};

END_NAMESPACE_RCZIP

#endif //__RCLzxDefs_h_
