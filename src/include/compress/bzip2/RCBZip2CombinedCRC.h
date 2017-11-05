/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCBZip2CombinedCRC_h_
#define __RCBZip2CombinedCRC_h_ 1

#include "base/RCDefs.h"

BEGIN_NAMESPACE_RCZIP

/** 带CRC校验BZIP2
*/
class RCBZip2CombinedCRC
{
public:

    /** 默认构造函数
    */
    RCBZip2CombinedCRC() ;
    
    /** 默认析构函数
    */
    ~RCBZip2CombinedCRC() ;
    
public:
    
    /** 初始化
    */
    void Init() ;
    
    /** 更新
    @param [in] v 
    */
    void Update(uint32_t v) ;
    
    /** 返回CRC校验
    @return 返回CRC校验
    */
    uint32_t GetDigest() const ;
    
private:
    
    /** 校验值
    */
    uint32_t m_value ;
};

END_NAMESPACE_RCZIP

#endif //__RCBZip2CombinedCRC_h_
