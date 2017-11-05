/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCDelta_h_
#define __RCDelta_h_ 1

#include "base/RCDefs.h"
#include "algorithm/Delta.h"

BEGIN_NAMESPACE_RCZIP

/** Delat 算法
*/
class RCDelta
{
public:
    
    /** 默认构造函数
    */
    RCDelta(): 
        m_delta(1) 
    {
    }

    /** 初始化
    */
    void DeltaInit() 
    { 
        Delta_Init(m_state); 
    }

protected:
    
    /** Delta
    */
    unsigned m_delta;

    /** 状态
    */
    byte_t m_state[DELTA_STATE_SIZE];

};

END_NAMESPACE_RCZIP

#endif //__RCDelta_h_
