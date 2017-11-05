/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCRar2Defs_h_
#define __RCRar2Defs_h_ 1

#include "base/RCDefs.h"

BEGIN_NAMESPACE_RCZIP

/** RAR2 常量定义
*/
class RCRar2Defs
{
public:
    static const uint32_t s_kNumRepDists = 4 ;
    static const uint32_t s_kDistTableSize = 48 ;    
    static const int32_t  s_kMMTableSize = 256 + 1 ;    
    static const uint32_t s_kMainTableSize = 298;
    static const uint32_t s_kLenTableSize = 28;    
    static const uint32_t s_kDistTableStart = s_kMainTableSize;
    static const uint32_t s_kLenTableStart = s_kDistTableStart + s_kDistTableSize;    
    static const uint32_t s_kHeapTablesSizesSum = s_kMainTableSize + s_kDistTableSize + s_kLenTableSize;   
    static const uint32_t s_kLevelTableSize = 19;    
    static const uint32_t s_kMMTablesSizesSum = s_kMMTableSize * 4;    
    static const uint32_t s_kMaxTableSize = s_kMMTablesSizesSum;    
    static const uint32_t s_kTableDirectLevels = 16;
    static const uint32_t s_kTableLevelRepNumber = s_kTableDirectLevels;
    static const uint32_t s_kTableLevel0Number = s_kTableLevelRepNumber + 1;
    static const uint32_t s_kTableLevel0Number2 = s_kTableLevel0Number + 1;    
    static const uint32_t s_kLevelMask = 0xF;    
    static const uint32_t s_kRepBothNumber = 256;
    static const uint32_t s_kRepNumber = s_kRepBothNumber + 1;
    static const uint32_t s_kLen2Number = s_kRepNumber + 4;    
    static const uint32_t s_kLen2NumNumbers = 8;
    static const uint32_t s_kReadTableNumber = s_kLen2Number + s_kLen2NumNumbers;
    static const uint32_t s_kMatchNumber = s_kReadTableNumber + 1;       
    static const uint32_t s_kDistLimit2 = 0x101 - 1;
    static const uint32_t s_kDistLimit3 = 0x2000 - 1;
    static const uint32_t s_kDistLimit4 = 0x40000 - 1;    
    static const uint32_t s_kMatchMaxLen = 255 + 2;
    static const uint32_t s_kMatchMaxLenMax = 255 + 5;
    static const uint32_t s_kNormalMatchMinLen = 3;
    static const int32_t  s_kNumChanelsMax = 4 ;
    static const int32_t  s_kNumHuffmanBits = 15 ;
};

END_NAMESPACE_RCZIP

#endif //__RCRar2Defs_h_
