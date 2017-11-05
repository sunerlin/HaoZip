/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "format/rar/RCRarHandler.h"
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
//RCRarHandler class implementation

BEGIN_NAMESPACE_RCZIP

static const char_t* s_rarHostOS[] =
{
    _T("MS DOS"),
    _T("OS/2"),
    _T("Win32"),
    _T("Unix"),
    _T("Mac OS"),
    _T("BeOS")
};

static const int32_t s_rarNumHostOSes = sizeof(s_rarHostOS) / sizeof(s_rarHostOS[0]);

static const char_t* s_rarUnknownOS = _T("Unknown");

static RCPropData s_rarProps[] =
{
    { RCString(_T("")), RCPropID::kpidPath, RCVariantType::RC_VT_STRING},
    { RCString(_T("")), RCPropID::kpidIsDir, RCVariantType::RC_VT_BOOL},
    { RCString(_T("")), RCPropID::kpidSize, RCVariantType::RC_VT_UINT64},
    { RCString(_T("")), RCPropID::kpidPackSize, RCVariantType::RC_VT_UINT64},
    { RCString(_T("")), RCPropID::kpidMTime, RCVariantType::RC_VT_UINT64},
    { RCString(_T("")), RCPropID::kpidCTime, RCVariantType::RC_VT_UINT64},
    { RCString(_T("")), RCPropID::kpidATime, RCVariantType::RC_VT_UINT64},
    { RCString(_T("")), RCPropID::kpidAttrib, RCVariantType::RC_VT_UINT64},

    { RCString(_T("")), RCPropID::kpidEncrypted, RCVariantType::RC_VT_BOOL},
    { RCString(_T("")), RCPropID::kpidSolid, RCVariantType::RC_VT_BOOL},
    { RCString(_T("")), RCPropID::kpidCommented, RCVariantType::RC_VT_BOOL},
    { RCString(_T("")), RCPropID::kpidSplitBefore, RCVariantType::RC_VT_BOOL},
    { RCString(_T("")), RCPropID::kpidSplitAfter, RCVariantType::RC_VT_BOOL},
    { RCString(_T("")), RCPropID::kpidCRC, RCVariantType::RC_VT_UINT64},
    { RCString(_T("")), RCPropID::kpidHostOS, RCVariantType::RC_VT_STRING},
    { RCString(_T("")), RCPropID::kpidMethod, RCVariantType::RC_VT_STRING},
    { RCString(_T("")), RCPropID::kpidUnpackVer, RCVariantType::RC_VT_UINT64}
};

static RCPropData s_rarArcProps[] =
{
    { RCString(_T("")), RCPropID::kpidSolid, RCVariantType::RC_VT_BOOL},
    { RCString(_T("")), RCPropID::kpidNumBlocks, RCVariantType::RC_VT_UINT64},
    { RCString(_T("")), RCPropID::kpidIsVolume, RCVariantType::RC_VT_BOOL},
    { RCString(_T("")), RCPropID::kpidNumVolumes, RCVariantType::RC_VT_UINT64},
    { RCString(_T("")), RCPropID::kpidPhySize, RCVariantType::RC_VT_UINT64},
    { RCString(_T("")), RCPropID::kpidCommented, RCVariantType::RC_VT_BOOL},
    { RCString(_T("")), RCPropID::kpidComment, RCVariantType::RC_VT_STRING },
    { RCString(_T("")), RCPropID::kpidArchiveType, RCVariantType::RC_VT_UINT64 }
};

HResult RCRarHandler::GetNumberOfProperties(uint32_t& numProperties)
{ 
    numProperties = sizeof(s_rarProps) / sizeof(s_rarProps[0]);

    return RC_S_OK; 
}

HResult RCRarHandler::GetPropertyInfo(uint32_t index, RCString& name, RCPropertyID& propID, RCVariantType::RCVariantID& varType)
{
    if (index >= sizeof(s_rarProps) / sizeof(s_rarProps[0])) 
    {
        return RC_E_INVALIDARG;
    }
    const RCPropData& srcItem = s_rarProps[index];

    propID  = srcItem.m_propID ; 
    varType = srcItem.m_varType ; 
    name    = srcItem.m_propName ;

    return RC_S_OK;
}

HResult RCRarHandler::GetNumberOfArchiveProperties(uint32_t& numProperties)
{

    numProperties = sizeof(s_rarArcProps) / sizeof(s_rarArcProps[0]);

    return RC_S_OK;
}

HResult RCRarHandler::GetArchivePropertyInfo(uint32_t index, RCString& name, RCPropertyID& propID, RCVariantType::RCVariantID& varType)
{
    if(index >= sizeof(s_rarArcProps) / sizeof(s_rarArcProps[0])) 
    {
        return RC_E_INVALIDARG;
    }

    const RCPropData& srcItem = s_rarArcProps[index];

    propID  = srcItem.m_propID ; 
    varType = srcItem.m_varType ; 
    name    = srcItem.m_propName ;

    return RC_S_OK; 
}

uint64_t RCRarHandler::GetPackSize(int32_t refIndex) const
{
    const RCRarRefItemPtr& refItem = m_refItems[refIndex];
    uint64_t totalPackSize = 0;
    for (int32_t i = 0; i < refItem->m_numItems; i++)
    {
        totalPackSize += m_items[refItem->m_itemIndex + i]->m_packSize;
    }
    return totalPackSize;
}

HResult RCRarHandler::GetArchiveProperty(RCPropertyID propID, RCVariant& value)
{
    ClearVariantValue(value) ;
    RCVariant& prop = value;
    switch(propID)
    {
    case RCPropID::kpidSolid:
        {
            prop = m_archiveInfo.IsSolid();
        }
        break;

    case RCPropID::kpidEncrypted: 
        prop = m_archiveInfo.IsEncrypted(); 
        break; // it's for encrypted names.
    case RCPropID::kpidIsVolume:
        {
            prop = m_archiveInfo.IsVolume();
        }
        break;

    case RCPropID::kpidNumVolumes:
        {
#ifdef COMPRESS_MT
            if (m_archives.size() > 0)
            {
                prop = (uint64_t)m_archives[0].size();

            }
            else
            {
                prop = (uint64_t)0;
            }
#else
            prop = (uint64_t)m_archives.size();
#endif
        }
        break;

    case RCPropID::kpidOffset:
        if (m_archiveInfo.m_startPosition != 0)
        {
            prop = m_archiveInfo.m_startPosition;
        }
        break;

    case RCPropID::kpidCommented:
        {
            prop = m_archiveInfo.IsCommented();
        }
        break;

    case RCPropID::kpidComment:
        {
            prop = m_archiveInfo.m_comment;
        }
        break;

    case RCPropID::kpidNumBlocks:
        {
            uint64_t numBlocks = 0;
            for (uint32_t i = 0; i < m_refItems.size(); i++)
            {
                if (!IsSolid(i))
                {
                    numBlocks++;
                }
            }
            prop = numBlocks;
        }
        break;
    case RCPropID::kpidArchiveType:
        {
            prop = uint64_t(RC_ARCHIVE_RAR) ;
            break;
        }
    }

    return RC_S_OK;
}

HResult RCRarHandler::GetNumberOfItems(uint32_t& numItems)
{
    numItems = (uint32_t)m_refItems.size();
    return RC_S_OK;
}

static bool RarTimeToFileTime(const RCRarTime& rarTime, RC_FILE_TIME& result)
{
    if (!RCFileTime::DosTimeToFileTime(rarTime.m_dosTime, result))
    {
        return false;
    }

    uint64_t value =  (((uint64_t)result.u32HighDateTime) << 32) + result.u32LowDateTime;
    value += (uint64_t)rarTime.m_lowSecond * 10000000;
    value += ((uint64_t)rarTime.m_subTime[2] << 16) +
             ((uint64_t)rarTime.m_subTime[1] << 8) +
             ((uint64_t)rarTime.m_subTime[0]);

    result.u32LowDateTime  = (uint32_t)value;
    result.u32HighDateTime = uint32_t(value >> 32);

    return true;
}

static void RarTimeToProp(const RCRarTime& rarTime, RCVariant& prop)
{
    RC_FILE_TIME localFileTime, utcFileTime;
    if (RarTimeToFileTime(rarTime, localFileTime))
    {
        if (!RCFileTime::LocalFileTimeToFileTime(localFileTime, utcFileTime))
        {
            utcFileTime.u32LowDateTime  = 0;
            utcFileTime.u32HighDateTime = 0;
        }
    }
    else
    {
        utcFileTime.u32LowDateTime  = 0;
        utcFileTime.u32HighDateTime = 0;
    }

    prop = RCFileTime::ConvertFromFileTime(utcFileTime);
}

