/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCPPMDInfo_h_
#define __RCPPMDInfo_h_ 1

#include "compress/ppmd/RCPPMDSubAlloc.h"
#include "compress/ppmd/RCPPMDSEE2Context.h"
#include "compress/ppmd/RCPPMDPPMContext.h"

BEGIN_NAMESPACE_RCZIP

/** PPMD 信息
*/
class RCPPMDInfo
{
public:

    /** 默认构造函数
    */
    RCPPMDInfo() ;
    
    /** 默认析构函数
    */
    ~RCPPMDInfo() ;
    
public:

    /** 取得位总计
    @param [in] rs 状态
    @param [in] numStates 状态数量
    */
    uint16_t& GetBinSumm(const RCPPMDPPMContext::TState& rs, int32_t numStates) ;

    /** 返回上下文
    @param [in] offset 偏移量
    @return 返回上下文
    */
    RCPPMDPPMContext* GetContext(uint32_t offset) const ;

    /** 返回上下文
    @param [in] offset 偏移量
    @return 返回上下文
    */
    RCPPMDPPMContext* GetContextNoCheck(uint32_t offset) const ;

    /** 返回状态
    @param [in] offset 偏移量
    @return 返回状态
    */
    RCPPMDPPMContext::TState* GetState(uint32_t offset) const ;

    /** 返回状态
    @param [in] offset 偏移量
    @return 返回状态
    */
    RCPPMDPPMContext::TState* GetStateNoCheck(uint32_t offset) const ;

    /** 重构模型
    */
    void RestartModelRare() ;

    /** 开始构造模型
    @param [in] maxOrder 最大顺序
    */
    void StartModelRare(int32_t maxOrder) ;

    /** 返回上下文
    @param [in] skip 是否跳过
    @param [in] p1 张贴
    @return 返回上下文
    */
    RCPPMDPPMContext* CreateSuccessors(bool skip, RCPPMDPPMContext::TState* p1) ;

    /** 更新模型
    */
    void UpdateModel() ;

    /** 清除掩码
    */
    void ClearMask() ;

    /** 更新1
    @param [in] p 状态
    */
    void update1(RCPPMDPPMContext::TState* p) ;

    /** 更新2
    @param [in] p 状态
    */
    void update2(RCPPMDPPMContext::TState* p) ;

    /** 返回上下文
    @param [in] Diff 差异
    @param [in] scale 尺度
    @return 返回上下文
    */
    RCPPMDSEE2Context* makeEscFreq2(int32_t Diff, uint32_t &scale) ;

    /** 重新计算
    */
    void rescale() ;

    /** 下一上下文
    */
    void NextContext() ;
       
public:

    /** 内存分配
    */
    RCPPMDSubAlloc m_subAllocator ;

    /** 上下文
    */
    RCPPMDSEE2Context m_SEE2Cont[25][16] ;

    /** 伪上下文
    */
    RCPPMDSEE2Context m_dummySEE2Cont ;

    /** 最小上下文
    */
    RCPPMDPPMContext* m_minContext ;

    /** 最大上下文
    */
    RCPPMDPPMContext* m_maxContext ;

    /** found next state transition
    */
    RCPPMDPPMContext::TState* m_foundState ; 

    /** 掩码数
    */
    int32_t m_numMasked ;

    /** 初始化
    */
    int32_t m_initEsc ;

    /** 顺序
    */
    int32_t m_orderFall ;

    /** 长度
    */
    int32_t m_runLength ;

    /** 初始化
    */
    int32_t m_initRL ;

    /** 最大顺序
    */
    int32_t m_maxOrder ;

    /** 掩码
    */
    byte_t m_charMask[256] ;

    /** 索引
    */
    byte_t m_NS2Indx[256] ;

    /** 索引
    */
    byte_t m_NS2BSIndx[256] ;

    /** 标记
    */
    byte_t m_HB2Flag[256] ;

    /** 数量
    */
    byte_t m_escCount ;

    /** 数量
    */
    byte_t m_printCount ;

    /** 成功数
    */
    byte_t m_prevSuccess ;

    /** 标志
    */
    byte_t m_hiBitsFlag ;


    /** binary SEE-contexts
    */
    uint16_t m_binSumm[128][64] ;
};

END_NAMESPACE_RCZIP

#endif //__RCPPMDInfo_h_
