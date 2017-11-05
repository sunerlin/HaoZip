/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCBitlDecoder_h_
#define __RCBitlDecoder_h_ 1

#include "common/RCBitlBaseDecoder.h"

BEGIN_NAMESPACE_RCZIP

template<class TInByte>
class RCBitlDecoder:
    public RCBitlBaseDecoder<TInByte>
{
public:

    /** 默认构造函数
    */
    RCBitlDecoder() ;
    
    /** 默认析构函数
    */
    ~RCBitlDecoder() ;
    
public:
    
    /** 初始化
    */
    void Init() ;
    
    /** 解码
    */
    void Normalize() ;  
    
    /** 获取位对应的值
    @param [in] numBits 位数
    @return 位对应的值
    */
    uint32_t GetValue(int32_t numBits) ;
    
    /** 移动到指定位
    @param [in] numBits 位数
    */
    void MovePos(int32_t numBits) ;
    
    /** 读取位数
    @param [in] numBits 位数
    @return 返回位对应值
    */
    uint32_t ReadBits(int32_t numBits) ;
    
    /** 对齐到字节
    */
    void AlignToByte() ;
    
    /** 读取字节
    @return 读取字节值
    */
    byte_t ReadByte() ;
    
private:
    
    /** 解码数据值
    */
    uint32_t m_normalValue;
};

END_NAMESPACE_RCZIP

#include "RCBitlDecoder.inl"

#endif //__RCBitlDecoder_h_
