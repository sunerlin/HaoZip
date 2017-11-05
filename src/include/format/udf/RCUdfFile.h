/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCUdfFile_h_
#define __RCUdfFile_h_ 1

#include "base/RCTypes.h"
#include "base/RCString.h"
#include "base/RCSmartPtr.h"
#include "common/RCVector.h"
#include "RCUdfCDString.h"
#include "RCUdfHeader.h"
#include "RCUdfCrc16.h"

BEGIN_NAMESPACE_RCZIP

/** Udf 文件
*/
struct RCUdfFile
{
    /** ID
    */
    RCUdfCDString m_id;

    /** 文件编号
    */
    int32_t m_itemIndex;

    /** 默认构造函数
    */
    RCUdfFile();

    /** 返回文件名
    */
    RCString GetName() const;
};

/** Udf My Extent 
*/
struct RCUdfMyExtent
{
    /** 位置
    */
    uint32_t m_pos;

    /** 长度
    */
    uint32_t m_len;

    /** 分区
    */
    int32_t m_partitionRef;

    /** 返回长度
    @return 返回长度
    */
    uint32_t GetLen() const;

    /** 返回类型
    @return 返回类型
    */
    uint32_t GetType() const;

    /** 是否为Rec
    @return Rec返回true,否则返回false
    */
    bool IsRecAndAlloc() const;
};

/** Udf 项
*/
struct RCUdfItem
{
    /** icb Tag
    */
    RCUdfIcbTag m_icbTag;

    /** 大小
    */
    uint64_t m_size;

    /** 块记录
    */
    uint64_t m_numLogBlockRecorded;

    /** 访问时间
    */
    RCUdfTime m_aTime;

    /** 修改时间
    */
    RCUdfTime m_mTime;

    /** 是否内联
    */
    bool m_isInline;

    /** 数据缓存
    */
    RCByteBuffer m_inlineData;

    /** extent 列表
    */
    RCVector<RCUdfMyExtent> m_extents;

    /** sub File 列表
    */
    RCVector<int32_t> m_subFiles;

    /** 分析
    @param [in] buf 数据缓存
    */
    void Parse(const byte_t *buf);

    /** 是否为Rec
    @return Rec返回true,否则返回false
    */
    bool IsRecAndAlloc() const;

    /** 返回Chunk总大小
    @return 返回Chunk总大小
    */
    uint64_t GetChunksSumSize() const;

    /** 检查Chunk大小
    @return 正确返回true,否则返回false
    */
    bool CheckChunkSizes() const;

    /** 是否是目录
    @return 目录返回true,否则返回false
    */
    bool IsDir() const;
};

typedef RCSharedPtr<RCUdfItem> RCUdfItemPtr;

/** Udf Tag
*/
struct RCUdfTag
{
    /** ID
    */
    uint16_t m_id;

    /** 版本
    */
    uint16_t m_version;

    /** 解析
    @param [in] buf 数据缓存
    @param [in] size 数据大小
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult Parse(const byte_t *buf, size_t size);
};

/** Udf File ID
*/
struct RCUdfFileId
{
    /** 文件编码
    */
    byte_t m_fileCharacteristics;

    /** ID
    */
    RCUdfCDString m_id;

    /** ICB
    */
    RCUdfLongAllocDesc m_icb;

    /** 是否级联
    @return 级联返回true,否则返回false
    */
    bool IsItLinkParent() const;

    /**
    @param [in] p 数据缓存
    @param [in] size 数据大小
    @param [out] processed 处理字节数
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult Parse(const byte_t *p, size_t size, size_t &processed);
};

/** Udf Extent
*/
struct RCUdfExtent
{
    /** 长度
    */
    uint32_t m_len;

    /** 位置
    */
    uint32_t m_pos;

    /** 解析
    @param [in] buf 数据缓存
    */
    void Parse(const byte_t *buf);
};

END_NAMESPACE_RCZIP

#endif //__RCUdfFile_h_
