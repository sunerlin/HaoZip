/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCDmgMethods_h_
#define __RCDmgMethods_h_ 1

#include "base/RCNonCopyable.h"
#include "common/RCVector.h"
#include "format/dmg/RCDmgMethodStat.h"
#include "base/RCString.h"

BEGIN_NAMESPACE_RCZIP

class RCDmgFile ;

class RCDmgMethods:
    private RCNonCopyable
{
public:

    /** 默认构造函数
    */
    RCDmgMethods() ;
    
public:

    /** 更新
    @param [in] file dmg文件
    */
    void Update(const RCDmgFile& file);

    /** 获取字符串
    @return 返回字符串
    */
    RCString GetString() const ;
    
public:

    /** 统计
    */
    RCVector<RCDmgMethodStatPtr> m_stats ;

    /** 类型
    */
    RCVector<uint32_t> m_types ;
    
private:

    /** 获取大小字符串
    @param [in] value 数值
    @return 返回字符串
    */
    RCString GetSizeString(uint64_t value) const ;
};

END_NAMESPACE_RCZIP

#endif //__RCDmgMethods_h_
