/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCBZip2CRC_h_
#define __RCBZip2CRC_h_ 1

#include "base/RCDefs.h"

BEGIN_NAMESPACE_RCZIP

/** BZip2CRC
*/
class RCBZip2CRC
{
public:

    /** 默认构造函数
    */
    RCBZip2CRC() ;
    
    /** 默认析构函数
    */
    ~RCBZip2CRC() ;
    
public:
    
    /** 初始化CRC表
    */
    static void InitTable() ;
    
    /** 初始化
    */
    void Init() ;
    
    /** 计算校验值
    @param [in] b 输入数据
    */
    void UpdateByte(byte_t b) ;
    
    /** 计算校验值
    @param [in] b 输入数据
    */
    void UpdateByte(uint32_t b) ;
    
    /** 返回CRC校验
    @return 返回CRC校验值
    */
    uint32_t GetDigest() const ;
    
private:
    
    /** 校验值
    */
    uint32_t m_value;
    
    /** CRC表
    */
    static uint32_t s_table[256] ;
};

END_NAMESPACE_RCZIP

#endif //__RCBZip2CRC_h_
