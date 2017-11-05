/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "format/7z/RC7zHandler.h"
#include "format/common/RCItemNameUtils.h"
#include "format/common/RCParseProperties.h"
#include "format/7z/RC7zOutArchive.h"
#include "format/7z/RC7zUpdate.h"
#include "common/RCStringUtil.h"

/////////////////////////////////////////////////////////////////
//RC7zHandlerOut class implementation

BEGIN_NAMESPACE_RCZIP

#ifndef RC_STATIC_SFX

static HResult GetTime(IArchiveUpdateCallback *updateCallback, int32_t index, bool writeTime, RCPropertyID propID, uint64_t& ft, bool& ftDefined)
{
    if(updateCallback == NULL)
    {
        return RC_E_INVALIDARG ;
    }
    ft = 0;
    ftDefined = false;
    if (!writeTime)
    {
        return RC_S_OK;
    }
    RCVariant prop;
    HResult hr = updateCallback->GetProperty(index, propID,prop) ;
    if(!IsSuccess(hr))
    {
        return hr ;
    }
    if (IsUInt64Type(prop))
    {
        ft = GetUInt64Value(prop) ;
        ftDefined = true;
    }
    else if(!IsEmptyType(prop))
    {
        return RC_E_INVALIDARG ;
    }
    return RC_S_OK;
}

static HResult GetBindInfoPart(RCString& srcString, uint32_t& coder, uint32_t& stream)
{
    stream = 0;
    int32_t index = RCParseProperties::ParseStringToUInt32(srcString, coder);
    if (index == 0)
    {
        return RC_E_INVALIDARG ;
    }
    srcString.erase(RCString::size_type(0), RCString::size_type(index));
    if (srcString[0] == _T('S'))
    {
        srcString.erase(srcString.begin());
        int32_t index = RCParseProperties::ParseStringToUInt32(srcString, stream);
        if (index == 0)
        {
            return RC_E_INVALIDARG ;
        }
        srcString.erase(RCString::size_type(0), RCString::size_type(index));
    }
    return RC_S_OK;
}

static HResult GetBindInfo(RCString& srcString, RC7zBind& bind)
{
    HResult hr = GetBindInfoPart(srcString, bind.m_outCoder, bind.m_outStream) ;
    if(!IsSuccess(hr))
    {
        return hr ;
    }
    if (srcString[0] != _T(':'))
    {
        return RC_E_INVALIDARG;
    }
    srcString.erase(srcString.begin());
    hr = GetBindInfoPart(srcString, bind.m_inCoder, bind.m_inStream) ;
    if(!IsSuccess(hr))
    {
        return hr ;
    }
    if (!srcString.empty())
    {
        return RC_E_INVALIDARG ;
    }
    return RC_S_OK;
}


