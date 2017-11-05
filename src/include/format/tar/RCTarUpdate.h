/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCTarUpdate_h_
#define __RCTarUpdate_h_ 1

#include "base/RCString.h"
#include "interface/IStream.h"
#include "interface/IArchive.h"
#include "interface/ICoder.h"
#include "RCTarItem.h"

BEGIN_NAMESPACE_RCZIP

/** Tar 更新
*/
class RCTarUpdate
{
public:

    /** Tar 更新项
    */
    struct RCTarUpdateItem
    {
        /** 包内编号
        */
        int32_t m_indexInArchive;

        /** 客户端编号
        */
        int32_t m_indexInClient;

        /** 时间
        */
        uint32_t m_time;

        /** 模式
        */
        uint32_t m_mode;

        /** 大小
        */
        uint64_t m_size;

        /** 名称
        */
        RCStringA m_name;

        /** 用户
        */
        RCStringA m_user;

        /** 用户组
        */
        RCStringA m_group;

        /** 是否新数据
        */
        bool m_newData;

        /** 是否新属性
        */
        bool m_newProps;

        /** 是否位目录
        */
        bool m_isDir;
    };

public:

    /** 更新的文档
    @param [in] inStream 输入流
    @param [in] outStream 输出流
    @param [in] inputItems 输入项
    @param [in] updateItems 更新项
    @param [in] updateCallback 更新回调接口
    @param [in] compressCodecsInfo 编码管理器
    @return 成功返回RC_S_OK，否则返回错误号
    */
    static HResult UpdateArchive(IInStream *inStream,
                                 ISequentialOutStream *outStream,
                                 const std::vector<RCTarItemEx> &inputItems,
                                 const std::vector<RCTarUpdateItem> &updateItems,
                                 IArchiveUpdateCallback *updateCallback,
                                 ICompressCodecsInfoPtr compressCodecsInfo);
};

END_NAMESPACE_RCZIP

#endif //__RCTarUpdate_h_
