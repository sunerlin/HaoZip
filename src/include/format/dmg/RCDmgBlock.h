/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCDmgBlock_h_
#define __RCDmgBlock_h_ 1

#include "base/RCNonCopyable.h"
#include "base/RCSmartPtr.h"

BEGIN_NAMESPACE_RCZIP

class RCDmgBlock:
    private RCNonCopyable
{
public:

    /** 默认构造函数
    */
    RCDmgBlock() ;

public:

    /** 获取下一个包的偏移
    @return 返回下一个包的偏移
    */
    uint64_t GetNextPackOffset() const
    {
        return m_packPos + m_packSize ;
    }
    
public:
    
    /** 类型
    */
    uint32_t m_type;

    /** 解压偏移
    */
    uint64_t m_unpPos;

    /** 解压后的大小
    */
    uint64_t m_unpSize;

    /** 解压前的大小
    */
    uint64_t m_packPos;

    /** 解压前的大小
    */
    uint64_t m_packSize;
};

/** RCDmgBlock智能指针
*/
typedef RCSharedPtr<RCDmgBlock> RCDmgBlockPtr ;

END_NAMESPACE_RCZIP

#endif //__RCDmgBlock_h_
