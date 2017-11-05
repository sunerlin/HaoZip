/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCQuantumModelDecoder_h_
#define __RCQuantumModelDecoder_h_ 1

#include "compress/quantum/RCQuantumDefs.h"

BEGIN_NAMESPACE_RCZIP

/** Quantum Range 解码器
*/
class RCQuantumRangeDecoder ;

/** Quantum Model 解码器
*/
class RCQuantumModelDecoder
{
public:

    /** 默认构造函数
    */
    RCQuantumModelDecoder() ;
    
    /** 默认析构函数
    */
    ~RCQuantumModelDecoder() ;
    
public:

    /** 初始化
    @param [in] numItems 项目数
    */
    void Init(uint32_t numItems) ;

    /** 解码
    @param [in] rangeDecoder 解码器
    @return 返回解码字节数
    */
    uint32_t Decode(RCQuantumRangeDecoder* rangeDecoder) ;
    
private:

    /** 项目数
    */
    uint32_t m_numItems;

    /** 记录数
    */
    uint32_t m_reorderCount;

    /** 频度
    */
    uint16_t m_freqs[RCQuantumDefs::s_kNumSymbolsMax + 1];

    /** 数值
    */
    byte_t m_values[RCQuantumDefs::s_kNumSymbolsMax];
};

END_NAMESPACE_RCZIP

#endif //__RCQuantumModelDecoder_h_
