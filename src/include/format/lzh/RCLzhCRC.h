/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCLzhCRC_h_
#define __RCLzhCRC_h_ 1

#include "base/RCDefs.h"

BEGIN_NAMESPACE_RCZIP

class RCLzhCRC
{
public:

    /** 默认构造函数
    */
    RCLzhCRC():
      m_value(0)
    {
    };

public:

    /** 初始化
    */
    void Init()
    {
        m_value = 0;
    }

    /** 更新
    @param [in] data 数据
    @param [in] size 大小
    */
    void Update(const void* data, size_t size);

    /** 获取数值
    @return 返回数值
    */
    uint16_t GetDigest() const
    {
        return m_value;
    }

public:

    /** table
    */
    static uint16_t s_table[256];

    /** 初始化table
    */
    static void InitTable();

private:

    /** 数值
    */
    uint16_t m_value;
};

END_NAMESPACE_RCZIP

#endif //__RCLzhCRC_h_
