/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCSha1Context_h_
#define __RCSha1Context_h_ 1

#include "crypto/hash/RCSha1ContextBase2.h"

BEGIN_NAMESPACE_RCZIP

/** Sha1 上下文
*/
class RCSha1Context:
    public RCSha1ContextBase2
{
public:

    /** 默认构造函数
    */
    RCSha1Context() ;
    
    /** 默认析构函数
    */
    ~RCSha1Context() ;

public:
    
    /** 更新
    @param [in] data 数据
    @param [in] size 大小
    @param [in] rar350Mode 是否rar350模式
    */
    void Update(byte_t *data, size_t size, bool rar350Mode = false);

    /** 更新
    @param [in] data 数据
    @param [in] size 大小
    */
    void Update(const byte_t *data, size_t size) ;

    /** 结束计算
    @param [out] digest 返回结果
    */
    void Final(byte_t *digest);
};

END_NAMESPACE_RCZIP

#endif //__RCSha1Context_h_
