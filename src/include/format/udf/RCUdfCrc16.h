/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCUdfCrc16_h_
#define __RCUdfCrc16_h_ 1

#include "base/RCTypes.h"
#include "algorithm/Types.h"

BEGIN_NAMESPACE_RCZIP

/** Udf Crc 16
*/
class RCUdfCrc16
{
public:

    /** 生成Crc表
    */
    static void MY_FAST_CALL Crc16GenerateTable(void);

    /** Crc 更新
    @param [in] v 
    @param [in] data 数据
    @param [in] size 数据大小
    @return 返回Crc
    */
    static uint16_t MY_FAST_CALL Crc16_Update(uint16_t v, const void *data, size_t size);

    /** Crc 计算
    @param [in] data 数据
    @param [in] size 数据大小
    @return 返回Crc
    */
    static uint16_t MY_FAST_CALL Crc16Calc(const void *data, size_t size);

private:

    /** Crc 数据表
    */
    static uint16_t s_crc16Table[256];
};

/** Udf Crc 16 表初始化
*/
struct RCUdfCrc16TableInit 
{ 
    /** 构造函数
    */
    RCUdfCrc16TableInit() 
    { 
        RCUdfCrc16::Crc16GenerateTable(); 
    } 
};

END_NAMESPACE_RCZIP

#endif //__RCUdfCrc16_h_
