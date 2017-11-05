/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCRar3BitDecoder_h_
#define __RCRar3BitDecoder_h_ 1

#include "base/RCNonCopyable.h"
#include "common/RCInBuffer.h"

BEGIN_NAMESPACE_RCZIP

/** RAR3 Bit 家麻雀
*/
class RCRar3BitDecoder:
    private RCNonCopyable
{
public:

    /** 默认构造函数
    */
    RCRar3BitDecoder() ;
    
    /** 默认析构函数
    */
    ~RCRar3BitDecoder() ;
    
public:

    /** 创建
    @param [in] bufferSize 缓存大小
    @return 成功返回true,否则返回false
    */
    bool Create(uint32_t bufferSize) ;

    /** 设置输入流
    @param [in] inStream 输入流
    */
    void SetStream(ISequentialInStream *inStream) ;

    /** 释放流
    */
    void ReleaseStream() ;

    /** 初始化
    */
    void Init() ;

    /** 返回已经处理大小
    @return 返回已经处理大小
    */
    uint64_t GetProcessedSize() const ;

    /** 返回Bit位置
    @return 返回Bit位置
    */
    uint32_t GetBitPosition() const ;

    /** 返回值
    @param [in] numBits 位数
    @return 返回值
    */
    uint32_t GetValue(uint32_t numBits) ;

    /** 移动位置
    @param [in] numBits 位数
    */
    void MovePos(uint32_t numBits) ;

    /** 读取位数据
    @param [in] numBits 位数
    @return 返回读取结果
    */
    uint32_t ReadBits(uint32_t numBits) ;
    
public:

    /** Bit 位置
    */
    uint32_t m_bitPos ;

    /** 输入流缓存
    */
    RCInBuffer m_stream ;
    
private:

    /** 值
    */
    uint32_t m_value ;
};

END_NAMESPACE_RCZIP

#endif //__RCRar3BitDecoder_h_