HResult RCRarHandler::GetProperty(uint32_t index, RCPropertyID propID, RCVariant& value)
{
    ClearVariantValue(value) ;
    RCVariant& prop = value;
    const RCRarRefItemPtr& refItem = m_refItems[index];
    const RCRarItemExPtr& item = m_items[refItem->m_itemIndex];

    switch(propID)
    {
    case RCPropID::kpidPath:
        {
            RCString u;
            if (item->HasUnicodeName() && !item->m_unicodeName.empty())
            {
                u = item->m_unicodeName;
            }
            else
            {
                u = RCStringConvert::MultiByteToUnicodeString(item->m_name, RC_CP_OEMCP);
            }
            prop = RCItemNameUtils::WinNameToOSName(u);
        }
        break;

    case RCPropID::kpidIsDir:
        {
            prop = item->IsDir();
        }
        break;

    case RCPropID::kpidSize:
        {
            prop = item->m_size;
        }
        break;

    case RCPropID::kpidPackSize:
        {
            prop = GetPackSize(index);
        }
        break;

    case RCPropID::kpidMTime:
        {
            RarTimeToProp(item->m_mTime, prop);
        }
        break;

    case RCPropID::kpidCTime:
        {
            if (item->m_cTimeDefined)
            {
                RarTimeToProp(item->m_cTime, prop);
            }
        }
        break;

    case RCPropID::kpidATime:
        {
            if (item->m_aTimeDefined)
            {
                RarTimeToProp(item->m_aTime, prop);
            }
        }
        break;

    case RCPropID::kpidAttrib:
        {
            prop = (uint64_t)item->GetWinAttributes();
        }
        break;

    case RCPropID::kpidEncrypted:
        {
            prop = item->IsEncrypted();
        }
        break;

    case RCPropID::kpidSolid:
        {
            prop = IsSolid(index);
        }
        break;

    case RCPropID::kpidCommented:
        {
            prop = item->IsCommented();
        }
        break;

    case RCPropID::kpidSplitBefore:
        {
            prop = item->IsSplitBefore();
        }
        break;

    case RCPropID::kpidSplitAfter:
        {
            prop = m_items[refItem->m_itemIndex + refItem->m_numItems - 1]->IsSplitAfter();
        }
        break;

    case RCPropID::kpidCRC:
        {
            const RCRarItemExPtr& lastItem = m_items[refItem->m_itemIndex + refItem->m_numItems - 1];
            prop = (uint64_t)((lastItem->IsSplitAfter()) ? item->m_fileCRC : lastItem->m_fileCRC);
        }
        break;

    case RCPropID::kpidUnpackVer:
        {
            prop = (uint64_t)item->m_unPackVersion;
        }
        break;

    case RCPropID::kpidMethod:
        {
            RCString method;
            if (item->m_method >= byte_t('0') && item->m_method <= byte_t('5'))
            {
                method = _T("m");
                char_t temp[32];
                RCStringUtil::ConvertUInt64ToString(item->m_method - byte_t('0'), temp);
                method += temp;
                if (!item->IsDir())
                {
                    method += _T(":");
                    RCStringUtil::ConvertUInt64ToString(16 + item->GetDictSize(), temp);
                    method += temp;
                }
            }
            else
            {
                char_t temp[32];
                RCStringUtil::ConvertUInt64ToString(item->m_method, temp);
                method += temp;
            }
            prop = method;
        }
        break;

   case RCPropID::kpidHostOS:
       {
           prop = RCString((item->m_hostOS < s_rarNumHostOSes) ? (s_rarHostOS[item->m_hostOS]) : s_rarUnknownOS);
       }
       break;
    }

    return RC_S_OK;
}

class RCRarVolumeName
{
public:

    /** 默认构造函数
    */
    RCRarVolumeName(): 
      m_newStyle(true) 
    {
    };

    /** 初始化分卷包的名字
    @param [in] name 文件名
    @param [in] newStyle 是否是新风格
    @return 初始化成功返回true，否则返回false
    */
    bool InitName(const RCString &name, bool newStyle)
    {
        m_first = true;
        m_newStyle = newStyle;
        int32_t dotPos = (int32_t)name.find_last_of('.');
        RCString basePart = name;
        if (dotPos >= 0)
        {
            RCString ext = RCStringUtil::Mid(name, dotPos + 1);
            if (RCStringUtil::CompareNoCase(ext, _T("rar")) == 0)
            {
                m_afterPart = RCStringUtil::Mid(name, dotPos);
                basePart = RCStringUtil::Left(name, dotPos);
            }
            else if (RCStringUtil::CompareNoCase(ext, _T("exe")) == 0)
            {
                m_afterPart = _T(".rar");
                basePart = RCStringUtil::Left(name, dotPos);
            }
            else if (!m_newStyle)
            {
                if (RCStringUtil::CompareNoCase(ext, _T("000")) == 0 ||
                    RCStringUtil::CompareNoCase(ext, _T("001")) == 0 ||
                    RCStringUtil::CompareNoCase(ext, _T("r00")) == 0 ||
                    RCStringUtil::CompareNoCase(ext, _T("r01")) == 0 )
                {
                    m_afterPart.clear();
                    m_first = false;
                    m_changedPart = ext;
                    m_unchangedPart = RCStringUtil::Left(name, dotPos + 1);
                    return true;
                }
            }
        }

        if (!m_newStyle)
        {
            m_afterPart.clear();
            m_unchangedPart = basePart + RCString(_T("."));
            m_changedPart = _T("r00");
            return true;
        }

        int32_t numLetters = 1;
        if (RCStringUtil::Right(basePart, numLetters) == _T("1") || 
            RCStringUtil::Right(basePart, numLetters) == _T("0"))
        {
            while (numLetters < (int32_t)basePart.length())
            {
                if (basePart[basePart.length() - numLetters - 1] != '0')
                {
                    break;
                }
                numLetters++;
            }
        }
        else
        {
            return false;
        }

        m_unchangedPart = RCStringUtil::Left(basePart, (int32_t)basePart.length() - numLetters);
        m_changedPart   = RCStringUtil::Right(basePart, numLetters);
        return true;
    }

    /** 获取下一个分卷包的名字
    @return 返回下一个分卷包的名字
    */
    RCString GetNextName()
    {
        RCString newName;
        if (m_newStyle || !m_first)
        {
            int32_t i;
            int32_t numLetters = (int32_t)m_changedPart.length();
            for (i = numLetters - 1; i >= 0; i--)
            {
                char_t c = m_changedPart[i];
                if (c == _T('9'))
                {
                    c = _T('0');
                    newName = c + newName;
                    if (i == 0)
                    {
                        newName = _T('1') + newName;
                    }
                    continue;
                }
                c++;
                newName = c + newName;
                i--;
                for (; i >= 0; i--)
                {
                    newName = m_changedPart[i] + newName;
                }
                break;
            }
            m_changedPart = newName;
        }
        m_first = false;
        return m_unchangedPart + m_changedPart + m_afterPart;
    }

    /** 初始化前一个分卷包的名字
    @param [in] name 文件名
    @param [in] newStyle 是否是新风格
    @return 初始化成功返回true，否则返回false
    */
    bool InitPreviousName(const RCString& name, bool newStyle)
    {
        if (!m_newStyle)
        {
            return false;
        }

        m_first = true;
        m_newStyle = newStyle;
        int32_t dotPos = (int32_t)name.find_last_of('.');
        RCString basePart = name;
        if (dotPos >= 0)
        {
            RCString ext = RCStringUtil::Mid(name, dotPos + 1);
            if (RCStringUtil::CompareNoCase(ext, _T("rar")) == 0)
            {
                m_afterPart = RCStringUtil::Mid(name, dotPos);
                basePart = RCStringUtil::Left(name, dotPos);
            }
            else if (RCStringUtil::CompareNoCase(ext, _T("exe")) == 0)
            {
                m_afterPart = _T(".rar");
                basePart = RCStringUtil::Left(name, dotPos);
            }
        }

        int32_t numLetters = 1;

        while (numLetters < (int32_t)basePart.length())
        {
            if (basePart[basePart.length() - numLetters - 1] < '0' ||
                basePart[basePart.length() - numLetters - 1] > '9')
            {
                break;
            }
            numLetters++;
        }

        m_unchangedPart = RCStringUtil::Left(basePart, (int32_t)basePart.length() - numLetters);
        m_changedPart   = RCStringUtil::Right(basePart, numLetters);
        return true;
    }

    /** 获取前一个分卷包的名字
    @return 返回前一个分卷包的名字
    */
    RCString GetPreviousName()
    {
        if (!m_newStyle)
        {
            return _T("");
        }

        RCString newName;
        if (m_newStyle || !m_first)
        {
            int32_t i;
            int32_t numLetters = (int32_t)m_changedPart.length();
            for (i = numLetters - 1; i >= 0; i--)
            {
                char_t c = m_changedPart[i];
                if (c == _T('0'))
                {
                    c = _T('9');
                    newName = c + newName;
                    if (i == 0)
                    {
                        return _T("");
                    }
                    continue;
                }
                c--;
                newName = c + newName;
                i--;
                for (; i >= 0; i--)
                {
                    newName = m_changedPart[i] + newName;
                }
                break;
            }
            m_changedPart = newName;
        }
        m_first = false;
        return m_unchangedPart + m_changedPart + m_afterPart;
    }

private:

