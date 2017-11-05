/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCPPMDSubAlloc_h_
#define __RCPPMDSubAlloc_h_ 1

#include "base/RCNonCopyable.h"

BEGIN_NAMESPACE_RCZIP

/** PPMD 子分配器
*/
class RCPPMDSubAlloc:
    private RCNonCopyable
{
public:

    /** Mem block
    */
    struct RCPPMDMemBlock
    {
        /** 标记
        */
        uint16_t m_stamp ;

        /** NU
        */
        uint16_t m_NU ;

        /** 下一块
        */
        uint32_t m_next ;

        /** 前一块
        */
        uint32_t m_prev ;

        /** 默认构造函数
        */
        RCPPMDMemBlock() ;

        /** 插入列表
        @param [in] Base 数据
        @param [in] p 插入位置
        */
        void InsertAt(byte_t* Base, uint32_t p) ;

        /** 移除
        @param [in] Base 移除数据
        */
        void Remove(byte_t* Base) ;
    };   

public:

    /** 默认构造函数
    */
    RCPPMDSubAlloc() ;
    
    /** 默认析构函数
    */
    ~RCPPMDSubAlloc() ;
    
public:
    
    /** 返回指针
    @param [in] offset 偏移
    @return 返回指针
    */
    void* GetPtr(uint32_t offset) const ;

    /** 返回前一块
    @param [in] offset 偏移
    @return 返回队列前一块
    */
    void* GetPtrNoCheck(uint32_t offset) const ;

    /** 返回偏移
    @param [in] ptr 指针
    @return 返回偏移
    */
    uint32_t GetOffset(void* ptr) const ;

    /** 返回偏移(无检查)
    @param [in] ptr 指针
    @return 返回偏移
    */
    uint32_t GetOffsetNoCheck(void* ptr) const ;
 
    /** 返回Blk
    @param [in] offset 偏移
    @return 返回Blk
    */
    RCPPMDMemBlock* GetBlk(uint32_t offset) const ;

    /** 返回节点
    @param [in] offset 偏移
    @return 返回指针
    */
    uint32_t* GetNode(uint32_t offset) const ;

    /** 插入节点
    @param [in] p 数据
    @param [in] index 插入位置
    */
    void InsertNode(void* p, int32_t index) ;

    /** 移除节点
    @param [in] index 序号
    @return 返回数据指针
    */
    void* RemoveNode(int32_t index) ;

    /** U2B
    @return 
    */
    uint32_t U2B(int32_t NU) const ;

    /** 分割块
    @param [in] pv 数据指针
    @param [in] oldIndex 旧编号
    @param [in] newIndex 新编号
    */
    void SplitBlock(void* pv, int32_t oldIndex, int32_t newIndex) ;

    /** 返回使用的内存数
    @return 返回使用的内存数
    */
    uint32_t GetUsedMemory() const ;

    /** 返回申请的内存数
    @return 返回申请的内存数
    */
    uint32_t GetSubAllocatorSize() const ;

    /** 停止分配
    */
    void StopSubAllocator() ;

    /** 开始分配
    @param [in] size 分配大小
    @return 成功返回true,否则返回false
    */
    bool StartSubAllocator(uint32_t size) ;

    /** 初始化
    */
    void InitSubAllocator() ;

    /** 合并空闲块
    */
    void GlueFreeBlocks() ;

    /** 分派单元
    @param [in] index 序号
    @return 返回分配的指针
    */
    void* AllocUnitsRare(int32_t index) ;

    /** 分派单元
    @return 返回分配的指针
    */
    void* AllocUnits(int32_t NU) ;

    /** 分派上下文
    @return 返回分派上下文
    */
    void* AllocContext() ;

    /** 扩展单元
    @param [in] oldPtr 旧指针
    @param [in] oldNU
    @return 返回扩展后指针
    */
    void* ExpandUnits(void* oldPtr, int32_t oldNU) ;

    /** 收缩单元
    @param [in] oldPtr 旧指针
    @param [in] oldNU
    @param [in] newNU
    @return 返回收缩后指针
    */
    void* ShrinkUnits(void* oldPtr, int32_t oldNU, int32_t newNU) ;

    /** 释放单元
    @param [in] ptr 释放指针
    @param [in] oldNU
    */
    void FreeUnits(void* ptr, int oldNU) ;
        
public:

    /** 数据指针
    */
    byte_t* m_pText ;

    /** 开始指针
    */
    byte_t* m_unitsStart ;
        
private:

    static const uint32_t N1 = 4 ;
    static const uint32_t N2 = 4 ;
    static const uint32_t N3 = 4 ;
    static const uint32_t N4 = (128+3-1*N1-2*N2-3*N3)/4 ;
    static const uint32_t UNIT_SIZE = 12 ;
    static const uint32_t N_INDEXES = N1 + N2 + N3 + N4 ;

public:

    /** Extra 1 * UNIT_SIZE for NULL support
        Extra 2 * UNIT_SIZE for s0 in GlueFreeBlocks()
    */
    static const uint32_t s_kExtraSize = (UNIT_SIZE * 3) ;

    /**
    */
    static const uint32_t s_kMaxMemBlockSize = 0xFFFFFFFF - s_kExtraSize ;
   
private:

    /** 分配大小
    */
    uint32_t m_subAllocatorSize ;

    /** 编号队列
    */
    byte_t m_indx2Units[N_INDEXES] ;

    /** 编号队列
    */
    byte_t m_units2Indx[128] ;

    /** glue数量
    */
    byte_t m_glueCount ;

    /** 空闲队列
    */
    uint32_t m_freeList[N_INDEXES] ;

    /** 基础地址
    */
    byte_t* m_base ;

    /** 堆开始位置
    */
    byte_t* m_heapStart ;

    /** 低位
    */
    byte_t* m_loUnit ;

    /** 高位
    */
    byte_t* m_hiUnit ;
};

END_NAMESPACE_RCZIP

#endif //__RCPPMDSubAlloc_h_
