/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCMtf8Encoder_h_
#define __RCMtf8Encoder_h_ 1

#include "base/RCNonCopyable.h"

BEGIN_NAMESPACE_RCZIP

/** Mtf8 解码器
*/
class RCMtf8Encoder:
    private RCNonCopyable
{
public:

    /** 默认构造函数
    */
    RCMtf8Encoder() ;
    
    /** 默认析构函数
    */
    ~RCMtf8Encoder() ;

public:
    
    /** 查找指定位数据，并且移动到最前面
    @param [in] value 需要查找的数据
    @return 返回数据原来的位置，如果没有找到，则返回-1
    */
    int32_t FindAndMove(byte_t value) ;
    
    /** 操作符 [] 
    */
    byte_t& operator[](uint32_t index) ;
    
    /** 常量操作符 [] 
    */
    const byte_t operator[](uint32_t index) const ;
    
public:
    
    /** 缓冲区大小
    */
    static const int32_t BUFFER_LEN = 256 ;
    
private:
    
    /** 数据缓冲区
    */
    byte_t m_buffer[BUFFER_LEN];
};

END_NAMESPACE_RCZIP

#endif //__RCMtf8Encoder_h_
