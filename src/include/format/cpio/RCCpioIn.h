/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCCpioIn_h_
#define __RCCpioIn_h_ 1

#include "interface/IStream.h"
#include "RCCpioHeader.h"
#include "RCCpioItem.h"

BEGIN_NAMESPACE_RCZIP

static const uint32_t s_maxBlockSize = RCCpioHeader::kRecordSize;

class RCCpioIn
{
public:

    /** 打开
    @param [in] inStream 输入流
    @return 成功返回RC_S_OK, 失败返回错误码
    */
    HResult Open(IInStream* inStream);

    /** 获取下项
    @param [in] filled 是否获取成功
    @param [out] itemInfo 获取到的数据内容
    @return 成功返回RC_S_OK, 失败返回错误码
    */
    HResult GetNextItem(bool& filled, RCCpioItemEx& itemInfo);

    /** 跳过
    @param [in] numBytes 跳过多少字节
    @return 成功返回RC_S_OK, 失败返回错误码
    */
    HResult Skip(uint64_t numBytes);

    /** 跳过数据记录
    @param [in] dataSize 数据大小
    @param [in] align 对齐字节
    @return 成功返回RC_S_OK, 失败返回错误码
    */
    HResult SkipDataRecords(uint64_t dataSize, uint32_t align);

private:

    /** 读取字节
    @return 返回读取的字节，如果失败返回0xFF
    @throws 如果读取数据失败抛异常
    */
    byte_t ReadByte();

    /** 读两个字节转成16位无符号整型
    @return 返回16位无符号整型
    */
    uint16_t ReadUInt16();

    /** 读四个字节转成32位无符号整型
    @return 返回32位无符号整型
    */
    uint32_t ReadUInt32();

    /** 读取八个字节转成十六进制数据
    @param [in] resultValue 返回十六进制数据
    @return 成功返回true，否则返回false
    */
    bool ReadNumber(uint32_t& resultValue);

    /** 读取四个字节转成十进制数据
    @param [in] size 读取多少位
    @param [in] resultValue 返回十进制数据
    @return 成功返回true，否则返回false
    */
    bool ReadOctNumber(int32_t size, uint32_t& resultValue);

    /** 读取多字节
    @param [out] data 存放读取结果的缓冲区地址
    @param [in] size 缓冲区的长度
    @param [in] processedSize 返回实际读到的大小
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult ReadBytes(void* data, uint32_t size, uint32_t& processedSize);

private:

    /** 输入流
    */
    IInStreamPtr m_inStream;

    /** 位置
    */
    uint64_t m_position;

    /** 块大小
    */
    uint16_t m_blockSize;

    /** 块
    */
    byte_t m_block[s_maxBlockSize];

    /** 块位置
    */
    uint32_t m_blockPos;
};

END_NAMESPACE_RCZIP

#endif //__RCCpioIn_h_
