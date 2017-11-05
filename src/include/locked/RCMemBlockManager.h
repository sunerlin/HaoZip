/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCMemBlockManager_h_
#define __RCMemBlockManager_h_ 1

#include "base/RCNonCopyable.h"

BEGIN_NAMESPACE_RCZIP

class RCMemBlockManager:
    private RCNonCopyable
{
public:

    /** 构造函数
    @param [in] blockSize 数据块大小
    */
    explicit RCMemBlockManager(size_t blockSize = (1 << 20) ) ;
    
    /** 默认析构函数
    */
    ~RCMemBlockManager() ;

public:
    
    /** 分配内存空间
    @param [in] numBlocks 分配的数据块个数
    @return 成功返回true, 失败则返回false
    */
    bool AllocateSpace(size_t numBlocks) ;
    
    /** 释放内存空间
    */
    void FreeSpace();
    
    /** 获取当前数据块大小
    */
    size_t GetBlockSize() const ;
    
    /** 分配一个数据块
    @return 返回数据块地址, 失败返回NULL
    */
    void* AllocateBlock() ;
    
    /** 释放数据块内存空间
    @param [in] p 数据块内存地址
    */
    void FreeBlock(void* p) ;
    
private:
    
    /** 缓存起始地址
    */
    void* m_data;
    
    /** 数据块大小
    */
    size_t m_blockSize;
    
    /** 第一个空闲内存地址
    */
    void* m_headFree;
};

END_NAMESPACE_RCZIP

#endif //__RCMemBlockManager_h_
