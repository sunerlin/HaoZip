/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RC7zFolderInStream_h_
#define __RC7zFolderInStream_h_ 1

#include "interface/IArchive.h"
#include "interface/IStream.h"
#include "interface/ICoder.h"
#include "interface/IUnknownImpl.h"
#include "common/RCVector.h"

BEGIN_NAMESPACE_RCZIP

class RCSequentialInStreamWithCRC ;

class RC7zFolderInStream:
    public IUnknownImpl2<ISequentialInStream,
                         ICompressGetSubStreamSize
                        >
{
public:

    /** 默认构造函数
    */
    RC7zFolderInStream() ;
    
    /** 默认析构函数
    */
    ~RC7zFolderInStream() ;
    
public:

    /** 初始化
    @param [in] updateCallback
    @param [in] fileIndices 文件索引队列
    @param [in] numFiles 文件个数
    */
    void Init(IArchiveUpdateCallback* updateCallback,
              const uint32_t* fileIndices, 
              uint32_t numFiles);

    /** 获取所有大小
    @return 返回大小
    */
    uint64_t GetFullSize() const ;
    
    /** 读取数据 
    @param [out] data 存放数据的缓冲区
    @param [in] size 缓冲区大小
    @param [out] processedSize 实际读取的大小
    @return 成功返回RC_S_OK，否则返回错误号
    */
    virtual HResult Read(void* data, uint32_t size, uint32_t* processedSize) ;
    
    /** 获取流大小
    @param [in] subStream 流的下标
    @param [out] size 流的大小
    @return 成功返回RC_S_OK，否则返回错误号
    */
    virtual HResult GetSubStreamSize(uint64_t subStream, uint64_t& size) ;

public:

    /** 已处理
    */
    RCBoolVector m_processed ;

    /** crc
    */
    RCVector<uint32_t> m_crcs ;

    /** 大小
    */
    RCVector<uint64_t> m_sizes ;
    
private:

    /** 打开流
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult OpenStream() ;

    /** 关闭流
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult CloseStream();

    /** 增加crc
    */
    void AddDigest();
    
private:

    /** crc的输入流
    */
    RCSequentialInStreamWithCRC* m_inStreamWithHashSpec ;

    /** hash的输入流
    */
    ISequentialInStreamPtr m_inStreamWithHash;

    /** 更新管理器回调接口
    */
    IArchiveUpdateCallbackPtr m_updateCallback;

    /** 是否有当前大小
    */
    bool m_currentSizeIsDefined ;

    /** 当前大小
    */
    uint64_t m_currentSize ;

    /** 文件是否被打开
    */
    bool m_fileIsOpen;

    /** 文件指针位置
    */
    uint64_t m_filePos;

    /** 文件索引队列
    */
    const uint32_t* m_fileIndices;

    /** 文件数
    */
    uint32_t m_numFiles;

    /** 文件索引
    */
    uint32_t m_fileIndex;
};

END_NAMESPACE_RCZIP

#endif //__RC7zFolderInStream_h_
