/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RC7zInArchive_h_
#define __RC7zInArchive_h_ 1

#include "base/RCNonCopyable.h"
#include "common/RCVector.h"
#include "format/7z/RC7zDefs.h"
#include "format/7z/RC7zInByte.h"
#include "interface/IStream.h"
#include "interface/ICoder.h"
#include "interface/IPassword.h"
#include "format/7z/RC7zInArchiveInfo.h"
#include "format/7z/RC7zItem.h"
#include "format/7z/RC7zArchiveDatabaseEx.h"

BEGIN_NAMESPACE_RCZIP

class RC7zInArchive:
    private RCNonCopyable
{
public:

    /** 默认构造函数
    */
    RC7zInArchive() ;

    /** 默认析构函数
    */
    ~RC7zInArchive() ;

public:


    /** 打开
    @param [in] stream 输入流
    @param [in] searchHeaderSizeLimit 最大查找范围
    @return 成功返回RC_S_OK，否则返回错误号，RC_S_FALSE表示不是7z格式
    */
    HResult Open(IInStream* stream, const uint64_t* searchHeaderSizeLimit); 

    /** 关闭
    */
    void Close() ;

    /** 读基本数据
    @param [in] codecsInfo 编码管理器
    @param [in] db 基本数据
    @param [in] getTextPassword 密码管理器
    @param [in] passwordIsDefined 是否有密码
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult ReadDatabase(ICompressCodecsInfo* codecsInfo,
                         RC7zArchiveDatabaseEx& db,
                         ICryptoGetTextPassword* getTextPassword, 
                         bool& passwordIsDefined) ;

private:

    class RC7zStreamSwitch ;

    /** 友元类RC7zStreamSwitch
    */
    friend class RC7zStreamSwitch ;

private:

    /** 添加字节流
    @param [in] buffer 数据
    @param [in] size 大小
    */
    void AddByteStream(const byte_t* buffer, size_t size) ;

    /** 删除字节流
    */
    void DeleteByteStream() ;

    /** 查找并读取签名
    @param [in] stream 输入流
    @param [in] searchHeaderSizeLimit 最大查找范围
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult FindAndReadSignature(IInStream* stream, const uint64_t* searchHeaderSizeLimit) ;

    /** 读字节
    @param [out] data 返回读取的内容
    @param [in] size 读取的大小
    */
    void ReadBytes(byte_t *data, size_t size) ;

    /** 读一个字节
    @return 返回一个字节
    */
    byte_t ReadByte() ;

    /** 读数
    @return 返回64位无符号整型
    */
    uint64_t ReadNumber() ;

    /** 读数
    @return 返回RC7zNum
    */
    RC7zNum ReadNum() ;

   /** 读ID
    @return 返回RC7zNum
    */
    uint64_t ReadID() ;

    /** 读四个字节转成32位无符号整型
    @return 返回32位无符号整型
    */
    uint32_t ReadUInt32() ;

    /** 读八个字节转成64位无符号整型
    @return 返回64位无符号整型
    */
    uint64_t ReadUInt64() ;

    /** 跳过
    @param [in] size 跳过的大小
    */
    void SkipData(uint64_t size) ;

    /** 跳过
    */
    void SkipData() ;

    /** 等待属性
    @param [in] attribute 属性
    */
    void WaitAttribute(uint64_t attribute) ;

    /** 读取文档属性
    @param [out] archiveInfo 返回文档属性
    */
    void ReadArchiveProperties(RC7zInArchiveInfo& archiveInfo) ;

    /** 获取下一个文件夹
    @param [out] itemInfo 返回文件信息
    */
    void GetNextFolderItem(RC7zFolder& itemInfo) ;

    /** 读取hash
    @param [in] numItems 多少项
    @param [in] digestsDefined 定义多少项目
    @param [in,out] digests 返回hash
    */
    void ReadHashDigests(int32_t numItems,
                         RCBoolVector& digestsDefined, 
                         RCVector<uint32_t>& digests) ;

    /** 读取包信息
    @param [in] dataOffset 数据偏移
    @param [in] packSizes 包大小
    @param [in] packCRCsDefined 是否定义包的crc
    @param [in] packCRCs crc列表
    */
    void ReadPackInfo(uint64_t& dataOffset,
                      RCVector<uint64_t>& packSizes,
                      RCBoolVector& packCRCsDefined,
                      RCVector<uint32_t>& packCRCs);

    /** 读取解压信息
    @param [in] dataVector 数据
    @param [in] folders 文件夹信息
    */
    void ReadUnpackInfo(const RCVector<RCByteBuffer>* dataVector,
                        RCVector<RC7zFolder>& folders);

    /** 读取子流的信息
    @param [in] folders 文件夹
    @param [in] numUnpackStreamsInFolders 包中解压的流个数
    @param [in] unpackSizes 解压大小
    @param [in] digestsDefined 摘要定义
    @param [in] digests 摘要信息
    */
    void ReadSubStreamsInfo(const RCVector<RC7zFolder>& folders,
                            RCVector<RC7zNum>& numUnpackStreamsInFolders,
                            RCVector<uint64_t>& unpackSizes,
                            RCBoolVector& digestsDefined,
                            RCVector<uint32_t>& digests) ;

    /** 读取流的信息
    @param [in] dataVector 数据
    @param [in] dataOffset 数据偏移
    @param [in] packSizes 包的大小
    @param [in] packCRCsDefined crc信息是否定义
    @param [in] packCRCs CRC信息
    @param [in] folders 文件夹
    @param [in] numUnpackStreamsInFolders 包中解压的流个数
    @param [in] unpackSizes 解压大小
    @param [in] digestsDefined 摘要定义
    @param [in] digests 摘要信息
    */
    void ReadStreamsInfo(const RCVector<RCByteBuffer>* dataVector,
                         uint64_t& dataOffset,
                         RCVector<uint64_t>& packSizes,
                         RCBoolVector& packCRCsDefined,
                         RCVector<uint32_t>& packCRCs,
                         RCVector<RC7zFolder>& folders,
                         RCVector<RC7zNum>& numUnpackStreamsInFolders,
                         RCVector<uint64_t>& unpackSizes,
                         RCBoolVector& digestsDefined,
                         RCVector<uint32_t>& digests);

    /** 读取RCBoolVector
    @param [in] numItems 读取多少个
    @param [out] v 返回RCBoolVector
    */
    void ReadBoolVector(int32_t numItems, RCBoolVector& v) ;

    /** 读取RCBoolVector
    @param [in] numItems 读取多少个
    @param [out] v 返回RCBoolVector
    */
    void ReadBoolVector2(int32_t numItems, RCBoolVector& v) ;

    /** 读取RC7zUInt64DefVector
    @param [in] dataVector 数据
    @param [out] v 返回RC7zUInt64DefVector
    @param [in] numFiles 读取多少个
    */
    void ReadUInt64DefVector(const RCVector<RCByteBuffer>& dataVector,
                             RC7zUInt64DefVector& v, 
                             int32_t numFiles);

    /** 读并解码包的流
    @param [in] codecsInfo 编码管理器
    @param [in] baseOffset 基本偏移
    @param [in] dataOffset 数据偏移
    @param [in] dataVector 数据
    @param [in] getTextPassword 密码管理器
    @param [in] passwordIsDefined 是否有密码
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult ReadAndDecodePackedStreams(ICompressCodecsInfo* codecsInfo,
                                       uint64_t baseOffset, 
                                       uint64_t& dataOffset,
                                       RCVector<RCByteBuffer>& dataVector, 
                                       ICryptoGetTextPassword* getTextPassword, 
                                       bool& passwordIsDefined);

    /** 读文信息
    @param [in] codecsInfo 编码管理器
    @param [in] db 基本数据
    @param [in] getTextPassword 密码管理器
    @param [in] passwordIsDefined 是否有密码
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult ReadHeader(ICompressCodecsInfo* codecsInfo,
                       RC7zArchiveDatabaseEx& db,
                       ICryptoGetTextPassword* getTextPassword, 
                       bool& passwordIsDefined);  

    /** 读基本信息
    @param [in] codecsInfo 编码管理器
    @param [in] db 基本数据
    @param [in] getTextPassword 密码管理器
    @param [in] passwordIsDefined 是否有密码
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult ReadDatabase2(ICompressCodecsInfo* codecsInfo,
                          RC7zArchiveDatabaseEx& db,
                          ICryptoGetTextPassword* getTextPassword, 
                          bool& passwordIsDefined);
                                
private:

    /** 检查签名
    @param [in] p 数据
    @return 是返回true，否则返回false
    */
    static bool TestSignatureCandidate(const byte_t* p) ;

private:

    /** 输入流
    */
    IInStreamPtr m_stream;

    /** 数据
    */
    RCVector<RC7zInBytePtr> m_inByteVector;

    /** 数据
    */
    RC7zInBytePtr m_inByteBack ; 

    /** 文档起始流的位置
    */
    uint64_t m_arhiveBeginStreamPosition;

    /** 头信息
    */
    byte_t m_header[RC7zDefs::s_headerSize];

    /** 头大小
    */
    uint64_t m_headersSize;
};

END_NAMESPACE_RCZIP

#endif //__RC7zInArchive_h_
