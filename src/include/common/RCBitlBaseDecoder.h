/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCBitlBaseDecoder_h_
#define __RCBitlBaseDecoder_h_ 1

#include "base/RCNonCopyable.h"
#include "common/RCBitlDecoderData.h"

BEGIN_NAMESPACE_RCZIP

class ISequentialInStream ;

template<class TInByte>
class RCBitlBaseDecoder:
    private RCNonCopyable
{
public:

    /** 默认构造函数
    */
    RCBitlBaseDecoder() ;
    
    /** 默认析构函数
    */
    ~RCBitlBaseDecoder() ;
    
public:
    
    /** 创建缓冲区
    @param [in] bufferSize 缓冲区大小
    @return 成功返回true, 失败则返回false
    */   
    bool Create(uint32_t bufferSize) ;
    
    /** 设置输入流接口
    @param [in] inStream 输入流接口指针
    */
    void SetStream(ISequentialInStream* inStream) ;
    
    /** 释放输入流
    */
    void ReleaseStream() ;
    
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
    
    /** 读取字节位
    @param [in] numBits 读取的位数
    @return 返回处理结果
    */
    uint32_t ReadBits(int32_t numBits) ;
    
    /** 判断ExtraBits是否已读取
    @return 如果已处理返回true, 否则返回false
    */
    bool ExtraBitsWereRead() const ;
    
    /** ExtraBits 的个数
    @return ExtraBits 的个数
    */
    uint32_t GetNumExtraBytes() const ;
    
protected:
    
    /** 当前位的位置
    */
    int32_t m_bitPos ;
    
    /** 编码值
    */
    uint32_t m_value ;
    
    /** 关联的输入流
    */
    TInByte m_stream ;
    
    /** ExtraBytes 个数
    */
    uint32_t m_numExtraBytes;
};

END_NAMESPACE_RCZIP

#include "RCBitlBaseDecoder.inl"

#endif //__RCBitlBaseDecoder_h_
