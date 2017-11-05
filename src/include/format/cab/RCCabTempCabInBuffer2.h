/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCCabTempCabInBuffer2_h_
#define __RCCabTempCabInBuffer2_h_ 1

#include "base/RCTypes.h"

BEGIN_NAMESPACE_RCZIP

class RCCabTempCabInBuffer2
{
public:

    /** 读取字节
    @return 返回读取字节值
    */
    byte_t ReadByte();

    /** 读四个字节转成32位无符号整型
    @return 返回32位无符号整型
    */
    uint32_t ReadUInt32();

    /** 读两个字节转成16位无符号整型
    @return 返回16位无符号整型
    */
    uint16_t ReadUInt16();

public:

    /** 数据块头部大小
    */
    static const uint32_t s_dataBlockHeaderSize = 8;

    /** 存放数据块头部的缓冲区
    */
    byte_t m_buffer[s_dataBlockHeaderSize];

    /** 位置
    */
    uint32_t m_pos;
};

END_NAMESPACE_RCZIP

#endif //__RCCabTempCabInBuffer2_h_