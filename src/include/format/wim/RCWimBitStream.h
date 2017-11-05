/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCWimBitStream_h_
#define __RCWimBitStream_h_ 1

#include "base/RCDefs.h"
#include "common/RCInBuffer.h"
#include "interface/IStream.h"

BEGIN_NAMESPACE_RCZIP

/** Wim 位数据流
*/
class RCWimBitStream
{
public:

    /** 默认构造函数
    */
    RCWimBitStream() ;
    
    /** 默认析构函数
    */
    ~RCWimBitStream() ;
    
public:

    /** 创建
    @param [in] bufferSize 缓存大小
    @return 成功返回true,否则返回false
    */
    bool Create(uint32_t bufferSize) ;

    /** 设置输入流
    @param [in] s 输入流
    */
    void SetStream(ISequentialInStream *s) ;

    /** 释放流
    */
    void ReleaseStream() ;

    /** 初始化
    */
    void Init() ;

    /** 直接读取字节
    @return 返回读取的字节
    */
    byte_t DirectReadByte() ;

    /** 正常化
    */
    void Normalize() ;

    /** 返回值
    @param [in] numBits 位数
    @return 返回uint32值
    */
    uint32_t GetValue(uint32_t numBits) ;

    /** 移动位置 
    @param [in] numBits 位数
    */
    void MovePos(uint32_t numBits) ;

    /** 读取位
    @param [in] numBits 位数
    @return 返回uint32
    */
    uint32_t ReadBits(uint32_t numBits) ;
    
private:

    /** 输入流
    */
    RCInBuffer m_stream;

    /**
    */
    uint32_t m_value;

    /**
    */
    uint32_t m_bitPos;
};

END_NAMESPACE_RCZIP

#endif //__RCWimBitStream_h_
