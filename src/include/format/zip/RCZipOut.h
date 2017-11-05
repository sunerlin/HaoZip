/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCZipOut_h_
#define __RCZipOut_h_ 1

#include "base/RCDefs.h"
#include "interface/IStream.h"
#include "common/RCOutBuffer.h"
#include "RCZipItem.h"

BEGIN_NAMESPACE_RCZIP

class RCZipOut
{
public:

    /** 默认析构函数
    */
    ~RCZipOut();

public:

    /** 创建流
    @param [in] outStream 输出流
    */
    void Create(IOutStream* outStream) ;

    /** 移动起始位置
    @param [in] distanceToMove 移动大小
    */
    void MoveBasePosition(uint64_t distanceToMove) ;

    /** 获取当前位置
    @return 返回当前位置
    */
    uint64_t GetCurrentPosition() const;

    /** 准备写zip64的压缩数据
    @param [in] fileNameLength 文件名长度
    @param [in] isZip64 是否是zip64
    @param [in] aesEncryption 是否是aes加密
    */
    void PrepareWriteCompressedDataZip64(uint16_t fileNameLength, 
                                         bool isZip64,
                                         bool aesEncryption) ;

    /** 准备写压缩数据
    @param [in] fileNameLength 文件名长度
    @param [in] unPackSize 解压大小
    @param [in] aesEncryption 是否是aes加密
    */    
    void PrepareWriteCompressedData(uint16_t fileNameLength,
                                    uint64_t unPackSize,
                                    bool aesEncryption) ;

    /** 准备写压缩数据
    @param [in] fileNameLength 文件名长度
    @param [in] unPackSize 解压大小
    @param [in] packSize 压缩大小
    @param [in] aesEncryption 是否是aes加密
    */
    void PrepareWriteCompressedData2(uint16_t fileNameLength,
                                     uint64_t unPackSize,
                                     uint64_t packSize,
                                     bool aesEncryption) ;

    /** 写本地头
    @param [in] item item信息
    */
    void WriteLocalHeader(RCZipItem& item) ;

    /** 写主目录
    @param [in] items item 信息
    @param [in] comment 注释信息
    */
    void WriteCentralDir(const RCVector<RCZipItemPtr>& items, 
                         const RCByteBuffer& comment) ;

    /** 为压缩创建流
    @param [in] outStream 输出流
    */
    void CreateStreamForCompressing(IOutStream** outStream) ;

    /** 为拷贝创建流
    @param [in] outStream 输出流
    */  
    void CreateStreamForCopying(ISequentialOutStream** outStream) ;

    /** 定位到包的起始位置
    */
    void SeekToPackedDataPosition() ;

    /** 修改分卷标记(如果是zip分卷但是为单卷)
    */
    void WriteSignleVolumeSignature();

private:

    /** 写字节
    @param [in] buffer 内存
    @param [in] size 大小
    */
    void WriteBytes(const void* buffer, uint32_t size) ;

    /** 写一个字节
    @param [in] b 字节
    */
    void WriteByte(byte_t b) ;

    /** 写两个字节
    @param [in] value 32位数值
    */
    void WriteUInt16(uint16_t value) ;

    /** 写四个字节
    @param [in] value 32位数值
    */
    void WriteUInt32(uint32_t value) ;

    /** 写八个字节
    @param [in] value 64位数值
    */
    void WriteUInt64(uint64_t value) ;

    /** 写主头
    @param [in] item item信息
    */
    void WriteCentralHeader(const RCZipItemPtr& item) ;

    /** 写扩展信息
    @param [in] extra 扩展信息
    */
    void WriteExtra(const RCZipExtraBlock& extra) ;

    /** 定位
    @param [in] offset 偏移
    */
    void SeekTo(uint64_t offset) ;

    /** 获取主头写入磁盘的大小
    @param [in] item item信息
    */
    uint64_t GetCentralHeaderSize(const RCZipItemPtr& item);

private:

    /** 输出流
    */
    IOutStreamPtr m_stream ;

    /** 输出大小
    */
    RCOutBuffer m_outBuffer ;

    /** 起始位置
    */
    uint64_t m_basePosition ;

    /** 本地文件头大小
    */
    uint32_t m_localFileHeaderSize ;

    /** 扩展大小
    */
    uint32_t m_extraSize ;

    /** 是否是zip64
    */
    bool m_isZip64 ;

    /** 是否生成zip分卷
    */
    bool m_isSplit;

    /** 分卷输出流操作接口
    */
    IMultiVolStreamPtr m_multiVolStream;
};

END_NAMESPACE_RCZIP

#endif //__RCZipOut_h_
