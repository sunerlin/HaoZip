/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCChmIn_h_
#define __RCChmIn_h_ 1

#include "common/RCInBuffer.h"
#include "RCChmItem.h"

BEGIN_NAMESPACE_RCZIP

class RCChmProgressVirt
{
public:

    /** 设置总数
    @param [in] numFiles 文件数
    @return 成功返回RC_S_OK, 否则返回错误码
    */
    virtual HResult SetTotal(uint64_t numFiles) = 0 ;

    /** 设置完成数
    @param [in] numFiles 文件数
    @return 成功返回RC_S_OK, 否则返回错误码
    */
    virtual HResult SetCompleted(uint64_t numFiles) = 0 ;
};

class RCChmIn
{
public:

    /** 打开chm文件
    @param [in] inStream 输入流
    @param [out] database 输出chm基本数据
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult OpenChm(IInStream* inStream, RCChmDatabase& database);

    /** 打开帮助文件
    @param [in] inStream 输入流
    @param [out] database 输出chm基本数据
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult OpenHelp2(IInStream* inStream, RCChmDatabase& database);

    /** 打开高级
    @param [in] inStream 输入流
    @param [out] database 输出chm基本数据
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult OpenHighLevel(IInStream* inStream, RCChmFilesDatabase& database);

    /** 打开
    @param [in] inStream 输入流
    @param [in] searchHeaderSizeLimit 查找头部的最大偏移位置
    @param [out] database 输出chm基本数据
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult Open2(IInStream* inStream, const uint64_t* searchHeaderSizeLimit, RCChmFilesDatabase& database);

    /** 打开
    @param [in] inStream 输入流
    @param [in] searchHeaderSizeLimit 查找头部的最大偏移位置
    @param [out] database 输出chm基本数据
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult Open(IInStream* inStream, const uint64_t* searchHeaderSizeLimit, RCChmFilesDatabase& database);

private:

    /** 读一个字节
    @return 返回一个字节
    */
    byte_t ReadByte();

    /** 读字节
    @param [out] data 返回读取的内容
    @param [in] size 读取的大小
    */
    void ReadBytes(byte_t* data, uint32_t size);

    /** 跳过
    @param [in] size 跳过的大小
    */
    void Skip(size_t size);

    /** 读两个字节转成16位无符号整型
    @return 返回16位无符号整型
    */
    uint16_t ReadUInt16();

    /** 读四个字节转成32位无符号整型
    @return 返回32位无符号整型
    */
    uint32_t ReadUInt32();

    /** 读八个字节转成64位无符号整型
    @return 返回64位无符号整型
    */
    uint64_t ReadUInt64();

    /** 读加密的八个字节
    return 返回64位无符号整型
    */
    uint64_t ReadEncInt();

    /** 读字符串
    @param [in] size 读的大小
    @param [out] s 返回字符串
    */
    void ReadString(int size, RCStringA &s);

    /** 读unicode字符串
    @param [in] size 读的大小
    @param [out] s 返回unicode字符串
    */
    void ReadUString(int size, RCString &s);

    /** 读GUID
    @param [out] g 返回GUDI
    */
    void ReadGUID(RC_GUID& g);

    /** 读快信息
    @param [in] inStream 输入流
    @param [in] pos 位置
    @param [in] size 大小
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult ReadChunk(IInStream* inStream, uint64_t pos, uint64_t size);

    /** 读目录入口
    @param [in] database 返回chm基本数据
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult ReadDirEntry(RCChmDatabase& database);

    /** 解压流
    @param [in] inStream 输入流
    @param [in] database 基本数据
    @param [in] name 名字
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult DecompressStream(IInStream* inStream, const RCChmDatabase& database, const RCStringA& name);

private:

    /** 起始位置
    */
    uint64_t m_startPosition;

    /** 输入缓冲区
    */
    RCInBuffer m_inBuffer;

    /** 块大小
    */
    uint64_t m_chunkSize;
};

END_NAMESPACE_RCZIP

#endif //__RCChmIn_h_
