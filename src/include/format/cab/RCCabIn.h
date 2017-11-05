/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCCabIn_h_
#define __RCCabIn_h_ 1

#include "base/RCTypes.h"
#include "base/RCString.h"
#include "common/RCVector.h"
#include "common/RCInBuffer.h"
#include "format/cab/RCCabHeader.h"
#include "format/cab/RCCabItem.h"
#include "interface/IStream.h"

BEGIN_NAMESPACE_RCZIP

struct RCCabOtherArchive
{
    /** 文件名
    */
    RCStringA m_fileName;

    /** 磁盘名
    */
    RCStringA m_diskName;
};

struct RCCabArchiveFileInfo
{
    /** 次版本号
    */
    byte_t  m_versionMinor;

    /** 主版本号
    */
    byte_t  m_versionMajor;

    /** 文件夹数
    */
    uint16_t m_numFolders;

    /** 文件数
    */
    uint16_t  m_numFiles;

    /** 标志
    */
    uint16_t  m_flags;

    /** 设置ID
    */
    uint16_t  m_setID;

    /** 内部快编号
    */
    uint16_t  m_cabinetNumber;

    /** 前一个内部块区域大小
    */
    uint16_t m_perCabinetAreaSize;

    /** 前一个文件夹区域大小
    */
    byte_t m_perFolderAreaSize;

    /** 前一个数据块区域大小
    */
    byte_t m_perDataBlockAreaSize;

    /** 前一个文档
    */
    RCCabOtherArchive m_prevArc;

    /** 下一个文档
    */
    RCCabOtherArchive m_nextArc;

    /** 是否保留块
    @return 是返回true，否则返回false
    */
    bool ReserveBlockPresent() const ;

    /** 是否有前一个内部块
    @return 有返回true，否则返回false
    */
    bool IsTherePrev() const ;

    /** 是否有下一个内部块
    @return 是返回true，否则返回false
    */
    bool IsThereNext() const ;

    /** 获取数据块保留大小
    @return 返回数据块保留大小
    */
    byte_t GetDataBlockReserveSize() const ;

    /** 默认构造函数
    */
    RCCabArchiveFileInfo();

    /** 清除
    */
    void Clear();
};

struct RCCabInArchiveInfo : 
    public RCCabArchiveFileInfo
{
    /** 大小
    */
    uint32_t m_size;

    /** 文件头部偏移
    */
    uint32_t m_fileHeadersOffset;
};

class RCCabDatabase
{
public:

    /** 清除
    */
    void Clear();

    /** 是否有前一个文件夹
    @return 是返回true，否则返回false
    */
    bool IsTherePrevFolder() const;

    /** 获取新文件的编号
    @return 返回新文件的编号
    */
    int32_t GetNumberOfNewFolders() const;

    /** 获取文件偏移
    @return 返回文件偏移
    */
    uint32_t GetFileOffset(int32_t index) const ;

    /** 获取文件大小
    @return 返回文件大小
    */
    uint32_t GetFileSize(int32_t index) const ;

public:

    /** 起始位置
    */
    uint64_t m_startPosition;

    /** cab文档信息
    */
    RCCabInArchiveInfo m_archiveInfo;

    /** cab文件夹信息
    */
    RCVector<RCCabFolder> m_folders;

    /** cab项信息
    */
    RCVector<RCCabItem> m_items;
};

class RCCabDatabaseEx: 
    public RCCabDatabase
{
public:

    /** 输入流
    */
    IInStreamPtr m_stream;
};

struct RCCabMvItem
{
    /** 卷索引
    */
    int32_t m_volumeIndex;

    /** 项索引
    */
    int32_t m_itemIndex;
};

class RCCabMvDatabaseEx
{
public:

    /** 获取文件夹索引
    @return 返回文件夹索引
    */
    int32_t GetFolderIndex(const RCCabMvItem *mvi) const;

    /** 清除
    */
    void Clear();

    /** 填充排序
    */
    void FillSortAndShrink();

    /** 检查
    @return 通过检查返回true，否则返回false
    */
    bool Check();

public:

    /** cab每个卷基本信息
    */
    RCVector<RCCabDatabaseEx> m_volumes;

    /** cabMv项
    */
    RCVector<RCCabMvItem> m_items;

    /** 在卷中的起始文件夹
    */
    RCVector<int32_t> m_startFolderOfVol;

    /** 文件夹中起始文件索引
    */
    RCVector<int32_t> m_folderStartFileIndex;

private:

    /** 项是否相等
    @param [in] i1 项索引一
    @param [in] i2 项索引二
    @return 相对返回true，否则返回false
    */
    bool AreItemsEqual(int32_t i1, int32_t i2);
};

class RCCabInArchive
{
public:

    /** 打开文档
    @param [in] searchHeaderSizeLimit 查找头部的最大偏移位置
    @param [in] db cab基本数据
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult Open(const uint64_t* searchHeaderSizeLimit, RCCabDatabaseEx& db);

    /** 比较cabMv项
    @param [in] p1 cabMv项一
    @param [in] p1 cabMv项二
    @param [in] param 参数
    @return 相对返回0，否则返回其他
    */
    static int32_t CompareMvItems(const RCCabMvItem* p1, const RCCabMvItem* p2, void* param);

public:

    /** 标志
    */
    static byte_t s_marker[RCCabHeaderDefs::kMarkerSize];

private:

    /** 读取一个字节
    @return 返回读取字节值
    */
    byte_t Read8();

    /** 读两个字节转成16位无符号整型
    @return 返回16位无符号整型
    */
    uint16_t Read16();

    /** 读四个字节转成32位无符号整型
    @return 返回32位无符号整型
    */
    uint32_t Read32();
    
    /** 安全的读文件名
    */
    RCStringA SafeReadName();

    /** 跳过
    @param [in] size 跳过的大小
    */
    void Skip(uint32_t size);

    /** 读其他文档
    @param [out] oa 返回cab文档信息 
    */
    void ReadOtherArchive(RCCabOtherArchive& oa);

private:

    /** 输入内存
    */
    RCInBuffer m_inBuffer;
};

END_NAMESPACE_RCZIP

#endif //__RCCabIn_h_