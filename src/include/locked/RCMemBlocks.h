/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCMemBlocks_h_
#define __RCMemBlocks_h_ 1

#include "base/RCNonCopyable.h"
#include "common/RCVector.h"

BEGIN_NAMESPACE_RCZIP

class RCMemBlockManagerMt ;
class ISequentialOutStream ;

class RCMemBlocks
{
    /** 内存块列表
    */
    typedef RCVector<void*> block_type ;

public:

    /** 默认构造函数
    */
    RCMemBlocks() ;
    
    /** 默认析构函数
    */
    ~RCMemBlocks() ;
    
public:
    
    /** 释放内存块空间
    @param [in] manager 内存管理器
    */
    void FreeOpt(RCMemBlockManagerMt* manager) ;
    
    /** 将数据写入输出流
    @param [in] blockSize 内存块大小
    @param [in] outStream 输出流接口指针
    @return 成功返回RC_S_OK, 失败则返回错误码
    */
    HResult WriteToStream(size_t blockSize, ISequentialOutStream* outStream) const ;
    
    /** 设置总大小
    @param [in] totalSize 数据总大小
    */
    void SetTotalSize(uint64_t totalSize) ;
    
    /** 获取总大小
    @return 返回数据总大小
    */
    uint64_t GetTotalSize(void) const ;
    
    /** 获取数据块大小
    @return 返回数据块大小字节数
    */
    size_t GetBlockSize() const ;
    
    /** 获取数据块列表
    */
    const block_type& GetBlock() const ;
    
    /** 增加一个内存块
    @param [in] item 内存块起始地址
    */
    void Add(void* item);
    
private:
    
    /** 释放内存块空间
    @param [in] manager 内存管理器
    */
    void Free(RCMemBlockManagerMt* manager);
    
protected:

    /** 数据块列表
    */
    block_type m_blocks ;
    
    /** 总大小
    */
    uint64_t m_totalSize ;
};

END_NAMESPACE_RCZIP

#endif //__RCMemBlocks_h_
