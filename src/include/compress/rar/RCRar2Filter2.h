/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCRar2Filter2_h_
#define __RCRar2Filter2_h_ 1

#include "compress/rar/RCRar2Defs.h"
#include "compress/rar/RCRar2Filter.h"

BEGIN_NAMESPACE_RCZIP

/** RAR2 过滤2
*/
class RCRar2Filter2
{
public:

    /** 默认构造函数
    */
    RCRar2Filter2() ;
    
    /** 默认析构函数
    */
    ~RCRar2Filter2() ;
    
public:

    /** 初始化
    */
    void Init() ;

    /** 解码
    @param [in] delta
    @return 返回解码字节
    */
    byte_t Decode(byte_t delta) ;
    
public:

    /** Filters
    */
    RCRar2Filter m_filters[RCRar2Defs::s_kNumChanelsMax];

    /** Channel Delta
    */
    int32_t m_channelDelta ;

    /** Current Channel
    */
    int32_t m_currentChannel ;
};

END_NAMESPACE_RCZIP

#endif //__RCRar2Filter2_h_