HResult RC7zHandler::UpdateItems(ISequentialOutStream* outStream, 
                                 uint32_t numItems, 
                                 IArchiveUpdateCallback* updateCallback)
{
    try
    {

        const RC7zArchiveDatabaseEx *db = 0;
#ifdef _7Z_VOL
        if(_volumes.size() > 1)
        {
            return RC_E_FAIL;
        }
        const CVolume *volume = 0;
        if (_volumes.size() == 1)
        {
            volume =& _volumes.Front();
            db =& volume->Database;
        }
#else
        if (m_inStream != 0)
        {
            db =& m_db;
        }
#endif

        RCVector<RC7zUpdateItemPtr> updateItems;
        uint64_t totalSize = 0;

        for (uint32_t i = 0; i < numItems; i++)
        {
            int32_t newData = 0 ;
            int32_t newProperties = 0 ;
            uint32_t indexInArchive = 0 ;
            if (!updateCallback)
            {
                return RC_E_FAIL;
            }
            HResult hr = updateCallback->GetUpdateItemInfo(i, newData, newProperties, indexInArchive) ;
            if(!IsSuccess(hr))
            {
                return hr ;
            }
            RC7zUpdateItemPtr ui(new RC7zUpdateItem);
            if (!ui)
            {
                RC_S_FALSE;
            }
            ui->m_newProperties = (newProperties != 0);
            ui->m_newData = (newData != 0);
            ui->m_indexInArchive = indexInArchive;
            ui->m_indexInClient = i;
            ui->m_isAnti = false;
            ui->m_size = 0;

            if (ui->m_indexInArchive != -1)
            {
                if (db == NULL || ui->m_indexInArchive >= (int32_t)db->m_files.size())
                {
                    return RC_E_INVALIDARG;
                }
                const RC7zFileItemPtr& fi = db->m_files[ui->m_indexInArchive];
                ui->m_name = fi->m_name;
                ui->m_isDir = fi->m_isDir;
                ui->m_size = fi->m_size;
                ui->m_isAnti = db->IsItemAnti(ui->m_indexInArchive);

                ui->m_cTimeDefined = db->m_cTime.GetItem(ui->m_indexInArchive, ui->m_cTime);
                ui->m_aTimeDefined = db->m_aTime.GetItem(ui->m_indexInArchive, ui->m_aTime);
                ui->m_mTimeDefined = db->m_mTime.GetItem(ui->m_indexInArchive, ui->m_mTime);
            }

            if (ui->m_newProperties)
            {
                bool nameIsDefined;
                bool folderStatusIsDefined;
                {
                    RCVariant prop ;
                    HResult hr = updateCallback->GetProperty(i, RCPropID::kpidAttrib, prop) ;
                    if(!IsSuccess(hr))
                    {
                        return hr ;
                    }
                    if (IsEmptyType(prop))
                    {
                        ui->m_attribDefined = false;
                    }
                    else if(!IsInteger64Type(prop))
                    {
                        return RC_E_INVALIDARG;
                    }
                    else
                    {
                        ui->m_attrib = static_cast<uint32_t>(GetInteger64Value(prop));
                        ui->m_attribDefined = true ;
                    }
                }

                // we need MTime to sort files.
                HResult hr = GetTime(updateCallback, i, m_writeCTime, RCPropID::kpidCTime, ui->m_cTime, ui->m_cTimeDefined) ;
                if(!IsSuccess(hr))
                {
                    return hr ;
                }
                hr = GetTime(updateCallback, i, m_writeATime, RCPropID::kpidATime, ui->m_aTime, ui->m_aTimeDefined) ;
                if(!IsSuccess(hr))
                {
                    return hr ;
                }
                hr = GetTime(updateCallback, i, true, RCPropID::kpidMTime, ui->m_mTime, ui->m_mTimeDefined) ;
                if(!IsSuccess(hr))
                {
                    return hr ;
                }

                {
                    RCVariant prop ;
                    hr = updateCallback->GetProperty(i, RCPropID::kpidPath,prop) ;
                    if(!IsSuccess(hr))
                    {
                        return hr ;
                    }
                    if (IsEmptyType(prop))
                    {
                        nameIsDefined = false;
                    }
                    else if (!IsStringType(prop))
                    {
                        return RC_E_INVALIDARG;
                    }
                    else
                    {
                        ui->m_name = RCItemNameUtils::MakeLegalName(GetStringValue(prop));
                        nameIsDefined = true;
                    }
                }
                {
                    RCVariant prop;
                    HResult hr = updateCallback->GetProperty(i, RCPropID::kpidIsDir,prop) ;
                    if(!IsSuccess(hr))
                    {
                        return hr ;
                    }
                    if (IsEmptyType(prop))
                    {
                        folderStatusIsDefined = false;
                    }
                    else if(!IsBooleanType(prop))
                    {
                        return RC_E_INVALIDARG;
                    }
                    else
                    {
                        ui->m_isDir = GetBooleanValue(prop) ;
                        folderStatusIsDefined = true;
                    }
                }

                {
                    RCVariant prop ;
                    HResult hr = updateCallback->GetProperty(i, RCPropID::kpidIsAnti,prop) ;
                    if(!IsSuccess(hr))
                    {
                        return hr ;
                    }
                    if (IsEmptyType(prop))
                    {
                        ui->m_isAnti = false;
                    }
                    else if(!IsBooleanType(prop))
                    {
                        return RC_E_INVALIDARG;
                    }
                    else
                    {
                        ui->m_isAnti = GetBooleanValue(prop) ;
                    }
                }

                if (ui->m_isAnti)
                {
                    ui->m_attribDefined = false;
                    ui->m_cTimeDefined = false;
                    ui->m_aTimeDefined = false;
                    ui->m_mTimeDefined = false;
                    ui->m_size = 0;
                }

                if (!folderStatusIsDefined && ui->m_attribDefined)
                {
                    ui->SetDirStatusFromAttrib();
                }
            }

            if (ui->m_newData)
            {
                RCVariant prop ;
                HResult hr = updateCallback->GetProperty(i, RCPropID::kpidSize, prop) ;
                if(!IsSuccess(hr))
                {
                    return hr ;
                }
                if(!IsInteger64Type(prop))
                {
                    return RC_E_INVALIDARG ;
                }
                ui->m_size = static_cast<uint64_t>(GetInteger64Value(prop));
                if (ui->m_size != 0 && ui->m_isAnti)
                {
                    return RC_E_INVALIDARG ;
                }
                totalSize += ui->m_size;
            }
            updateItems.push_back(ui);
        }

        RC7zCompressionMethodMode methodMode, headerMethod;
        HResult hr = SetCompressionMethod(methodMode, headerMethod, totalSize) ;
        if(!IsSuccess(hr))
        {
            return hr ;
        }
#ifdef COMPRESS_MT
        methodMode.m_numThreads = m_numThreads;
        headerMethod.m_numThreads = 1;
#endif

        hr = SetPassword(methodMode, updateCallback) ;
        if(!IsSuccess(hr))
        {
            return hr ;
        }

        bool compressMainHeader = m_compressHeaders;  // check it

        bool encryptHeaders = false;

        if (methodMode.m_passwordIsDefined)
        {
            if (m_encryptHeadersSpecified)
            {
                encryptHeaders = m_encryptHeaders;
            }
            else
            {
                encryptHeaders = m_passwordIsDefined;
            }

            compressMainHeader = true;
            if(encryptHeaders)
            {
                headerMethod.m_passwordIsDefined = methodMode.m_passwordIsDefined ;
                headerMethod.m_password = methodMode.m_password ;
            }
        }

        if (numItems < 2)
        {
            compressMainHeader = false;
        }

        RC7zUpdateOptions options;
        options.m_method = &methodMode ;
        options.m_headerMethod = (m_compressHeaders || encryptHeaders) ?& headerMethod : 0;
        options.m_useFilters = m_level != 0 && m_autoFilter;
        options.m_maxFilter = m_level >= 8;

        options.m_headerOptions.m_compressMainHeader = compressMainHeader;
        options.m_headerOptions.m_writeCTime = m_writeCTime;
        options.m_headerOptions.m_writeATime = m_writeATime;
        options.m_headerOptions.m_writeMTime = m_writeMTime;

        options.m_numSolidFiles = m_numSolidFiles;
        options.m_numSolidBytes = m_numSolidBytes;
        options.m_solidExtension = m_solidExtension;
        options.m_removeSfxBlock = m_removeSfxBlock;
        options.m_volumeMode = m_volumeMode;

        RC7zOutArchive archive;
        RC7zArchiveDatabase newDatabase;
        HResult res = RC7zUpdate::Update(m_codecsInfo.Get(),
#ifdef _7Z_VOL
                                         volume ? volume->Stream: 0,
                                         volume ? db : 0,
#else
                                        m_inStream.Get(),
                                        db,
#endif
                                        updateItems,
                                        archive, 
                                        newDatabase, 
                                        outStream, 
                                        updateCallback,
                                        options) ;

        if(!IsSuccess(res))
        {
            return res ;
        }

        updateItems.clear();

        return archive.WriteDatabase(m_codecsInfo.Get(),
                                     newDatabase, 
                                     options.m_headerMethod, 
                                     options.m_headerOptions) ;

    }
    catch(...)
    {
        return RC_S_FALSE ;
    }
}

