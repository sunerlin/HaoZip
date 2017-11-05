/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCCabCheckSum2_h_
#define __RCCabCheckSum2_h_ 1

#include "base/RCTypes.h"

BEGIN_NAMESPACE_RCZIP

class RCCabCheckSum2
{
public:

    /** 默认构造函数
    */
    RCCabCheckSum2();

public:

    /** 初始化
    */
    void Init() ;

    /** 更新
    @param [in] data 内存数据
    @param [in] size 内存大小
    */
    void Update(const void *data, uint32_t size);

    /** 完成数据更新
    */
    void FinishDataUpdate();

    /** 更新UInt32
    @param [in] v 数值
    */
    void UpdateUInt32(uint32_t v) ;

    /** 获取结果
    @return 返回结果
    */
    uint32_t GetResult() const ;

private:

    /** 数值
    */
    uint32_t m_value;

    /** 位置
    */
    int32_t m_pos;

    /** 历史数据
    */
    byte_t m_hist[4];
};

END_NAMESPACE_RCZIP

#endif //__RCCabCheckSum2_h_