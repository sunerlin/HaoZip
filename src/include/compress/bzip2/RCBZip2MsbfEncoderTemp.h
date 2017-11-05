/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCBZip2MsbfEncoderTemp_h_
#define __RCBZip2MsbfEncoderTemp_h_ 1

#include "base/RCNonCopyable.h"

BEGIN_NAMESPACE_RCZIP

/** BZip2 Msbf 编码
*/
class RCBZip2MsbfEncoderTemp:
    private RCNonCopyable
{
public:

    /** 默认构造函数
    */
    RCBZip2MsbfEncoderTemp() ;
    
    /** 默认析构函数
    */
    ~RCBZip2MsbfEncoderTemp() ;
    
public:
    
    /** 设置流
    @param [in] buffer 缓存指针
    */
    void SetStream(byte_t* buffer) ;
    
    /** 返回流指针
    @return 返回流指针
    */
    byte_t* GetStream() const ;
    
    /** 初始化
    */
    void Init() ;
    
    /** Flush数据
    */
    void Flush() ;
    
    /** 写入位
    @param [in] value 数值
    @param [in] numBits 位数
    */
    void WriteBits(uint32_t value, int32_t numBits) ;
    
    /** 返回字节位置
    @return 返回字节位置
    */
    uint32_t GetBytePos() const ;
    
    /** 返回位置
    @return 返回位置
    */
    uint32_t GetPos() const ;
    
    /** 返回当前字节
    @return 返回当前字节
    */
    byte_t GetCurByte() const ;
    
    /** 设置位置
    @param [in] bitPos 位置
    */
    void SetPos(uint32_t bitPos) ;
    
    /** 设置状态
    @param [in] bitPos 位置
    @param [in] curByte 当前字节
    */
    void SetCurState(int32_t bitPos, byte_t curByte) ;
    
private:
    
    /** 位置
    */
    uint32_t m_pos;
    
    /** bit 位置
    */
    int32_t m_bitPos;
    
    /** 当前字节
    */
    byte_t m_curByte;
    
    /** 缓存指针
    */
    byte_t* m_buffer;
};

END_NAMESPACE_RCZIP

#endif //__RCBZip2MsbfEncoderTemp_h_
