/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCGZipItem_h_
#define __RCGZipItem_h_ 1

#include "base/RCString.h"
#include "common/RCBuffer.h"
#include "format/gzip/RCGZipHeader.h"
#include "interface/ICoder.h"
#include "interface/IStream.h"

BEGIN_NAMESPACE_RCZIP

class RCGZipItem
{
public:

    /** 方法
    */
    byte_t m_method;

    /** 标志
    */
    byte_t m_flags;

    /** 时间
    */
    uint32_t m_time;

    /** 解压标志
    */
    byte_t m_extraFlags;

    /** 系统平台
    */
    byte_t m_hostOS;

    /** crc
    */
    uint32_t m_crc;

    /** 大小
    */
    uint32_t m_size32;

    /** 名字
    */
    RCStringA m_name;

    /** 注释
    */
    RCStringA m_comment;

public:

    /** 头部是否进行crc
    @return 是返回true，否则返回false
    */
    bool HeaderCrcIsPresent() const ;

    /** 是否解压
    @return 是返回true，否则返回false
    */
    bool ExtraFieldIsPresent() const ;

    /** 是否有名字
    @return 有返回true，否则返回false
    */
    bool NameIsPresent() const ;

    /** 是否有注释
    @return 有返回true，否则返回false
    */
    bool CommentIsPresent() const ;

    /** 清除
    */
    void Clear() ;

    /** 读取头部
    @param [in] stream 压缩/解压数据接口
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult ReadHeader(IGZipResumeDecoder* stream) ;

    /** 读取尾部
    @param [in] stream 压缩/解压数据接口
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult ReadFooter1(IGZipResumeDecoder* stream) ;

    /** 读取尾部
    @param [in] stream 输入流
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult ReadFooter2(ISequentialInStream* stream) ;

    /** 写头部
    @param [in] stream 输出流
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult WriteHeader(ISequentialOutStream* stream) ;

    /** 写尾部
    @param [in] stream 输出流
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult WriteFooter(ISequentialOutStream* stream) ;

private:

    /** 读取多字节
    @param [in] stream 压缩/解压数据接口
    @param [out] buf 存放读取结果的缓冲区地址
    @param [in] size 缓冲区的长度
    @return 成功返回RC_S_OK，否则返回错误号
    */
    static HResult ReadBytes(IGZipResumeDecoder* stream, byte_t* data, uint32_t size) ;

    /** 跳过字节
    @param [in] stream 压缩/解压数据接口
    @param [in] size 跳过的大小
    @return 成功返回RC_S_OK，否则返回错误号
    */
    static HResult SkipBytes(IGZipResumeDecoder* stream, uint32_t size) ;

    /** 读取字节
    @param [in] stream 压缩/解压数据接口
    @param [in] value 返回读取的字节
    @return 成功返回RC_S_OK，否则返回错误号
    */
    static HResult ReadUInt16(IGZipResumeDecoder* stream, uint16_t& value) ;

    /** 读取一个字符串
    @param [in] stream 压缩/解压数据接口
    @param [out] s 返回读取的字符串
    @param [in] limit 最大读取的大小
    @return 成功返回RC_S_OK，否则返回错误号
    */
    static HResult ReadString(IGZipResumeDecoder* stream, RCStringA& s, uint32_t limit) ;

    /** 检测标志
    @param [in] flag 标志
    @return 通过检测返回true，否则返回false
    */
    bool TestFlag(byte_t flag) const ;
};

END_NAMESPACE_RCZIP

#endif //__RCGZipItem_h_
