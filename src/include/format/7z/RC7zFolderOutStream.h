/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RC7zFolderOutStream_h_
#define __RC7zFolderOutStream_h_ 1

#include "base/RCNonCopyable.h"
#include "common/RCVector.h"
#include "interface/IArchive.h"
#include "interface/IStream.h"
#include "interface/IUnknownImpl.h"

BEGIN_NAMESPACE_RCZIP

class RC7zArchiveDatabaseEx ;
class RCOutStreamWithCRC ;

class RC7zFolderOutStream:
    public IUnknownImpl<ISequentialOutStream>
{
public:

    /** 默认构造函数
    */
    RC7zFolderOutStream() ;
    
    /** 默认析构函数
    */
    ~RC7zFolderOutStream() ;

public:

    /** 初始化
    @param [in] archiveDatabase 文档基本数据
    @param [in] ref2Offset 偏移
    @param [in] startIndex 起始索引
    @param [in] extractStatuses 解压状态
    @param [in] extractCallback 解压管理器回调接口
    @param [in] testMode 是否是测试模式
    @param [in] checkCrc 是否需要检查CRC
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult Init(const RC7zArchiveDatabaseEx* archiveDatabase,
                 uint32_t ref2Offset,
                 uint32_t startIndex,
                 const RCBoolVector* extractStatuses,
                 IArchiveExtractCallback* extractCallback,
                 bool testMode,
                 bool checkCrc) ;

    /** flush损坏的数据
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult FlushCorrupted(int32_t resultEOperationResult) ;

    /** 是否等待完成
    @return 是返回RC_S_OK，否则返回RC_E_FAIL
    */
    HResult WasWritingFinished() const ;

    /** 写数据  
    @param [in] data 存放数据的缓冲区
    @param [in] size 缓冲区大小
    @param [out] processedSize 实际读取的大小
    @return 是返回RC_S_OK，否则返回错误号
    */
    virtual HResult Write(const void* data, uint32_t size, uint32_t* processedSize) ;

private:

    /** 打开文件
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult OpenFile() ;

    /** 关闭文件并设置结果
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult CloseFileAndSetResult(uint32_t index, int32_t res) ;

    /** 关闭文件并设置结果
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult CloseFileAndSetResult() ;

    /** 处理空文件
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult ProcessEmptyFiles() ;
    
private:

    /** 带CRC的输出流
    */
    RCOutStreamWithCRC* m_crcStreamSpec ;

    /** CRC输出流
    */
    ISequentialOutStreamPtr m_crcStream ;

    /** 基本数据
    */
    const RC7zArchiveDatabaseEx* m_db ;

    /** 解压状态
    */
    const RCBoolVector* m_extractStatuses ;

    /** 起始索引
    */
    uint32_t m_startIndex ;

    /** 偏移
    */
    uint32_t m_ref2Offset ;

    /** 当前索引
    */
    int32_t m_currentIndex ;

    /** 解压管理器回调接口
    */
    IArchiveExtractCallbackPtr m_extractCallback ;

    /** 是否是测试模式
    */
    bool m_testMode;

    /** 文件是否打开
    */
    bool m_fileIsOpen ;

    /** 是否检查CRC
    */
    bool m_checkCrc ;

    /** 大小
    */
    uint64_t m_rem ;
};

END_NAMESPACE_RCZIP

#endif //__RC7zFolderOutStream_h_
