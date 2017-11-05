/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCUdfCDString_h_
#define __RCUdfCDString_h_ 1

#include "base/RCTypes.h"
#include "base/RCString.h"
#include "common/RCBuffer.h"

BEGIN_NAMESPACE_RCZIP

/** Udf CD String
*/
struct RCUdfCDString128
{
    /** 数据
    */
    byte_t m_data[128];

    /** 解析
    @param [in] buf 数据缓存
    */
    void Parse(const byte_t *buf);

    /** 返回字符串
    @return 返回字符串
    */
    RCString GetString() const;
};

/** Udf CD String
*/
struct RCUdfCDString
{
    /** 数据缓存
    */
    RCByteBuffer m_data;

    /** 解析
    @param [in] p 数据
    @param [in] size 数据大小
    */
    void Parse(const byte_t *p, uint32_t size);

    /** 返回字符串
    @return 返回字符串
    */
    RCString GetString() const;
};

END_NAMESPACE_RCZIP

#endif //__RCUdfCDString_h_
