/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCMemLockBlocks_h_
#define __RCMemLockBlocks_h_ 1

#include "locked/RCMemBlocks.h"

BEGIN_NAMESPACE_RCZIP

class RCMemLockBlocks:
    public RCMemBlocks
{
public:

    /** 默认构造函数
    */
    RCMemLockBlocks() ;
    
    /** 默认析构函数
    */
    ~RCMemLockBlocks() ;
    
public:
    
    /** 释放内存块空间
    @param [in] memManager 内存管理器
    */
    void Free(RCMemBlockManagerMt* memManager);  
    
    /** 切换到无锁模式
    @param [in] memManager 内存管理器
    @return 成功返回RC_S_OK, 失败则返回错误码
    */
    HResult SwitchToNoLockMode(RCMemBlockManagerMt* memManager);
    
    /** 释放内存管理
    @param [in] blocks 内存块管理器
    @param [in] memManager 内存管理器
    */
    void Detach(RCMemLockBlocks& blocks, RCMemBlockManagerMt* memManager);
    
    /** 设置锁属性
    @param [in] isLockMode 是否为加锁模式
    */
    void SetLockMode(bool isLockMode) ;
    
    /** 获取加锁属性
    @return 加锁模式返回true, 否则返回false
    */
    bool GetLockMode() const;

private:
    
    /** 释放内存块
    @param [in] index 内存块下标值
    @param [in] memManager 内存管理器
    */
    void FreeBlock(uint32_t index, RCMemBlockManagerMt* memManager);
    
private:
    
    /** 是否为加锁模式
    */
    bool m_lockMode ;
};

END_NAMESPACE_RCZIP

#endif //__RCMemLockBlocks_h_
