/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCRangeBitModel_h_
#define __RCRangeBitModel_h_ 1

#include "compress/range/RCRangeCoderDefs.h"

BEGIN_NAMESPACE_RCZIP

/** Range Bit 模型
*/
template <int32_t numMoveBits>
class RCRangeBitModel
{
public:

    /** 默认构造函数
    */
    RCRangeBitModel() ;
    
    /** 返回Prob
    @return 返回Prob
    */
    uint32_t GetProb() const ;

    /** 设置Prob
    @param [in] prob 值
    */
    void SetProb(uint32_t prob) ;

    /** 初始化
    */
    void Init() ;

    /** 更新模型 
    @param [in] symbol 符号
    */
    void UpdateModel(uint32_t symbol) ;

protected:

    /** prob
    */
    uint32_t m_prob ;
};

END_NAMESPACE_RCZIP

#include "RCRangeBitModel.inl"

#endif //__RCRangeBitModel_h_
