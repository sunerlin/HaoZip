/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RC7zItem_h_
#define __RC7zItem_h_ 1

#include "format/7z/RC7zDefs.h"
#include "common/RCVector.h"
#include "base/RCString.h"
#include "base/RCSmartPtr.h"
#include "common/RCBuffer.h"

BEGIN_NAMESPACE_RCZIP

struct RC7zCoderInfo
{
    /** 模式ID
    */
    RCMethodID m_methodID;

    /** 属性
    */
    RCByteBuffer m_properties;

    /** 输入流个数
    */
    RC7zNum m_numInStreams;

    /** 输出流个数
    */
    RC7zNum m_numOutStreams;

    /** 是否是简单编码
    @return 是返回true，否则返回false
    */
    bool IsSimpleCoder() const ;    
};

struct RC7zBindPair
{
    /** 输入索引
    */
    RC7zNum m_inIndex ;

    /** 输出索引
    */
    RC7zNum m_outIndex ;
};

struct RC7zFolder
{
    /** 编码器
    */
    RCVector<RC7zCoderInfo> m_coders;

    /** 绑定信息
    */
    RCVector<RC7zBindPair> m_bindPairs;

    /** 包的流
    */
    RCVector<RC7zNum> m_packStreams;

    /** 解压后大小
    */
    RCVector<uint64_t> m_unpackSizes;

    /** 解压CRC
    */
    uint32_t m_unpackCRC;

    /** 是否有解压CRC
    */
    bool m_unpackCRCDefined;

    /** 默认构造函数
    */
    RC7zFolder() ;

    /** 获取解压后的大小
    @throws 失败抛异常
    @return 解压后大小
    */
    uint64_t GetUnpackSize() const  ;

    /** 获取输出流
    @return 输出流
    */
    RC7zNum GetNumOutStreams() const ;

    /** 在输入流中查找绑定信息
    @param [in] inStreamIndex 输入流的索引
    @return 成功返回在BindPair索引，否则返回-1
    */
    int32_t FindBindPairForInStream(RC7zNum inStreamIndex) const ; 

    /** 在输出流中查找绑定信息
    @param [in] outStreamIndex 输出流的索引
    @return 成功返回在BindPair索引，否则返回-1
    */
    int32_t FindBindPairForOutStream(RC7zNum outStreamIndex) const ;

    /** 在流列表中查找绑定信息
    @param [in] inStreamIndex 流的索引
    @return 成功返回在BindPair索引，否则返回-1
    */
    int32_t FindPackStreamArrayIndex(RC7zNum inStreamIndex) const ;

    /** 检查结构
    @return 通过检查返回true，否则返回false
    */
    bool CheckStructure() const;
};

struct RC7zUInt64DefVector
{
    /** 值
    */
    RCVector<uint64_t> m_values;

    /** 是否定义
    */
    RCVector<bool> m_defined;

    /** 清楚
    */
    void Clear() ;

    /** 储备
    */
    void ReserveDown() ;

    /** 获取项 
    @param [in] index 索引
    @param [out] value 项
    @return 成功返回true，否则返回false
    */
    bool GetItem(int32_t index, uint64_t& value) const ;

    /** 设置项
    @param [in] index 索引
    @param [in] defined 是否定义
    @param [in] value 数值
    */
    void SetItem(int32_t index, bool defined, uint64_t value) ;

    /** 检查大小
    @param [in] size 大小
    @return 通过检查返回true，否则返回false
    */
    bool CheckSize(int32_t size) const ;
};

struct RC7zFileItem
{
    /** 大小
    */
    uint64_t m_size;

    /** 属性
    */
    uint32_t m_attrib;

    /** crc
    */
    uint32_t m_crc;

    /** 名字
    */
    RCString m_name;

    /** 是否有流(Test it !!! it means that there is)
    */
    bool m_hasStream; 

    /** 是否是路径(stream in some folder. It can be empty stream)
    */
    bool m_isDir;

