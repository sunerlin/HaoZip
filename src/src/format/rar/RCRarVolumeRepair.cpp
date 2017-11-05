/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "format/rar/RCRarVolumeRepair.h"
#include "filesystem/RCFileTime.h"
#include "common/RCStringConvert.h"
#include "common/RCStringUtil.h"
#include "format/common/RCItemNameUtils.h"
#include "format/common/RCOutStreamWithCRC.h"
#include "common/RCCreateCoder.h"
#include "common/RCFilterCoder.h"
#include "common/RCLocalProgress.h"
#include "interface/RCMethodDefs.h"
#include "crypto/Rar20/RCRar20Decoder.h"
#include "crypto/WzAES/RCWzAESDecoder.h"
#include "format/common/RCPropData.h"
#include "format/rar/RCRarGetComment.h"
#include "format/rar/RCRarFileInfoParse.h"
#include "filesystem/RCFindFile.h"
#include "filesystem/RCFileInfo.h"
#include "filesystem/RCFileSystemUtils.h"
#include "thread/RCThread.h"
#include "thread/RCSynchronization.h"
#include "locked/RCMtExtractProgressMixer.h"
#include "locked/RCMtExtractProgress.h"
#include "locked/RCMtLocalProgress.h"
#include "interface/RCArchiveType.h"

/////////////////////////////////////////////////////////////////
//RCRarVolumeRepair class implementation

BEGIN_NAMESPACE_RCZIP

void RCRarVolumeRepair::Push(RCString filename, unsigned short volumeIndex, bool isFirst, bool isEnd)
{
    RCArchiveVolumeItem item;
    item.m_filename = filename;
    item.m_volumeIndex = volumeIndex;
    item.m_isFirst = isFirst;
    item.m_isEnd = isEnd;

    m_items.push_back(item);
}

bool RCRarVolumeRepair::Repair(RCVector<RCString>& outputFilenames, RCString& volumeFirst, bool silent, IArchiveOpenCallbackPtr openArchiveCallbackWrap)
{
    /** 自动检测混乱的分卷

        1 看序号能否组成首位相连的无冗余序号链条，并且首末序号链条正确
        2 如果2不满足，则看能否组成首位相连的有冗余序号链条
        3 如果3满足，则弹出对话框供选择。
    */
    if (m_items.size() == 0)
    {
        return false;
    }

    sort(m_items.begin(), m_items.end());

    // 能否组成首位相连的无冗余序号链条
    for (uint32_t i = 0; i < m_items.size(); i++)
    {
        if (m_items[i].m_volumeIndex == i)
        {
            if (i == 0)
            {
                if (!m_items[i].m_isFirst)
                {
                    break;
                }
                else if (m_items[i].m_isEnd)
                {
                    // 有且只有一个分卷
                    volumeFirst = m_items[i].m_filename;
                    return true;
                }
            }
            else if (m_items[i].m_isEnd)
            {
                // 多个分卷
                volumeFirst = m_items[0].m_filename;
                for (uint32_t j = 1; j < m_items.size(); j++)
                {
                    outputFilenames.push_back(m_items[j].m_filename);
                }
                return true;
            }
        }
        else
        {
            break;
        }
    }

    // 能否组成首位相连的有冗余序号链条
    bool find = false;

    // 加1等于已经寻找到的分卷的最后序号。
    uint32_t chainCurrent = 0;
    for (uint32_t i = 0; i < m_items.size(); i++)
    {
        if (static_cast<uint32_t>(m_items[i].m_volumeIndex + 1) == chainCurrent)
        {
            // 当前分卷号等于已经寻找到的分卷的最后序号，则判断是否是最后卷，如果是最后卷则找到了。

            if (i == 0 && (!m_items[i].m_isFirst))
            {
                continue;
            }

            if (m_items[i].m_isEnd)
            {
                find = true;
                break;
            }
        }
        else if (static_cast<uint32_t>(m_items[i].m_volumeIndex + 1) == (chainCurrent + 1))
        {
            // 当前分卷号等于已经寻找到的分卷的最后序号的下一分卷，则已经寻找到的分卷的最后序号增加。

            if (m_items[i].m_isEnd)
            {
                find = true;
                break;
            }

            chainCurrent++;
        }
        else
        {
            // 有缺失
            break;
        }
    }

    if (silent || (!find) || (!openArchiveCallbackWrap))
    {
        return false;
    }

    //弹出对话框
    if (openArchiveCallbackWrap->VolumeRepair(m_items, volumeFirst, outputFilenames) == RC_S_OK)
    {
        return true;
    }

    outputFilenames.clear();
    return false;
}

END_NAMESPACE_RCZIP
