/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCDmgMethodStat_h_
#define __RCDmgMethodStat_h_ 1

#include "base/RCNonCopyable.h"
#include "base/RCSmartPtr.h"

BEGIN_NAMESPACE_RCZIP

class RCDmgMethodStat:
    private RCNonCopyable
{
public:

    /** 默认构造函数
    */
    RCDmgMethodStat() ;

public:

    /** 块哥数据
    */
    uint32_t m_numBlocks ;

    /** 解压包的大小
    */
    uint64_t m_packSize ;

    /** 解压后的大小
    */
    uint64_t m_unpSize ;
};

/** RCDmgMethodStat智能指针
*/
typedef RCSharedPtr<RCDmgMethodStat> RCDmgMethodStatPtr ;

END_NAMESPACE_RCZIP

#endif //__RCDmgMethodStat_h_
