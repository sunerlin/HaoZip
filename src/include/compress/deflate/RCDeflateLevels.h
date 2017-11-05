/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCDeflateLevels_h_
#define __RCDeflateLevels_h_ 1

#include "base/RCDefs.h"
#include "compress/deflate/RCDeflateDefs.h"

BEGIN_NAMESPACE_RCZIP

/** Deflate 级别
*/
class RCDeflateLevels
{
public:

    /** 默认构造函数
    */
    RCDeflateLevels() ;
    
    /** 默认析构函数
    */
    ~RCDeflateLevels() ;
    
public:
    
    /** 清除
    */
    void SubClear() ;
    
    /** 设置固定级别
    */
    void SetFixedLevels() ;
    
public:
    
    /** 级别长度
    */
    byte_t m_litLenLevels[RCDeflateDefs::s_kFixedMainTableSize];
        
    /** 级别
    */
    byte_t m_distLevels[RCDeflateDefs::s_kFixedDistTableSize];
};

END_NAMESPACE_RCZIP

#endif //__RCDeflateLevels_h_