    /** 是否定义了CRC
    */
    bool m_crcDefined;

    /** 是否定义属性
    */
    bool m_attribDefined;

    /** 默认构造函数
    */
    RC7zFileItem() ;

    /** 设置属性
    @param [in] attrib 属性
    */
    void SetAttrib(uint32_t attrib) ;
};

struct RC7zFileItem2
{
    /** 创建时间
    */
    uint64_t m_cTime;

    /** 访问时间
    */
    uint64_t m_aTime;

    /** 修改时间
    */
    uint64_t m_mTime;

    /** 起始偏移
    */
    uint64_t m_startPos;

    /** 是否有创建时间
    */
    bool m_cTimeDefined;

    /** 是否有访问时间
    */
    bool m_aTimeDefined;

    /** 是否有修改时间
    */
    bool m_mTimeDefined;

    /** 是否有起始偏移
    */
    bool m_startPosDefined;

    /** 是否是anti
    */
    bool m_isAnti;
};

/** RC7zFileItem智能指针
*/
typedef RCSharedPtr<RC7zFileItem>  RC7zFileItemPtr ;

/** RC7zFileItem2智能指针
*/
typedef RCSharedPtr<RC7zFileItem2>  RC7zFileItem2Ptr ;

struct RC7zArchiveDatabase
{
    /** 包的大小
    */
    RCVector<uint64_t> m_packSizes;

    /** 是否定义了crc
    */
    RCVector<bool> m_packCRCsDefined;

    /** crc
    */
    RCVector<uint32_t> m_packCRCs;

    /** 文件夹
    */
    RCVector<RC7zFolder> m_folders;

    /** 解压流的个数
    */
    RCVector<RC7zNum> m_numUnpackStreamsVector;

    /** 文件
    */
    RCVector<RC7zFileItemPtr> m_files;

    /** 创建时间
    */
    RC7zUInt64DefVector m_cTime;

    /** 访问时间
    */
    RC7zUInt64DefVector m_aTime;

    /** 更改时间
    */
    RC7zUInt64DefVector m_mTime;

    /** 是否偏移
    */
    RC7zUInt64DefVector m_startPos;

    /** 是否是anti
    */
    RCVector<bool> m_isAnti;

    /** 注释起始位置
    */
    uint64_t m_commentStartPos;

    /** 是否有注释
    */
    bool m_isCommented;

    /** 注释信息
    */
    RCString m_comment;

    /** 默认构造函数
    */
    RC7zArchiveDatabase() :
        m_commentStartPos(0),
        m_isCommented(false)
    {}

    /** 清楚
    */
    void Clear() ;

    /** 储备
    */
    void ReserveDown() ;

    /** 是否是空
    @return 是返回true，否则返回false
    */
    bool IsEmpty() const ;

    /** 检查文件个数
    @return 通过检查返回true，否则返回false
    */
    bool CheckNumFiles() const ;

    /** 是否是固实压缩
    @return 是返回true，否则返回false
    */
    bool IsSolid() const ;    

    /** 是否是anti
    @param [in] index 索引
    @return 是返回true，否则返回false
    */
    bool IsItemAnti(int32_t index) const ;

    /** 设置项的anti
    @param [in] index 索引
    @param [in] isAnti 是否anti
    */
    void SetItemAnti(int32_t index, bool isAnti) ;

    /** 获取文件信息
    @param [in] index 索引
    @param [out] file 文件信息
    @param [out] file2 文件信息
    */
    void GetFile(int32_t index, RC7zFileItemPtr& file, RC7zFileItem2Ptr& file2) const;

    /** 添加文件
    @param [in] file 文件信息
    @param [in] file2 文件信息
    */
    void AddFile(const RC7zFileItemPtr& file, const RC7zFileItem2Ptr& file2);
};

END_NAMESPACE_RCZIP

#endif //__RC7zItem_h_
