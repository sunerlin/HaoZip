/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "compress/ppmd/RCPPMDSubAlloc.h"
#include "common/RCAlloc.h"

/////////////////////////////////////////////////////////////////
//RCPPMDSubAlloc class implementation

BEGIN_NAMESPACE_RCZIP

RCPPMDSubAlloc::RCPPMDMemBlock::RCPPMDMemBlock():
    m_stamp(0),
    m_NU(0),
    m_next(0),
    m_prev(0)
{
}

void RCPPMDSubAlloc::RCPPMDMemBlock::InsertAt(byte_t* Base, uint32_t p)
{
    m_prev = p;
    RCPPMDMemBlock *pp = (RCPPMDMemBlock *)(Base + p);
    m_next = pp->m_next;
    pp->m_next = ((RCPPMDMemBlock*)(Base + m_next))->m_prev = (uint32_t)((byte_t*)this - Base) ;
}

void RCPPMDSubAlloc::RCPPMDMemBlock::Remove(byte_t* Base)
{
    ((RCPPMDMemBlock*)(Base + m_prev))->m_next = m_next ;
    ((RCPPMDMemBlock*)(Base + m_next))->m_prev = m_prev ;
}

RCPPMDSubAlloc::RCPPMDSubAlloc():
    m_pText(NULL),
    m_unitsStart(NULL),
    m_subAllocatorSize(0),
    m_glueCount(0),
    m_base(NULL),
    m_heapStart(NULL),
    m_loUnit(NULL),
    m_hiUnit(NULL)    
{
    memset(m_indx2Units, 0, sizeof(m_indx2Units));
    memset(m_freeList, 0, sizeof(m_freeList));
}

RCPPMDSubAlloc::~RCPPMDSubAlloc()
{
    StopSubAllocator() ;
}

void* RCPPMDSubAlloc::GetPtr(uint32_t offset) const
{
    return (offset == 0) ? 0 : (void*)(m_base + offset) ;
}

void* RCPPMDSubAlloc::GetPtrNoCheck(uint32_t offset) const
{
    return (void*)(m_base + offset);
}

uint32_t RCPPMDSubAlloc::GetOffset(void* ptr) const
{
    return (ptr == 0) ? 0 : (uint32_t)((byte_t*)ptr - m_base);
}

uint32_t RCPPMDSubAlloc::GetOffsetNoCheck(void* ptr) const
{
    return (uint32_t)((byte_t*)ptr - m_base) ;
}

RCPPMDSubAlloc::RCPPMDMemBlock* RCPPMDSubAlloc::GetBlk(uint32_t offset) const
{
    return (RCPPMDMemBlock*)(m_base + offset);
}

uint32_t* RCPPMDSubAlloc::GetNode(uint32_t offset) const
{
    return (uint32_t*)(m_base + offset);
}

void RCPPMDSubAlloc::InsertNode(void* p, int32_t index)
{
    *(uint32_t*)p = m_freeList[index] ;
    m_freeList[index] = GetOffsetNoCheck(p) ;
}

void* RCPPMDSubAlloc::RemoveNode(int32_t index)
{
    uint32_t offset = m_freeList[index];
    uint32_t* p = GetNode(offset);
    m_freeList[index] = *p ;
    return (void*)p ;
}

uint32_t RCPPMDSubAlloc::U2B(int32_t NU) const
{
    return (uint32_t)(NU) * UNIT_SIZE ;
}

void RCPPMDSubAlloc::SplitBlock(void* pv, int32_t oldIndx, int32_t newIndex)
{
    int32_t i = 0 ;
    int32_t UDiff = m_indx2Units[oldIndx] - m_indx2Units[newIndex];
    byte_t* p = ((byte_t*)pv) + U2B(m_indx2Units[newIndex]);
    if (m_indx2Units[i = m_units2Indx[UDiff-1]] != UDiff)
    {
        InsertNode(p, --i);
        p += U2B(i = m_indx2Units[i]);
        UDiff -= i;
    }
    InsertNode(p, m_units2Indx[UDiff - 1]) ;
}

