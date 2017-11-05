/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCIsoIn_h_
#define __RCIsoIn_h_ 1

#include "base/RCTypes.h"
#include "base/RCString.h"
#include "common/RCVector.h"
#include "common/RCStringBuffer.h"
#include "common/RCStringUtil.h"
#include "interface/IArchive.h"
#include "interface/IStream.h"
#include "RCIsoItem.h"
#include "RCIsoHeader.h"
#include "RCIsoDir.h"

BEGIN_NAMESPACE_RCZIP

struct RCIsoDateTime
{
    /** 年
    */
    uint16_t m_year;

    /** 月
    */
    byte_t m_month;

    /** 日
    */
    byte_t m_day;

    /** 小时
    */
    byte_t m_hour;

    /** 分钟
    */
    byte_t m_minute;

    /** 秒
    */
    byte_t m_second;

    /** 百分之一
    */
    byte_t m_hundredths;

    /** gmt偏移
    */
    signed char m_gmtOffset;

    /** 不指定
    @return 是返回ture，否则返回false
    */
    bool NotSpecified() const ;
};

struct RCIsoBootRecordDescriptor
{
    /** 引导系统id
    */
    byte_t m_bootSystemId[32];

    /** 引导id
    */
    byte_t m_bootId[32];

    /** 引导系统使用的缓冲区
    */
    byte_t m_bootSystemUse[1977];
};

struct RCIsoBootValidationEntry
{
    /** 平台id
    */
    byte_t m_platformId;

    /** id
    */
    byte_t m_id[24];
};

struct RCIsoBootInitialEntry
{
    /** 引导表
    */
    bool m_bootable;

    /** 引导媒介类型
    */
    byte_t m_bootMediaType;

    /** 加载段
    */
    uint16_t m_loadSegment;

    /** 系统类型
    */
    byte_t m_systemType;

    /** 扇区数
    */
    uint16_t m_sectorCount;

    /** 加载RBA
    */
    uint32_t m_loadRBA;

    /** 获取大小
    @return 返回大小
    */
    uint64_t GetSize() const;

    /** 获取名字
    @return 返回名字
    */
    RCString GetName() const;
};

struct RCIsoVolumeDescriptor
{
    /** 卷标志
    */
    byte_t m_volFlags;

    /** 系统id
    */
    byte_t m_systemId[32];

    /** 卷id
    */
    byte_t m_volumeId[32];

    /** 卷空间大小
    */
    uint32_t m_volumeSpaceSize;

    /** 转义序列
    */
    byte_t m_escapeSequence[32];

    /** 卷设置大小
    */
    uint16_t m_volumeSetSize;

    /** 卷序列号
    */
    uint16_t m_volumeSequenceNumber;

    /** 逻辑块大小
    */
    uint16_t m_logicalBlockSize;

    /** 路径表大小
    */
    uint32_t m_pathTableSize;

    /** 路径表位置(高位)
    */
    uint32_t m_lPathTableLocation;

    /** 操作路径表位置(高位)
    */
    uint32_t m_lOptionalPathTableLocation;

    /** 路径表位置(低位)
    */
    uint32_t m_mPathTableLocation;

    /** 操作路径表位置(低位)
    */
    uint32_t m_mOptionalPathTableLocation;

    /** 根目录记录
    */
    RCIsoDirRecord m_rootDirRecord;

    /** 卷的设置id
    */
    byte_t m_volumeSetId[128];

    /** 出版id
    */
    byte_t m_publisherId[128];

    /** 数据申报id 
    */
    byte_t m_dataPreparerId[128];

    /** 应用程序id
    */
    byte_t m_applicationId[128];

    /** 公司文件id 
    */
    byte_t m_copyrightFileId[37];

    /** 抽象文件id
    */
    byte_t m_abstractFileId[37];

    /** bib文件id
    */
    byte_t m_bibFileId[37];

    /** 创建的日期机器
    */
    RCIsoDateTime m_cTime;

    /** 更改的日期时间
    */
    RCIsoDateTime m_mTime;

    /** 无效日期时间
    */
    RCIsoDateTime m_expirationTime;

    /** 有效日期时间
    */
    RCIsoDateTime m_effectiveTime;

    /** 文件结构版本(default 1)
    */
    byte_t m_fileStructureVersion;

    /** 应用程序使用的缓冲区
    */
    byte_t m_applicationUse[512];

    /** 是否是joliet
    */
    bool IsJoliet() const;
};

struct RCIsoRef
{
    /** 目录
    */
    RCIsoDir* m_dir;

    /** 索引
    */
    uint32_t m_index;
};

static const uint32_t s_blockSize = 1 << 11;

class RCIsoInArchive
{
public:

    /** 默认构造函数
    */
    RCIsoInArchive() ;

public:

    /** 打开文档
    @param [in] stream 输入流接口
    @param [in] openArchiveCallback 打开文档回调接口指针
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult Open(IInStream* inStream, IArchiveOpenCallback* openArchiveCallback);

    /** 清除
    */
    void Clear();

    /** 重置
    */
    void Reset();

    /** 是否是joliet
    */
    bool IsJoliet() const ;

