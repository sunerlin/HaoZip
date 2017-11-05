/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCLzhIn_h_
#define __RCLzhIn_h_ 1

#include "interface/IStream.h"
#include "RCLzhItem.h"

BEGIN_NAMESPACE_RCZIP

class RCLzhIn
{
public:

    /** 打开文档
    @param [in] inStream 输入流
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult Open(IInStream* inStream);

    /** 获取下一个项
    @param [out] filled 是否已填充
    @param [out] item 返回下一个项内容
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult GetNextItem(bool& filled, RCLzhItemEx& item);

    /** 跳过
    @param [in] numBytes 跳过的字节数
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult Skip(uint64_t numBytes);

private:

    /** 读取多字节
    @param [out] data 存放读取结果的缓冲区地址
    @param [in] size 缓冲区的长度
    @param [out] processedSize 返回实际读取的长度
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult ReadBytes(void* data, uint32_t size, uint32_t& processedSize);

    /** 检查读取多字节
    @param [out] data 存放读取结果的缓冲区地址
    @param [in] size 缓冲区的长度
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult CheckReadBytes(void* data, uint32_t size);

private:

    /** 输入流
    */
    IInStreamPtr m_stream;

    /** 偏移位置
    */
    uint64_t m_position;
};

END_NAMESPACE_RCZIP

#endif //__RCLzhIn_h_
