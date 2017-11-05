/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCMtf8Decoder_h_
#define __RCMtf8Decoder_h_ 1

#include "base/RCNonCopyable.h"

BEGIN_NAMESPACE_RCZIP

/** Mtf8 解码器
*/
class RCMtf8Decoder:
    private RCNonCopyable
{
public:
    
    /** 宏与类型定义
    */
#ifdef RCZIP_BIT_64
    typedef uint64_t RCMtfVar ;
    static const int32_t RC_MTF_MOVS = 3 ;
#else
    typedef uint32_t RCMtfVar ;
    static const int32_t RC_MTF_MOVS = 2 ;
#endif

    /** 缓冲区位置的最大值
    */
    static const int32_t POS_LEN = 256 ;

public:

    /** 默认构造函数
    */
    RCMtf8Decoder() ;
    
    /** 默认析构函数
    */
    ~RCMtf8Decoder() ;
    
public:
    
    /** 缓冲区数据清零
    */
    void StartInit() ;
    
    /** 数据赋值
    @param [in] pos 数值下标
    @param [in] value 位数据值
    */
    void Add(uint32_t pos, byte_t value) ;
    
    /** 获取缓冲区第一个数据
    @return 返回位数据值
    */
    byte_t GetHead() const ;
    
    /** 获取并把数据移动到第一个位置
    @param [in] pos 数据下标
    @return 返回第一个数据值
    */
    byte_t GetAndMove(uint32_t pos) ;
    
private:
    
    /** 数据缓冲区
    */
    RCMtfVar Buffer[POS_LEN >> RC_MTF_MOVS] ;
};

END_NAMESPACE_RCZIP

#endif //__RCMtf8Decoder_h_
