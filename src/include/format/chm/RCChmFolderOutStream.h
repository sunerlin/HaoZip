/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCChmFolderOutStream_h_
#define __RCChmFolderOutStream_h_ 1

#include "common/RCVector.h"
#include "interface/IArchive.h"
#include "interface/IStream.h"
#include "interface/IUnknownImpl.h"
#include "RCChmItem.h"

BEGIN_NAMESPACE_RCZIP

class RCChmFolderOutStream:
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

    /** 写数据
    @param [in] data 存放数据的缓冲区
    @param [in] size 缓冲区大小
    @param [out] processedSize 实际读取的大小
    @param [in] isOK 是否ok
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult Write2(const void* data, uint32_t size, uint32_t* processedSize, bool isOK);

    /** 初始化
    @param [in] database chm基本数据
    @param [in] extractCallback 解压回调接口指针
    @param [in] testMode 是否是测试模式
    @return 成功返回RC_S_OK，否则返回错误号
    */
    void Init(const RCChmFilesDatabase* database, IArchiveExtractCallback* extractCallback, bool testMode);

    /** flush脏数据
    @param [in] maxSize 最大大小
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult FlushCorrupted(uint64_t maxSize);

private:
    
    /** 打开文件
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult OpenFile();

    /** 写空文件
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult WriteEmptyFiles();

public:

    /** 文件夹大小
    */
    uint64_t m_folderSize;

    /** 文件夹中位置
    */
    uint64_t m_posInFolder;

    /** 段中位置
    */
    uint64_t m_posInSection;

    /** 解压状态
    */
    const RCVector<bool>* m_extractStatuses;

    /** 起始索引
    */
    int32_t m_startIndex;

    /** 当前索引
    */
    int32_t m_currentIndex;

    /** 文件数
    */
    int32_t m_numFiles;

private:

    /** 基本数据 
    */
    const RCChmFilesDatabase* m_database;

    /** 解压回调接口指针
    */
    IArchiveExtractCallbackPtr m_extractCallback;

    /** 是否是测试模式
    */
    bool m_testMode;

    /** 是否ok
    */
    bool m_isOk;

    /** 文件是否打开
    */
    bool m_fileIsOpen;

    /** 剩余文件大小
    */
    uint64_t m_remainFileSize;

    /** 输出流
    */
    ISequentialOutStreamPtr m_realOutStream;
};

END_NAMESPACE_RCZIP

#endif //__RCChmFolderOutStream_h_
