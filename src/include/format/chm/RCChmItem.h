/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCChmItem_h_
#define __RCChmItem_h_ 1

#include "base/RCString.h"
#include "common/RCVector.h"
#include "common/RCBuffer.h"
#include "archive/common/RCGuidDefs.h"

BEGIN_NAMESPACE_RCZIP

struct RCChmItem
{
    /** 段
    */
    uint64_t m_section;

    /** 偏移
    */
    uint64_t m_offset;

    /** 大小
    */
    uint64_t m_size;

    /** 名字
    */
    RCStringA m_name;

    /** 是否是相关项目
    @return 是返回true，否则返回false
    */
    bool IsFormatRelatedItem() const
    {
        if (m_name.length() < 2)
        {
            return false;
        }
        return m_name[0] == ':' && m_name[1] == ':';
    }

    /** 是否是用户项
    @return 是返回true，否则返回false
    */
    bool IsUserItem() const
    {
        if (m_name.length() < 2)
        {
            return false;
        }
        return m_name[0] == '/';
    }

    /** 是否是目录
    @return 是返回true，否则返回false
    */
    bool IsDir() const
    {
        if (m_name.length() == 0)
        {
            return false;
        }
        return (m_name[m_name.length() - 1] == '/');
    }
};

struct RCChmDatabase
{
    /** 内容偏移
    */
    uint64_t m_contentOffset;

    /** chm项
    */
    RCVector<RCChmItem> m_items;

    /** 新格式字符串
    */
    RCStringA m_newFormatString;

    /** 帮助格式
    */
    bool m_help2Format;

    /** 新格式
    */
    bool m_newFormat;

    /** 查找项
    @param [in] name 项名字
    @return 返回项索引
    */
    int32_t FindItem(const RCStringA& name) const
    {
        for (int32_t i = 0; i <(int32_t)m_items.size(); i++)
        {
            if (m_items[i].m_name == name)
            {
                return i;
            }
        }
        return -1;
    }

    /** 清除
    */
    void Clear()
    {
        m_newFormat = false;
        m_newFormatString.clear();
        m_help2Format = false;
        m_items.clear();
    }
};

struct RCChmResetTable
{
    /** 解压大小
    */
    uint64_t m_uncompressedSize;

    /** 压缩大小
    */
    uint64_t m_compressedSize;

    /** 块大小
    */
    uint64_t m_blockSize;

    /** 重置偏移
    */
    RCVector<uint64_t> m_resetOffsets;

    /** 获取块中的压缩大小
    @param [in] blockIndex 块索引 
    @param [in] numBlocks 快个数
    @param [out] size 返回大小
    @return 成功获取返回true，否则返回false
    */
    bool GetCompressedSizeOfBlocks(uint64_t blockIndex, uint32_t numBlocks, uint64_t &size) const
    {
        if (blockIndex >= m_resetOffsets.size())
        {
            return false;
        }

        uint64_t startPos = m_resetOffsets[(int32_t)blockIndex];
        if (blockIndex + numBlocks >= m_resetOffsets.size())
        {
            size = m_compressedSize - startPos;
        }
        else
        {
            size = m_resetOffsets[(int32_t)(blockIndex + numBlocks)] - startPos;
        }
        return true;
    }

    /** 获取块中的压缩大小
    @param [in] blockIndex 块索引 
    @param [out] size 返回大小
    @return 成功获取返回true，否则返回false
    */
    bool GetCompressedSizeOfBlock(uint64_t blockIndex, uint64_t& size) const
    {
        return GetCompressedSizeOfBlocks(blockIndex, 1, size);
    }

    /** 获取块的个数
    @param [in] size 大小
    @return 返回块个数
    */
    uint64_t GetNumBlocks(uint64_t size) const
    {
        return (size + m_blockSize - 1) / m_blockSize;
    }
};

struct RCChmLzxInfo
{
    /** 版本
    */
    uint32_t m_version;

    /** 重置间隔
    */
    uint32_t m_resetInterval;

    /** 窗口大小
    */
    uint32_t m_windowSize;

    /** cache大小
    */
    uint32_t m_cacheSize;

    /** 重置表
    */
    RCChmResetTable m_resetTable;

    /** 获取字典位数
    @return 返回字典位数
    */
    uint32_t GetNumDictBits() const
    {
        if (m_version == 2 || m_version == 3)
        {
            for (int32_t i = 0; i <= 31; i++)
            {
                if (((uint32_t)1 << i) >= m_windowSize)
                {
                    return 15 + i;
                }
            }
        }
        return 0;
    }

    /** 获取文件夹大小
    @return 返回文件夹大小
    */
    uint64_t GetFolderSize() const
    { 
        return m_resetTable.m_blockSize * m_resetInterval; 
    };

    /** 获取文件夹个数
    @param [in] 文件夹偏移
    @return 返回文件夹个数
    */
    uint64_t GetFolder(uint64_t offset) const
    { 
        return offset / GetFolderSize();
    };

