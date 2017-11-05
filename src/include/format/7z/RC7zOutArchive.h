/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RC7zOutArchive_h_
#define __RC7zOutArchive_h_ 1

#include "base/RCNonCopyable.h"
#include "interface/IStream.h"
#include "interface/ICoder.h"
#include "format/7z/RC7zWriteBufferLoc.h"
#include "format/7z/RC7zEncoder.h"
#include "common/RCOutBuffer.h"
#include "common/RCBuffer.h"
#include "format/7z/RC7zItem.h"
#include "format/7z/RC7zHeader.h"

BEGIN_NAMESPACE_RCZIP

struct RC7zHeaderOptions
{
    /** 是否压缩主头
    */
    bool m_compressMainHeader;

    /** 是否写创建时间
    */
    bool m_writeCTime;

    /** 是否写访问时间
    */
    bool m_writeATime;

    /** 是否写更改时间
    */
    bool m_writeMTime;

    /** 默认构造函数
    */
    RC7zHeaderOptions():
        m_compressMainHeader(true),
        m_writeCTime(false),
        m_writeATime(false),
        m_writeMTime(true)
    {
    }
};

class RC7zOutArchive:
    private RCNonCopyable
{
public:

    /** 默认构造函数
    */
    RC7zOutArchive() ;
    
    /** 默认析构函数
    */
    ~RC7zOutArchive() ;
    
public:

    /** 创建
    @param [in] stream 输出流
    @param [in] endMarker 是否有结束标记
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult Create(ISequentialOutStream* stream, bool endMarker) ;

    /** 关闭
    */
    void Close() ;

    /** 跳过文档头
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult SkipPrefixArchiveHeader() ;

    /** 写基本数据
    @param [in] codecsInfo 编码管理器
    @param [in] db 基本数据
    @param [in] options 压缩选项
    @param [in] headerOptions 头选项
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult WriteDatabase(ICompressCodecsInfo* codecsInfo,
                          const RC7zArchiveDatabase& db,
                          const RC7zCompressionMethodMode* options,
                          const RC7zHeaderOptions& headerOptions) ;

#ifdef _7Z_VOL

    /** 获取卷的头大小
    @param [in] dataSize 数据大小
    @param [in] nameLength 名字长度
    @param [in] props 属性
    @return 卷的头大小
    */
    static uint32_t GetVolHeadersSize(uint64_t dataSize, int32_t nameLength = 0, bool props = false);

    /** 获取卷的数据大小
    @param [in] volSize 卷大小
    @param [in] nameLength 名字长度
    @param [in] props 属性
    @return 卷的数据大小
    */
    static uint64_t GetVolPureSize(uint64_t volSize, int32_t nameLength = 0, bool props = false);
#endif
  
public:

    /** 获取输出流接口
    @return 输出流
    */
    ISequentialOutStreamPtr GetSeqOutStream() ;
    
private:
    
    /** 写主数据
    @param [in] data 数据缓冲
    @param [in] size 数据大小
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult WriteDirect(const void* data, uint32_t size) ;

    /** 获取当前位置
    @return 返回当前位置
    */
    uint64_t GetPos() const ;

    /** 写入多字节
    @param [in] data 字节起始地址
    @param [in] size 字节长度
    */
    void WriteBytes(const void* data, size_t size) ;

    /** 写入多字节
    @param [in] data 写入的byte buffer
    */
    void WriteBytes(const RCByteBuffer& data) ;

    /** 写入字节
    @param [in] b 需要写入的字节值
    */
    void WriteByte(byte_t b) ;

    /** 写四个字节
    @param [in] value 32位数值
    */
    void WriteUInt32(uint32_t value) ;

    /** 写八个字节
    @param [in] value 64位数值
    */
    void WriteUInt64(uint64_t value) ;

    /** 写数据
    @param [in] value 64位数值
    */
    void WriteNumber(uint64_t value) ;

    /** 写ID
    @param [in] value 64位数值
    */
    void WriteID(uint64_t value) ;

    /** 写文件夹
    @param [in] folder 文件夹
    */
    void WriteFolder(const RC7zFolder& folder) ;

    /** 写bool向量
    @param [in] boolVector bool向量
    */
    void WriteBoolVector(const RCBoolVector& boolVector) ;

    /** 写hash
    @param [in] digestsDefined 是否有hash摘要
    @param [in] hashDigests  hash摘要
    */
    void WriteHashDigests(const RCBoolVector& digestsDefined,
                          const RCVector<uint32_t>& hashDigests) ;

    /** 写压缩信息
    @param [in] dataOffset 数据偏移
    @param [in] packSizes 压缩包大小
    @param [in] packCRCsDefined 是否有包的crc
    @param [in] packCRCs 包的crc
    */
    void WritePackInfo(uint64_t dataOffset,
                       const RCVector<uint64_t>& packSizes,
                       const RCBoolVector& packCRCsDefined,
                       const RCVector<uint32_t>& packCRCs) ;

    /** 写解压信息
    @param [in] folders 文件夹
    */
    void WriteUnpackInfo(const RCVector<RC7zFolder>& folders);

    /** 写流信息
    @param [in] folders 文件夹
    @param [in] numUnpackStreamsInFolders 文件夹中有多少个戒烟流
    @param [in] unpackSizes 解压大小
    @param [in] digestsDefined 是否有hash摘要
    @param [in] hashDigests hash摘要
    */
    void WriteSubStreamsInfo(const RCVector<RC7zFolder>& folders,
                             const RCVector<RC7zNum>& numUnpackStreamsInFolders,
                             const RCVector<uint64_t>& unpackSizes,
                             const RCBoolVector& digestsDefined,
                             const RCVector<uint32_t>& hashDigests);

    /** 跳过对齐
    @param [in] pos 位置
    @param [in] alignSize 跳过多大
    */
    void SkipAlign(uint32_t pos, uint32_t alignSize) ; 

    /**写RCBoolVector
    @param [in] v bool向量
    @param [in] numDefined 定义了多少个
    @param [in] type 类型
    @param [in] itemSize 项的大小
    */
    void WriteAlignedBoolHeader(const RCBoolVector& v, int32_t numDefined, byte_t type, uint32_t itemSize) ;

    /** 写RC7zUInt64DefVector
    @param [in] v RC7zUInt64DefVector
    @param [in] type 类型
    */
    void WriteUInt64DefVector(const RC7zUInt64DefVector& v, byte_t type) ;  

    /** 压缩流
    @param [in] codecsInfo 编码管理器
    @param [in] encoder 压缩接口
    @param [in] data 数据
    @param [in] packSizes 包的大小
    @param [in] folders 文件夹
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult EncodeStream(ICompressCodecsInfo* codecsInfo,
                         RC7zEncoder& encoder, 
                         const RCByteBuffer& data,
                         RCVector<uint64_t>& packSizes, 
                         RCVector<RC7zFolder>& folders) ;

    /** 写头信息
    @param [in] db 基本数据
    @param [in] headerOptions 头选项
    @param [in] headerOffset 头偏移
    */
    void WriteHeader(const RC7zArchiveDatabase& db,
                     const RC7zHeaderOptions& headerOptions,
                     uint64_t& headerOffset) ;

    /** 写签名
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult WriteSignature() ; 

    /** 写起始头信息
    @param [in] h 起始头信息
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult WriteStartHeader(const RC7zStartHeader& h) ; 
    
#ifdef _7Z_VOL

    /** 写完成签名
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult WriteFinishSignature() ;

    /** 写完成头
    @param [in] h 完成头
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult WriteFinishHeader(const RC7zFinishHeader& h) ;

#endif
  
private:

    /** 写多字节字节
    @param [in] stream 输出流
    @param [in] data 内存数据
    @param [in] size 内存大小
    @return 成功返回RC_S_OK，否则返回错误号
    */
    static HResult WriteBytes(ISequentialOutStream* stream, const void* data, size_t size) ; 

    /** 获取大数据大小
    @param [in] value
    @return 返回大数据大小
    */
    static uint32_t GetBigNumberSize(uint64_t value) ;

    /** 设置无符号32位
    @param [out] p 输出指针
    @param [in] d 32位数据
    */
    static void SetUInt32(byte_t* p, uint32_t d) ;

    /** 设置无符号64位
    @param [in] p 输出指针
    @param [in] d 64位数据
    */
    static void SetUInt64(byte_t* p, uint64_t d) ;
    
private:

    /** 输出流
    */
    ISequentialOutStreamPtr m_spSeqOutStream ;

    /** 头偏移
    */
    uint64_t m_prefixHeaderPos ;

    /** 是否是计算模式
    */
    bool m_countMode;

    /** 是否写道流
    */
    bool m_writeToStream;

    /** 计算大小
    */
    size_t m_countSize;

    /** crc
    */
    uint32_t m_crc;

    /** 输出缓冲区
    */
    RCOutBuffer m_outByte;

    /** 输出缓冲区
    */
    RC7zWriteBufferLoc m_outByte2;

    /**输出缓冲区
    */
    IOutStreamPtr m_spOutStream ;
    
#ifdef _7Z_VOL
    
    /** 是否有结束标记
    */
    bool m_endMarker;

#endif
};

END_NAMESPACE_RCZIP

#endif //__RC7zOutArchive_h_
