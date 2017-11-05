/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCCabItem_h_
#define __RCCabItem_h_ 1

#include "base/RCTypes.h"
#include "base/RCDefs.h"
#include "base/RCString.h"
#include "format/cab/RCCabHeader.h"
#ifdef RCZIP_OS_WIN
    #include "base/RCWindowsDefs.h"
#endif

BEGIN_NAMESPACE_RCZIP

struct RCCabFolder
{
    /** 数据起始位置
    */
    uint32_t m_dataStart; 

    /** 数据块哥数据
    */
    uint16_t m_numDataBlocks;

    /** 压缩类型的主版本
    */
    byte_t m_compressionTypeMajor;

    /** 压缩类型的次版本
    */
    byte_t m_compressionTypeMinor;

    /** 获取压缩类型
    @return [out] 返回压缩类型
    */
    byte_t GetCompressionMethod() const ;
};

struct RCCabItem
{
    /** 名字
    */
    RCStringA m_name;

    /** 偏移
    */
    uint32_t m_offset;

    /** 大小
    */
    uint32_t m_size;

    /** 时间
    */
    uint32_t m_time;

    /** 文件夹索引
    */
    uint16_t m_folderIndex;

    /** 标志
    */
    uint16_t m_flags;

    /** 属性
    */
    uint16_t m_attributes;

    /** 获取结束偏移
    @return 返回结束偏移
    */
    uint64_t GetEndOffset() const ;

    /** 获取windows的属性
    @return 返回windows的属性
    */
    uint32_t GetWinAttributes() const ;

    /** 名字是否是UTF
    @return 是返回true，否则返回false
    */
    bool IsNameUTF() const ;

    /** 是否是目录
    @return 是返回true，否则返回false
    */
    bool IsDir() const ;

    /** 是否从前一个继续
    @return 是返回true，否则返回false
    */
    bool ContinuedFromPrev() const ;

    /** 是否从下一个继续
    @return 是返回true，否则返回false
    */
    bool ContinuedToNext() const ;

    /** 获取文件夹索引
    @param [in] numFolders 文件夹数
    @return 返回文件夹索引
    */
    int32_t GetFolderIndex(int32_t numFolders) const;
};

END_NAMESPACE_RCZIP

#endif //__RCCabItem_h_