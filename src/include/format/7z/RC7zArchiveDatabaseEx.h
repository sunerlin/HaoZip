/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RC7zArchiveDatabaseEx_h_
#define __RC7zArchiveDatabaseEx_h_ 1

#include "format/7z/RC7zItem.h"
#include "format/7z/RC7zInArchiveInfo.h"

BEGIN_NAMESPACE_RCZIP

class RC7zArchiveDatabaseEx:
    public RC7zArchiveDatabase
{
public:

    /** 默认构造函数
    */
    RC7zArchiveDatabaseEx() ;
    
    /** 默认析构函数
    */
    ~RC7zArchiveDatabaseEx() ;

public:

    /** 清理
    */
    void Clear() ;

    /** 初始文件夹起始包的流
    */
    void FillFolderStartPackStream();

    /** 初始起始位置
    */
    void FillStartPos();

    /** 初始文件夹起始文件索引
    @throws 失败抛异常
    */
    void FillFolderStartFileIndex();

    /** 初始
    */
    void Fill() ;

    /** 获取文件夹流的位置
    @param [in] folderIndex 文件夹索引
    @param [in] indexInFolder 在文件中索引
    @return 返回文件夹流的位置
    */
    uint64_t GetFolderStreamPos(int32_t folderIndex, int32_t indexInFolder) const ;

    /** 获取文件夹中所有包的大小
    @param [in] folderIndex 文件夹索引
    @return 返回文件夹中所有包的大小
    */
    uint64_t GetFolderFullPackSize(int32_t folderIndex) const ;

    /** 获取文件夹包的流大小
    @param [in] folderIndex 文件夹索引
    @param [in] streamIndex 起始索引
    @return 返回文件夹包的流大小
    */
    uint64_t GetFolderPackStreamSize(int32_t folderIndex, int32_t streamIndex) const ;

    /** 获取文件包的大小
    @param [in] fileIndex 文件索引
    @return 文件包的大小
    */
    uint64_t GetFilePackSize(RC7zNum fileIndex) const ;

public:

    /** 文档信息
    */
    RC7zInArchiveInfo m_archiveInfo;

    /** 包的流起始位置
    */
    RCVector<uint64_t> m_packStreamStartPositions;

    /** 文件夹起始包的流索引
    */
    RCVector<RC7zNum> m_folderStartPackStreamIndex;

    /** 文件夹起始文件索引
    */
    RCVector<RC7zNum> m_folderStartFileIndex;

    /** 文件索引与文件夹索引的map
    */
    RCVector<RC7zNum> m_fileIndexToFolderIndexMap;

    /** 头文件大小
    */
    uint64_t m_headersSize;

    /** 物理大小
    */
    uint64_t m_phySize;
};

END_NAMESPACE_RCZIP

#endif //__RC7zArchiveDatabaseEx_h_
