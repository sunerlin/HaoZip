/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCQuantumDefs_h_
#define __RCQuantumDefs_h_ 1

#include "base/RCDefs.h"

BEGIN_NAMESPACE_RCZIP

/** Quantum 常量定义
*/
class RCQuantumDefs
{
public:

    static const int32_t s_kNumLitSelectorBits = 2;
    static const uint32_t s_kNumLitSelectors = (1 << s_kNumLitSelectorBits);
    static const uint32_t s_kNumLitSymbols = 1 << (8 - s_kNumLitSelectorBits);
    static const uint32_t s_kNumMatchSelectors = 3;
    static const uint32_t s_kNumSelectors = s_kNumLitSelectors + s_kNumMatchSelectors;
    static const uint32_t s_kNumLen3PosSymbolsMax = 24;
    static const uint32_t s_kNumLen4PosSymbolsMax = 36;
    static const uint32_t s_kNumLen5PosSymbolsMax = 42;
    static const uint32_t s_kNumLenSymbols = 27;
    
    static const uint32_t s_kNumSymbolsMax = s_kNumLitSymbols; // 64
    
    static const uint32_t s_kMatchMinLen = 3;
    static const uint32_t s_kNumSimplePosSlots = 4;
    static const uint32_t s_kNumSimpleLenSlots = 6;
    
    static const uint16_t s_kUpdateStep = 8;
    static const uint16_t s_kFreqSumMax = 3800;
    static const uint16_t s_kReorderCountStart = 4;
    static const uint16_t s_kReorderCount = 50;
};

END_NAMESPACE_RCZIP

#endif //__RCQuantumDefs_h_
