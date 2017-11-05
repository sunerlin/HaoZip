/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCMemBlockManagerMt_h_
#define __RCMemBlockManagerMt_h_ 1

#include "locked/RCMemBlockManager.h"
#include "thread/RCMutex.h"
#include "thread/RCSynchronization.h"

BEGIN_NAMESPACE_RCZIP

class RCMemBlockManagerMt:
    private RCMemBlockManager
{
public:

    /** 默认构造函数
    @param [in] blockSize 数据块大小
    */
    explicit RCMemBlockManagerMt(size_t blockSize = (1 << 20)) ;
    
    /** 默认析构函数
    */
    ~RCMemBlockManagerMt() ;

public:
    
    /** 分配内存空间
    @param [in] numBlocks 分配的数据块个数
    @param [in] numNoLockBlocks 解锁计数
    @return 成功返回RC_S_OK, 失败则返回错误码
    */
    HResult AllocateSpace(size_t numBlocks, size_t numNoLockBlocks = 0) ;
    
    /** 分配内存空间
    @param [in] desiredNumberOfBlocks 分配的数据块个数
    @param [in] numNoLockBlocks 解锁计数
    @return 成功返回RC_S_OK, 失败则返回错误码
    */
    HResult AllocateSpaceAlways(size_t desiredNumberOfBlocks, size_t numNoLockBlocks = 0);
    
    /** 释放内存空间
    */
    void FreeSpace();
    
    /** 分配一个数据块
    @return 返回数据块地址, 失败返回NULL
    */
    void* AllocateBlock() ;
    
    /** 释放数据块内存空间
    @param [in] p 数据块内存地址
    @param [in] lockMode 是否为加锁模式
    */
    void FreeBlock(void* p, bool lockMode = true);
    
    /** 释放加锁块的锁计数
    @param [in] number 释放的计数
    @return 成功返回RC_S_OK, 失败则返回错误码
    */
    HResult ReleaseLockedBlocks(int32_t number) ;
    
    /** 获取当前数据块大小
    */
    size_t GetBlockSize() const ;
    
public:
    
    /** 同步信号量
    */
    RCSemaphore m_semaphore ;

private:
    
    /** 同步锁
    */
    RCMutex m_lock ;
};

END_NAMESPACE_RCZIP

#endif //__RCMemBlockManagerMt_h_
