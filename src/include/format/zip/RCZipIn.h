/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCZipIn_h_
#define __RCZipIn_h_ 1

#include "base/RCDefs.h"
#include "common/RCBuffer.h"
#include "common/RCInBuffer.h"
#include "interface/IStream.h"
#include "base/RCString.h"
#include "RCZipItem.h"

BEGIN_NAMESPACE_RCZIP

namespace RCZipInException
{
    /** RCZipIn中的异常类型
    */
    enum RCZipInExceptionType
    {
        kUnexpectedEndOfArchive = 0,

        kArchiceHeaderCRCError,

        kFileHeaderCRCError,

        kIncorrectArchive,

        kMultiVolumeArchiveAreNotSupported,

        kReadStreamError,

        kSeekStreamError
    };
}

class RCZipInArchiveInfo
{
public:

    /** 默认构造函数
    */
    RCZipInArchiveInfo():
      m_base(0),
      m_startPosition(0),
      m_commentSizePosition(0) 
    {}

public:

    /** 清除
    */
    void Clear()
    {
        m_base = 0;
        m_startPosition = 0;
        m_commentSizePosition = 0;
        m_comment.SetCapacity(0) ;
    }

public:

    /** 基地址
    */
    uint64_t m_base ;

    /** 起始位置
    */
    uint64_t m_startPosition ;

    /** 注释大小位置
    */
    uint64_t m_commentSizePosition ;

    /** 注释
    */
    RCByteBuffer m_comment ;
};

class RCZipProgressVirt
{
public:

    /** 设置总数
    @param [in] numFiles 文件数
    */
    virtual HResult SetTotal(uint64_t numFiles) = 0 ;

    /** 设置完成数
    @param [in] numFiles 文件数
    */
    virtual HResult SetCompleted(uint64_t numFiles) = 0 ;
};

struct RCZipCdInfo
{
    /** 大小
    */
    uint64_t m_size ;

    /** 偏移
    */
    uint64_t m_offset ;
};

class RCZipIn
{
public:

    /** 默认构造函数
    */
    RCZipIn() ;
    
private:

    /** 输入流
    */
    IInStreamPtr m_stream ;

    /** 签名
    */
    uint32_t m_signature ;

    /** 流起始位置
    */
    uint64_t m_streamStartPosition ;

    /** 位置
    */
    uint64_t m_position ;

    /** 缓冲类型
    */
    bool m_inBufMode ;

    /** 缓冲
    */
    RCInBuffer m_inBuffer ;

public:

    /** 定位
    @param [in] offset 文件数
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult Seek(uint64_t offset) ;

    /** 查找并读取标志
    @param [in] stream 输入流
    @param [in] searchHeaderSizeLimit 最大查找范围
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult FindAndReadMarker(IInStream* stream, const uint64_t* searchHeaderSizeLimit) ;

    /** 读四个字节转成32位无符号整型
    @param [out] value 返回读取的值
    @return 成功返回true，否则返回false
    */
    bool ReadUInt32(uint32_t& value) ;

    /** 读文件名
    @param [in] nameSize 名字长度
    @param [out] dest 返回文件名
    */
    void ReadFileName(uint32_t nameSize, RCStringA& dest) ;

    /** 读字节
    @param [out] data 返回读取的内容
    @param [in] size 读取的大小
    @param [out] processedSize 返回实际读到的大小
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult ReadBytes(void* data, uint32_t size, uint32_t* processedSize) ;

    /** 读字节并检查是否读到指定大小
    @param [out] data 返回实际读到的内容
    @param [in] size 读取的大小
    @return 成功返回RC_S_OK，否则返回错误号
    */
    bool ReadBytesAndTestSize(void* data, uint32_t size) ;

    /** 安全读字节
    @param [out] data 返回实际读到的内容
    @param [in] size 读取的大小
    */
    void SafeReadBytes(void* data, uint32_t size) ;

    /** 读内存
    @param [out] buffer 返回实际读到的内容
    @param [in] size 读取的大小
    */
    void ReadBuffer(RCByteBuffer& buffer, uint32_t size) ;

    /** 读一个字节
    @return 返回一个字节
    */
    byte_t ReadByte() ;

    /** 读两个字节转成16位无符号整型
    @return 返回16位无符号整型
    */
    uint16_t ReadUInt16() ;

    /** 读四个字节转成32位无符号整型
    @return 返回32位无符号整型
    */
    uint32_t ReadUInt32() ;

    /** 读八个字节转成64位无符号整型
    @return 返回64位无符号整型
    */
    uint64_t ReadUInt64() ;

    /** 跳过
    @param [in] num 跳过的个数
    */
    void Skip(uint64_t num) ;

