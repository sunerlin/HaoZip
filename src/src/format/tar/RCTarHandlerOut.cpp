/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#include "format/tar/RCTarHandler.h"
#include "format/tar/RcTarUpdate.h"
#include "format/common/RCItemNameUtils.h"
#include "interface/RCPropertyID.h"
#include "filesystem/RCFileTime.h"
#include "common/RCVariant.h"
#include "common/RCStringConvert.h" 

BEGIN_NAMESPACE_RCZIP

HResult RCTarHandler::GetFileTimeType(uint32_t& type)
{
  type = RC_ARCHIVE_FILE_TIME_TYPE_UNIX;

  return RC_S_OK;
}

static HResult GetPropString(IArchiveUpdateCallback* callback, uint32_t index, RCPropertyID propId, RCStringA& res)
{
    RCVariant prop ;
    HResult hr = callback->GetProperty(index, propId, prop) ;
    if(!IsSuccess(hr))
    {
        return hr ;
    }
    if (IsStringType(prop))
    {
        res = RCStringConvert::UnicodeStringToMultiByte(prop.GetStringValue(), RC_CP_OEMCP) ;
    }
    else if (!IsEmptyType(prop))
    {
        return RC_E_INVALIDARG ;
    }
    return RC_S_OK ;
}

HResult RCTarHandler::UpdateItems(ISequentialOutStream* outStream, 
                                  uint32_t numItems,
                                  IArchiveUpdateCallback* updateCallback)
{
    if ((m_stream && !m_isGood) || m_seqStream)
    {
        return RC_E_NOTIMPL ;
    }
    
    RCVector<RCTarUpdate::RCTarUpdateItem> updateItems;

    for (uint32_t i = 0; i < numItems; i++)
    {
        RCTarUpdate::RCTarUpdateItem ui;
        int32_t newData;
        int32_t newProps;
        uint32_t indexInArchive;
        if (!updateCallback)
        {
            return RC_E_FAIL;
        }

        HResult rs = updateCallback->GetUpdateItemInfo(i, newData, newProps, indexInArchive);
        if (rs != RC_S_OK)
        {
            return rs;
        }

        ui.m_newProps = IntToBool(newProps);
        ui.m_newData = IntToBool(newData);
        ui.m_indexInArchive = indexInArchive;
        ui.m_indexInClient = i;

        if (IntToBool(newProps))
        {
            {
                RCVariant prop ;
                HResult hr = updateCallback->GetProperty(i, RCPropID::kpidIsDir, prop);
                if (!IsSuccess(hr))
                {
                    return hr ;
                }
                
                if (IsEmptyType(prop))
                {
                    ui.m_isDir = false ;
                }
                else if (!IsBooleanType(prop))
                {
                    return RC_E_INVALIDARG;
                }
                else
                {
                     bool val = GetBooleanValue(prop);
                     ui.m_isDir = (val != false);
                }
            }
            {
                RCVariant prop;
                HResult hr = updateCallback->GetProperty(i, RCPropID::kpidPosixAttrib, prop) ;
                if (!IsSuccess(hr))
                {
                    return hr ;
                }                
                if (IsEmptyType(prop))
                {
                    ui.m_mode = 0777 | (ui.m_isDir ? 0040000 : 0100000);
                }
                else if (!IsInteger64Type(prop))
                {
                    return RC_E_INVALIDARG ;
                }
                else
                {
                    ui.m_mode = (uint32_t)GetInteger64Value(prop) ;
                }
            }
            {
                RCVariant prop ;
                HResult hr = updateCallback->GetProperty(i, RCPropID::kpidMTime, prop) ;
                if (!IsSuccess(hr))
                {
                    return hr ;
                }
                if (IsEmptyType(prop))
                {
                    ui.m_time = 0;
                }
                if (!IsUInt64Type(prop))
                {
                    return RC_E_INVALIDARG;
                }
                uint64_t val = GetUInt64Value(prop) ;
                RC_FILE_TIME utcTime ;
                RCFileTime::ConvertToFileTime(val, utcTime) ;                
                if (!RCFileTime::FileTimeToUnixTime(utcTime, ui.m_time))
                {
                    ui.m_time = 0 ;
                }
            }
            {
                RCVariant prop ;
                HResult hr = updateCallback->GetProperty(i, RCPropID::kpidPath, prop) ;
                if (!IsSuccess(hr))
                {
                    return hr ;
                }
                if (IsStringType(prop))
                {
                    ui.m_name = RCStringConvert::UnicodeStringToMultiByte(RCItemNameUtils::MakeLegalName(prop.GetStringValue()), RC_CP_OEMCP);
                }
                else if (!IsEmptyType(prop))
                {
                    return RC_E_INVALIDARG;
                }
                if (ui.m_isDir)
                {
                    ui.m_name += '/';
                }
            }
            HResult hr = GetPropString(updateCallback, i, RCPropID::kpidUser, ui.m_user) ;
            if (!IsSuccess(hr))
            {
                return hr ;
            }
            
            hr = GetPropString(updateCallback, i, RCPropID::kpidGroup, ui.m_group) ;
            if (!IsSuccess(hr))
            {
                return hr ;
            }
        }

        if (IntToBool(newData))
        {
            RCVariant prop;
            HResult rs = updateCallback->GetProperty(i, RCPropID::kpidSize, prop);
            if (rs != RC_S_OK)
            {
                return rs;
            }
            if (!IsUInt64Type(prop))
            {
                return RC_E_INVALIDARG;
            }
            
            ui.m_size = GetUInt64Value(prop);
        }
        updateItems.push_back(ui);
    }

    return RCTarUpdate::UpdateArchive(m_stream.Get(),
                                      outStream,
                                      m_items,
                                      updateItems,
                                      updateCallback,
                                      m_compressCodecsInfo);
}

END_NAMESPACE_RCZIP