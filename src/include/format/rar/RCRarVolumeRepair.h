/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCRarVolumeRepair_h_
#define __RCRarVolumeRepair_h_ 1

#include "interface/IArchive.h"
#include "interface/ICoder.h"
#include "interface/IUnknownImpl.h"

BEGIN_NAMESPACE_RCZIP

class RCRarVolumeRepair
{
public:

    /** 增加原始记录
    @param [in] filename 文件名 
    @param [in] volumeIndex 分卷号 
    @param [in] isFirst 是否首个分卷 
    @param [in] isEnd 是否最后分卷
    */
    void Push(RCString filename, unsigned short volumeIndex, bool isFirst, bool isEnd);

    /** 修复
    @param [out] outputFilenames 修复后的分卷序列
    @param [in] volumeFirst 第一个分卷路径
    @param [in] silent 是否静默
    @param [in] openArchiveCallbackWrap 界面回调
    @return 成功返回true，否则返回false
    */
    bool Repair(RCVector<RCString>& outputFilenames, RCString& volumeFirst, bool silent, IArchiveOpenCallbackPtr openArchiveCallbackWrap);

private:

    /** 项
    */
    RCVector<RCArchiveVolumeItem> m_items;
};

END_NAMESPACE_RCZIP

#endif //RCRarVolumeRepair_h_
