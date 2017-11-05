/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCPPMDPPMContext_h_
#define __RCPPMDPPMContext_h_ 1

#include "base/RCDefs.h"

BEGIN_NAMESPACE_RCZIP

/** 内存分配
*/
class RCPPMDSubAlloc ;

/** PPMD 编码上下文
*/
class RCPPMDPPMContext
{
public:

    /** 默认构造函数
    */
    RCPPMDPPMContext() ;
    
    /** 默认析构函数
    */
    ~RCPPMDPPMContext() ;
    
public:

    /** 状态数
    */
    uint16_t m_numStats ; // sizeof(uint16_t) > sizeof(byte_t)

    /** 频度
    */
    uint16_t m_summFreq ;

    /** 状态
    */
    uint32_t m_stats;

    /** 后缀
    */
    uint32_t m_suffix;

public:

    /** 状态
    */
    class TState
    {
    public:

        /** 符号
        */
        byte_t m_symbol ;

        /** 频度
        */
        byte_t m_freq ;

        /** 后继低字
        */
        uint16_t m_successorLow ;

        /** 后继高字
        */
        uint16_t m_successorHigh ;

    public:

        /** 构造函数
        */
        TState() ;

        /** 返回后继
        @return 返回后继
        */
        uint32_t GetSuccessor() const ;

        /** 设置后继
        @param [in] v 后继数值
        */
        void SetSuccessor(uint32_t v) ;
    };

public:

    /** 创建子项
    @param [in] subAllocator 分配器
    @param [in] pStats 父状态
    @param [out] FirstState 第一个状态
    @return 返回创建的上下文
    */
    RCPPMDPPMContext* CreateChild(RCPPMDSubAlloc& subAllocator, TState* pStats, TState& FirstState) ;

    /** 状态设置
    @return 返回状态
    */
    TState& OneState() const ;
};

END_NAMESPACE_RCZIP

#endif //__RCPPMDPPMContext_h_
