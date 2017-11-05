/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "format/udf/RCUdfMap32.h"

/////////////////////////////////////////////////////////////////
//RCUdfMap32 class implementation

BEGIN_NAMESPACE_RCZIP

struct RCUdfMap32::RCUdfMapNode
{
    uint32_t m_key ;
    uint32_t m_keys[2] ;
    uint32_t m_values[2] ;
    uint16_t m_len ;
    byte_t   m_isLeaf[2] ;
};

static const uint32_t s_kUdfMap32NumBitsMax = sizeof(uint32_t) * 8;

static inline uint32_t GetSubBit(uint32_t v, uint32_t n)
{
    return (uint32_t)(v >> n) & 1 ;
}

RCUdfMap32::~RCUdfMap32()
{
    Clear() ;
}

uint32_t RCUdfMap32::GetSubBits(uint32_t value, uint32_t startPos, uint32_t numBits)
{
    if (startPos == sizeof(value) * 8)
    {
        return 0 ;
    }
    value >>= startPos ;
    if (numBits == sizeof(value) * 8)
    {
        return value;
    }
    return value & (((uint32_t)1 << numBits) - 1) ;
}

bool RCUdfMap32::Set(uint32_t key, uint32_t value)
{
    if (m_nodes.empty())
    {
        RCUdfMapNode* node = new RCUdfMapNode ;
        node->m_key = key ;
        node->m_keys[0] = key ;
        node->m_keys[1] = key ;
        node->m_values[0] = value ;
        node->m_values[1] = value ;
        node->m_isLeaf[0] = 1 ;
        node->m_isLeaf[1] = 1 ;
        node->m_len = s_kUdfMap32NumBitsMax;
        m_nodes.push_back(node) ;
        return false;
    }
    if (m_nodes.size() == 1)
    {
        RCUdfMapNode* node = m_nodes[0] ;
        if (node->m_len == s_kUdfMap32NumBitsMax)
        {
            if (key == node->m_key)
            {
                node->m_values[0] = value ;
                node->m_values[1] = value ;
                return true;
            }
            uint32_t i = s_kUdfMap32NumBitsMax - 1;
            for (;GetSubBit(key, i) == GetSubBit(node->m_key, i); --i)
            {
                if(i == 0)
                {
                    break ;
                }
            }
            node->m_len = (uint16_t)(s_kUdfMap32NumBitsMax - (1 + i));
            uint32_t newBit = GetSubBit(key, i);
            node->m_values[newBit] = value;
            node->m_keys[newBit] = key;
            return false;
        }
    }

    int32_t cur = 0;
    uint32_t bitPos = s_kUdfMap32NumBitsMax;
    for (;;)
    {
        RCUdfMapNode* node = m_nodes[cur] ;
        bitPos -= node->m_len;
        if (GetSubBits(key, bitPos, node->m_len) != GetSubBits(node->m_key, bitPos, node->m_len))
        {
            uint32_t i = node->m_len - 1;
            for (; GetSubBit(key, bitPos + i) == GetSubBit(node->m_key, bitPos + i); --i)
            {
                if(i == 0)
                {
                    break ;
                }
            }

            RCUdfMapNode* e2 = new RCUdfMapNode(*node) ;
            e2->m_len = (uint16_t)i ;

            node->m_len = (uint16_t)(node->m_len - (1 + i));
            uint32_t newBit = GetSubBit(key, bitPos + i);
            node->m_values[newBit] = value;
            node->m_isLeaf[newBit] = 1;
            node->m_isLeaf[1 - newBit] = 0;
            node->m_keys[newBit] = key;
            node->m_keys[1 - newBit] = (uint32_t)m_nodes.size();
            m_nodes.push_back(e2);
            return false;
        }
        uint32_t bit = GetSubBit(key, --bitPos);

        if (node->m_isLeaf[bit])
        {
            if (key == node->m_keys[bit])
            {
                node->m_values[bit] = value;
                return true;
            }
            uint32_t i = bitPos - 1;
            for (;GetSubBit(key, i) == GetSubBit(node->m_keys[bit], i); --i)
            {
                if(i == 0)
                {
                    break ;
                }
            }

            RCUdfMapNode* e2 = new RCUdfMapNode ;

            uint32_t newBit = GetSubBit(key, i);
            e2->m_values[newBit] = value;
            e2->m_values[1 - newBit] = node->m_values[bit];
            e2->m_isLeaf[newBit] = e2->m_isLeaf[1 - newBit] = 1;
            e2->m_keys[newBit] = key;
            e2->m_keys[1 - newBit] = e2->m_key = node->m_keys[bit];
            e2->m_len = (uint16_t)(bitPos - (1 + i));

            node->m_isLeaf[bit] = 0;
            node->m_keys[bit] = (uint32_t)m_nodes.size();

            m_nodes.push_back(e2);
            return false;
        }
        cur = (int32_t)node->m_keys[bit] ;
    }
}

bool RCUdfMap32::Find(uint32_t key, uint32_t& valueRes) const
{
    valueRes = (uint32_t)int32_t(-1) ;
    if (m_nodes.empty())
    {
        return false ;
    }
    if (m_nodes.size() == 1)
    {
        const RCUdfMapNode* node = m_nodes[0] ;
        if (node->m_len == s_kUdfMap32NumBitsMax)
        {
            valueRes = node->m_values[0];
            return (key == node->m_key) ;
        }
    }

    int32_t cur = 0 ;
    uint32_t bitPos = s_kUdfMap32NumBitsMax;
    for (;;)
    {
        const RCUdfMapNode* node = m_nodes[cur] ;
        bitPos -= node->m_len;
        if (GetSubBits(key, bitPos, node->m_len) != GetSubBits(node->m_key, bitPos, node->m_len))
        {
            return false;
        }
        uint32_t bit = GetSubBit(key, --bitPos);
        if (node->m_isLeaf[bit])
        {
            valueRes = node->m_values[bit];
            return (key == node->m_keys[bit]);
        }
        cur = (int32_t)node->m_keys[bit];
    }
    return false ;
}

void RCUdfMap32::Clear()
{
    size_t count = m_nodes.size() ;
    for(size_t index = 0; index < count; ++index)
    {
        RCUdfMapNode* node = m_nodes[index] ;
        delete node ;
    }
    m_nodes.clear() ;
}

END_NAMESPACE_RCZIP
