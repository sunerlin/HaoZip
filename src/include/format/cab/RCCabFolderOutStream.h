/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCCabFolderOutStream_h_
#define __RCCabFolderOutStream_h_ 1

#include "interface/IStream.h"
#include "interface/IArchive.h"
#include "interface/IUnknownImpl.h"
#include "common/RCBuffer.h"
#include "format/cab/RCCabIn.h"

BEGIN_NAMESPACE_RCZIP

class RCCabFolderOutStream :
    public IUnknownImpl<ISequentialOutStream>
{
public:

    /** 写数据  
    @param [in] data 存放数据的缓冲区
    @param [in] size 缓冲区大小
    @param [out] processedSize 实际读取的大小
    @return 成功返回RC_S_OK，否则返回错误号
    */
    virtual HResult Write(const void* data, uint32_t size, uint32_t* processedSize);

public:

    /** 写空文件
    */
    HResult WriteEmptyFiles();

    /** 初始化  
    @param [in] database cab基本数据
    @param [in] extractStatuses 缓冲区大小
    @param [in] startIndex 起始索引
    @param [in] folderSize 文件夹大小
    @param [in] extractCallback 解压回调接口指针
    @param [in] testMode 是否是测试模式
    */
    void Init(const RCCabMvDatabaseEx* database, 
              const RCVector<bool>* extractStatuses, 
              int32_t startIndex,
              uint64_t folderSize,
              IArchiveExtractCallback *extractCallback,
              bool testMode);

    /** flush损坏数据
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult FlushCorrupted();

    /** 是否支持
    @return 是返回RC_S_OK，否则返回错误号
    */
    HResult Unsupported();

    /** 获取剩余大小
    @return 返回剩余大小
    */
    uint64_t GetRemain() const ;

    /** 获取文件夹中的偏移位置
    @return 偏移位置
    */
    uint64_t GetPosInFolder() const ;

private:

    /** 打开文件
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult OpenFile();

    /** 关闭文件
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult CloseFile() ;

    /** 写文件
    @param [in] data 存放数据的缓冲区
    @param [in] size 缓冲区大小
    @param [in] processedSize 实际写入的大小
    @param [in] isOK 是否ok
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult Write2(const void* data, uint32_t size, uint32_t* processedSize, bool isOK);

private:

    /** cab基本数据
    */
    const RCCabMvDatabaseEx* m_database;

    /** 解压状态
    */
    const RCVector<bool>* m_extractStatuses;

    /** 起始索引
    */
    int32_t m_startIndex;

    /** 当前索引
    */
    int32_t m_currentIndex;

    /** 解压回调接口指针
    */
    IArchiveExtractCallbackPtr m_extractCallback;

    /** 是否是测试模式
    */
    bool m_testMode;

    /** 真实输出流
    */
    ISequentialOutStreamPtr m_realOutStream;

    /** 是否ok
    */
    bool m_isOk;

    /** 文件是否被打开
    */
    bool m_fileIsOpen;

    /** 剩余文件大小
    */
    uint32_t m_remainFileSize;

    /** 文件夹大小
    */
    uint64_t m_folderSize;

    /** 文件中的偏移位置
    */
    uint64_t m_posInFolder;

    /** 数据缓冲区
    */
    RCByteBuffer m_tempBuf;

    /** 是否是临时缓冲区模式
    */
    bool m_tempBufMode;

    /** 是否支持
    */
    bool m_isSupported;

    /** 文件中数据起始偏移
    */
    uint32_t m_bufStartFolderOffset;
};

END_NAMESPACE_RCZIP

#endif //__RCCabFolderOutStream_h_