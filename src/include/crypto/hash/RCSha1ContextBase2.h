/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCSha1ContextBase2_h_
#define __RCSha1ContextBase2_h_ 1

#include "crypto/hash/RCSha1ContextBase.h"
#include "crypto/hash/RCSha1Defs.h"

BEGIN_NAMESPACE_RCZIP

/** Sha1 上下文基类2
*/
class RCSha1ContextBase2:
    public RCSha1ContextBase
{
public:

    /** 默认构造函数
    */
    RCSha1ContextBase2() ;
    
    /** 默认析构函数
    */
    ~RCSha1ContextBase2() ;

public:

    /** 初始化
    */
    void Init() ;

protected:

    /** 更新块
    */
    void UpdateBlock() ;
            
protected:

    /** 计数
    */
    uint32_t m_count2 ;

    /** 缓存
    */
    uint32_t m_buffer[RCSha1Defs::kBlockSizeInWords];  
};

END_NAMESPACE_RCZIP

#endif //__RCSha1ContextBase2_h_
