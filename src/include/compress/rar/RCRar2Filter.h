/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCRar2Filter_h_
#define __RCRar2Filter_h_ 1

#include "base/RCDefs.h"

BEGIN_NAMESPACE_RCZIP

/** RAR2 过滤
*/
class RCRar2Filter
{
public:

    /** 默认构造函数
    */
    RCRar2Filter() ;
    
    /** 默认析构函数
    */
    ~RCRar2Filter() ;
    
    /** 初始化数据
    */
    void Init() ;
    
    /** 解码数据
    @param [in] channelDelta
    @param [in] deltaByte
    @return 
    */
    byte_t Decode(int32_t& channelDelta, byte_t deltaByte) ;
    
public:

    int32_t m_k1;
    int32_t m_k2; 
    int32_t m_k3;
    int32_t m_k4;
    int32_t m_k5;
    int32_t m_d1;
    int32_t m_d2;
    int32_t m_d3;
    int32_t m_d4;
    int32_t m_lastDelta;
    uint32_t m_dif[11];
    uint32_t m_byteCount;
    int32_t m_lastChar;
};

END_NAMESPACE_RCZIP

#endif //__RCRar2Filter_h_
