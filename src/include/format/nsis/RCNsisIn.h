/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCNsisIn_h_
#define __RCNsisIn_h_ 1

#include "interface/ICoder.h"
#include "interface/IStream.h"
#include "common/RCBuffer.h"
#include "common/RCVector.h"
#include "RCNsisDecode.h"
#include "RCNsisItem.h"

BEGIN_NAMESPACE_RCZIP

class RCNsisIn
{
public:

    /** 打开文档
    @param [in] codecsInfo 编码解码器指针
    @param [in] inStream 输入流
    @param [in] maxCheckStartPosition 查找文件格式签名的最大偏移位置
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult Open(ICompressCodecsInfo* codecsInfo, IInStream* inStream, const uint64_t* maxCheckStartPosition);

    /** 清除
    */
    void Clear();

    /** 获取偏移
    @return 返回偏移
    */
    uint32_t GetOffset() const
    {
        return m_isSolid ? 4 : 0;
    }

    /** 获取数据位置
    @param [in] index 索引
    @return 返回数据位置
    */
    uint64_t GetDataPos(int index)
    {
        const RCNsisItem& item = m_items[index];
        return GetOffset() + m_firstHeader.m_headerLength + item.m_pos;
    }

    /**  获取固实项的位置
    @param [in] index 索引
    @return 返回固实项的位置
    */
    uint64_t GetPosOfSolidItem(int index) const
    {
        const RCNsisItem& item = m_items[index];
        return 4 + m_firstHeader.m_headerLength + item.m_pos;
    }

    /** 获取无固实项的位置
    @param [in] index 索引
    @return 返回无固实项的位置
    */
    uint64_t GetPosOfNonSolidItem(int index) const
    {
        const RCNsisItem& item = m_items[index];
        return m_streamOffset + m_nonSolidStartOffset + 4  + item.m_pos;
    }

    /** 释放
    */
    void Release()
    {
        m_decoder.Release();
    }

private:

    /** 读取字节
    @return 读取字节值
    */
    byte_t ReadByte();

    /** 读取无符号整数
    @param [out] v 返回值
    @return 成功返回true,否则返回false
    */
    uint32_t ReadUInt32();

    /** 打开文档
    @param [in] codecsInfo 编码解码器指针
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult Open2(ICompressCodecsInfo* codecsInfo);

    /** 读取块的头部
    @param [out] bh 返回块的头部数据
    */
    void ReadBlockHeader(RCNsisBlockHeader& bh);

    /** 读取ascii字符串
    @param [in] pos 偏移
    @return 返回ascii字符串
    */
    RCStringA ReadStringA(uint32_t pos) const;

    /** 读取unicode字符串
    @param [in] pos 偏移
    @return 返回unicode字符串
    */
    RCString ReadStringU(uint32_t pos) const;

    /** 读取字符串
    @param [in] pos 偏移
    @return 返回字符串
    */
    RCStringA ReadString2A(uint32_t pos) const;

    /** 读取字符串
    @param [in] pos 偏移
    @return 返回字符串
    */
    RCString ReadString2U(uint32_t pos) const;

    /** 读取字符串
    @param [in] pos 偏移
    @return 返回字符串
    */
    RCStringA ReadString2(uint32_t pos) const;

    /** 读取字符串
    @param [in] pos 偏移
    @return 返回字符串
    */
    RCStringA ReadString2Qw(uint32_t pos) const;

    /** 读取入口
    @param [in] bh 块的头部数据
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult ReadEntries(const RCNsisBlockHeader& bh);

    /** 解析
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult Parse();

public:

    /** 流偏移
    */
    uint64_t m_streamOffset;

    /** nsid解码器
    */
    RCNsisDecode m_decoder;

    /** nsis项
    */
    RCVector<RCNsisItem> m_items;

    /** 第一个头部
    */
    RCNsisFirstHeader m_firstHeader;

    /** 压缩方法
    */
    RCNsisMethodType::EEnum m_method;

    /** 字典大小
    */
    uint32_t m_dictionarySize;

    /** 是否是固实
    */
    bool m_isSolid;

    /** 是否用过滤
    */
    bool m_useFilter;

    /** 过滤标志 
    */
    bool m_filterFlag;

    /** 是否是unicode
    */
    bool m_isUnicode;

#ifdef NSIS_SCRIPT

    /** 脚本
    */
    RCStringA m_script;

#endif

private:

    /** 文档大小
    */
    uint64_t m_archiveSize;

    /** 输入流
    */
    IInStreamPtr m_stream;

    /** 数据
    */
    RCByteBuffer m_data;

    /** 大小
    */
    uint64_t m_size;

    /** 数据位置
    */
    size_t m_posInData;

    /** 字符位置
    */
    uint32_t m_stringsPos;

    /** 头部是否压缩
    */
    bool m_headerIsCompressed;

    /**  无固实起始偏移
    */
    uint32_t m_nonSolidStartOffset;
};

END_NAMESPACE_RCZIP

#endif //__RCNsisIn_h_
