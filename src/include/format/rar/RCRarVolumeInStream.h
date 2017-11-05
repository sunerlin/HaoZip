/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCRarVolumeInStream_h_
#define __RCRarVolumeInStream_h_ 1

#include "interface/IStream.h"
#include "interface/IUnknownImpl.h"
#include "common/RCVector.h"
#include "base/RCSmartPtr.h"
#include "RCRarIn.h"
#include "RCRarItem.h"

BEGIN_NAMESPACE_RCZIP

struct RCRarRefItem
{
    /** 卷索引
    */
    int32_t m_volumeIndex;

    /** 项索引
    */
    int32_t m_itemIndex;

    /** 项个数
    */
    int32_t m_numItems;
};

/** RCRarRefItem智能指针
*/
typedef RCSharedPtr<RCRarRefItem>  RCRarRefItemPtr ;

class RCRarFolderInStream:
    public IUnknownImpl<ISequentialInStream>
{

public:

    /** 读取数据
    @param [out] data 存放数据的缓冲区
    @param [in] size 缓冲区大小
    @param [out] processedSize 实际读取的大小
    @return 成功返回RC_S_OK, 失败返回错误码
    */
    virtual HResult Read(void* data, uint32_t size, uint32_t* processedSize);

    /** 初始化
    @param [in] archives 文档
    @param [in] items 项
    @param [in] refItem ref项
    */
    void Init(RCVector<RCRarIn>* archives,
              const RCVector<RCRarItemExPtr>* items,
              const RCRarRefItemPtr& refItem);

private:

    /** 打开流
    @return 成功返回RC_S_OK, 失败返回错误码
    */
    HResult OpenStream();

    /** 关闭流
    @return 成功返回RC_S_OK, 失败返回错误码
    */
    HResult CloseStream();

private:

    /** crc
    */
    RCVector<uint32_t> m_crcs;

    /** 文档
    */
    RCVector<RCRarIn>* m_archives;

    /** 项
    */
    const RCVector<RCRarItemExPtr>* m_items;

    /** ref项
    */
    RCRarRefItemPtr m_refItem;

    /** 当前索引
    */
    int32_t m_curIndex;

    /** crc
    */
    uint32_t m_crc;

    /** 文件是否被打开
    */
    bool m_fileIsOpen;

    /** 输入流
    */
    ISequentialInStreamPtr m_stream;
};

END_NAMESPACE_RCZIP

#endif //__RCRarVolumeInStream_h_