    /** 获取引导项大小
    @param [in] index 索引
    @return 返回引导项大小
    */
    uint64_t GetBootItemSize(int32_t index) const;

public:

    /** 文档大小
    */
    uint64_t m_archiveSize;

    /** iso ref
    */
    RCVector<RCIsoRef> m_refs;

    /** 卷描述
    */
    RCVector<RCIsoVolumeDescriptor> m_volDescs;

    /** 主卷描述索引
    */
    int32_t m_mainVolDescIndex;

    /** 块大小
    */
    uint32_t m_blockSize;

    /** 引导入口
    */
    RCVector<RCIsoBootInitialEntry> m_bootEntries;

    /** 是否是susp
    */
    bool m_isSusp;

    /** susp调过的大小
    */
    int32_t m_suspSkipSize;

private:

    /** 跳过
    @param [in] size 跳过的大小
    */
    void Skip(size_t size);

    /** 调过0字节
    @param [in] size 跳过的大小
    */
    void SkipZeros(size_t size);

    /** 读取字节
    @return 返回读取的字节，如果失败返回0xFF
    @throws 如果读取数据失败抛异常
    */
    byte_t ReadByte();

    /** 读取多字节
    @param [out] buf 存放读取结果的缓冲区地址
    @param [in]  size 缓冲区的长度
    @return 返回实际读取的长度
    */
    void ReadBytes(byte_t *data, uint32_t size);

    /** 读两个字节转成16位无符号整型
    @return 返回16位无符号整型
    */
    uint16_t ReadUInt16Spec();

    /** 读两个字节转成16位无符号整型
    @return 返回16位无符号整型
    */
    uint16_t ReadUInt16();

    /** 读四个字节转成32位无符号整型(小字节序)
    @return 返回32位无符号整型
    */
    uint32_t ReadUInt32Le();

    /** 读四个字节转成32位无符号整型(大字节序)
    @return 返回32位无符号整型
    */
    uint32_t ReadUInt32Be();

    /** 读四个字节转成32位无符号整型
    @return 返回32位无符号整型
    */
    uint32_t ReadUInt32();

    /** 读八个字节转成64位无符号整型
    @return 返回64位无符号整型
    */
    uint64_t ReadUInt64();

    /** 读取数字
    @param [in] numDigits 数据
    @retrun 返回数据
    */
    uint32_t ReadDigits(int32_t numDigits);

    /** 读取日期时间
    @param [out] t 返回日期时间
    */
    void ReadDateTime(RCIsoDateTime& d);

    /** 读取记录日期时间
    @param [out] t 返回记录日期时间
    */
    void ReadRecordingDateTime(RCIsoRecordingDateTime& t);

    /** 读取路径记录
    @param [out] r 返回路径记录
    @param [in] len 读取的长度
    */
    void ReadDirRecord2(RCIsoDirRecord &r, byte_t len);

    /** 读取引导记录
    @param [out] r 返回引导记录
    */
    void ReadDirRecord(RCIsoDirRecord &r);

    /** 读卷的引导记录的描述信息
    @param [out] d 返回引导记录的描述信息
    */
    void ReadBootRecordDescriptor(RCIsoBootRecordDescriptor& d);

    /** 读卷的描述
    @param [out] d 返回卷的描述信息
    */
    void ReadVolumeDescriptor(RCIsoVolumeDescriptor& d);

    /** 定位到块
    @param [in] blockIndex 块的索引
    */
    void SeekToBlock(uint32_t blockIndex);

    /** 读路径
    @param [in] d 路径信息
    @param [in] level 等级
    */
    void ReadDir(RCIsoDir& d, int32_t level);

    /** 创建Refs
    */
    void CreateRefs(RCIsoDir &d);

    /** 读引导信息
    */
    void ReadBootInfo();

    /** 打开
    @return 成功返回RC_S_OK，否则返回错误号 
    */
    HResult Open2();

    /** 检查签名
    @param [in] sig 签名
    @param [in] data 缓冲数据
    @return 通过检查返回true，否则返回false
    */
    static inline bool CheckSignature(const byte_t* sig, const byte_t* data);

private:

    /** 输入流
    */
    IInStreamPtr m_stream;

    /** 偏移位置
    */
    uint64_t m_position;

    /** 缓冲区
    */
    byte_t m_buffer[s_blockSize];

    /** 缓冲偏移
    */
    uint32_t m_bufferPos;

    /** 根目录
    */
    RCIsoDir m_rootDir;

    /** 引导是否定义
    */
    bool m_bootIsDefined;

    /** 引导记录描述信息
    */
    RCIsoBootRecordDescriptor m_bootDesc;

    /** 打开文档回调接口
        次要的错误的抛异常改成显示错误
    */
    IArchiveOpenCallback* m_openArchiveCallback;

    static const byte_t s_sigCD001[5];

    static const byte_t s_sigNSR02[5];

    static const byte_t s_sigNSR03[5];

    static const byte_t s_sigBEA01[5];

    static const byte_t s_sigTEA01[5];

};

END_NAMESPACE_RCZIP

#endif //__RCIsoIn_h_