uint32_t RCPPMDSubAlloc::GetUsedMemory() const
{
    uint32_t RetVal = m_subAllocatorSize - (uint32_t)(m_hiUnit - m_loUnit) - (uint32_t)(m_unitsStart - m_pText);
    for (uint32_t i = 0; i < N_INDEXES; i++)
    {
        for (uint32_t pn = m_freeList[i]; pn != 0; RetVal -= (uint32_t)m_indx2Units[i] * UNIT_SIZE)
        {
            pn = *GetNode(pn);
        }
    }
    return (RetVal >> 2) ;
}

uint32_t RCPPMDSubAlloc::GetSubAllocatorSize() const
{
    return m_subAllocatorSize ;
}

void RCPPMDSubAlloc::StopSubAllocator()
{
    if (m_subAllocatorSize != 0)
    {
        RCAlloc::Instance().BigFree(m_base);
        m_subAllocatorSize = 0;
        m_base = 0;
    }
}

bool RCPPMDSubAlloc::StartSubAllocator(uint32_t size)
{
    if (m_subAllocatorSize == size)
    {
        return true;
    }
    StopSubAllocator();
    if (size == 0)
    {
        m_base = 0;
    }
    else
    {
        if ((m_base = (byte_t *)RCAlloc::Instance().BigAlloc(size + s_kExtraSize)) == 0)
        {
            return false;
        }
        m_heapStart = m_base + UNIT_SIZE; // we need such code to support NULL;
    }
    m_subAllocatorSize = size;
    return true ; 
}

void RCPPMDSubAlloc::InitSubAllocator()
{
    int32_t i = 0 ;
    int32_t k = 0 ;
    memset(m_freeList, 0, sizeof(m_freeList));
    m_hiUnit = (m_pText = m_heapStart) + m_subAllocatorSize;
    uint32_t Diff = UNIT_SIZE * (m_subAllocatorSize / 8 / UNIT_SIZE * 7);
    m_loUnit = m_unitsStart = m_hiUnit - Diff;
    for (i = 0, k=1; i < N1 ; i++, k += 1)
    {
        m_indx2Units[i] = (byte_t)k;
    }
    for (k++; i < N1 + N2      ;i++, k += 2)
    {
        m_indx2Units[i] = (byte_t)k;
    }
    for (k++; i < N1 + N2 + N3   ;i++,k += 3)
    {
        m_indx2Units[i] = (byte_t)k;
    }
    for (k++; i < N1 + N2 + N3 + N4; i++, k += 4)
    {
        m_indx2Units[i] = (byte_t)k;
    }
    m_glueCount = 0;
    for (k = i = 0; k < 128; k++)
    {
        i += (m_indx2Units[i] < k+1);
        m_units2Indx[k] = (byte_t)i;
    }
}

void RCPPMDSubAlloc::GlueFreeBlocks()
{
    uint32_t s0 = (uint32_t)(m_heapStart + m_subAllocatorSize - m_base);

    // We need add exta RCPPMDMemBlock with Stamp=0
    GetBlk(s0)->m_stamp = 0;
    s0 += UNIT_SIZE;
    RCPPMDMemBlock* ps0 = GetBlk(s0);

    uint32_t p = 0 ;
    int32_t i = 0 ;
    if (m_loUnit != m_hiUnit)
    {
        *m_loUnit=0;
    }
    ps0->m_next = ps0->m_prev = s0;

    for (i = 0; i < N_INDEXES; i++)
    {
        while (m_freeList[i] != 0)
        {
            RCPPMDMemBlock *pp = (RCPPMDMemBlock*)RemoveNode(i);
            pp->InsertAt(m_base, s0);
            pp->m_stamp = 0xFFFF;
            pp->m_NU = m_indx2Units[i];
        }
    }
    for (p = ps0->m_next; p != s0; p = GetBlk(p)->m_next)
    {
        for (;;)
        {
            RCPPMDMemBlock *pp = GetBlk(p);
            RCPPMDMemBlock *pp1 = GetBlk(p + pp->m_NU * UNIT_SIZE);
            if (pp1->m_stamp != 0xFFFF || int32_t(pp->m_NU) + pp1->m_NU >= 0x10000)
            {
                break;
            }
            pp1->Remove(m_base);
            pp->m_NU = (uint16_t)(pp->m_NU + pp1->m_NU);
        }
    }
    while ((p = ps0->m_next) != s0)
    {
        RCPPMDMemBlock *pp = GetBlk(p);
        pp->Remove(m_base);
        int32_t sz = 0 ;
        for (sz = pp->m_NU; sz > 128; sz -= 128, p += 128 * UNIT_SIZE)
        {
            InsertNode(m_base + p, N_INDEXES - 1);
        }
        if (m_indx2Units[i = m_units2Indx[sz-1]] != sz)
        {
            int32_t k = sz - m_indx2Units[--i];
            InsertNode(m_base + p + (sz - k) * UNIT_SIZE, k - 1);
        }
        InsertNode(m_base + p, i);
    }
}

