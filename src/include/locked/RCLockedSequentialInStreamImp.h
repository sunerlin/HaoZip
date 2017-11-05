/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCLockedSequentialInStreamImp_h_
#define __RCLockedSequentialInStreamImp_h_ 1

#include "interface/IStream.h"
#include "interface/IUnknownImpl.h"

BEGIN_NAMESPACE_RCZIP

class RCLockedInStream ;

class RCLockedSequentialInStreamImp:
    public IUnknownImpl<ISequentialInStream>
{
public:

    /** 默认构造函数
    */
    RCLockedSequentialInStreamImp() ;
    
    /** 默认析构函数
    */
    ~RCLockedSequentialInStreamImp() ;
    
public:
    
    /** 初始化
    @param [in] lockedInStream 线程安全的输入流接口
    @param [in] startPos 数据起始偏移
    */
    void Init(RCLockedInStream* lockedInStream, uint64_t startPos) ;
    
    /** 读取数据 
    @param [out] data 存放数据的缓冲区
    @param [in] size 缓冲区大小
    @param [out] processedSize 实际读取的大小
    @return 成功返回RC_S_OK, 失败则返回错误码
    */
    virtual HResult Read(void* data, uint32_t size, uint32_t* processedSize) ;
    
private:
    
    /** 线程安全的数据输入流接口指针
    */
    RCLockedInStream* m_lockedInStream ;
    
    /** 当前数据位置偏移
    */
    uint64_t m_pos ;
};

END_NAMESPACE_RCZIP

#endif //__RCLockedSequentialInStreamImp_h_