    /** 是否是第一个卷
    */
    bool m_first;

    /** 是否是新风格
    */
    bool m_newStyle;

    /** 名字不变部分
    */
    RCString m_unchangedPart;

    /** 名字改变部分
    */
    RCString m_changedPart;

    /** 名字后缀部分
    */
    RCString m_afterPart;

};

HResult RCRarHandler::Open2(IInStream* stream,
                            const uint64_t* maxCheckStartPosition,
                            IArchiveOpenCallback* openArchiveCallback) 
{
    IArchiveOpenVolumeCallbackPtr openVolumeCallback;
    ICryptoGetTextPasswordPtr getTextPassword;
    IArchiveOpenCallbackPtr openArchiveCallbackWrap = openArchiveCallback;

    RCRarVolumeName seqName;

    uint64_t totalBytes = 0;
    uint64_t curBytes = 0;

    if (openArchiveCallback != NULL)
    {
        openArchiveCallbackWrap.QueryInterface(IID_IArchiveOpenVolumeCallback,
                                               openVolumeCallback.GetAddress());

        openArchiveCallbackWrap.QueryInterface(IID_ICryptoGetTextPassword,
                                               getTextPassword.GetAddress());
    }

    if (!openVolumeCallback)
    {
        return RC_S_OK;
    }

#ifdef COMPRESS_MT
    RCVector<RCRarIn> tempArchives;
    uint32_t numThreads;
    if ((uint32_t)m_numThreads > RCRarHeader::kMaxThreadNums)
    {
        numThreads = RCRarHeader::kMaxThreadNums;
    }
    else
    {
        numThreads = m_numThreads;
    }

    RCVector<RCRarIn>* archivesList = new RCVector<RCRarIn>[numThreads];
    RCScopedArrayPtr<RCVector<RCRarIn> > spArchivesList(archivesList) ;
    if (!archivesList)
    {
        return RC_E_FAIL;
    }
    RCVector<IInStreamPtr>* streamsList = new RCVector<IInStreamPtr>[numThreads];
    RCScopedArrayPtr<RCVector<IInStreamPtr> > spStreamsList(streamsList) ;
    if (!streamsList)
    {
        return RC_E_FAIL;
    }
#endif

    /** 分卷修复系列数据，依次为：

        是否需要修复分卷功能 needVolumeRebuild

        第一个卷的名称 volumeFirst
        第一个卷的风格 volumeFirstHaveNewVolumeName

        分卷修复成功后的卷名集合，不包括第一个卷 volumeNames
        分卷修复成功后的解压到得当前卷序号 volumePosition

        卷的路径 volumePath
    */
    bool needVolumeRebuild = false;
    RCString volumeFirst;
    bool volumeFirstHaveNewVolumeName = false;
    RCVector<RCString> volumeNames;
    uint32_t volumePosition = 0;
    RCString volumePath;
    if (openVolumeCallback->GetFolderPrefix(volumePath) != RC_S_OK)
    {
        volumePath.clear();
    }

    // 检查是否是分卷包
    IInStreamPtr streamVolume = stream;
    {
        RCRarIn archiveVolume;
        RCRarInArchiveInfo archiveInfoVolume;

        for (;;)
        {
            HResult hr = archiveVolume.Open(streamVolume.Get(), maxCheckStartPosition, openArchiveCallback);
            if (hr != RC_S_OK)
            {
                break;
            }

            archiveVolume.GetArchiveInfo(archiveInfoVolume);
            if (archiveInfoVolume.IsVolume())
            {
                RCString baseName;
                {
                    RCVariant prop;
                    hr = openVolumeCallback->GetProperty(RCPropID::kpidName, prop);
                    if (hr != RC_S_OK)
                    {
                        break;
                    }

                    if (!IsStringType(prop)) 
                    {
                        break;
                    }

                    baseName = GetStringValue(prop);
                }
                
                volumeFirst = baseName;
                volumeFirstHaveNewVolumeName = archiveInfoVolume.HaveNewVolumeName();

                RCRarVolumeName seqName;
                if (!seqName.InitPreviousName(baseName, archiveInfoVolume.HaveNewVolumeName()))
                {
                    break;
                }

                RCString fullName = seqName.GetPreviousName();
                if (fullName.empty())
                {
                    break;
                }

                IInStreamPtr newStreamVolume ;
                hr = openVolumeCallback->GetStream(fullName, newStreamVolume.GetAddress());
                if (hr == RC_S_FALSE)
                {
                    break;
                }
                streamVolume = newStreamVolume ;
                stream = streamVolume.Get();
                volumeFirst = fullName;
            }
            else
            {
               break;
            }
        }
    }

    // 分卷包的首个分卷包
    if ((!volumeFirst.empty()) && (!volumePath.empty()))
    {
        RCRarVolumeName seqName;
        if (seqName.InitName(volumeFirst, volumeFirstHaveNewVolumeName))
        {
            bool needVolumeNext = false;

            {
                IInStreamPtr newStreamVolume ;
                HResult hr = openVolumeCallback->GetStream(volumeFirst, newStreamVolume.GetAddress());

                if (hr == RC_S_FALSE)
                {
                    needVolumeRebuild = true;
                }
                else
                {
                    RCRarFileInfoParse parse(newStreamVolume.Get());

                    if (parse.ReadInfoFromStream() && parse.m_mainHeader.IsVolume())
                    {
                        // 如果不是第一个分卷，那肯定有错误。
                        if (!parse.m_mainHeader.IsFirstVolume())
                        {
                            needVolumeRebuild = true;
                        }
                        else
                        {
                            needVolumeNext = (!parse.m_endHeader.IsLastVolume());
                        }

                    }
                }
            }

            unsigned short volumnIndex = 1;

            while (needVolumeNext)
            {
                needVolumeNext = false;

                RCString volumeNext = seqName.GetNextName();

                IInStreamPtr newStreamVolume ;
                HResult hr = openVolumeCallback->GetStream(volumeNext, newStreamVolume.GetAddress());

                if (hr == RC_S_FALSE)
                {
                    needVolumeRebuild = true;
                }
                else
                {
                    RCRarFileInfoParse parse(newStreamVolume.Get());

                    if (parse.ReadInfoFromStream() && parse.m_mainHeader.IsVolume())
                    {
                        if (parse.m_endHeader.m_volNumber != volumnIndex)
                        {
                            needVolumeRebuild = true;
                        }
                        else
                        {
                            needVolumeNext = (!parse.m_endHeader.IsLastVolume());
                        }
                        volumnIndex++;
                    }
                }
            }
        }
        else
        {
            needVolumeRebuild = true;
        }

        if (needVolumeRebuild)
        {
            // 遍历目录下的所有RAR文件，如果是分卷文件，并且有分卷序号的都列出来

            RCRarVolumeRepair repair;

            const RCString wildcard(volumePath + _T("\\*"));

            RCFileInfo fileInfo;
            RCFindFile findFile;
            if (findFile.FindFirst(wildcard, fileInfo))
            {
                do
                {
                    if (fileInfo.IsDots())
                    {

                    }
                    else if (fileInfo.IsDir())
                    {            
                        
                    }
                    else
                    {
                        IInStreamPtr newStreamVolume ;
                        HResult hr = openVolumeCallback->GetStream(fileInfo.GetFileName(), newStreamVolume.GetAddress());

                        if (hr != RC_S_FALSE)
                        {
                            RCRarFileInfoParse parse(newStreamVolume.Get());

                            if (parse.ReadInfoFromStream() && parse.m_mainHeader.IsVolume())
                            {
                                repair.Push(fileInfo.GetFileName(), parse.m_endHeader.m_volNumber, parse.m_mainHeader.IsFirstVolume(), parse.m_endHeader.IsLastVolume());
                            }
                        }
                    }
                }
                while (findFile.FindNext(fileInfo));
            }
            
            if (repair.Repair(volumeNames, volumeFirst, false, openArchiveCallbackWrap))
            {
                // 打开第一个分卷
                IInStreamPtr newStreamVolume ;
                HResult hr = openVolumeCallback->GetStream(volumeFirst, newStreamVolume.GetAddress());
                if (hr == RC_S_FALSE)
                {
                    return RC_E_FAIL;
                }
                streamVolume = newStreamVolume;
                stream = streamVolume.Get();
            }
            else
            {
                volumeNames.clear();
            }
        }
    }

    for (;;)
    {
        HResult hr;
        IInStreamPtr inStream;

#ifdef COMPRESS_MT
        if (!tempArchives.empty())
#else
        if (!m_archives.empty())
#endif
        {
            if (!openVolumeCallback)
            {
                break;
            }

#ifdef COMPRESS_MT
            if (tempArchives.size() == 1)
#else
            if (m_archives.size() == 1)
#endif
            {
                if (!m_archiveInfo.IsVolume())
                {
                    break;
                }

                if (volumeFirst.empty())
                {
                    break;
                }

                seqName.InitName(volumeFirst, volumeFirstHaveNewVolumeName);
            }

            RCString fullName = seqName.GetNextName();

            // 如果有修正后的分卷名称，优先使用修正后的分卷名称。
            if (!volumeNames.empty())
            {
                if (volumePosition < volumeNames.size())
                {
                    fullName = volumeNames[volumePosition];

                    volumePosition++;
                }
            }

            IInStreamPtr newStreamVolume ;
            hr = openVolumeCallback->GetStream(fullName, newStreamVolume.GetAddress());
            if (hr == RC_S_FALSE)
            {
                break;
            }
            inStream = newStreamVolume;

            if (hr != RC_S_OK)
            {
                return hr;
            }

            if (!stream)
            {
                break;
            }
        }
        else
        {
            inStream = stream;
        }

        uint64_t endPos = 0;
        if (openArchiveCallback != NULL)
        {
            hr = stream->Seek(0, RC_STREAM_SEEK_END, &endPos);
            if (hr != RC_S_OK)
            {
                return hr;
            }

            hr = stream->Seek(0, RC_STREAM_SEEK_SET, NULL);
            if (hr != RC_S_OK)
            {
                return hr;
            }

            totalBytes += endPos;

            hr = openArchiveCallback->SetTotal(NULL, totalBytes);
            if (hr != RC_S_OK)
            {
                return hr;
            }
        }

#ifdef COMPRESS_MT
        RCVector<RCRarIn> archives ;
        RCVector<IInStreamPtr> streams;
        RCRarIn archive;

        for (uint32_t i = 0; i < numThreads; i++)
        {
            ICloneStreamPtr spCloneStream ;
            hr = inStream->QueryInterface(IID_ICloneStream,(void**)spCloneStream.GetAddress()) ;
            if (!IsSuccess(hr) || !spCloneStream)
            {
                return RC_S_FALSE ;
            }
            IUnknown* cloneStream = spCloneStream->Clone() ;
            if(NULL == cloneStream)
            {
                return RC_S_FALSE ;
            }
            IInStreamPtr spInStream((IInStream*)cloneStream) ;

            hr = archive.Open(spInStream.Get(), maxCheckStartPosition, openArchiveCallback);
            if (hr != RC_S_OK)
            {
                return hr;
            }
            streams.push_back(spInStream);
            archives.push_back(archive);
        }
#else
        RCRarIn archive;
        hr = archive.Open(inStream.Get(), maxCheckStartPosition, openArchiveCallback);
        if (hr != RC_S_OK)
        {
            return hr;
        }
#endif

#ifdef COMPRESS_MT
        if (tempArchives.empty())
#else
        if (m_archives.empty())
#endif
        {
            archive.GetArchiveInfo(m_archiveInfo);
        }

        for (;;)
        {
            RCRarItemExPtr item(new RCRarItemEx);
            if (!item)
            {
                return RC_S_FALSE;
            }
            bool decryptionError = false ;
            HResult hr = archive.GetNextItem(m_compressCodecsInfo.Get(), item, getTextPassword.Get(), openArchiveCallback, decryptionError);
            if (hr == RC_E_Password)
            {
                if (decryptionError && m_items.empty())
                {
                    return RC_E_Password ;
                }
                break;
            }
            
            if (hr == RC_S_FALSE)
            {
                break;
            }

            if (hr != RC_S_OK)
            {
                return hr;
            }

            if (item->IgnoreItem())
            {
                continue;
            }

            bool needAdd = true;
            if (item->IsSplitBefore())
            {
                if (!m_refItems.empty())
                {
                    RCRarRefItemPtr& refItem = m_refItems.back();
                    refItem->m_numItems++;
                    needAdd = false;
                }
            }

            if (needAdd)
            {
                RCRarRefItemPtr refItem(new RCRarRefItem);
                if (!refItem)
                {
                    return RC_S_FALSE;
                }

                refItem->m_itemIndex   = (int32_t)m_items.size();
                refItem->m_numItems    = 1;

#ifdef COMPRESS_MT
                refItem->m_volumeIndex = (int32_t)tempArchives.size();
#else
                refItem->m_volumeIndex = (int32_t)m_archives.size();
#endif
                m_refItems.push_back(refItem);
            }

            m_items.push_back(item);

            if (openArchiveCallback != NULL && m_items.size() % 100 == 0)
            {
                uint64_t numFiles = m_items.size();
                uint64_t numBytes = curBytes + item->m_position;
                hr = openArchiveCallback->SetCompleted(numFiles, numBytes);
                if (hr != RC_S_OK)
                {
                    return hr;
                }
            }
        }
        curBytes += endPos;

#ifdef COMPRESS_MT
        for (uint32_t i = 0; i < numThreads; i++)
        {
            archivesList[i].push_back(archives[i]);
            streamsList[i].push_back(streams[i]);
        }
        tempArchives.push_back(archive);
#else
        m_archives.push_back(archive);
#endif
    }

#ifdef COMPRESS_MT
    for (uint32_t i = 0; i < numThreads; i++)
    {
        m_archives.push_back(archivesList[i]);
        m_spInStream.push_back(streamsList[i]);
    }
#endif

    return RC_S_OK;
}

