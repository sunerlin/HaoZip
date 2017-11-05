/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCComIn_h_
#define __RCComIn_h_ 1

#include "base/RCTypes.h"
#include "base/RCString.h"
#include "common/RCVector.h"
#include "interface/IStream.h"
#include "filesystem/RCFileDefs.h"

BEGIN_NAMESPACE_RCZIP

struct RCUInt32Buf
{
public:

    /** 默认构造函数
    */
    RCUInt32Buf();

    /** 默认析构函数
    */
    ~RCUInt32Buf();

    /** 释放内存
    */
    void Free();

    /** 分配内存
    */
    bool Allocate(uint32_t numItems);

    /** 重置取地址操作符
    @return 返回地址
    */
    operator uint32_t *() const;

private:

    /** 内存数据
    */
    uint32_t* m_buf;
};

class RCComDefs
{
public:
    static const uint32_t s_free       = 0xFFFFFFFF;
    static const uint32_t s_endOfChain = 0xFFFFFFFE;
    static const uint32_t s_fatSector  = 0xFFFFFFFD;
    static const uint32_t s_matSector  = 0xFFFFFFFC;
    static const uint32_t s_maxValue   = 0xFFFFFFFA;
    static const byte_t s_empty = 0;
    static const byte_t s_storage = 1;
    static const byte_t s_stream = 2;
    static const byte_t s_lockBytes = 3;
    static const byte_t s_property = 4;
    static const byte_t s_rootStorage = 5;
    static const uint32_t s_nameSizeMax = 64;
};

struct RCComItem
{
    /** 名字
    */
    byte_t m_name[RCComDefs::s_nameSizeMax];

    /** 创建时间
    */
    RC_FILE_TIME m_cTime;

    /** 修改时间
    */
    RC_FILE_TIME m_mTime;

    /** 大小
    */
    uint64_t m_size;

    /** 左节点
    */
    uint32_t m_leftDid;

    /** 右节点
    */
    uint32_t m_rightDid;

    /** 子节点
    */
    uint32_t m_sonDid;

    /** SID
    */
    uint32_t m_sid;

    /** 类型
    */
    byte_t m_type;

    /** 是否是空
    @return 是返回true，否则返回false
    */
    bool IsEmpty() const;

    /** 是否是目录
    @return 是返回true，否则返回false
    */
    bool IsDir() const;

    /** 解析
    @param [in] p 内存数据
    @param [in] mode64bit 是否是64位
    */
    void Parse(const byte_t* p, bool mode64bit) ;
};

struct RCComRef
{
    /** 父节点
    */
    int32_t m_parent;

    /** Did
    */
    uint32_t m_did;
};

class RCComDatabase
{
public:

    /** fat
    */
    RCUInt32Buf m_fat;

    /** fat大小
    */
    uint32_t m_fatSize;

    /** SID
    */
    RCUInt32Buf m_miniSids;

    /** 流中有多少个卷
    */
    uint32_t m_numSectorsInMiniStream;

    /** mat
    */
    RCUInt32Buf m_mat;

    /** mat大小
    */
    uint32_t m_matSize;

    /** com项
    */
    RCVector<RCComItem> m_items;

    /** comRef
    */
    RCVector<RCComRef> m_refs;

    /** 长流最小大小
    */
    uint32_t m_longStreamMinSize;

    /** 卷的字节大小
    */
    int32_t m_sectorSizeBits;

    /** 卷的最小字节大小
    */
    int32_t m_miniSectorSizeBits;

public:

    /** 添加节点
    @param [in] parent 父节点
    @param [in] did did
    */
    HResult AddNode(int32_t parent, uint32_t did);

    /** 清除 
    */
    void Clear();

    /** 是否是大的流
    @param [in] 流大小
    @return 是返回true，否则返回false
    */
    bool IsLargeStream(uint64_t size) const ;

    /** 获取项路径
    @param [in] index 索引
    @return 返回路径
    */
    RCString GetItemPath(uint32_t index) const;

    /** 获取项的包大小
    @param [in] size 大小
    @return 返回包大小
    */
    uint64_t GetItemPackSize(uint64_t size) const ;

    /** 获取Mini簇
    @param [in] sid SID
    @param [out] res 簇号
    @return 成功返回true，否则返回false
    */
    bool GetMiniCluster(uint32_t sid, uint64_t& res) const ;

    /** 打开
    @param [in] inStream 输入流
    @return 成功返回RC_S_OK，否则返回错误号 
    */
    HResult Open(IInStream* inStream) ;
};

class RCComFuncImpl
{
public:

    /** 读取扇区
    @param [in] inStream 输入流
    @param [out] buf 返回读取的数据
    @param [in] sectorSizeBits 扇区大小
    @param [in] sid SID
    @return 成功返回RC_S_OK，否则返回错误号 
    */
    static HResult ReadSector(IInStream* inStream, byte_t* buf, int32_t sectorSizeBits, uint32_t sid);

    /** 读取ID
    @param [in] inStream 输入流
    @param [out] buf 返回读取的数据
    @param [in] sectorSizeBits 扇区大小
    @param [in] sid SID
    @param [in] dest 返回ID
    @return 成功返回RC_S_OK，否则返回错误号 
    */
    static HResult ReadIDs(IInStream* inStream, byte_t* buf, int32_t sectorSizeBits, uint32_t sid, uint32_t* dest);

    /** 获取文件时间
    @param [in] p 内存数据
    @param [out] ft 返回文件时间
    */
    static void GetFileTimeFromMem(const byte_t* p, RC_FILE_TIME* ft);

    /** 从文件名组合名字
    @param [in] s 文件名
    @return 返回组合名
    */
    static RCString CompoundNameToFileName(const RCString& s);

    /** 从文件名组合Msi名字
    @param [in] name 文件名
    @param [out] resultName Msi名字
    @return 成功返回true，否则返回false
    */
    static bool CompoundMsiNameToFileName(const RCString& name, RCString &resultName);

    /** 转换名字
    @param [in] p 内存数据
    @return 返回名字
    */
    static RCString ConvertName(const byte_t *p);

    /** 文件时间转换成UInt64
    @param [in] tt 文件时间
    @return 返回UInt64表示的文件时间
    */
    static uint64_t RCFileTime2UInt64(const RC_FILE_TIME& tt);
};

END_NAMESPACE_RCZIP

#endif //__RCComIn_h_