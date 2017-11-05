/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCSha1ContextBase_h_
#define __RCSha1ContextBase_h_ 1

#include "base/RCDefs.h"

BEGIN_NAMESPACE_RCZIP

/** Sha1 上下文基类
*/
class RCSha1ContextBase
{
public:

    /** 默认构造函数
    */
    RCSha1ContextBase() ;
    
    /** 默认析构函数
    */
    ~RCSha1ContextBase() ;
    
public:

    /** 初始化
    */
    void Init();

    /** 取得块摘要
    @param [in] blockData 块数据
    @param [out] destDigest 摘要
    @param [in] returnRes 是否返回Res
    */
    void GetBlockDigest(uint32_t *blockData, uint32_t *destDigest, bool returnRes = false);
    
    /** PrepareBlock can be used only when size <= 13. size in Words
    @param [in] block 块
    @param [in] size 大小
    */
    void PrepareBlock(uint32_t *block, uint32_t size) const;
    
protected:

    /** 更新块
    @param [in] data 数据
    @param [in] returnRes 是否返回Res
    */
    void UpdateBlock(uint32_t *data, bool returnRes = false) ;
        
protected:
    
    /** 状态
    */
    uint32_t m_state[5] ;

    /** 计数
    */
    uint64_t m_count ;
};

END_NAMESPACE_RCZIP

#endif //__RCSha1ContextBase_h_