HResult RCRarHandler::Open(IInStream* stream,
                           const uint64_t maxCheckStartPosition,
                           IArchiveOpenCallback* openArchiveCallback)
{
    IStreamGetFileNamePtr get_filename;
    HResult rs = stream->QueryInterface(IStreamGetFileName::IID, (void**)get_filename.GetAddress());
    if (!IsSuccess(rs) || !get_filename)
    {
        return RC_S_FALSE ;
    }
    RCString filename;
    rs = get_filename->GetFileName(filename);
    if (!IsSuccess(rs))
    {
        return rs;
    }

    uint64_t postion = maxCheckStartPosition; 
    HResult hr = Open2(stream, &postion, openArchiveCallback);
    if (hr != RC_S_OK)
    {
        Close();
    }
    if (IsSuccess(hr))
    {   //如果打开rar成功,尝试取得rar 注释
        ICryptoGetTextPasswordPtr getTextPassword;
        IArchiveOpenCallbackPtr openArchiveCallbackWrap = openArchiveCallback;
        if (openArchiveCallback)
        {
            openArchiveCallbackWrap.QueryInterface(IID_ICryptoGetTextPassword,getTextPassword.GetAddress());
            if (getTextPassword)
            {
                RCRarGetComment::GetRarComment(filename.c_str(), m_archiveInfo.m_comment, getTextPassword.Get());
            }
        }
    }
    return hr;
}

HResult RCRarHandler::Close()
{
    m_refItems.clear();
    m_items.clear();
    m_archives.clear();
    m_spInStream.clear() ;
    return RC_S_OK;
}

struct RCRarMethodItem
{
    /** 解压版本
    */
    byte_t m_rarUnPackVersion;

    /** 解码器
    */
    ICompressCoderPtr m_coder;
};

#ifdef COMPRESS_MT

static void RarDecoderThread(void *threadDecoderInfo);

struct RCRarDecoderThreadInfo
{
    /** 编码/解码器
    */
    ICompressCoderPtr m_commonCoder;

    /** 输入流
    */
    ISequentialInStreamPtr m_inStream;

    /** 输出流
    */
    ISequentialOutStreamPtr m_outStream;

    /** 解压回调接口
    */
    IArchiveExtractCallbackPtr m_extractCallback;

    /** 解压进度接口
    */
    ICompressProgressInfoPtr m_decompressProgress;

    /** 线程数
    */
    uint32_t m_numThreads;

    /** 结果
    */
    int32_t m_result;

    /** 解压结果
    */
    int32_t m_decompressingResult;

    /** 项
    */
    RCRarItemExPtr m_item;

    /** 项索引
    */
    uint32_t m_itemIndex;

    /** 包大小
    */
    uint64_t m_packSize;

    /** crc
    */
    uint32_t m_fileCRC;

    /** 带CRC的输出流
    */
    RCOutStreamWithCRC* m_outStreamSpec;

    /** 过滤器
    */
    RCFilterCoder* m_filterStreamSpec;

    /** 是否是空闲线程
    */
    volatile bool m_isFree;

    /** 线程
    */
    RCThreadPtr m_thread;

    /** 解压事件
    */
    RCAutoResetEvent m_decompressEvent;

    /** 解压完成事件
    */
    RCAutoResetEvent m_decompressionCompletedEvent;

    /** 线程是否退出
    */
    volatile bool m_exitThread;

