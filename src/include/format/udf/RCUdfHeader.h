/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCUdfHeader_h_
#define __RCUdfHeader_h_ 1

#include "base/RCTypes.h"
#include "base/RCString.h"
#include "RCUdfMap32.h"

#ifdef RCZIP_OS_WIN
#define CHAR_PATH_SEPARATOR '\\'
#define WCHAR_PATH_SEPARATOR L'\\'
#define WSTRING_PATH_SEPARATOR L"\\"
#else
#define CHAR_PATH_SEPARATOR '/'
#define WCHAR_PATH_SEPARATOR L'/'
#define WSTRING_PATH_SEPARATOR L"/"
#endif

BEGIN_NAMESPACE_RCZIP

/** Udf 常量定义
*/
class RCUdfDefs
{
public:
    enum EShortAllocDescType
    {
        SHORT_ALLOC_DESC_TYPE_RECORDED_AND_ALLOCATED = 0,
        SHORT_ALLOC_DESC_TYPE_NOT_RECORDED_BUT_ALLOCATED = 1,
        SHORT_ALLOC_DESC_TYPE_NOT_RECORDED_AND_NOT_ALLOCATED = 2,
        SHORT_ALLOC_DESC_TYPE_NEXT_EXTENT = 3
    };

    enum EIcbFileType
    {
        ICB_FILE_TYPE_DIR = 4,
        ICB_FILE_TYPE_FILE = 5
    };

    enum EIcbDescriptorType
    {
        ICB_DESC_TYPE_SHORT = 0,
        ICB_DESC_TYPE_LONG = 1,
        ICB_DESC_TYPE_EXTENDED = 2,
        ICB_DESC_TYPE_INLINE = 3
    };

    enum EDescriptorType
    {
        DESC_TYPE_SPOARING_TABLE = 0, // UDF
        DESC_TYPE_PRIM_VOL = 1,
        DESC_TYPE_ANCHOR_VOL_PTR = 2,
        DESC_TYPE_VOL_PTR = 3,
        DESC_TYPE_IMPL_USE_VOL = 4,
        DESC_TYPE_PARTITION = 5,
        DESC_TYPE_LOGICAL_VOL = 6,
        DESC_TYPE_UNALLOC_SPACE = 7,
        DESC_TYPE_TERMINATING = 8,
        DESC_TYPE_LOGICAL_VOL_INTEGRITY = 9,
        DESC_TYPE_FILESET = 256,
        DESC_TYPE_FILEID  = 257,
        DESC_TYPE_ALLOCATION_EXTENT = 258,
        DESC_TYPE_INDIRECT = 259,
        DESC_TYPE_TERMINAL = 260,
        DESC_TYPE_FILE = 261,
        DESC_TYPE_EXTENDED_ATTR_HEADER = 262,
        DESC_TYPE_UNALLOCATED_SPACE = 263,
        DESC_TYPE_SPACE_BITMAP = 264,
        DESC_TYPE_PARTITION_INTEGRITY = 265,
        DESC_TYPE_EXTENDED_FILE = 266,
    };

    static const int32_t s_numPartitionsMax = 64;
    static const int32_t s_numLogVolumesMax = 64;
    static const int32_t s_numRecureseLevelsMax = 1 << 10;
    static const int32_t s_numItemsMax = 1 << 27;
    static const int32_t s_numFilesMax = 1 << 28;
    static const int32_t s_numRefsMax = 1 << 28;
    static const uint32_t s_numExtentsMax = (uint32_t)1 << 30;
    static const uint64_t s_fileNameLengthTotalMax = (uint64_t)1 << 33;
    static const uint64_t s_inlineExtentsSizeMax = (uint64_t)1 << 33;
    static const byte_t s_fileidCharacsParent = (1 << 3);
};

/** Udf Time
*/
struct RCUdfTime
{
    /** 数据
    */
    byte_t m_data[12];

    /** 返回时间类型
    */
    unsigned GetType() const;

    /** 是否为本地时间
    @return 本地返回true,否则返回false
    */
    bool IsLocal() const;

    /** 返回分钟
    @return 返回分钟偏移
    */
    int32_t GetMinutesOffset() const;

    /** 返回年
    @return 返回年份
    */
    unsigned GetYear() const;

    /** 解析
    @param [in] buf 数据
    */
    void Parse(const byte_t *buf);
};

/** Udf 分区
*/
struct RCUdfPartition
{
    /** 数值
    */
    uint16_t m_number;

    /** 位置
    */
    uint32_t m_pos;

    /** 长度
    */
    uint32_t m_len;

    /** 卷编号
    */
    int32_t m_volIndex;

    /** 对照表
    */
    RCUdfMap32Ptr m_map;

    /** 构造函数
    */
    RCUdfPartition();

    /** 拷贝构造函数
    @param [in] 拷贝原型
    */
    RCUdfPartition(const RCUdfPartition& val);

    /** =操作符重载
    */
    RCUdfPartition& operator=(const RCUdfPartition& val);
};

/** Udf 逻辑块地址
*/
struct RCUdfLogBlockAddr
{
    /** 位置
    */
    uint32_t m_pos;

    /** 扇区
    */
    uint16_t m_partitionRef;

    /** 解析
    @param [in] buf 数据
    */
    void Parse(const byte_t *buf);
};

/** Udf 短描述
*/
struct RCUdfShortAllocDesc
{
    /** 长度
    */
    uint32_t m_len;

    /** 位置
    */
    uint32_t m_pos;

    /** 解析
    @param [in] buf 数据
    */
    void Parse(const byte_t *buf);
};

/** Udf 长描述
*/
struct RCUdfLongAllocDesc
{
    /** 长度
    */
    uint32_t m_len;

    /** 地址
    */
    RCUdfLogBlockAddr m_location;

    /** 返回长度
    @return 返回长度
    */
    uint32_t GetLen() const;

    /** 返回类型
    @return 返回类型
    */
    uint32_t GetType() const;

    /** 返回是否为Rec
    @return Rec返回true,否则返回false
    */
    bool IsRecAndAlloc() const;

    /** 解析
    @param [in] buf 数据
    */
    void Parse(const byte_t *buf);
};

/** Udf 分区对照表
*/
struct RCUdfPartitionMap
{
    /** 类型
    */
    byte_t m_type;

    /** 分区数
    */
    uint16_t m_partitionNumber;

    /** 分区编号
    */
    int32_t m_partitionIndex;
};

/** Udf Icb Tag
*/
struct RCUdfIcbTag
{
    /** 文件类型
    */
    byte_t m_fileType;

    /** 标志
    */
    uint16_t m_flags;

    /** 是否目录
    @return 目录返回true,否则返回false
    */
    bool IsDir() const;

    /** 返回描述类型
    @return 返回描述类型
    */
    int32_t GetDescriptorType() const;

    /** 解析
    @param [in] p 数据
    */
    void Parse(const byte_t *p);
};

END_NAMESPACE_RCZIP

#endif //__RCUdfHeader_h_
