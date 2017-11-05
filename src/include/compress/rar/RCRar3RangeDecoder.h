/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCRar3RangeDecoder_h_
#define __RCRar3RangeDecoder_h_ 1

#include "compress/rar/RCRar3BitDecoder.h"
#include "compress/ppmd/RCPPMDRangeDecoderVirt.h"

BEGIN_NAMESPACE_RCZIP

/** RAR3 Range 解码器
*/
class RCRar3RangeDecoder:
    public RCRar3BitDecoder,
    public RCPPMDRangeDecoderVirt
{
public:

    /** 默认构造函数
    */
    RCRar3RangeDecoder() ;
    
    /** 默认析构函数
    */
    ~RCRar3RangeDecoder() ;
    
public:

    /** 正规化
    */
    void Normalize() ;

    /** 初始化解码器
    */
    void InitRangeCoder() ;

    /** 返回阀值
    @param [in] total 总数
    @return 返回阀值
    */
    virtual uint32_t GetThreshold(uint32_t total) ;

    /** 解码
    @param [in] start 开始
    @param [in] size 大小
    */
    virtual void Decode(uint32_t start, uint32_t size) ;

    /** 位解码
    @param [in] size0 大小
    @param [in] numTotalBits 位总数
    @return 返回解码结果
    */
    virtual uint32_t DecodeBit(uint32_t size0, uint32_t numTotalBits) ;
    
public:

    /** range
    */
    uint32_t m_range ;

    /** low
    */
    uint32_t m_low ;

    /** code
    */
    uint32_t m_code ;
};

END_NAMESPACE_RCZIP

#endif //__RCRar3RangeDecoder_h_
