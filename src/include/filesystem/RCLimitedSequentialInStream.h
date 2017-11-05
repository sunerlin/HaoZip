/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCLimitedSequentialInStream_h_
#define __RCLimitedSequentialInStream_h_ 1

#include "interface/IStream.h"
#include "interface/IUnknownImpl.h"

BEGIN_NAMESPACE_RCZIP

class RCLimitedSequentialInStream:
    public IUnknownImpl<ISequentialInStream>
{
public:

    /** 默认构造函数
    */
    RCLimitedSequentialInStream() ;
    
    /** 默认析构函数
    */
    ~RCLimitedSequentialInStream() ;
    
public:
    
    /** 设置输入流接口
    @param [in] stream 输入流接口指针
    */
    void SetStream(ISequentialInStream* stream) ;
    
    /** 初始化
    @param [in] streamSize 流数据大小
    */
    void Init(uint64_t streamSize) ;
    
    /** 获取数据大小
    @return 返回数据大小值
    */
    uint64_t GetSize() const ;
    
    /** 是否已经到达结束位置
    @return 返回true表示已达结束位置，否则返回false
    */
    bool WasFinished() const ;
    
    /** 读取数据 
    @param [out] data 存放数据的缓冲区
    @param [in] size 缓冲区大小
    @param [out] processedSize 实际读取的大小
    @return 成功返回RC_S_OK, 失败则返回错误码
    */
    virtual HResult Read(void* data, uint32_t size, uint32_t* processedSize) ;
    
private:
    
    /** 输入流接口
    */
    ISequentialInStreamPtr m_spInStream;
    
    /** 数据大小
    */
    uint64_t m_size;
    
    /** 数据偏移位置
    */
    uint64_t m_pos;
    
    /** 数据是否已经读完
    */
    bool m_wasFinished;
};

/** 智能指针定义
*/
typedef RCComPtr<RCLimitedSequentialInStream>   RCLimitedSequentialInStreamPtr ;

END_NAMESPACE_RCZIP

#endif //__RCLimitedSequentialInStream_h_
