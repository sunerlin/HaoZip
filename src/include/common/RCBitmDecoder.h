/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCBitmDecoder_h_
#define __RCBitmDecoder_h_ 1

#include "base/RCNonCopyable.h"
#include "interface/IStream.h"

BEGIN_NAMESPACE_RCZIP

template<class TInByte>
class RCBitmDecoder:
    private RCNonCopyable
{
public:

    /** 默认构造函数
    */
    RCBitmDecoder() ;
    
    /** 默认析构函数
    */
    ~RCBitmDecoder() ;

public:
    
    /** 创建缓冲区
    @param [in] bufferSize 缓冲区大小
    @return 创建成功返回true, 失败则返回false
    */
    bool Create(uint32_t bufferSize) ;
    
    /** 设置输入流接口
    @param [in] inStream 输入流接口指针
    */
    void SetStream(ISequentialInStream* inStream) ;
    
    /** 释放输入流
    */
    void ReleaseStream(); 
    
    /** 初始化
    */
    void Init() ;  
    
    /** 获取已处理的数据字节数
    @return 返回已处理的数据字节数
    */
    uint64_t GetProcessedSize() const ;
    
    /** 解码
    */
    void Normalize() ;
    
    /** 获取位对应的值
    @param [in] numBits 位数
    @return 位对应的值
    */
    uint32_t GetValue(uint32_t numBits) const ;  
    
    /** 移动到指定位
    @param [in] numBits 位数
    */
    void MovePos(uint32_t numBits) ;  
    
    /** 读取位数
    @param [in] numBits 位数
    @return 返回位对应值
    */
    uint32_t ReadBits(uint32_t numBits) ;
    
    /** 对齐到字节
    */
    void AlignToByte() ;
    
private:
    
    /** 当前位的位置
    */
    uint32_t m_bitPos;
    
    /** 编码值
    */
    uint32_t m_value;
    
    /** 关联的输入流
    */
    TInByte m_stream ;
    
private:
    
    /** 大数值位数
    */
    static const int32_t s_kNumBigValueBits = 8 * 4 ;
    
    /** 数值字节数
    */
    static const int32_t s_kNumValueBytes = 3 ;
    
    /** 数值位数
    */
    static const int32_t s_kNumValueBits = 8  * s_kNumValueBytes ;
    
    /** 数值掩码
    */
    static const uint32_t s_kMask = (1 << s_kNumValueBits) - 1 ;
};

END_NAMESPACE_RCZIP

#include "RCBitmDecoder.inl"

#endif //__RCBitmDecoder_h_
