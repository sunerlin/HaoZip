/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "format/zip/RCZipHandler.h"
#include "format/zip/RCZipUpdate.h"
#include "format/zip/RCZipDefs.h"
#include "filesystem/RCFileTime.h"
#include "format/common/RCItemNameUtils.h"
#include "interface/Ipassword.h"
#include "crypto/WzAES/RCWzAESDefs.h"

/////////////////////////////////////////////////////////////////
//RCZipHandler class implementation

BEGIN_NAMESPACE_RCZIP

HResult RCZipHandler::GetFileTimeType(uint32_t& type)
{
    type = RC_ARCHIVE_FILE_TIME_TYPE_DOS;
    return RC_S_OK;
}

static HResult GetTime(IArchiveUpdateCallback* updateCallback,
                       uint32_t index,
                       RCPropertyID propID,
                       RC_FILE_TIME& filetime)
{
    filetime.u32HighDateTime = 0;
    filetime.u32LowDateTime = 0;

    RCVariant prop;

    HResult hr = updateCallback->GetProperty(index, propID, prop);
    if (hr != RC_S_OK)
    {
        return hr;
    }

    if (IsUInt64Type(prop))
    {
        RCFileTime::ConvertToFileTime(GetUInt64Value(prop), filetime);
    }
    else if (!IsEmptyType(prop))
    {
        return RC_E_INVALIDARG;
    }
    return RC_S_OK;
}