    /** 默认构造函数
    */
    RCRarDecoderThreadInfo():
        m_numThreads(0),
        m_result(0),
        m_decompressingResult(0),
        m_itemIndex(0),
        m_packSize(0),
        m_fileCRC(0),
        m_outStreamSpec(0),
        m_filterStreamSpec(0),
        m_isFree(true),        
        m_exitThread(false)
    {
        
    }

    /** 创建事件
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult CreateEvents()
    {
        HResult hr = m_decompressEvent.CreateIfNotCreated();
        if (!IsSuccess(hr))
        {
            return hr;
        }
        return m_decompressionCompletedEvent.CreateIfNotCreated();
    }

    /** 创建线程
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult CreateThread() 
    { 
        RCThread* thread = new RCThread(&RarDecoderThread, this);
        if (!thread)
        {
            return RC_S_FALSE;
        }

        m_thread.reset(thread);

        if (thread->Start())
        {
            return RC_S_OK;
        }
        return RC_S_FALSE;
    }

    /** 等待解压
    */
    void WaitAndDecode() ;

    /** 停止并等待关闭
    */
    void StopWaitClose()
    {
        m_exitThread = true;
        if (m_decompressEvent.IsCreated())
        {
            m_decompressEvent.Set();
        }
        m_thread->Join();
    }
};

void RCRarDecoderThreadInfo::WaitAndDecode()
{
    for (;;)
    {
        m_decompressEvent.Lock();
        if (m_exitThread)
        {
            break;
        }

        m_result = m_commonCoder->Code(m_inStream.Get(), m_outStream.Get(), &m_packSize, &m_item->m_size, m_decompressProgress.Get());
        if (m_item->IsEncrypted())
        {
            m_filterStreamSpec->ReleaseInStream();
        }

        if (m_result == RC_S_FALSE)
        {
            m_outStream.Release();
            m_result = m_extractCallback->SetOperationResult(m_itemIndex,RC_ARCHIVE_EXTRACT_RESULT_DATA_ERROR);
            if (m_result != RC_S_OK)
            {
                break;
            }
            m_decompressionCompletedEvent.Set();
            continue;
        }

        if (m_result != RC_S_OK)
        {
            break;
        }

        bool crcOK = (m_outStreamSpec->GetCRC() == m_fileCRC);
        m_outStream.Release();
        
        m_result = m_extractCallback->SetOperationResult(m_itemIndex,
                                                         crcOK ? 
                                                         RC_ARCHIVE_EXTRACT_RESULT_OK:
                                                         RC_ARCHIVE_EXTRACT_RESULT_CRC_ERROR);
                                                         
        if (m_result != RC_S_OK)
        {
            break;
        }

        m_decompressionCompletedEvent.Set();
    }
    m_decompressionCompletedEvent.Set();
}

static void RarDecoderThread(void *threadDecoderInfo)
{
    ((RCRarDecoderThreadInfo *)threadDecoderInfo)->WaitAndDecode();
    return;
}

class RCRarDecoderThreads
{
public:

    /** 默认析构函数
    */
    ~RCRarDecoderThreads()
    {
        for (int32_t i = 0; i < (int32_t)m_threads.size(); i++)
        {
            m_threads[i].StopWaitClose();
        }
    }

public:

    /** 线程
    */
    RCVector<RCRarDecoderThreadInfo> m_threads;
};

#endif