    /** 获取文件夹偏移
    @param [in] folderIndex 文件夹索引
    @return 返回文件夹偏移
    */
    uint64_t GetFolderPos(uint64_t folderIndex) const
    {
        return folderIndex * GetFolderSize();
    };

    /** 从文件夹中的索引获取块索引
    @param [in] folderIndex 文件夹索引
    @return 返回块索引
    */
    uint64_t GetBlockIndexFromFolderIndex(uint64_t folderIndex) const
    {
        return folderIndex * m_resetInterval;
    };

    /** 获取文件夹中的偏移
    @param [in] folderIndex 文件夹索引
    @param [out] offset 返回偏移
    @return 成功返回true，否则返回false
    */
    bool GetOffsetOfFolder(uint64_t folderIndex, uint64_t& offset) const
    {
        uint64_t blockIndex = GetBlockIndexFromFolderIndex(folderIndex);
        if (blockIndex >= m_resetTable.m_resetOffsets.size())
        {
            return false;
        }
        offset = m_resetTable.m_resetOffsets[(int32_t)blockIndex];
        return true;
    }

    /** 获取文件中的压缩大小
    @param [in] folderIndex 文件夹索引
    @param [out] size 返回大小
    @return 成功返回true，否则返回false
    */
    bool GetCompressedSizeOfFolder(uint64_t folderIndex, uint64_t& size) const
    {
        uint64_t blockIndex = GetBlockIndexFromFolderIndex(folderIndex);
        return m_resetTable.GetCompressedSizeOfBlocks(blockIndex, m_resetInterval, size);
    }
};

struct RCChmMethodInfo
{
    /** GUID
    */
    RC_GUID m_guid;

    /** 控制数据
    */
    RCByteBuffer m_controlData;

    /** lzx信息
    */
    RCChmLzxInfo m_lzxInfo;

    /** 是否是lzx
    @return 是返回true，否则返回false
    */
    bool IsLzx() const;

    /** 是否是des
    @return 是返回true，否则返回false
    */
    bool IsDes() const;

    /** 获取GUID字符
    @return 返回GUID字符
    */
    RCStringA GetGuidString() const;

    /** 获取名字
    @return 返回名字
    */
    RCString GetName() const;
};

struct RCChmSectionInfo
{
    /** 偏移
    */
    uint64_t m_offset;

    /** 压缩大小
    */
    uint64_t m_compressedSize;

    /** 解压大小
    */
    uint64_t m_uncompressedSize;

    /** 名字
    */
    RCStringA m_name;

    /** 方法
    */
    RCVector<RCChmMethodInfo> m_methods;

    /** 是否是lzx
    @return 是返回true，否则返回false
    */
    bool IsLzx() const;

    /** 获取方法名
    @return 返回方法名
    */
    RCString GetMethodName() const;
};

class RCChmFilesDatabase: public RCChmDatabase
{
public:

    /** 获取文件大小
    @param [in] 文件索引
    @return 文件大小
    */
    uint64_t GetFileSize(int32_t fileIndex) const
    { 
        return m_items[m_indices[fileIndex]].m_size;
    }

    /** 获取文件偏移
    @param [in] 文件索引
    @return 文件偏移
    */
    uint64_t GetFileOffset(int32_t fileIndex) const
    { 
        return m_items[m_indices[fileIndex]].m_offset;
    }

    /** 获取文件夹个数
    @param [in] fileIndex 文件索引
    @return 返回文件夹个数
    */
    uint64_t GetFolder(int32_t fileIndex) const
    {
        const RCChmItem& item = m_items[m_indices[fileIndex]];
        const RCChmSectionInfo& section = m_sections[(int32_t)item.m_section];
        if (section.IsLzx())
        {
            return section.m_methods[0].m_lzxInfo.GetFolder(item.m_offset);
        }
        return 0;
    }

    /** 获取最后一个文件夹个数
    @param [in] fileIndex 文件索引
    @return 返回文件夹个数
    */
    uint64_t GetLastFolder(int32_t fileIndex) const
    {
        const RCChmItem& item = m_items[m_indices[fileIndex]];
        const RCChmSectionInfo& section = m_sections[(int32_t)item.m_section];
        if (section.IsLzx())
        {
            return section.m_methods[0].m_lzxInfo.GetFolder(item.m_offset + item.m_size - 1);
        }
        return 0;
    }

    /** 高等级清除
    */
    void HighLevelClear()
    {
        m_lowLevel = true;
        m_indices.clear();
        m_sections.clear();
    }

    /** 清除
    */
    void Clear()
    {
        RCChmDatabase::Clear();
        HighLevelClear();
    }

    /** 设置索引序列
    */
    void SetIndices();

    /** 排序
    */
    void Sort();

    /** 检查
    @return 通过检查返回true，否则返回false
    */
    bool Check();

public:

    /** 低等
    */
    bool m_lowLevel;

    /** 索引序列
    */
    RCVector<int32_t> m_indices;

    /** 段信息
    */
    RCVector<RCChmSectionInfo> m_sections;
};

END_NAMESPACE_RCZIP

#endif //__RCChmItem_h_