void* RCPPMDSubAlloc::AllocUnitsRare(int32_t index)
{
    if ( !m_glueCount )
    {
        m_glueCount = 255;
        GlueFreeBlocks();
        if (m_freeList[index] != 0)
        {
            return RemoveNode(index);
        }
    }
    int32_t i = index ;
    do
    {
        if (++i == N_INDEXES)
        {
            m_glueCount--;
            i = U2B(m_indx2Units[index]);
            return (m_unitsStart - m_pText > i) ? (m_unitsStart -= i) : (NULL);
        }
    }while (m_freeList[i] == 0);
    void* RetVal = RemoveNode(i);
    SplitBlock(RetVal, i, index);
    return RetVal;
}

void* RCPPMDSubAlloc::AllocUnits(int32_t NU)
{
    int32_t indx = m_units2Indx[NU - 1];
    if (m_freeList[indx] != 0)
    {
        return RemoveNode(indx);
    }
    void* RetVal = m_loUnit;
    m_loUnit += U2B(m_indx2Units[indx]);
    if (m_loUnit <= m_hiUnit)
    {
        return RetVal;
    }
    m_loUnit -= U2B(m_indx2Units[indx]);
    return AllocUnitsRare(indx);
}

void* RCPPMDSubAlloc::AllocContext()
{
    if (m_hiUnit != m_loUnit)
    {
        return (m_hiUnit -= UNIT_SIZE);
    }
    if (m_freeList[0] != 0)
    {
        return RemoveNode(0);
    }
    return AllocUnitsRare(0);
}

void* RCPPMDSubAlloc::ExpandUnits(void* oldPtr, int32_t oldNU)
{
    int32_t i0=m_units2Indx[oldNU - 1] ;
    int32_t i1=m_units2Indx[oldNU - 1 + 1];
    if (i0 == i1)
    {
        return oldPtr;
    }
    void* ptr = AllocUnits(oldNU + 1);
    if (ptr)
    {
        memcpy(ptr, oldPtr, U2B(oldNU));
        InsertNode(oldPtr, i0);
    }
    return ptr;
}

void* RCPPMDSubAlloc::ShrinkUnits(void* oldPtr, int32_t oldNU, int32_t newNU)
{
    int32_t i0 = m_units2Indx[oldNU - 1] ;
    int32_t i1 = m_units2Indx[newNU - 1];
    if (i0 == i1)
    {
        return oldPtr;
    }
    if (m_freeList[i1] != 0)
    {
        void* ptr = RemoveNode(i1);
        memcpy(ptr, oldPtr, U2B(newNU));
        InsertNode(oldPtr,i0);
        return ptr;
    }
    else
    {
        SplitBlock(oldPtr, i0, i1);
        return oldPtr;
    }
}

void RCPPMDSubAlloc::FreeUnits(void* ptr, int32_t oldNU)
{
    InsertNode(ptr, m_units2Indx[oldNU - 1]) ;
}
  
END_NAMESPACE_RCZIP
