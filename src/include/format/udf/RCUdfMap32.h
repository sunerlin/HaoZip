/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCUdfMap32_h_
#define __RCUdfMap32_h_ 1

#include "base/RCNonCopyable.h"
#include "base/RCSmartPtr.h"
#include "common/RCVector.h"

BEGIN_NAMESPACE_RCZIP

/** Udf Map
*/
class RCUdfMap32:
    private RCNonCopyable
{
public:
    
    /** 默认析构函数
    */
    ~RCUdfMap32() ;
    
    /** 设置数据, 如果键已经存在，更新值数据，如果不存在，则增加新节点
    @param [in] key 键数据
    @param [in] value 值数据
    @return 如果该key已经存在，返回true, 否则返回false
    */
    bool Set(uint32_t key, uint32_t value) ;
    
    /** 查找数据
    @param [in] key 需要查找的键数据
    @param [out] valueRes 返回查找到的值
    @return 如果找到返回true, 否则返回false
    */
    bool Find(uint32_t key, uint32_t& valueRes) const ;
    
    /** 清除数据
    */
    void Clear() ;
    
private:
    
    /** 内部实现函数
    @param [in] value 
    @param [in] startPos 开始位置
    @param [in] numBits 位数
    */
    static uint32_t GetSubBits(uint32_t value, uint32_t startPos, uint32_t numBits) ;
    
private:

    /** 节点类型定义
    */
    struct RCUdfMapNode ;
    
    /** 节点数据
    */
    RCVector<RCUdfMapNode*> m_nodes ;
};

typedef RCSharedPtr<RCUdfMap32> RCUdfMap32Ptr;

END_NAMESPACE_RCZIP

#endif //__RCUdfMap32_h_
