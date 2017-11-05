/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCRandomGenerator_h_
#define __RCRandomGenerator_h_ 1

#include "crypto/hash/RCSha1Defs.h"
#include "base/RCNonCopyable.h"
#include "base/RCSingleton.h"

BEGIN_NAMESPACE_RCZIP

class RCMutex ;

/** 随机数生成
*/
class RCRandomGeneratorImpl:
    private RCNonCopyable
{
protected:

    /** 默认构造函数
    */
    RCRandomGeneratorImpl() ;
    
    /** 默认析构函数
    */
    ~RCRandomGeneratorImpl() ;

public:

    /** 随机数生成
    @param [in] data 数据缓存
    @param [in] size 缓存大小
    */
    void Generate(byte_t *data, uint32_t size) ;
    
private:

    /** 初始化
    */
    void Init();
    
private:

    /** 缓存
    */
    byte_t m_buff[RCSha1Defs::kDigestSize] ;

    /** 是否初始化
    */
    bool m_needInit ;

    /** 同步互斥量
    */
    RCMutex* m_lock ;
};

typedef RCSingleton<RCRandomGeneratorImpl> RCRandomGenerator ;

END_NAMESPACE_RCZIP

#endif //__RCRandomGenerator_h_
