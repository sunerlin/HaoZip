/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCDeflateDefs_h_
#define __RCDeflateDefs_h_ 1

#include "base/RCDefs.h"

BEGIN_NAMESPACE_RCZIP

/** Deflate 常量定义
*/
class RCDeflateDefs
{
public:

    static const int32_t  s_kNumHuffmanBits = 15;
    static const uint32_t s_kHistorySize32 = (1 << 15);
    static const uint32_t s_kHistorySize64 = (1 << 16);
    static const uint32_t s_kDistTableSize32 = 30;
    static const uint32_t s_kDistTableSize64 = 32;      
    static const uint32_t s_kNumLenSymbols32 = 256;
    static const uint32_t s_kNumLenSymbols64 = 255; // don't change it. It must be <= 255.
    static const uint32_t s_kNumLenSymbolsMax = s_kNumLenSymbols32;      
    static const uint32_t s_kNumLenSlots = 29;    
    static const uint32_t s_kFixedDistTableSize = 32;
    static const uint32_t s_kFixedLenTableSize = 31;    
    static const uint32_t s_kSymbolEndOfBlock = 0x100;
    static const uint32_t s_kSymbolMatch = s_kSymbolEndOfBlock + 1;    
    static const uint32_t s_kMainTableSize = s_kSymbolMatch + s_kNumLenSlots;
    static const uint32_t s_kFixedMainTableSize = s_kSymbolMatch + s_kFixedLenTableSize;    
    static const uint32_t s_kLevelTableSize = 19;    
    static const uint32_t s_kTableDirectLevels = 16;
    static const uint32_t s_kTableLevelRepNumber = s_kTableDirectLevels;
    static const uint32_t s_kTableLevel0Number = s_kTableLevelRepNumber + 1;
    static const uint32_t s_kTableLevel0Number2 = s_kTableLevel0Number + 1;    
    static const uint32_t s_kLevelMask = 0xF;
    static const uint32_t s_kMatchMinLen = 3;
    static const uint32_t s_kMatchMaxLen32 = s_kNumLenSymbols32 + s_kMatchMinLen - 1; //256 + 2
    static const uint32_t s_kMatchMaxLen64 = s_kNumLenSymbols64 + s_kMatchMinLen - 1; //255 + 2
    static const uint32_t s_kMatchMaxLen = s_kMatchMaxLen32;
    static const int32_t  s_kFinalBlockFieldSize = 1;
    static const int32_t  s_kBlockTypeFieldSize = 2 ;
    static const int32_t  s_kNumLenCodesFieldSize = 5;
    static const int32_t  s_kNumDistCodesFieldSize = 5;
    static const int32_t  s_kNumLevelCodesFieldSize = 4;
    static const uint32_t s_kNumLitLenCodesMin = 257;
    static const uint32_t s_kNumDistCodesMin = 1;
    static const uint32_t s_kNumLevelCodesMin = 4;
    static const int32_t  s_kLevelFieldSize = 3;
    static const int32_t  s_kStoredBlockLengthFieldSize = 16;
    
    static const int32_t s_kLenIdFinished = -1;
    static const int32_t s_kLenIdNeedInit = -2;
    
    static const uint32_t s_kNumOptsBase = 1 << 12;
    static const uint32_t s_kNumOpts = s_kNumOptsBase + s_kMatchMaxLen;
    
    enum
    {
        s_kNotFinalBlock = 0,
        s_kFinalBlock = 1
    };

    enum
    {
        s_kStored = 0,
        s_kFixedHuffman = 1,
        s_kDynamicHuffman = 2
    };
};

static const byte_t s_kDeflateLenStart32[RCDeflateDefs::s_kFixedLenTableSize] =
                    {0,1,2,3,4,5,6,7,8,10,12,14,16,20,24,28,32,40,48,56,64,80,96,
                     112,128,160,192,224, 255, 0, 0};
static const byte_t s_kDeflateLenStart64[RCDeflateDefs::s_kFixedLenTableSize] =
                    {0,1,2,3,4,5,6,7,8,10,12,14,16,20,24,28,32,40,48,56,64,80,96,
                     112,128,160,192,224, 0, 0, 0};
static const byte_t s_kDeflateLenDirectBits32[RCDeflateDefs::s_kFixedLenTableSize] =
                    {0,0,0,0,0,0,0,0,1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4,
                     4,  5,  5,  5,  5, 0, 0, 0};
static const byte_t s_kDeflateLenDirectBits64[RCDeflateDefs::s_kFixedLenTableSize] =
                    {0,0,0,0,0,0,0,0,1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 
                     4,  5,  5,  5,  5, 16, 0, 0};
static const uint32_t s_kDeflateDistStart[RCDeflateDefs::s_kDistTableSize64]  =
                    {0,1,2,3,4,6,8,12,16,24,32,48,64,96,128,192,256,384,512,768,
                    1024,1536,2048,3072,4096,6144,8192,12288,16384,24576,32768,49152};
static const byte_t s_kDeflateDistDirectBits[RCDeflateDefs::s_kDistTableSize64] =
                    {0,0,0,0,1,1,2,2,3,3,4,4,5,5,6,6,7,7,8,8,9,9,10,10,11,11,12,
                     12,13,13,14,14};
static const byte_t s_kDeflateLevelDirectBits[3] = {2, 3, 7};
static const byte_t s_kDeflateCodeLengthAlphabetOrder[RCDeflateDefs::s_kLevelTableSize] = 
                    {16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15};

 
END_NAMESPACE_RCZIP

#endif //__RCDeflateDefs_h_