HResult RCRarHandler::Extract(const std::vector<uint32_t>& indices,
                              int32_t testMode,
                              IArchiveExtractCallback* extractCallback)
{
    if(extractCallback)
    {
        extractCallback->SetMultiThreadMode(false) ;
    }

    ICryptoGetTextPasswordPtr getTextPassword;
    bool btestMode = (testMode != 0);

    uint64_t censoredTotalUnPacked = 0;
    uint64_t importantTotalUnPacked = 0;

    uint32_t numItems = (uint32_t)indices.size();

    bool allFilesMode = (numItems == 0);
    if (allFilesMode)
    {
        numItems = (uint32_t)m_refItems.size();
    }

    if(numItems == 0)
    {
        return RC_S_OK;
    }

    HResult hr = RC_S_OK ;
    int32_t lastIndex = 0;
    RCVector<int32_t> importantIndexes;
    RCVector<bool>    extractStatuses;
    bool isSolid = false;

    for (uint32_t t = 0; t < numItems; t++)
    {
        int32_t index = allFilesMode ? t : indices[t];
        const RCRarRefItemPtr& refItem = m_refItems[index];
        const RCRarItemExPtr& item = m_items[refItem->m_itemIndex];
        censoredTotalUnPacked += item->m_size;

        int32_t j;
        for(j = lastIndex; j <= index; j++)
            // if(!_items[_refItems[j].ItemIndex].IsSolid())
        {
            if(!IsSolid(j))
            {
                lastIndex = j;
            }
            else if (!isSolid)
            {
                isSolid = true;
            }
        }

        for(j = lastIndex; j <= index; j++)
        {
            const RCRarRefItemPtr& refItem = m_refItems[j];
            const RCRarItemExPtr& item = m_items[refItem->m_itemIndex];

            // const RCRarItemExPtr &item = m_items[j];

            importantTotalUnPacked += item->m_size;
            // importantTotalPacked += item->PackSize;
            importantIndexes.push_back(j);
            extractStatuses.push_back(j == index);
        }
        lastIndex = index + 1;
    }

    extractCallback->SetTotal(importantTotalUnPacked);

    RCVector<RCRarMethodItem> methodItems;

    ICompressCoderPtr copyCoder;
    hr = RCCreateCoder::CreateCoder(m_compressCodecsInfo.Get(),
                                    RCMethod::ID_COMPRESS_COPY,
                                    copyCoder,
                                    false);
    if (hr != RC_S_OK)
    {
        return hr;
    }

    RCFilterCoder* filterStreamSpec = new RCFilterCoder;
    ISequentialInStreamPtr filterStream = filterStreamSpec;

    RCRar20Decoder* rar20CryptoDecoderSpec = NULL;
    ICompressFilterPtr rar20CryptoDecoder;

    RCRarAESDecoder* rar29CryptoDecoderSpec = NULL;
    ICompressFilterPtr rar29CryptoDecoder;

    RCRarFolderInStream* folderInStreamSpec = NULL;
    ISequentialInStreamPtr folderInStream;

    ICompressFilterPtr filterDecoder;

    bool solidStart = true;

#ifdef COMPRESS_MT
    uint32_t numThreads = m_numThreads;
    if (numThreads > RCRarHeader::kMaxThreadNums)
    {
        numThreads = RCRarHeader::kMaxThreadNums;
    }

    bool mtMode = (m_numThreads > 1);

    if (mtMode)
    {
        //解压包内文件数小于2，用单线程
        if (importantIndexes.size() <= 1)
        {
            mtMode = false;
        }
    }

    //固始压缩，用单线程
    if (isSolid)
    {
        mtMode = false;
    }
#endif

#ifdef COMPRESS_MT
    if (!mtMode)
    {
#endif
    //单线程解压
    uint64_t currentImportantTotalUnPacked = 0;
    uint64_t currentImportantTotalPacked  = 0;
    uint64_t currentUnPackSize = 0 ;
    uint64_t currentPackSize = 0 ;
    
    RCLocalProgress* lps = new RCLocalProgress;
    ICompressProgressInfoPtr progress = lps;
    lps->Init(extractCallback, false);
    
    for(int32_t i = 0; i < (int32_t)importantIndexes.size(); i++,
                                                             currentImportantTotalUnPacked += currentUnPackSize,
                                                             currentImportantTotalPacked += currentPackSize)
    {
        lps->SetInSize(currentImportantTotalPacked);
        lps->SetOutSize(currentImportantTotalUnPacked);
        hr = lps->SetCur();
        
        if (hr != RC_S_OK)
        {
            return hr;
        }

        ISequentialOutStreamPtr realOutStream;

        int32_t askMode = 0 ;
        if(extractStatuses[i])
        {
            askMode = btestMode ? RC_ARCHIVE_EXTRACT_ASK_MODE_TEST : RC_ARCHIVE_EXTRACT_ASK_MODE_EXTRACT;
        }
        else
        {
            askMode = RC_ARCHIVE_EXTRACT_ASK_MODE_SKIP;
        }

        uint32_t index = importantIndexes[i];

        const RCRarRefItemPtr& refItem = m_refItems[index];
        const RCRarItemExPtr& item = m_items[refItem->m_itemIndex];

        currentUnPackSize = item->m_size;
        currentPackSize = GetPackSize(index);

        if(item->IgnoreItem())
        {
            continue;
        }

        hr = extractCallback->GetStream(index, realOutStream.GetAddress(), askMode);
        if (hr != RC_S_OK)
        {
            return hr;
        }

        if (!IsSolid(index))
        {
            solidStart = true;
        }

        if(item->IsDir())
        {
            hr = extractCallback->PrepareOperation(index,askMode);
            if (hr != RC_S_OK)
            {
                return hr;
            }

            hr = extractCallback->SetOperationResult(index,RC_ARCHIVE_EXTRACT_RESULT_OK);
            if (hr != RC_S_OK)
            {
                return hr;
            }
            continue;
        }

        bool mustBeProcessedAnywhere = false;
        if(i < (int32_t)importantIndexes.size() - 1)
        {
            mustBeProcessedAnywhere = IsSolid(importantIndexes[i + 1]);
        }

        if (!mustBeProcessedAnywhere && !btestMode && !realOutStream)
        {
            continue;
        }

        if (!realOutStream && !btestMode)
        {
            askMode = RC_ARCHIVE_EXTRACT_ASK_MODE_SKIP;
        }

        hr = extractCallback->PrepareOperation(index,askMode);
        if (hr != RC_S_OK)
        {
            return hr;
        }

        RCOutStreamWithCRC *outStreamSpec = new RCOutStreamWithCRC;
        ISequentialOutStreamPtr outStream(outStreamSpec);
        outStreamSpec->SetStream(realOutStream.Get());
        outStreamSpec->Init();
        realOutStream.Release();

        if (!folderInStream)
        {
            folderInStreamSpec = new RCRarFolderInStream;
            folderInStream = folderInStreamSpec;
        }

#ifdef COMPRESS_MT
        folderInStreamSpec->Init(&m_archives[0], &m_items, refItem);
#else
        folderInStreamSpec->Init(&m_archives, &m_items, refItem);
#endif
        uint64_t packSize = currentPackSize;

        ISequentialInStreamPtr inStream;
        if (item->IsEncrypted())
        {
            ICryptoSetPasswordPtr cryptoSetPassword;
            if (item->m_unPackVersion >= 29)
            {
                if (!rar29CryptoDecoder)
                {
                    ICompressFilterPtr filter;
                    hr = RCCreateCoder::CreateFilter(m_compressCodecsInfo.Get(),
                                                     RCMethod::ID_CRYPTO_RAR_29,
                                                     filter,
                                                     false);
                    if (hr != RC_S_OK)
                    {
                        return hr;
                    }

                    rar29CryptoDecoderSpec = dynamic_cast<RCRarAESDecoder*>(filter.Get());
                    rar29CryptoDecoder     = filter;
                    
                    filterDecoder = rar29CryptoDecoder;
               }

                //SetRar350Mode
                RCPropertyIDPairArray proparray;
                ICompressSetCoderPropertiesPtr propset;

                HResult hr = rar29CryptoDecoderSpec->QueryInterface(IID_ICompressSetCoderProperties,
                                                                    (void**)propset.GetAddress());
                if (hr != RC_S_OK)
                {
                    return hr;
                }

                proparray.push_back(RCPropertyIDPairArray::value_type(RCMethodProperty::ID_SET_RAR_350_MODE, item->m_unPackVersion < 36));

                hr = propset->SetCoderProperties(proparray);
                if (hr != RC_S_OK)
                {
                    return hr;
                }

                ICompressSetDecoderProperties2Ptr cryptoProperties;

                hr = rar29CryptoDecoder.QueryInterface(IID_ICompressSetDecoderProperties2,
                                                       cryptoProperties.GetAddress());
                if (hr != RC_S_OK)
                {
                    return hr;
                }

                hr = cryptoProperties->SetDecoderProperties2(item->m_salt,
                                                             item->HasSalt() ? sizeof(item->m_salt) : 0);
                if (hr != RC_S_OK)
                {
                    return hr;
                } 

                filterStreamSpec->SetFilter(rar29CryptoDecoder.Get());
            }
            else if (item->m_unPackVersion >= 20)
            {
                if (!rar20CryptoDecoder)
                {
                    ICompressFilterPtr filter;
                    hr = RCCreateCoder::CreateFilter(m_compressCodecsInfo.Get(),
                                                     RCMethod::ID_CRYPTO_RAR_20,
                                                     filter,
                                                     false);
                    if (hr != RC_S_OK)
                    {
                        return hr;
                    }

                    rar20CryptoDecoderSpec = dynamic_cast<RCRar20Decoder*>(filter.Get());
                    rar20CryptoDecoder     = filter;

                    filterDecoder = rar20CryptoDecoder;
                }

                filterStreamSpec->SetFilter(rar20CryptoDecoder.Get());
            }
            else
            {
                outStream.Release();
                hr = extractCallback->SetOperationResult(index,RC_ARCHIVE_EXTRACT_RESULT_UNSUPPORTED_METHOD);
                if (hr != RC_S_OK)
                {
                    return hr;
                }
                continue;
            }

            hr = filterDecoder->QueryInterface(IID_ICryptoSetPassword,
                                               (void**)cryptoSetPassword.GetAddress());
            if (hr != RC_S_OK)
            {
                return hr;
            }

            if (!getTextPassword)
            {
                extractCallback->QueryInterface(IID_ICryptoGetTextPassword,
                                                (void**)getTextPassword.GetAddress());
            }

            if (getTextPassword)
            {
                RCString password;
                hr = getTextPassword->CryptoGetTextPassword(password);
                if (hr != RC_S_OK)
                {
                    return hr;
                }

                if (item->m_unPackVersion >= 29)
                {
                    RCByteBuffer buffer;
                    RCString unicodePassword(password);
                    const uint32_t sizeInBytes = (uint32_t)unicodePassword.length() * 2;
                    buffer.SetCapacity(sizeInBytes);
                    for (int32_t i = 0; i < (int32_t)unicodePassword.length(); i++)
                    {
                        char_t c = unicodePassword[i];
                        ((byte_t *)buffer.data())[i * 2] = (byte_t)c;
                        ((byte_t *)buffer.data())[i * 2 + 1] = (byte_t)(c >> 8);
                    }

                    hr = cryptoSetPassword->CryptoSetPassword((const byte_t *)buffer.data(),
                                                              sizeInBytes);
                    if (hr != RC_S_OK)
                    {
                        return hr;
                    }
                }
                else
                {
                    RCStringA oemPassword = RCStringConvert::UnicodeStringToMultiByte(password.c_str(),
                                                                                      RC_CP_OEMCP);

                    hr = cryptoSetPassword->CryptoSetPassword((const byte_t *)oemPassword.c_str(),
                                                              (uint32_t)oemPassword.length());
                    if (hr != RC_S_OK)
                    {
                        return hr;
                    }
                }
            }
            else
            {
                hr = cryptoSetPassword->CryptoSetPassword(0, 0);
                if (hr != RC_S_OK)
                {
                    return hr;
                }
            }
            filterStreamSpec->SetInStream(folderInStream.Get());
            inStream = filterStream;
        }
        else
        {
            inStream = folderInStream;
        }
        
        ICompressCoderPtr commonCoder;
        switch(item->m_method)
        {
        case '0':
            {
                commonCoder = copyCoder;
            }
            break;

        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
            {
                int32_t m;
                for (m = 0; m < (int32_t)methodItems.size(); m++)
                {
                    if (methodItems[m].m_rarUnPackVersion == item->m_unPackVersion)
                    {
                        break;
                    }
                }

                if (m == (int32_t)methodItems.size())
                {
                    RCRarMethodItem mi;
                    mi.m_rarUnPackVersion = item->m_unPackVersion;

                    mi.m_coder.Release();
                    if (item->m_unPackVersion <= 30)
                    {
                        uint32_t methodID = 0x040300;
                        if (item->m_unPackVersion < 20)
                        {
                            methodID += 1;
                        }
                        else if (item->m_unPackVersion < 29)
                        {
                            methodID += 2;
                        }
                        else
                        {
                            methodID += 3;
                        }

                        hr = RCCreateCoder::CreateCoder(m_compressCodecsInfo.Get(),
                                                        methodID,
                                                        mi.m_coder,
                                                        false);
                        if (hr != RC_S_OK)
                        {
                            return hr;
                        }
                    }

                    if (mi.m_coder == 0)
                    {
                        outStream.Release();
                        hr = extractCallback->SetOperationResult(index,RC_ARCHIVE_EXTRACT_RESULT_UNSUPPORTED_METHOD);
                        if (hr != RC_S_OK)
                        {
                            return hr;
                        }
                        continue;
                    }

                    methodItems.push_back(mi);

                    m = (int32_t)methodItems.size() - 1;
                }

                ICompressCoderPtr decoder = methodItems[m].m_coder;
                ICompressSetDecoderProperties2Ptr compressSetDecoderProperties;

                hr = decoder.QueryInterface(IID_ICompressSetDecoderProperties2,
                                            compressSetDecoderProperties.GetAddress());
                if (hr != RC_S_OK)
                {
                    return hr;
                }

                byte_t isSolid = (byte_t)((IsSolid(index) || item->IsSplitBefore()) ? 1: 0);
                if (solidStart)
                {
                    isSolid = false;
                    solidStart = false;
                }

                hr = compressSetDecoderProperties->SetDecoderProperties2(&isSolid, 1);
                if (hr != RC_S_OK)
                {
                    return hr;
                }

                commonCoder = decoder;
            }
            break;

        default:
            {
                outStream.Release();
                hr = extractCallback->SetOperationResult(index,RC_ARCHIVE_EXTRACT_RESULT_UNSUPPORTED_METHOD);
                if (hr != RC_S_OK)
                {
                    return hr;
                }
            }
            continue;
        }

        hr = commonCoder->Code(inStream.Get(), outStream.Get(), &packSize, &item->m_size, progress.Get());
        if (item->IsEncrypted())
        {
            filterStreamSpec->ReleaseInStream();
        }

        if (hr == RC_S_FALSE)
        {
            outStream.Release();
            hr = extractCallback->SetOperationResult(index,RC_ARCHIVE_EXTRACT_RESULT_DATA_ERROR);

            if (hr != RC_S_OK)
            {
                return hr;
            }
            continue;
        }

        if (hr != RC_S_OK)
        {
            return hr;
        }

        {
            const RCRarItemExPtr& lastItem = m_items[refItem->m_itemIndex + refItem->m_numItems - 1];
            bool crcOK = outStreamSpec->GetCRC() == lastItem->m_fileCRC;
            outStream.Release();
            hr = extractCallback->SetOperationResult(index,crcOK ?  RC_ARCHIVE_EXTRACT_RESULT_OK:
                                                                    RC_ARCHIVE_EXTRACT_RESULT_CRC_ERROR);
            if (hr != RC_S_OK)
            {
                return hr;
            }
        }      
    }
    //单线程解压结束，返回
    return RC_S_OK;
#ifdef COMPRESS_MT
    }
#endif

#ifdef COMPRESS_MT

    {
        //多线程解压
        if(extractCallback)
        {
            extractCallback->SetMultiThreadMode(true) ;
        }
        RCMtLocalProgress* lps = new RCMtLocalProgress ;
        ICompressProgressInfoPtr progress = lps ;
        lps->Init(extractCallback, false);
        
        RCMtExtractProgressMixer extractProgressMT ;
        extractProgressMT.Init(numItems,lps) ;

        RCRarDecoderThreads threads;
        RCVector<HANDLE> decompressingCompletedEvents;
        RCVector<int32_t> threadIndices; 

        uint32_t i = 0 ;
        for (i = 0; i < (uint32_t)numThreads; i++)
        {
            threads.m_threads.push_back(RCRarDecoderThreadInfo());
        }

        for (i = 0; i < numThreads; i++)
        {
            RCRarDecoderThreadInfo& threadInfo = threads.m_threads[i];
            hr = threadInfo.CreateEvents();
            if (!IsSuccess(hr))
            {
                return hr;
            }
            threadInfo.m_isFree = true;

            hr = threadInfo.CreateThread();
            if (!IsSuccess(hr))
            {
                return hr;
            }
        }

        uint32_t whichThread = 0;

        for(int32_t i = 0; i < (int32_t)importantIndexes.size(); ++i)
        {
            if ((uint32_t)threadIndices.size() < numThreads)
            {
                for (uint32_t i = 0; i < numThreads; i++)
                {
                    RCRarDecoderThreadInfo &threadInfo = threads.m_threads[i];
                    if (threadInfo.m_isFree)
                    {
                        whichThread = i;
                        break;
                    }
                }
            }

            ISequentialOutStreamPtr realOutStream;

            int32_t askMode = 0 ;
            if(extractStatuses[i])
            {
                askMode = btestMode ? RC_ARCHIVE_EXTRACT_ASK_MODE_TEST : RC_ARCHIVE_EXTRACT_ASK_MODE_EXTRACT;
            }
            else
            {
                askMode = RC_ARCHIVE_EXTRACT_ASK_MODE_SKIP;
            }

            uint32_t index = importantIndexes[i];

            const RCRarRefItemPtr& refItem = m_refItems[index];
            const RCRarItemExPtr& item = m_items[refItem->m_itemIndex];

            if(item->IgnoreItem())
            {
                continue;
            }

            hr = extractCallback->GetStream(index, realOutStream.GetAddress(), askMode);
            if (hr != RC_S_OK)
            {
                return hr;
            }

            if (!IsSolid(index))
            {
                solidStart = true;
            }

            if(item->IsDir())
            {
                hr = extractCallback->PrepareOperation(index,askMode);
                if (hr != RC_S_OK)
                {
                    return hr;
                }

                hr = extractCallback->SetOperationResult(index,RC_ARCHIVE_EXTRACT_RESULT_OK);
                if (hr != RC_S_OK)
                {
                    return hr;
                }
                continue;
            }

            bool mustBeProcessedAnywhere = false;
            if(i < (int32_t)importantIndexes.size() - 1)
            {
                mustBeProcessedAnywhere = IsSolid(importantIndexes[i + 1]);
            }

            if (!mustBeProcessedAnywhere && !btestMode && !realOutStream)
            {
                continue;
            }

            if (!realOutStream && !btestMode)
            {
                askMode = RC_ARCHIVE_EXTRACT_ASK_MODE_SKIP;
            }

            hr = extractCallback->PrepareOperation(index,askMode);
            if (hr != RC_S_OK)
            {
                return hr;
            }
            RCFilterCoder* filterStreamSpec = new RCFilterCoder;
            ISequentialInStreamPtr filterStream = filterStreamSpec;

            RCOutStreamWithCRC *outStreamSpec = new RCOutStreamWithCRC;
            ISequentialOutStreamPtr outStream(outStreamSpec);

            outStreamSpec->SetStream(realOutStream.Get());
            outStreamSpec->Init();
            realOutStream.Release();

            folderInStreamSpec = new RCRarFolderInStream;
            folderInStream = folderInStreamSpec;

            folderInStreamSpec->Init(&m_archives[whichThread], &m_items, refItem);
            uint64_t packSize = GetPackSize(index) ;

            ISequentialInStreamPtr inStream;
            if (item->IsEncrypted())
            {
                ICryptoSetPasswordPtr cryptoSetPassword;
                if (item->m_unPackVersion >= 29)
                {
                    //if (!rar29CryptoDecoder)
                    {
                        ICompressFilterPtr filter;
                        hr = RCCreateCoder::CreateFilter(m_compressCodecsInfo.Get(),
                                                         RCMethod::ID_CRYPTO_RAR_29,
                                                         filter,
                                                         false);
                        if (hr != RC_S_OK)
                        {
                            return hr;
                        }

                        rar29CryptoDecoderSpec = dynamic_cast<RCRarAESDecoder*>(filter.Get());
                        rar29CryptoDecoder     = filter;

                        filterDecoder = rar29CryptoDecoder;
                    }

                    //SetRar350Mode
                    RCPropertyIDPairArray proparray;
                    ICompressSetCoderPropertiesPtr propset;

                    HResult hr = rar29CryptoDecoderSpec->QueryInterface(IID_ICompressSetCoderProperties,(void**)propset.GetAddress());
                    if (hr != RC_S_OK)
                    {
                        return hr;
                    }

                    proparray.push_back(RCPropertyIDPairArray::value_type(RCMethodProperty::ID_SET_RAR_350_MODE, item->m_unPackVersion < 36));

                    hr = propset->SetCoderProperties(proparray);
                    if (hr != RC_S_OK)
                    {
                        return hr;
                    }

                    ICompressSetDecoderProperties2Ptr cryptoProperties;

                    hr = rar29CryptoDecoder.QueryInterface(IID_ICompressSetDecoderProperties2,cryptoProperties.GetAddress());
                    if (hr != RC_S_OK)
                    {
                        return hr;
                    }

                    hr = cryptoProperties->SetDecoderProperties2(item->m_salt,item->HasSalt() ? sizeof(item->m_salt) : 0);
                    if (hr != RC_S_OK)
                    {
                        return hr;
                    } 

                    filterStreamSpec->SetFilter(rar29CryptoDecoder.Get());
                }
                else if (item->m_unPackVersion >= 20)
                {
                    //if (!rar20CryptoDecoder)
                    {
                        ICompressFilterPtr filter;
                        hr = RCCreateCoder::CreateFilter(m_compressCodecsInfo.Get(),
                                                         RCMethod::ID_CRYPTO_RAR_20,
                                                         filter,
                                                         false);
                        if (hr != RC_S_OK)
                        {
                            return hr;
                        }

                        rar20CryptoDecoderSpec = dynamic_cast<RCRar20Decoder*>(filter.Get());
                        rar20CryptoDecoder     = filter;

                        filterDecoder = rar20CryptoDecoder;
                    }

                    filterStreamSpec->SetFilter(rar20CryptoDecoder.Get());
                }
                else
                {
                    outStream.Release();
                    hr = extractCallback->SetOperationResult(index,RC_ARCHIVE_EXTRACT_RESULT_UNSUPPORTED_METHOD);
                    if (hr != RC_S_OK)
                    {
                        return hr;
                    }
                    continue;
                }

                hr = filterDecoder->QueryInterface(IID_ICryptoSetPassword,(void**)cryptoSetPassword.GetAddress());
                if (hr != RC_S_OK)
                {
                    return hr;
                }

                if (!getTextPassword)
                {
                    extractCallback->QueryInterface(IID_ICryptoGetTextPassword,(void**)getTextPassword.GetAddress());
                }

                if (getTextPassword)
                {
                    RCString password;
                    hr = getTextPassword->CryptoGetTextPassword(password);
                    if (hr != RC_S_OK)
                    {
                        return hr;
                    }

                    if (item->m_unPackVersion >= 29)
                    {
                        RCByteBuffer buffer;
                        RCString unicodePassword(password);
                        const uint32_t sizeInBytes = (uint32_t)unicodePassword.length() * 2;
                        buffer.SetCapacity(sizeInBytes);
                        for (int32_t i = 0; i < (int32_t)unicodePassword.length(); i++)
                        {
                            char_t c = unicodePassword[i];
                            ((byte_t *)buffer.data())[i * 2] = (byte_t)c;
                            ((byte_t *)buffer.data())[i * 2 + 1] = (byte_t)(c >> 8);
                        }

                        hr = cryptoSetPassword->CryptoSetPassword((const byte_t *)buffer.data(), sizeInBytes);
                        if (hr != RC_S_OK)
                        {
                            return hr;
                        }
                    }
                    else
                    {
                        RCStringA oemPassword = RCStringConvert::UnicodeStringToMultiByte(password.c_str(), RC_CP_OEMCP);

                        hr = cryptoSetPassword->CryptoSetPassword((const byte_t *)oemPassword.c_str(),(uint32_t)oemPassword.length());
                        if (hr != RC_S_OK)
                        {
                            return hr;
                        }
                    }
                }
                else
                {
                    hr = cryptoSetPassword->CryptoSetPassword(0, 0);
                    if (hr != RC_S_OK)
                    {
                        return hr;
                    }
                }
                filterStreamSpec->SetInStream(folderInStream.Get());
                inStream = filterStream;
            }
            else
            {
                inStream = folderInStream;
            }

            ICompressCoderPtr commonCoder;
            RCVector<RCRarMethodItem> methodItems;

            switch(item->m_method)
            {
            case '0':
                {
                    ICompressCoderPtr copyCoder;
                    hr = RCCreateCoder::CreateCoder(m_compressCodecsInfo.Get(),
                                                    RCMethod::ID_COMPRESS_COPY,
                                                    copyCoder,
                                                    false);
                    if (hr != RC_S_OK)
                    {
                        return hr;
                    }

                    commonCoder = copyCoder;
                }
                break;

            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
                {
                    int32_t m = 0 ;
                    for (m = 0; m < (int32_t)methodItems.size(); m++)
                    {
                        if (methodItems[m].m_rarUnPackVersion == item->m_unPackVersion)
                        {
                            break;
                        }
                    }

                    if (m == (int32_t)methodItems.size())
                    {
                        RCRarMethodItem mi;
                        mi.m_rarUnPackVersion = item->m_unPackVersion;

                        mi.m_coder.Release();
                        if (item->m_unPackVersion <= 30)
                        {
                            uint32_t methodID = 0x040300;
                            if (item->m_unPackVersion < 20)
                            {
                                methodID += 1;
                            }
                            else if (item->m_unPackVersion < 29)
                            {
                                methodID += 2;
                            }
                            else
                            {
                                methodID += 3;
                            }

                            hr = RCCreateCoder::CreateCoder(m_compressCodecsInfo.Get(),
                                                            methodID,
                                                            mi.m_coder,
                                                            false);
                            if (hr != RC_S_OK)
                            {
                                return hr;
                            }
                        }

                        if (mi.m_coder == 0)
                        {
                            outStream.Release();
                            hr = extractCallback->SetOperationResult(index,RC_ARCHIVE_EXTRACT_RESULT_UNSUPPORTED_METHOD);
                            if (hr != RC_S_OK)
                            {
                                return hr;
                            }
                            continue;
                        }

                        methodItems.push_back(mi);

                        m = (int32_t)methodItems.size() - 1;
                    }

                    ICompressCoderPtr decoder = methodItems[m].m_coder;
                    ICompressSetDecoderProperties2Ptr compressSetDecoderProperties;

                    hr = decoder.QueryInterface(IID_ICompressSetDecoderProperties2, compressSetDecoderProperties.GetAddress());
                    if (hr != RC_S_OK)
                    {
                        return hr;
                    }

                    byte_t isSolid = (byte_t)((IsSolid(index) || item->IsSplitBefore()) ? 1: 0);
                    if (solidStart)
                    {
                        isSolid = false;
                        solidStart = false;
                    }

                    hr = compressSetDecoderProperties->SetDecoderProperties2(&isSolid, 1);
                    if (hr != RC_S_OK)
                    {
                        return hr;
                    }

                    commonCoder = decoder;
                }
                break;

            default:
                {
                    outStream.Release();
                    hr = extractCallback->SetOperationResult(index,RC_ARCHIVE_EXTRACT_RESULT_UNSUPPORTED_METHOD);
                    if (hr != RC_S_OK)
                    {
                        return hr;
                    }
                }
                continue;
            }

            if ((uint32_t)threadIndices.size() < numThreads)
            {
                RCRarDecoderThreadInfo &threadInfo = threads.m_threads[whichThread];
                if (threadInfo.m_isFree)
                {
                    threadInfo.m_isFree = false;
                    threadInfo.m_commonCoder = commonCoder;
                    threadInfo.m_inStream = inStream;
                    threadInfo.m_outStream = outStream;
                    threadInfo.m_extractCallback = extractCallback;

                    threadInfo.m_packSize = packSize;
                    threadInfo.m_item = item;
                    threadInfo.m_itemIndex = index ;

                    const RCRarItemExPtr& lastItem = m_items[refItem->m_itemIndex + refItem->m_numItems - 1];
                    threadInfo.m_fileCRC = lastItem->m_fileCRC;

                    threadInfo.m_outStreamSpec = outStreamSpec; 
                    if (item->IsEncrypted())
                    {
                        threadInfo.m_filterStreamSpec = filterStreamSpec;
                    }
                    
                    RCMtExtractProgress* mtProgress = new RCMtExtractProgress ;
                    ICompressProgressInfoPtr spCompressProgressInfo( mtProgress ) ;
                    mtProgress->Init(&extractProgressMT,index) ;
                    mtProgress->Reinit() ;
                    threadInfo.m_decompressProgress = spCompressProgressInfo ;


                    threadInfo.m_decompressEvent.Set();

                    decompressingCompletedEvents.push_back(threadInfo.m_decompressionCompletedEvent);
                    threadIndices.push_back(whichThread);
                    if ((uint32_t)threadIndices.size() < numThreads)
                    {
                        continue;
                    }
                }
            }

            uint32_t result = ::WaitForMultipleObjects((uint32_t)decompressingCompletedEvents.size(),
                                                       &decompressingCompletedEvents.front(),
                                                       FALSE,
                                                       INFINITE);
            int32_t t = (int32_t)(result - WAIT_OBJECT_0);

            RCRarDecoderThreadInfo &threadInfo = threads.m_threads[threadIndices[t]];
            threadInfo.m_isFree = true;
            hr = threadInfo.m_result;
            if (!IsSuccess(hr))
            {
                return hr;
            }
            RCVectorUtils::Delete(threadIndices, t);
            RCVectorUtils::Delete(decompressingCompletedEvents, t);

            if (hr != RC_S_OK)
            {
                return hr;
            }
        }

        if (decompressingCompletedEvents.size() > 0)
        {
            ::WaitForMultipleObjects((uint32_t)decompressingCompletedEvents.size(),
                                    &decompressingCompletedEvents.front(),
                                    TRUE,
                                    INFINITE);
        }        
        return RC_S_OK;
    }
#endif
}

HResult RCRarHandler::GetVolumeType(IInStream* inStream, uint64_t maxCheckStartPosition, bool& isVolume)
{
    RCRarIn archive;
    uint64_t postion = maxCheckStartPosition; 
    HResult hr = archive.Open(inStream, &postion);
    if (!IsSuccess(hr))
    {
        return hr;
    }

    RCRarInArchiveInfo archiveInfo;
    archive.GetArchiveInfo(archiveInfo);
    if (archiveInfo.IsVolume())
    {
        isVolume = true;
    }
    else
    {
        isVolume = false;
    }

    return RC_S_OK;
}

END_NAMESPACE_RCZIP
