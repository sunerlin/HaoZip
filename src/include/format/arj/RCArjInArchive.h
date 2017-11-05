/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCArjInArchive_h_
#define __RCArjInArchive_h_ 1

#include "base/RCTypes.h"
#include "interface/IStream.h"
#include "interface/IArchive.h"
#include "RCArjHeader.h"
#include "RCArjItem.h"

BEGIN_NAMESPACE_RCZIP

class RCArjInArchive
{
public:
    /** 打开文档
    @param [in] searchHeaderSizeLimit 查找头部的最大偏移位置
    */
    HResult Open(const uint64_t *searchHeaderSizeLimit);

    /** 获取下一项 
    @param [out] filled 是否填充
    @param [out] item arj项
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult GetNextItem(bool& filled, RCArjItem &item);

public:

    /** 文档头部
    */
    RCArjArchiveHeader m_header;

    /** 输入流
    */
    IInStream* m_stream;

    /** 打开文档回调接口指针
    */
    IArchiveOpenCallback* m_openArchiveCallback;

    /** 文件数
    */
    uint64_t m_numFiles;

    /** 字节数
    */
    uint64_t m_numBytes;

private:

    /** 读取块
    @param [out] filled 是否填充
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult ReadBlock(bool& filled);

    /** 读取签名和块
    @param [out] filled 是否填充
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult ReadSignatureAndBlock(bool& filled);

    /** 跳过扩展头
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult SkipExtendedHeaders();

    /** 安全读字节
    @param [out] data 返回实际读到的内容
    @param [in] size 读取的大小
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult SafeReadBytes(void *data, uint32_t size);

private:

    /** 块大小
    */
    uint32_t m_blockSize;

    /** 块内容
    */
    byte_t m_block[RCArjDefs::kBlockSizeMax + 4];
};

END_NAMESPACE_RCZIP

#endif //__RCArjInArchive_h_