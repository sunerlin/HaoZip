/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCUdfIn_h_
#define __RCUdfIn_h_ 1

#include "base/RCTypes.h"
#include "base/RCString.h"
#include "interface/IStream.h"
#include "common/RCVector.h"
#include "common/RCBuffer.h"
#include "RCUdfCDString.h"
#include "RCUdfHeader.h"
#include "RCUdfFile.h"

BEGIN_NAMESPACE_RCZIP

/** Udf Ref
*/
struct RCUdfRef
{
    /** 父编号
    */
    int32_t m_parent;

    /** 文件编号
    */
    int32_t m_fileIndex;
};

/** Udf 文件集合
*/
struct RCUdfFileSet
{
    /** 时间
    */
    RCUdfTime m_recodringTime;

    /** root ICB
    */
    RCUdfLongAllocDesc m_rootDirICB;

    /** ref 列表
    */
    RCVector<RCUdfRef> m_refs;
};

/** Udf 逻辑卷
*/
struct RCUdfLogVol
{
    /** ID
    */
    RCUdfCDString128 m_id;

    /** 块大小
    */
    uint32_t m_blockSize;

    /** 文件集描述
    */
    RCUdfLongAllocDesc m_fileSetLocation;

    /** 分区对照表
    */
    RCVector<RCUdfPartitionMap> m_partitionMaps;

    /** 文件集
    */
    RCVector<RCUdfFileSet> m_fileSets;

    /** 返回名称
    @return 返回名称
    */
    RCString GetName() const;
};

/** Udf Progress Virt
*/
struct RCUdfProgressVirt
{
    /** 设置总数
    @param [in] numBytes 总字节数
    @return 成功返回RC_S_OK，否则返回错误号
    */
    virtual HResult SetTotal(uint64_t numBytes) = 0;

    /** 设置完成数
    @param [in] numFiles 完成文件数
    @param [in] numBytes 完成字节数
    @return 成功返回RC_S_OK，否则返回错误号
    */
    virtual HResult SetCompleted(uint64_t numFiles, uint64_t numBytes) = 0;

    /** 设置完成
    @return 成功返回RC_S_OK，否则返回错误号
    */
    virtual HResult SetCompleted() = 0;
};

/** Udf 输入处理
*/
class RCUdfInArchive
{   
public:

    /** 打开
    @param [in] inStream 输入流
    @param [in] progress 打开进度
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult Open(IInStream *inStream, RCUdfProgressVirt *progress);

    /** 清楚
    */
    void Clear();

    /** 返回注释
    @return 返回注释
    */
    RCString GetComment() const;

    /** 取得Item路径
    @param [in] volIndex 卷编号
    @param [in] fsIndex 文件编号
    @param [in] refIndex ref编号
    @param [in] showVolName 是否显示卷名
    @param [in] showFsName 是否显示名称
    @return 返回Item路径
    */
    RCString GetItemPath(int32_t volIndex, 
                         int32_t fsIndex, 
                         int32_t refIndex, 
                         bool showVolName, 
                         bool showFsName) const;

    /** 检查Item Extent
    @param [in] volIndex 卷编号
    @param [in] item 项信息
    @return 正确返回true,否则返回false
    */
    bool CheckItemExtents(int32_t volIndex, const RCUdfItem &item) const;

public:

    /** 分区列表
    */
    RCVector<RCUdfPartition> m_partitions;

    /** 逻辑卷列表
    */
    RCVector<RCUdfLogVol> m_logVols;

    /** 文件项列表
    */
    RCVector<RCUdfItemPtr> m_items;

    /** 文件列表
    */
    RCVector<RCUdfFile> m_files;

    /** 逻辑扇区数
    */
    int32_t m_secLogSize;

private:

    /** 读取数据
    @param [in] volIndex 卷编号
    @param [in] partitionRef 分区号
    @param [in] blockPos 块位置
    @param [in] len 长度
    @param [in] buf 缓存指针
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult Read(int32_t volIndex, int32_t partitionRef, uint32_t blockPos, uint32_t len, byte_t *buf);

    /** 读取数据
    @param [in] volIndex 卷编号
    @param [in] lad 描述
    @param [in] buf 缓存指针
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult Read(int32_t volIndex, const RCUdfLongAllocDesc &lad, byte_t *buf);

    /** 文件读取
    @param [in] volIndex 卷编号
    @param [in] item 文件项
    @param [in] buf 缓存指针
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult ReadFromFile(int32_t volIndex, const RCUdfItem &item, RCByteBuffer &buf);

    /** 文件项读取
    @param [in] volIndex 卷编号
    @param [in] fsIndex fs编号
    @param [in] lad 描述
    @param [in] numRecurseAllowed 允许的递归数
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult ReadFileItem(int32_t volIndex, int32_t fsIndex, const RCUdfLongAllocDesc &lad, int32_t numRecurseAllowed);

    /** 读取项
    @param [in] volIndex 卷编号
    @param [in] fsIndex fs编号
    @param [in] lad 描述
    @param [in] numRecurseAllowed 允许的递归数
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult ReadItem(int32_t volIndex, int32_t fsIndex, const RCUdfLongAllocDesc &lad, int32_t numRecurseAllowed);

    /** 打开
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult Open2();

    /** 读取文件Ref 
    @param [in] fs
    @param [in] fileIndex 文件编号
    @param [in] parent 父编号
    @param [in] numRecurseAllowed 允许的递归数
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult FillRefs(RCUdfFileSet &fs, int32_t fileIndex, int32_t parent, int32_t numRecurseAllowed);

    /** 检查Extent
    @param [in] volIndex 卷编号
    @param [in] partitionRef  分区Ref
    @param [in] blockPos 块位置
    @param [in] len 长度
    @return 成功返回true,否则返回false
    */
    bool CheckExtent(int32_t volIndex, int32_t partitionRef, uint32_t blockPos, uint32_t len) const;

    /** 更新名称
    @param [out] res 字符串
    @param [in] addString 附件字符串
    */
    static void UpdateWithName(RCString &res, const RCString &addString);

    /** 取得Spec名称
    @param [in] name 名称
    @return 返回Spec名称
    */
    static RCString GetSpecName(const RCString &name);

private:

    /** 输入流
    */
    IInStreamPtr m_stream;

    /** 进度回调接口
    */
    RCUdfProgressVirt* m_progress;

    /** 已经处理字节
    */
    uint64_t m_processedProgressBytes;

    /** 文件名长度
    */
    uint64_t m_fileNameLengthTotal;

    /** Ref数量
    */
    int32_t m_numRefs;

    /** Extent数量
    */
    uint32_t m_numExtents;

    /** Extent 总数
    */
    uint64_t m_inlineExtentsSize;
};

END_NAMESPACE_RCZIP

#endif //__RCUdfIn_h_