    /** 相对移动文件指针的位置
    @param [in] addValue 移动的大小
    */
    void IncreaseRealPosition(uint64_t addValue) ;

    /** 读扩展信息
    @param [in] 扩展大小
    @param [out] extraBlock 返回扩展块
    @param [out] unpackSize 返回解压大小
    @param [out] packSize 返回压缩大小
    @param [out] localHeaderOffset 返回头偏移
    @param [out] diskStartNumber 返回磁盘起始号
    */
    void ReadExtra(uint32_t extraSize,
                   RCZipExtraBlock& extraBlock,
                   uint64_t& unpackSize,
                   uint64_t& packSize,
                   uint64_t& localHeaderOffset,
                   uint32_t& diskStartNumber) ;

    /** 读取本地item
    @param [out] item 返回zip item信息
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult ReadLocalItem(RCZipItemEx& item) ;

    /** 读取本地item描述
    @param [out] item 返回zip item信息
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult ReadLocalItemDescriptor(RCZipItemExPtr& item) ;

    /** 读取cd item
    @param [out] item 返回zip item信息
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult ReadCdItem(RCZipItemEx& item) ;

    /** 尝试读取ecd64
    @param [in] offset 偏移
    @param [out] cdInfo 返回cd信息
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult TryEcd64(uint64_t offset, RCZipCdInfo& cdInfo) ;

    /** 查找cd
    @param [out] cdInfo 返回cd信息
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult FindCd(RCZipCdInfo& cdInfo) ;

    /** 尝试读取cd
    @param [out] items 返回zip items信息
    @param [out] cdOffset 返回cd偏移
    @param [out] cdSize 返回cd大小
    @param [in] progress 进度管理器
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult TryReadCd(RCVector <RCZipItemExPtr>& items,
                      uint64_t cdOffset,
                      uint64_t cdSize,
                      RCZipProgressVirt* progress) ;

    /** 读取cd
    @param [out] items 返回zip items信息
    @param [out] cdOffset 返回cd偏移
    @param [out] cdSize 返回cd大小
    @param [in] progress 进度管理器
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult ReadCd(RCVector<RCZipItemExPtr>& items,
                   uint64_t& cdOffset,
                   uint64_t& cdSize,
                   RCZipProgressVirt* progress) ;

    /** 读取本地和cd
    @param [out] items 返回zip items信息
    @param [in] progress 进度管理器
    @param [out] cdOffset 返回cd偏移
    @param [out] numCdItems 返回cd个数
    @return 成功返回RC_S_OK，否则返回错误号
   */
    HResult ReadLocalsAndCd(RCVector<RCZipItemExPtr>& items,
                            RCZipProgressVirt* progress,
                            uint64_t& cdOffset,
                            int32_t& numCdItems) ;

public:

    /** zip文档信息
    */
    RCZipInArchiveInfo m_archiveInfo ;

    /** 是否是zip64
    */
    bool m_isZip64 ;

    /** 是否有头信息
    */
    bool m_isOkHeaders ;

public:

    /** 读取头信息
    @param [out] items 返回zip items信息
    @param [in] progress 进度管理器
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult ReadHeaders(RCVector<RCZipItemExPtr>& items, RCZipProgressVirt* progress) ;

    /** 读取cd item后的item
    @param [out] items 返回zip items信息
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult ReadLocalItemAfterCdItem(RCZipItemEx& item) ;

    /** 读取所有cd item后的item
    @param [out] items 返回zip items信息
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult ReadLocalItemAfterCdItemFull(RCZipItemEx& item) ;

    /** 打开
    @param [in] stream 输入流
    @param [in] searchHeaderSizeLimit 最大查找范围
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult Open(IInStream* stream, const uint64_t* searchHeaderSizeLimit) ;

    /** 关闭
    */
    void Close() ;

    /** 得到文档信息
    @param [out] archiveInfo 返回文档信息
    */
    void GetArchiveInfo(RCZipInArchiveInfo& archiveInfo) const;

    /** 在文档中定位
    @param [in] position 位置
    @return 成功返回true，否则返回false
    */
    bool SeekInArchive(uint64_t position) ;

    /** 创建最大流
    @param [in] position 位置
    @param [in] size 大小
    @return 返回流接口
    */
    ISequentialInStream* CreateLimitedStream(uint64_t position, uint64_t size) ;

    /** 创建流
    @return 返回流接口
    */
    IInStream* CreateStream() ;

    /** 是否打开流
    @return 打开返回true，否则返回false
    */
    bool IsOpen() const
    { 
        return m_stream != NULL;
    }
};

/** RCZipIn智能指针
*/
typedef RCSharedPtr<RCZipIn> RCZipInPtr ;

END_NAMESPACE_RCZIP

#endif //__RCZipIn_h_
