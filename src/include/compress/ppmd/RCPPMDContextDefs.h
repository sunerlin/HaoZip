/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/
// This code is based on Dmitry Shkarin's PPMdH code

#ifndef __RCPPMDContextDefs_h_
#define __RCPPMDContextDefs_h_ 1

#include "base/RCDefs.h"

BEGIN_NAMESPACE_RCZIP

/** PPMD 常量定义
*/

static const int32_t s_intBits = 7 ;
static const int32_t s_periodBits = 7 ;
static const int32_t s_totBits = s_intBits + s_periodBits ;
static const int32_t s_interval = 1 << s_intBits ;
static const int32_t s_binScale = 1 << s_totBits ;
static const int32_t s_maxFreq = 124 ;

static const uint16_t s_initBinEsc[] = {0x3CDD, 0x1F3F, 0x59BF, 0x48F3, 0x64A1, 0x5ABC, 0x6632, 0x6051} ;

/** Tabulated escapes for exponential symbol distribution
*/
static const byte_t s_expEscape[16]={ 25,14, 9, 7, 5, 5, 4, 4, 4, 3, 3, 3, 2, 2, 2, 2 };

#define GET_MEAN(SUMM,SHIFT,ROUND) ((SUMM+(1 << (SHIFT-ROUND))) >> (SHIFT))

END_NAMESPACE_RCZIP

#endif //__RCPPMDContextDefs_h_
