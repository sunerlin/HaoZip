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
#include "format/zip/RCZipDefs.h"
#include "format/common/RCParseProperties.h"
#include "common/RCStringUtil.h"

/////////////////////////////////////////////////////////////////
//RCZipHandler class implementation

BEGIN_NAMESPACE_RCZIP

RCVector<RCZipExtFilterPtr> RCZipHandler::s_extFilters;

RCZipFileFilter RCZipHandler::s_fileFilter;

RCZipHandler::RCZipHandler():
    m_hasExtFilters(false),
    m_hasFileFilters(false)
{
    InitMethodProperties();
}

RCZipHandler::~RCZipHandler()
{
    if(m_hasExtFilters)
    {
        s_extFilters.clear();
    }
    if(m_hasFileFilters)
    {
        s_fileFilter.m_fileList.erase();
    }
}

HResult RCZipHandler::SetCompressCodecsInfo(ICompressCodecsInfo* compressCodecsInfo)
{
    if (!compressCodecsInfo)
    {
        return RC_S_FALSE;
    }

    m_compressCodecsInfo = compressCodecsInfo;

    return RC_S_OK;
}

HResult RCZipHandler::SetProperties(const RCPropertyNamePairArray& propertyArray)
{
    HResult hr;

#ifdef COMPRESS_MT
    const uint32_t numProcessors = RCSystemUtils::GetNumberOfProcessors();
    m_numThreads = numProcessors;
#endif

    InitMethodProperties();

    for (int32_t i = 0; i < (int32_t)propertyArray.size(); i++)
    {
        RCString name = propertyArray[i].first;
        RCStringUtil::MakeUpper(name);

        if (name.empty())
        {
            return RC_E_INVALIDARG;
        }

        const RCVariant& prop = propertyArray[i].second;

        if (name[0] == _T('X'))
        {
            uint32_t level = 9;
            hr = RCParseProperties::ParsePropValue(RCStringUtil::Mid(name, 1), prop, level);
            if (hr != RC_S_OK)
            {
                return hr;
            }

            m_level = level;
            continue;
        }
        else if (name == _T("M"))
        {
            if (IsStringType(prop))
            {
                RCString valueString = GetStringValue(prop);
                RCStringUtil::MakeUpper(valueString);

                if (valueString == L"COPY")
                {
                    m_mainMethod = RCZipHeader::NCompressionMethod::kStored;
                }
                else if (valueString == L"DEFLATE")
                {
                    m_mainMethod = RCZipHeader::NCompressionMethod::kDeflated;
                }
                else if (valueString == L"DEFLATE64")
                {
                    m_mainMethod = RCZipHeader::NCompressionMethod::kDeflated64;
                }
                else if (valueString == L"BZIP2")
                {
                    m_mainMethod = RCZipHeader::NCompressionMethod::kBZip2;
                }     
                else if (valueString == L"LZMA") 
                {
                    m_mainMethod = RCZipHeader::NCompressionMethod::kLZMA;
                }
                else
                {
                    return RC_E_INVALIDARG;
                }
            }
            else if (IsUInt64Type(prop))
            {
                uint64_t method = GetUInt64Value(prop);

                switch (method)
                {
                case RCZipHeader::NCompressionMethod::kStored:
                case RCZipHeader::NCompressionMethod::kDeflated:
                case RCZipHeader::NCompressionMethod::kDeflated64:
                case RCZipHeader::NCompressionMethod::kBZip2:
                case RCZipHeader::NCompressionMethod::kLZMA:
                    {
                        m_mainMethod = (byte_t)method;
                    }
                    break;

                default:
                    {
                        return RC_E_INVALIDARG;
                    }
                }
            }
            else
            {
                return RC_E_INVALIDARG;
            }
        }
        else if (RCStringUtil::Left(name, 2) == _T("EM"))
        {
            if (IsStringType(prop))
            {
                RCString valueString = GetStringValue(prop);
                RCStringUtil::MakeUpper(valueString);

                if (RCStringUtil::Left(valueString, 3) == _T("AES"))
                {
                    valueString = RCStringUtil::Mid(valueString, 3);
                    if (valueString == _T("128"))
                    {
                        m_aesKeyMode = 1;
                    }
                    else if (valueString == _T("192"))
                    {
                        m_aesKeyMode = 2;
                    }
                    else if (valueString == _T("256") || valueString.empty())
                    {
                        m_aesKeyMode = 3;
                    }
                    else
                    {
                        return RC_E_INVALIDARG;
                    }

                    m_isAesMode = true;
                    m_forceAesMode = true;
                }
                else if (valueString == _T("ZIPCRYPTO"))
                {
                    m_isAesMode = false;                            
                    m_forceAesMode = true;
                }
                else
                {
                    return RC_E_INVALIDARG;
                }
            }
            else
            {
                return RC_E_INVALIDARG;
            }
        }
        else if (name[0] == _T('D'))
        {
            uint32_t dicSize = RCZipDefs::s_bzip2DicSizeX5;

            hr = RCParseProperties::ParsePropDictionaryValue(RCStringUtil::Mid(name, 1), prop, dicSize);
            if (hr != RC_S_OK)
            {
                return hr;
            }

            m_dicSize = dicSize;
        }
        else if (RCStringUtil::Left(name, 4) == _T("PASS"))
        {
            uint32_t num = RCZipDefs::s_deflateNumPassesX9;

            hr = RCParseProperties::ParsePropValue(RCStringUtil::Mid(name, 4), prop, num);
            if (hr != RC_S_OK)
            {
                return hr;
            }

            m_numPasses = num;
        }
        else if (RCStringUtil::Left(name, 2) == _T("FB"))
        {
            uint32_t num = RCZipDefs::s_deflateNumFastBytesX9;

            hr = RCParseProperties::ParsePropValue(RCStringUtil::Mid(name, 2), prop, num);
            if (hr != RC_S_OK)
            {
                return hr;
            }

            m_numFastBytes = num;
        }
        else if (RCStringUtil::Left(name, 2) == _T("MC"))
        {
            uint32_t num = 0xFFFFFFFF;

            hr = RCParseProperties::ParsePropValue(RCStringUtil::Mid(name, 2), prop, num);
            if (hr != RC_S_OK)
            {
                return hr;
            }

            m_numMatchFinderCycles = num;
            m_numMatchFinderCyclesDefined = true;
        }
        else if (RCStringUtil::Left(name, 2) == _T("MT"))
        {
#ifdef COMPRESS_MT
            hr = RCParseProperties::ParseMtProp(RCStringUtil::Mid(name, 2), prop, numProcessors, m_numThreads);
            if (hr != RC_S_OK)
            {
                return hr;
            }
#endif
        }
        else if (RCStringUtil::Left(name, 1) == _T("A"))
        {
            uint32_t num = RCZipDefs::s_lzAlgoX5;

            hr = RCParseProperties::ParsePropValue(RCStringUtil::Mid(name, 1), prop, num);
            if (hr != RC_S_OK)
            {
                return hr;
            }

            m_algo = num;
        }
        else if (RCStringUtil::CompareNoCase(name, _T("TC")) == 0)
        {
            hr = RCParseProperties::SetBoolProperty(m_writeNtfsTimeExtra, prop);
            if (hr != RC_S_OK)
            {
                return hr;
            }
        }
        else if (RCStringUtil::CompareNoCase(name, _T("CL")) == 0)
        {
            hr = RCParseProperties::SetBoolProperty(m_forseLocal, prop);
            if (hr != RC_S_OK)
            {
                return hr;
            }

            if (m_forseLocal)
            {
                m_forseUtf8 = false;
            }
        }
        else if (RCStringUtil::CompareNoCase(name, _T("CU")) == 0)
        {
            hr = RCParseProperties::SetBoolProperty(m_forseUtf8, prop);
            if (hr != RC_S_OK)
            {
                return hr;
            }

            if (m_forseUtf8)
            {
                m_forseLocal = false;
            }
        }
        else if (RCStringUtil::CompareNoCase(name, _T("EF")) == 0)
        {
            if (IsStringType(prop))
            {
                s_extFilters.clear() ;
                m_hasExtFilters = true ;
                bool last = false;
                RCString valueString = GetStringValue(prop);

                while (!last)
                {
                    RCString::size_type size = valueString.size();
                    RCString::size_type pos = valueString.find_last_of(';');
                    RCString group;

                    if (pos != RCString::npos)
                    {
                        group = RCStringUtil::Mid(valueString, (int32_t)pos+1);
                        if (pos < size)
                        {
                            valueString.erase(pos, size - pos);
                        }

                        pos = group.find_last_of('=');
                        if (pos == RCString::npos)
                        {
                            return RC_E_INVALIDARG;
                        }
                    }
                    else
                    {
                        last = true;
                        group = valueString;
                        pos = group.find_last_of('=');
                        if (pos == RCString::npos)
                        {
                            return RC_E_INVALIDARG;
                        }
                    }

                    RCMethodID methodID = 0 ;
                    int32_t level = -1;
                    RCString ext = RCStringUtil::Left(group, (int32_t)pos);
                    RCString name = RCStringUtil::Mid(group, (int32_t)pos+1);

                    size = name.size();
                    pos = name.find_last_of(':');
                    if (pos != RCString::npos)
                    {
                        RCString str = RCStringUtil::Mid(name, (int32_t)pos+1);
                        if (str.length() > 0)
                        {
                            const RCString::value_type* start = str.c_str() ;
                            const RCString::value_type* end = NULL ;
                            uint64_t v = RCStringUtil::ConvertStringToUInt64(start, &end);
                            level = static_cast<uint32_t>(v) ;
                        }

                        if (pos < size)
                        {
                            name.erase(pos, size - pos);
                        }
                    }

                    if ((ext.length() == 0) || (name.length() == 0))
                    {
                        return RC_E_INVALIDARG;
                    }
                   
                    RCZipExtFilterPtr item(new RCZipExtFilter);
                    if (level < 0)
                    {
                        level = 5;
                    }

                    if (RCStringUtil::CompareNoCase(name, L"COPY") == 0)
                    {
                        methodID = RCZipHeader::NCompressionMethod::kStored;
                    }
                    else if ((RCStringUtil::CompareNoCase(name, L"DEFLATE") == 0) || 
                             (RCStringUtil::CompareNoCase(name, L"DEFLATE64") == 0))
                    {
                        if (RCStringUtil::CompareNoCase(name, L"DEFLATE") == 0)
                        {
                            methodID = RCZipHeader::NCompressionMethod::kDeflated;
                        }
                        else if (RCStringUtil::CompareNoCase(name, L"DEFLATE64") == 0)
                        {
                            methodID = RCZipHeader::NCompressionMethod::kDeflated64;
                        }
                            
                        item->m_deflate.m_numPasses = (level >= 9 ? RCZipDefs::s_deflateNumPassesX9 :
                            (level >= 7 ? RCZipDefs::s_deflateNumPassesX7 :
                            RCZipDefs::s_deflateNumPassesX1));

                        item->m_deflate.m_numFastBytes = (level >= 9 ? RCZipDefs::s_deflateNumFastBytesX9 :
                            (level >= 7 ? RCZipDefs::s_deflateNumFastBytesX7 :
                            RCZipDefs::s_deflateNumFastBytesX1));
                            
                        item->m_deflate.m_algo = (level >= 5 ? RCZipDefs::s_lzAlgoX5 :
                            RCZipDefs::s_lzAlgoX1);
                    }
                    else if (RCStringUtil::CompareNoCase(name, L"BZIP2") == 0)
                    {
                        methodID = RCZipHeader::NCompressionMethod::kBZip2;

                        item->m_bzip2.m_numPasses = (level >= 9 ? RCZipDefs::s_bzip2NumPassesX9 :
                            (level >= 7 ? RCZipDefs::s_bzip2NumPassesX7 :
                            RCZipDefs::s_bzip2NumPassesX1));

                        item->m_bzip2.m_dicSize = (level >= 5 ? RCZipDefs::s_bzip2DicSizeX5 :
                            (level >= 3 ? RCZipDefs::s_bzip2DicSizeX3 :
                            RCZipDefs::s_bzip2DicSizeX1));
                    }     
                    else if (RCStringUtil::CompareNoCase(name, L"LZMA") == 0) 
                    {
                        methodID = RCZipHeader::NCompressionMethod::kLZMA;

                        item->m_lzma.m_dicSize =
                            (level >= 9 ? RCZipDefs::s_lzmaDicSizeX9 :
                            (level >= 7 ? RCZipDefs::s_lzmaDicSizeX7 :
                            (level >= 5 ? RCZipDefs::s_lzmaDicSizeX5 :
                            (level >= 3 ? RCZipDefs::s_lzmaDicSizeX3 :
                            RCZipDefs::s_lzmaDicSizeX1))));

                        item->m_lzma.m_numFastBytes = (level >= 7 ? RCZipDefs::s_lzmaNumFastBytesX7 :
                            RCZipDefs::s_lzmaNumFastBytesX1);

                        item->m_lzma.m_matchFinder = (level >= 5 ? RCZipDefs::s_lzmaMatchFinderX5 :
                            RCZipDefs::s_lzmaMatchFinderX1);

                        item->m_lzma.m_algo = (level >= 5 ? RCZipDefs::s_lzAlgoX5 :
                            RCZipDefs::s_lzAlgoX1);
                    }
                    else
                    {
                        return RC_E_INVALIDARG;
                    }

                    item->m_fileExt = ext;
                    item->m_methodID = methodID;
                    item->m_level = level;

                    s_extFilters.push_back(item);
                }
            }
            else
            {
                return RC_E_INVALIDARG;
            }
        }
        else if (RCStringUtil::CompareNoCase(name, _T("FF")) == 0)
        {
            if (IsStringType(prop))
            {
                s_fileFilter.m_fileList.erase();
                m_hasFileFilters = true ;

                RCString name;
                RCString valueString = GetStringValue(prop);

                RCString::size_type size = valueString.size();
                RCString::size_type pos = valueString.find_last_of('=');
                if (pos != RCString::npos)
                {
                    name = RCStringUtil::Mid(valueString, (int32_t)pos+1);
                    if (pos < size)
                    {
                        valueString.erase(pos, size - pos);
                    }
                }
                else
                {
                    return RC_E_INVALIDARG;
                }

                if (valueString.length() == 0)
                {
                    return RC_E_INVALIDARG;
                }

                RCMethodID methodID = 0 ;
                int32_t level = -1;

                size = name.size();
                pos = name.find_last_of(':');
                if (pos != RCString::npos)
                {
                    RCString str = RCStringUtil::Mid(name, (int32_t)pos+1);
                    if (str.length() > 0)
                    {
                        const RCString::value_type* start = str.c_str() ;
                        const RCString::value_type* end = NULL ;
                        uint64_t v = RCStringUtil::ConvertStringToUInt64(start, &end);
                        level = static_cast<uint32_t>(v) ;
                    }

                    if (pos < size)
                    {
                        name.erase(pos, size - pos);
                    }
                }

                if (level < 0)
                {
                    level = 5;
                }

                if (RCStringUtil::CompareNoCase(name, L"COPY") == 0)
                {
                    methodID = RCZipHeader::NCompressionMethod::kStored;
                }
                else if ((RCStringUtil::CompareNoCase(name, L"DEFLATE") == 0) ||
                         (RCStringUtil::CompareNoCase(name, L"DEFLATE64") == 0))
                {
                    if (RCStringUtil::CompareNoCase(name, L"DEFLATE") == 0)
                    {
                        methodID = RCZipHeader::NCompressionMethod::kDeflated;
                    }
                    else if (RCStringUtil::CompareNoCase(name, L"DEFLATE64") == 0)
                    {
                        methodID = RCZipHeader::NCompressionMethod::kDeflated64;
                    }

                    s_fileFilter.m_deflate.m_numPasses = (level >= 9 ? RCZipDefs::s_deflateNumPassesX9 :
                        (level >= 7 ? RCZipDefs::s_deflateNumPassesX7 :
                        RCZipDefs::s_deflateNumPassesX1));

                    s_fileFilter.m_deflate.m_numFastBytes = (level >= 9 ? RCZipDefs::s_deflateNumFastBytesX9 :
                        (level >= 7 ? RCZipDefs::s_deflateNumFastBytesX7 :
                        RCZipDefs::s_deflateNumFastBytesX1));

                    s_fileFilter.m_deflate.m_algo = (level >= 5 ? RCZipDefs::s_lzAlgoX5 :
                        RCZipDefs::s_lzAlgoX1);
                }
                else if (RCStringUtil::CompareNoCase(name, L"BZIP2") == 0)
                {
                    methodID = RCZipHeader::NCompressionMethod::kBZip2;

                    s_fileFilter.m_bzip2.m_numPasses = (level >= 9 ? RCZipDefs::s_bzip2NumPassesX9 :
                        (level >= 7 ? RCZipDefs::s_bzip2NumPassesX7 :
                        RCZipDefs::s_bzip2NumPassesX1));

                    s_fileFilter.m_bzip2.m_dicSize = (level >= 5 ? RCZipDefs::s_bzip2DicSizeX5 :
                        (level >= 3 ? RCZipDefs::s_bzip2DicSizeX3 :
                        RCZipDefs::s_bzip2DicSizeX1));
                }     
                else if (RCStringUtil::CompareNoCase(name, L"LZMA") == 0) 
                {
                    methodID = RCZipHeader::NCompressionMethod::kLZMA;

                    s_fileFilter.m_lzma.m_dicSize =
                        (level >= 9 ? RCZipDefs::s_lzmaDicSizeX9 :
                        (level >= 7 ? RCZipDefs::s_lzmaDicSizeX7 :
                        (level >= 5 ? RCZipDefs::s_lzmaDicSizeX5 :
                        (level >= 3 ? RCZipDefs::s_lzmaDicSizeX3 :
                        RCZipDefs::s_lzmaDicSizeX1))));

                    s_fileFilter.m_lzma.m_numFastBytes = (level >= 7 ? RCZipDefs::s_lzmaNumFastBytesX7 :
                        RCZipDefs::s_lzmaNumFastBytesX1);

                    s_fileFilter.m_lzma.m_matchFinder = (level >= 5 ? RCZipDefs::s_lzmaMatchFinderX5 :
                        RCZipDefs::s_lzmaMatchFinderX1);

                    s_fileFilter.m_lzma.m_algo = (level >= 5 ? RCZipDefs::s_lzAlgoX5 :
                        RCZipDefs::s_lzAlgoX1);
                }
                else
                {
                    return RC_E_INVALIDARG;
                }

                s_fileFilter.m_methodID = methodID;
                s_fileFilter.m_level = level;
                s_fileFilter.m_fileList = valueString;
            }
            else
            {
                return RC_E_INVALIDARG;
            }
        }
        else
        {
            return RC_E_INVALIDARG;
        }
    }

    return RC_S_OK;
}

END_NAMESPACE_RCZIP
