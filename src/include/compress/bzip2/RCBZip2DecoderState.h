/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCBZip2DecoderState_h_
#define __RCBZip2DecoderState_h_ 1

#include "thread/RCSynchronization.h"
#include "thread/RCThread.h"

BEGIN_NAMESPACE_RCZIP

/** BZip2 解码器
*/
class RCBZip2Decoder ;

/** BZip2解码状态
*/
class RCBZip2DecoderState
{
public:

    /** 默认构造函数
    */
    RCBZip2DecoderState() ;
    
    /** 默认析构函数
    */
    ~RCBZip2DecoderState() ;
    
public:
    
    /** 内存申请
    @return 成功返回true,否则返回false
    */
    bool Alloc() ;
    
    /** 内存是否
    */
    void Free() ;

public:
    
    /** 计数
    */
    uint32_t* m_counters ;
        
public:

#ifdef COMPRESS_BZIP2_MT

    /** 解码器指针
    */
    RCBZip2Decoder* m_decoder ;
    
    /** 线程指针
    */
    RCThreadPtr m_spThread ;
    
    /** 是否表优化
    */
    bool m_optimizeNumTables ;

    /** 同步事件
    */
    RCAutoResetEvent m_streamWasFinishedEvent ;
    
    /** 同步事件
    */
    RCAutoResetEvent m_waitingWasStartedEvent ;

    /** 同步事件
    */
    RCAutoResetEvent m_canWriteEvent ;

    /** mt Pad
    */
    byte_t m_mtPad[1 << 8]; // It's pad for Multi-Threading. Must be >= Cache_Line_Size.

    /** 创建
    @return 成功返回RC_S_OK,否则返回错误号
    */
    HResult Create() ;
    
    /** 完成
    */
    void FinishStream() ;
    
    /** 线程函数
    */
    void ThreadFunc();

#endif //COMPRESS_BZIP2_MT
};

END_NAMESPACE_RCZIP

#endif //__RCBZip2DecoderState_h_
