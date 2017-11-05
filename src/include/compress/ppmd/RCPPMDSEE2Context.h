/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCPPMDSEE2Context_h_
#define __RCPPMDSEE2Context_h_ 1

#include "base/RCNonCopyable.h"

BEGIN_NAMESPACE_RCZIP

/** SEE-contexts for PPM-contexts with masked symbols
*/
class RCPPMDSEE2Context:
    private RCNonCopyable
{
public:

    /** 默认构造函数
    */
    RCPPMDSEE2Context() ;
    
    /** 默认析构函数
    */
    ~RCPPMDSEE2Context() ;

public:

    /** 总计
    */
    uint16_t m_summ ;

    /** 偏移
    */
    byte_t m_shift ;

    /** 个数
    */
    byte_t m_count ;

public:

    /** 初始化
    @param [in] InitVal 初始值
    */
    void init(int32_t InitVal) ;

    /** 取得Mean
    @return 返回Mean
    */
    uint32_t getMean() ;

    /** 更新
    */
    void update() ;
};

END_NAMESPACE_RCZIP

#endif //__RCPPMDSEE2Context_h_
