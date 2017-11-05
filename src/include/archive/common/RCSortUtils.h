/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCSortUtils_h_
#define __RCSortUtils_h_ 1

#include "base/RCString.h"
#include "common/RCVector.h"

BEGIN_NAMESPACE_RCZIP

/** 排序辅助类
*/
class RCSortUtils
{
public:

    /** 文件名排序函数, 对于文件名的比较，调用RCWildcardUtils::CompareFileNames函数实现
    @param [in] fileNames 需要排序的字符串数组
    @param [out] indices 排序结果，数组内元素为strings中的下标值
    @param [in] useSysSort 是否使用Windows系统提供的排序方法
    */
    static void SortFileNames(const RCVector<RCString>& fileNames, RCIntVector& indices, bool useSysSort = false) ;

    /** 文件名比较
    @param [in] lhs
    @param [in] rhs
    @param [in] useSysSort 是否使用Windows系统提供的排序方法
    @return 返回值定义如下：
            0  - 相等
            1  - lhs > rhs
            -1 - lhs < rhs
    */
    static int32_t Compare(const RCString& lhs, const RCString& rhs, bool useSysSort = false);
};

END_NAMESPACE_RCZIP

#endif //__RCSortUtils_h_