HResult RCZipHandler::UpdateItems(ISequentialOutStream* outStream, 
                                  uint32_t numItems, 
                                  IArchiveUpdateCallback* updateCallback)
{
    HResult hr;
    RCVector<RCZipUpdate::RCZipUpdateItemPtr> updateItems;
    bool thereAreAesUpdates = false;
    uint64_t totalSize = 0;

    for (uint32_t i = 0; i < numItems; i++)
    {
        RCZipUpdate::RCZipUpdateItemPtr ui(new RCZipUpdate::RCZipUpdateItem);
        if (!ui)
        {
            return RC_S_FALSE;
        }

        int32_t newData;
        int32_t newProperties;
        uint32_t indexInArchive;
        if (!updateCallback)
        {
            return RC_E_FAIL;
        }

        hr = updateCallback->GetUpdateItemInfo(i, newData, newProperties, indexInArchive);
        if (hr != RC_S_OK)
        {
            return hr;
        }

        ui->m_newProperties = IntToBool(newProperties);
        ui->m_newData = IntToBool(newData);
        ui->m_indexInArchive = indexInArchive;
        ui->m_indexInClient = i;

        bool existInArchive = (indexInArchive != (uint32_t)-1);
        if (existInArchive && newData)
        {
            if (m_zipItems[indexInArchive]->IsAesEncrypted())
            {
                thereAreAesUpdates = true;
            }
        }

        if (IntToBool(newProperties))
        {
            RCString name;
            {
                RCVariant prop;
                hr = updateCallback->GetProperty(i, RCPropID::kpidAttrib, prop);
                if (hr != RC_S_OK)
                {
                    return hr;
                }

                if (IsEmptyType(prop))
                {
                    ui->m_attributes = 0;
                }
                else if (!IsUInt64Type(prop))
                {
                    return RC_E_INVALIDARG;
                }
                else
                {
                    ui->m_attributes = (uint32_t)GetUInt64Value(prop);
                }
            }

            {
                RCVariant prop;
                hr = (updateCallback->GetProperty(i, RCPropID::kpidPath, prop));
                if (hr != RC_S_OK)
                {
                    return hr;
                }

                if (IsEmptyType(prop))
                {
                    name.clear();
                }
                else if (!IsStringType(prop))
                {
                    return RC_E_INVALIDARG;
                }
                else
                {
                    name = GetStringValue(prop);
                }
            }
            {
                RCVariant prop;
                hr = updateCallback->GetProperty(i, RCPropID::kpidIsDir, prop);
                if (hr != RC_S_OK)
                {
                    return hr;
                }

                if (IsEmptyType(prop))
                {
                    ui->m_isDir = false;
                }
                else if (!IsBooleanType(prop))
                {
                    return RC_E_INVALIDARG;
                }
                else
                {
                    bool val = GetBooleanValue(prop);
                    ui->m_isDir = (val != false);
                }
            }

            {
                RCVariant prop;
                hr = updateCallback->GetProperty(i, RCPropID::kpidTimeType, prop);
                if (IsUInt64Type(prop))
                {
                    ui->m_ntfsTimeIsDefined = (GetUInt64Value(prop) == RC_ARCHIVE_FILE_TIME_TYPE_WINDOWS);
                }
                else
                {
                    ui->m_ntfsTimeIsDefined = m_writeNtfsTimeExtra;
                }
            }

            hr = GetTime(updateCallback, i, RCPropID::kpidMTime, ui->m_ntfsMTime);
            if (hr != RC_S_OK)
            {
                return hr;
            }

            hr = GetTime(updateCallback, i, RCPropID::kpidATime, ui->m_ntfsATime);
            if (hr != RC_S_OK)
            {
                return hr;
            }

            hr = GetTime(updateCallback, i, RCPropID::kpidCTime, ui->m_ntfsCTime);
            if (hr != RC_S_OK)
            {
                return hr;
            }

            {
                RC_FILE_TIME localFileTime;

                if (ui->m_ntfsMTime.u32HighDateTime != 0 ||
                    ui->m_ntfsMTime.u32LowDateTime  != 0)
                {
                    if (!RCFileTime::FileTimeToLocalFileTime(ui->m_ntfsMTime, localFileTime))
                    {
                        return RC_E_INVALIDARG;
                    }
                }

                RCFileTime::FileTimeToDosTime(localFileTime, ui->m_time);
            }

            name = RCItemNameUtils::MakeLegalName(name);
            bool needSlash = ui->m_isDir;
            const char_t kSlash = _T('/');

            if (!name.empty())
            {
                if (name[name.length() - 1] == kSlash)
                {
                    if (!ui->m_isDir)
                    {
                        return RC_E_INVALIDARG;
                    }
                    needSlash = false;
                }
            }

            if (needSlash)
            {
                name += kSlash;
            }

            bool tryUtf8 = true;
            if (m_forseLocal || !m_forseUtf8)
            {
                bool defaultCharWasUsed;
                ui->m_name = RCStringConvert::UnicodeStringToMultiByte(name, RC_CP_OEMCP, '_', defaultCharWasUsed);
                tryUtf8 = (!m_forseLocal && (defaultCharWasUsed ||
                    RCStringConvert::MultiByteToUnicodeString(ui->m_name, RC_CP_OEMCP) != name));
            }

            if (tryUtf8)
            {
                int32_t i;
                for (i = 0; i < (int32_t)name.length() && (unsigned)name[i] < 0x80; i++);

                ui->m_isUtf8 = (i != (int32_t)name.length());
                if (!RCStringConvert::ConvertUnicodeToUTF8(name.c_str(), name.size(), ui->m_name))
                {
                    return RC_E_INVALIDARG;
                }
            }

            if (ui->m_name.length() >= (1<<16))
            {
                return RC_E_INVALIDARG;
            }

            ui->m_indexInClient = i;
            /*
            if (existInArchive)
            {
            const CItemEx &itemInfo = m_Items[indexInArchive];
            // ui.Commented = itemInfo.IsCommented();
            ui.Commented = false;
            if (ui.Commented)
            {
            ui.CommentRange.Position = itemInfo.GetCommentPosition();
            ui.CommentRange.Size  = itemInfo.CommentSize;
            }
            }
            else
            ui.Commented = false;
            */
        }

        if (IntToBool(newData))
        {
            uint64_t size;
            {
                RCVariant prop;
                hr = updateCallback->GetProperty(i, RCPropID::kpidSize, prop);
                if (!IsUInt64Type(prop))
                {
                    return RC_E_INVALIDARG;
                }
                size = GetUInt64Value(prop);
            }
            ui->m_size = size;
            totalSize += size;
        }
        updateItems.push_back(ui);
    }

    ICryptoGetTextPassword2Ptr getTextPassword;

    {
        IArchiveUpdateCallbackPtr udateCallBack2(updateCallback);
        udateCallBack2.QueryInterface(IID_ICryptoGetTextPassword2, getTextPassword.GetAddress());
    }

    RCZipCompressionMethodMode options;

    if (getTextPassword)
    {
        RCString password;
        int32_t passwordIsDefined = 0 ;
        hr = getTextPassword->CryptoGetTextPassword2(&passwordIsDefined, password);
        if (hr != RC_S_OK)
        {
            return hr;
        }

        options.m_passwordIsDefined = IntToBool(passwordIsDefined);
        if (options.m_passwordIsDefined)
        {
            options.m_isAesMode = (m_forceAesMode ? m_isAesMode : thereAreAesUpdates);
            options.m_aesKeyMode = m_aesKeyMode;

            //if (!IsAsciiString(password))
            //{
            //    return RC_E_INVALIDARG;
            //}
            
            RCStringA compressPassword = RCStringConvert::UnicodeStringToMultiByte(password, RC_CP_OEMCP) ;
            if (options.m_isAesMode)
            {
                if (compressPassword.size() > RCWzAESDefs::s_kPasswordSizeMax)
                {
                    //密码长度超过限制
                    return RC_E_Password_Too_Long;
                }
            }
            options.m_password = compressPassword ;
        }
    }
    else
    {
        options.m_passwordIsDefined = false;
    }

    int32_t level = m_level;
    if (level < 0)
    {
        level = 5;
    }

    byte_t mainMethod;
    if (m_mainMethod < 0)
    {
        mainMethod = (byte_t)(((level == 0) ?
            RCZipHeader::NCompressionMethod::kStored :
        RCZipHeader::NCompressionMethod::kDeflated));
    }
    else
    {
        mainMethod = (byte_t)m_mainMethod;
    }

    options.m_methodSequence.push_back(mainMethod);
    if (mainMethod != RCZipHeader::NCompressionMethod::kStored)
    {
        options.m_methodSequence.push_back(RCZipHeader::NCompressionMethod::kStored);
    }

    bool isDeflate = (mainMethod == RCZipHeader::NCompressionMethod::kDeflated) ||
        (mainMethod == RCZipHeader::NCompressionMethod::kDeflated64);
    bool isLZMA = (mainMethod == RCZipHeader::NCompressionMethod::kLZMA);
    bool isLz = (isLZMA || isDeflate);
    bool isBZip2 = (mainMethod == RCZipHeader::NCompressionMethod::kBZip2);

    options.m_numPasses    = m_numPasses;
    options.m_dicSize      = m_dicSize;
    options.m_numFastBytes = m_numFastBytes;
    options.m_numMatchFinderCycles = m_numMatchFinderCycles;
    options.m_numMatchFinderCyclesDefined = m_numMatchFinderCyclesDefined;
    options.m_algo = m_algo;

#ifdef COMPRESS_MT
    options.m_numThreads = m_numThreads;
#endif

    if (isLz)
    {
        if (isDeflate)
        {
            if (options.m_numPasses == 0xFFFFFFFF)
                options.m_numPasses = (level >= 9 ? RCZipDefs::s_deflateNumPassesX9 :
                (level >= 7 ? RCZipDefs::s_deflateNumPassesX7 :
                RCZipDefs::s_deflateNumPassesX1));

            if (options.m_numFastBytes == 0xFFFFFFFF)
                options.m_numFastBytes = (level >= 9 ? RCZipDefs::s_deflateNumFastBytesX9 :
                (level >= 7 ? RCZipDefs::s_deflateNumFastBytesX7 :
                RCZipDefs::s_deflateNumFastBytesX1));
        }
        else if (isLZMA)
        {
            if (options.m_dicSize == 0xFFFFFFFF)
                options.m_dicSize =
                (level >= 9 ? RCZipDefs::s_lzmaDicSizeX9 :
                (level >= 7 ? RCZipDefs::s_lzmaDicSizeX7 :
                (level >= 5 ? RCZipDefs::s_lzmaDicSizeX5 :
                (level >= 3 ? RCZipDefs::s_lzmaDicSizeX3 :
                RCZipDefs::s_lzmaDicSizeX1))));

            if (options.m_numFastBytes == 0xFFFFFFFF)
                options.m_numFastBytes = (level >= 7 ? RCZipDefs::s_lzmaNumFastBytesX7 :
                RCZipDefs::s_lzmaNumFastBytesX1);

            options.m_matchFinder =
                (level >= 5 ? RCZipDefs::s_lzmaMatchFinderX5 :
                RCZipDefs::s_lzmaMatchFinderX1);

            if ((totalSize > 0) &&
                (options.m_dicSize > RCZipDefs::s_lzmaDicSizeX1) &&
                (totalSize < options.m_dicSize/2))
            {
                if (totalSize > RCZipDefs::s_lzmaDicSizeX3)
                {
                    //压缩总大小大于1M以1M为基
                    options.m_dicSize = (uint32_t)totalSize*2 & (((uint64_t)1<<32)-RCZipDefs::s_lzmaDicSizeX3);
                }
                else
                {
                    //压缩总大小大于64K以64K为基
                    options.m_dicSize = (uint32_t)totalSize*2 & (((uint64_t)1<<32)-RCZipDefs::s_lzmaDicSizeX1);
                }
            }
        }

        if (options.m_algo == 0xFFFFFFFF)
            options.m_algo = (level >= 5 ? RCZipDefs::s_lzAlgoX5 :
            RCZipDefs::s_lzAlgoX1);
    }
    if (isBZip2)
    {
        if (options.m_numPasses == 0xFFFFFFFF)
        {
            options.m_numPasses = (level >= 9 ? RCZipDefs::s_bzip2NumPassesX9 :
            (level >= 7 ? RCZipDefs::s_bzip2NumPassesX7 :
            RCZipDefs::s_bzip2NumPassesX1));
        }

        if (options.m_dicSize == 0xFFFFFFFF)
        {
            options.m_dicSize = (level >= 5 ? RCZipDefs::s_bzip2DicSizeX5 :
            (level >= 3 ? RCZipDefs::s_bzip2DicSizeX3 :
            RCZipDefs::s_bzip2DicSizeX1));
        }
    }
#ifdef COMPRESS_MT
    bool isOpen = false;
    if (m_zipInArchive.size() > 0)
    {
        isOpen = m_zipInArchive[0]->IsOpen();
    }
#endif 

    return RCZipUpdate::UpdateArchive(m_compressCodecsInfo.Get(),
                                      m_zipItems,
                                      updateItems,
                                      outStream,
#ifdef COMPRESS_MT
                                      isOpen ? m_zipInArchive[0].get() : NULL,
#else
                                      m_zipInArchive.IsOpen() ? &m_zipInArchive : NULL,

#endif
                                      &options,
                                      updateCallback);
}

END_NAMESPACE_RCZIP
