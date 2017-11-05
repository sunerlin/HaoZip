/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCRandom_h_
#define __RCRandom_h_ 1

#include "base/RCNonCopyable.h"

BEGIN_NAMESPACE_RCZIP

class RCRandom:
    private RCNonCopyable
{
public:

    /** 默认构造函数
    */
    RCRandom() ;
    
    /** 默认析构函数
    */
    ~RCRandom() ;
    
public:
    
    /** 使用当前时间初始化
    */
    void Init() ;
    
    /** 指定种子进行初始化
    @param [in] seed 初始化所用种子值
    */
    void Init(uint32_t seed) ;
    
    /** 生成随机数
    @return 返回生成的随机数值
    */
    int32_t Generate() const ;
};

END_NAMESPACE_RCZIP

#endif //__RCRandom_h_
