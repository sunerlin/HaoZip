/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCSha1Context32_h_
#define __RCSha1Context32_h_ 1

#include "crypto/hash/RCSha1ContextBase2.h"

BEGIN_NAMESPACE_RCZIP

/** Sha1 上下文
*/
class RCSha1Context32:
    public RCSha1ContextBase2
{
public:

    /** 默认构造函数
    */
    RCSha1Context32() ;
    
    /** 默认析构函数
    */
    ~RCSha1Context32() ;
public:

    /** 更新
    @param [in] data 数据
    @param [in] size 数据大小
    */
    void Update(const uint32_t *data, size_t size);

    /** 结束计算
    @param [out] digest 返回计算结果
    */
    void Final(uint32_t *digest);
};

END_NAMESPACE_RCZIP

#endif //__RCSha1Context32_h_