HResult RC7zHandler::GetFileTimeType(uint32_t& type)
{
    type = RC_ARCHIVE_FILE_TIME_TYPE_WINDOWS;
    return RC_S_OK ;
}

HResult RC7zHandler::SetProperties(const RCPropertyNamePairArray& propertyArray)
{
    try
    {
        m_binds.clear();
        BeforeSetProperty();
        RCPropertyNamePairArray::const_iterator pos = propertyArray.begin() ;
        for (; pos != propertyArray.end(); ++pos)
        {
            RCString name = pos->first ;
            RCStringUtil::MakeUpper(name) ;
            if (name.empty())
            {
                return RC_E_INVALIDARG;
            }
            const RCVariant& value = pos->second ;
            if (name[0] == _T('B'))
            {
                name.erase(name.begin()) ;
                RC7zBind bind ;
                HResult hr = GetBindInfo(name, bind) ;
                if(!IsSuccess(hr))
                {
                    return hr ;
                }
                m_binds.push_back(bind);
                continue;
            }

            HResult hr = SetProperty(name, value) ;
            if(!IsSuccess(hr))
            {
                return hr ;
            }
        }
        return RC_S_OK;
    }
    catch(...)
    {
        return RC_S_FALSE ;
    }
}

#endif //RC_STATIC_SFX

END_NAMESPACE_RCZIP
