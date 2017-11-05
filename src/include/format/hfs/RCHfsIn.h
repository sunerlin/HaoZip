/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCHfsIn_h_
#define __RCHfsIn_h_ 1

#include "base/RCString.h"
#include "common/RCVector.h"
#include "interface/IStream.h"
#include "common/RCBuffer.h"

BEGIN_NAMESPACE_RCZIP

struct RCHfsExtent
{
    /** 偏移位置
    */
    uint32_t m_pos ;

    /** 块的个数
    */
    uint32_t m_numBlocks ;
};

struct RCHfsFork
{
    /** 大小
    */
    uint64_t m_size;

    /** 块的个数
    */
    uint32_t m_numBlocks;

    /** 扩展信息
    */
    RCHfsExtent m_extents[8];

    /** 解析
    @param [in] p 内存数据
    */
    void Parse(const byte_t* p) ;
};

struct RCHfsVolHeader
{
    /** 头部
    */
    byte_t m_header[2];

    /** 版本
    */
    uint16_t m_version;

    /** 创建时间
    */
    uint32_t m_cTime;

    /** 修改时间
    */
    uint32_t m_mTime;

    /** 块的日志大小
    */
    int32_t m_blockSizeLog;

    /** 块的个数
    */
    uint32_t m_numBlocks;

    /** 空闲块的个数
    */
    uint32_t m_numFreeBlocks;

    /** 是否是扩展文件
    */
    RCHfsFork m_extentsFile;

    /** 是否是日志文件
    */
    RCHfsFork m_catalogFile;

    /** 是否是hfsx
    @return 是返回true，否则返回false
    */
    bool IsHfsX() const ;
};

enum ERecordType
{
    RECORD_TYPE_FOLDER = 1,
    RECORD_TYPE_FILE   = 2,
    RECORD_TYPE_FOLDER_THREAD = 3,
    RECORD_TYPE_FILE_THREAD = 4
};

struct RCHfsItem
{
    /** 名字
    */
    RCString m_name;

    /** 父ID
    */
    uint32_t m_parentID;

    /** 类型
    */
    uint16_t m_type;

    /** ID
    */
    uint32_t m_id;

    /** 创建时间
    */
    uint32_t m_cTime;

    /** 修改时间
    */
    uint32_t m_mTime;

    /** 访问时间
    */
    uint32_t m_aTime;

    /** 大小
    */
    uint64_t m_size;

    /** 块的个数
    */
    uint32_t m_numBlocks;

    /** 扩展信息
    */
    RCVector<RCHfsExtent> m_extents ;

    /** 默认构造函数
    */
    RCHfsItem() ;

    /** 是否是目录
    @return 是目录返回true，否则返回false
    */
    bool IsDir() const ;
};

struct RCHfsIdIndexPair
{
    /** ID
    */
    uint32_t m_id ;

    /** 索引
    */
    int32_t m_index ;
};

struct RCHfsProgressVirt
{
    /** 总数量
    @param [in] numFiles 总的文件个数
    @return 成功返回RC_S_OK，否则返回错误号
    */
    virtual HResult SetTotal(uint64_t numFiles) = 0 ;

    /** 完成数量
    @param [in] numFiles 已经完成的文件个数
    @return 成功返回RC_S_OK, 失败则返回错误码
    */
    virtual HResult SetCompleted(uint64_t numFiles) = 0 ;
};

class RCHfsDatabase
{
public:

    /** 默认构造函数
    */
    RCHfsDatabase() ;

    /** 默认析构函数
    */
    ~RCHfsDatabase() ;

public:

    /** 清除
    */
    void Clear() ;

    /** 获取路径
    @param [in] index item索引
    @return 返回路径
    */
    RCString GetItemPath(int32_t index) const ;

    /** 打开文档
    @param [in] inStream 输入流
    @param [in] progress 进度接口
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult Open(IInStream* inStream, RCHfsProgressVirt* progress) ; 
    
public:

    /** 头部信息
    */
    RCHfsVolHeader m_header ;

    /** hfs项信息
    */
    RCVector<RCHfsItem> m_items ;

private:

    /** 获取扩展文件信息
    @param [in] inStream 输入流
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult LoadExtentFile(IInStream* inStream) ;

    /** 获取日志信息
    @param [in] inStream 输入流
    @param [in] progress 进度接口
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult LoadCatalog(IInStream* inStream, RCHfsProgressVirt* progress) ; 

    /** 读取文件
    @param [in] fork fork信息
    @param [out] buf 返回读取的内容
    @param [in] inStream 输入流
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult ReadFile(const RCHfsFork& fork, RCByteBuffer& buf, IInStream* inStream) ;
       
private:

    /** ID到索引的MAP 
    */
    RCVector<RCHfsIdIndexPair> m_idToIndexMap ;
};

END_NAMESPACE_RCZIP

#endif //__RCHfsIn_h_
