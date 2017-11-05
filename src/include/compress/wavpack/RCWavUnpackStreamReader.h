/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCWavUnpackStreamReader_h_
#define __RCWavUnpackStreamReader_h_ 1

#include "base/RCTypes.h"
#include "common/RCBuffer.h"
#include "interface/IStream.h"
#include "interface/ICoder.h"
#include "wavpack/wavpack.h"

BEGIN_NAMESPACE_RCZIP

/** 流操作函数
@param [in] inStream 输入流
@param [in] error 错误信息
@param [in] flags 标志
@param [in] norm_offset 偏移量
@return 返回解压上下文指针
*/
WavpackContext* RcWavpackOpenInputStream(void* inStream, char* error, int flags, int norm_offset);

/** WavPack文件流操作
*/
class RCWavPackBufferStream
{
public:

    /** 构造函数
    @param [in] inStream 输入流
    */
    RCWavPackBufferStream(ISequentialInStream* inStream);

public:

    /** 读取字节
    @param [in] data 数据缓存
    @param [in] bcount 缓存大小
    */
    int32_t ReadByte(void* data, int32_t bcount);

    /** 返回位置
    @return 返回当前位置
    */
    uint32_t GetPos(void);

    /** 回退字节
    @param [in] c 回退字节数
    @return 返回回退字节数
    */
    int PushBackByte(int c);

private:
    
    enum 
    {
        /** 定义缓存大小
        */
        ENUM_BUFFER_SIZE = 0x2000,
    };

    /** 输入流
    */
    ISequentialInStream* m_inStream;

    /** 缓存
    */
    RCByteBuffer m_buffer;

    /** 数据位置
    */
    uint32_t m_dataPos;

    /** 读入总大小
    */
    uint32_t m_totalReadSize;
};

END_NAMESPACE_RCZIP

#endif //__RCWavUnpackStreamReader_h_
