/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCZipUpdate_h_
#define __RCZipUpdate_h_ 1

#include "base/RCDefs.h"
#include "base/RCString.h"
#include "base/RCSmartPtr.h"
#include "filesystem/RCFileDefs.h"
#include "interface/ICoder.h"
#include "interface/IArchive.h"
#include "RCZipItem.h"
#include "RCZipIn.h"
#include "RCZipCompressionMethodMode.h"

BEGIN_NAMESPACE_RCZIP

class RCZipUpdate
{
public:

    struct RCZipUpdateRange
    {
        /** 位置
        */
        uint64_t m_position;

        /** 大小
        */
        uint64_t m_size;

        /** 默认构造函数
        */
        RCZipUpdateRange():
            m_position(0),
            m_size(0)
        {
        };

        /** 构造函数
        */
        RCZipUpdateRange(uint64_t position, uint64_t size): 
            m_position(position), 
            m_size(size)
        {
        };
    };

    struct RCZipUpdateItem
    {
        /** 是否是新数据
        */
        bool m_newData;

        /** 是否是新属性
        */
        bool m_newProperties;

        /** 是否是目录
        */
        bool m_isDir;

        /** 是否定义ntfs时间
        */
        bool m_ntfsTimeIsDefined;

        /** 是否UTF8
        */
        bool m_isUtf8;

        /** 在archive中的索引
        */
        int32_t m_indexInArchive;

        /** 在client中的索引
        */
        int32_t m_indexInClient;

        /** 属性
        */
        uint32_t m_attributes;

        /** 时间
        */
        uint32_t m_time;

        /** 大小
        */
        uint64_t m_size;

        /** 名字
        */
        RCStringA m_name;

        /** ntfs修改时间
        */
        RC_FILE_TIME m_ntfsMTime;

        /** ntfs访问时间
        */
        RC_FILE_TIME m_ntfsATime;

        /** ntfs创建时间
        */
        RC_FILE_TIME m_ntfsCTime;

        /** 默认构造函数
        */
        RCZipUpdateItem(): 
            m_newData(false),
            m_newProperties(false),
            m_isDir(false),
            m_ntfsTimeIsDefined(false), 
            m_isUtf8(false), 
            m_indexInArchive(0),
            m_indexInClient(0),
            m_attributes(0),
            m_time(0),
            m_size(0)
        {
        }
    };

    /** RCZipUpdateItem智能指针
    */
    typedef RCSharedPtr<RCZipUpdateItem>  RCZipUpdateItemPtr ;

public:

    /** 压缩数据
    @param [in] codecsInfo 编码管理器
    @param [in] inputItems 输入items信息
    @param [in] updateItems 更新items信息
    @param [in] seqOutStream 输出流
    @param [in] inArchive 输入文档
    @param [in] compressionMethodMode 压缩模式
    @param [in] updateCallback 更新管理器回调接口
    @return 成功返回RC_S_OK，否则返回错误号
    */
    static HResult UpdateArchive(ICompressCodecsInfo* codecsInfo,
                                 const RCVector<RCZipItemExPtr>& inputItems,
                                 const RCVector<RCZipUpdateItemPtr>& updateItems,
                                 ISequentialOutStream* seqOutStream,
                                 RCZipIn* inArchive,
                                 RCZipCompressionMethodMode* compressionMethodMode,
                                 IArchiveUpdateCallback* updateCallback);
};

END_NAMESPACE_RCZIP

#endif //__RCZipUpdate_h_
