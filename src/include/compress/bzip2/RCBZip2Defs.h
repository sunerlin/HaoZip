/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCBZip2Defs_h_
#define __RCBZip2Defs_h_ 1

#include "base/RCDefs.h"

BEGIN_NAMESPACE_RCZIP

/** BZip2 常量定义
*/
class RCBZip2Defs
{
public:

    static const byte_t s_kArSig0 = 'B';
    static const byte_t s_kArSig1 = 'Z';
    static const byte_t s_kArSig2 = 'h';
    static const byte_t s_kArSig3 = '0';

    static const byte_t s_kArIszSig0 = 'I';
    static const byte_t s_kArIszSig1 = 'S';
    static const byte_t s_kArIszSig2 = 'z';

    static const byte_t s_kFinSig0 = 0x17;
    static const byte_t s_kFinSig1 = 0x72;
    static const byte_t s_kFinSig2 = 0x45;
    static const byte_t s_kFinSig3 = 0x38;
    static const byte_t s_kFinSig4 = 0x50;
    static const byte_t s_kFinSig5 = 0x90;
    
    static const byte_t s_kBlockSig0 = 0x31;
    static const byte_t s_kBlockSig1 = 0x41;
    static const byte_t s_kBlockSig2 = 0x59;
    static const byte_t s_kBlockSig3 = 0x26;
    static const byte_t s_kBlockSig4 = 0x53;
    static const byte_t s_kBlockSig5 = 0x59;
    
    static const int32_t s_kNumOrigBits = 24;
    
    static const int32_t s_kNumTablesBits = 3;
    static const int32_t s_kNumTablesMin = 2;
    static const int32_t s_kNumTablesMax = 6;
    
    static const int32_t s_kNumLevelsBits = 5;
    
    static const int32_t s_kMaxHuffmanLen = 20; // Check it
    
    static const int32_t s_kMaxAlphaSize = 258;
    
    static const int32_t s_kGroupSize = 50;
    
    static const int32_t  s_kBlockSizeMultMin = 1;
    static const int32_t  s_kBlockSizeMultMax = 9;
    static const uint32_t s_kBlockSizeStep = 100000;
    static const uint32_t s_kBlockSizeMax = s_kBlockSizeMultMax * s_kBlockSizeStep;
    
    static const int32_t  s_kNumSelectorsBits = 15;
    static const uint32_t s_kNumSelectorsMax = (2 + (s_kBlockSizeMax / s_kGroupSize));
    
    static const int32_t  s_kRleModeRepSize = 4;
    static const int32_t  s_kNumPassesMax = 10 ;
};

END_NAMESPACE_RCZIP

#endif //__RCBZip2Defs_h_
