/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "format/7z/RCArchiveOutHandler.h"
#include "common/RCStringUtil.h"
#include "interface/RCPropertyID.h"
#include "common/RCSystemUtils.h"
#include "interface/RCMethodDefs.h"
#include "format/common/RCParseProperties.h"

/////////////////////////////////////////////////////////////////
//RCArchiveOutHandler class implementation

BEGIN_NAMESPACE_RCZIP

RCVector<RC7zExtFilterPtr> RCArchiveOutHandler::s_extFilters ;

RC7zFileFilter RCArchiveOutHandler::s_fileFilter ;

static const char_t* s_lzmaMatchFinderX1 = _T("HC4") ;
static const char_t* s_lzmaMatchFinderX5 = _T("BT4") ;

static const uint32_t s_lzmaAlgoX1 = 0;
static const uint32_t s_lzmaAlgoX5 = 1;

static const uint32_t s_lzmaDicSizeX1 = 1 << 16;
static const uint32_t s_lzmaDicSizeX3 = 1 << 20;
static const uint32_t s_lzmaDicSizeX5 = 1 << 24;
static const uint32_t s_lzmaDicSizeX7 = 1 << 25;
static const uint32_t s_lzmaDicSizeX9 = 1 << 26;

static const uint32_t s_lzmaFastBytesX1 = 32;
static const uint32_t s_lzmaFastBytesX7 = 64;

static const uint32_t s_ppmdMemSizeX1 = (1 << 22);
static const uint32_t s_ppmdMemSizeX5 = (1 << 24);
static const uint32_t s_ppmdMemSizeX7 = (1 << 26);
static const uint32_t s_ppmdMemSizeX9 = (192 << 20);

static const uint32_t s_ppmdMinMemSize = (1 << 11);
static const uint32_t s_ppmdMaxMemBlockSize = (0xFFFFFFFF - 36);

static const uint32_t s_ppmdOrderX1 = 4;
static const uint32_t s_ppmdOrderX5 = 6;
static const uint32_t s_ppmdOrderX7 = 16;
static const uint32_t s_ppmdOrderX9 = 32;

static const uint32_t s_deflateAlgoX1 = 0;
static const uint32_t s_deflateAlgoX5 = 1;

static const uint32_t s_deflateFastBytesX1 = 32;
static const uint32_t s_deflateFastBytesX7 = 64;
static const uint32_t s_deflateFastBytesX9 = 128;

static const uint32_t s_deflatePassesX1 = 1;
static const uint32_t s_deflatePassesX7 = 3;
static const uint32_t s_deflatePassesX9 = 10;

static const uint32_t s_bzip2NumPassesX1 = 1;
static const uint32_t s_bzip2NumPassesX7 = 2;
static const uint32_t s_bzip2NumPassesX9 = 7;

static const uint32_t s_bzip2DicSizeX1 = 100000;
static const uint32_t s_bzip2DicSizeX3 = 500000;
static const uint32_t s_bzip2DicSizeX5 = 900000;

static const char_t* s_defaultMethodName = RC_METHOD_NAME_COMPRESS_LZMA;

static const char_t* s_lzmaMatchFinderForHeaders = _T("BT2") ;
static const uint32_t s_dictionaryForHeaders = 1 << 20;
static const uint32_t s_numFastBytesForHeaders = 273;
static const uint32_t s_algorithmForHeaders = s_lzmaAlgoX5;

static inline bool AreEqual(const RCString& methodName, const char_t *s)
{
    return (RCStringUtil::CompareNoCase(methodName,s) == 0) ;
}

bool RCOneMethodInfo::IsLzma() const
{
    return AreEqual(m_methodName, RC_METHOD_NAME_COMPRESS_LZMA) ||
           AreEqual(m_methodName, RC_METHOD_NAME_COMPRESS_LZMA2) ;
}

bool RCOneMethodInfo::IsBZip2() const
{
    return AreEqual(m_methodName, RC_METHOD_NAME_COMPRESS_BZIP2);
}

bool RCOneMethodInfo::IsPPMd() const
{
    return AreEqual(m_methodName, RC_METHOD_NAME_COMPRESS_PPMD);
}

bool RCOneMethodInfo::IsDeflate() const
{
    return AreEqual(m_methodName, RC_METHOD_NAME_COMPRESS_DEFLATE_COM) ||
           AreEqual(m_methodName, RC_METHOD_NAME_COMPRESS_DEFLATE_COM_64) ;
}

struct RCNameToPropID
{
    /** 属性ID
    */
    RCPropertyID m_propID;

    /** 属性类型
    */
    RCVariantType::RCVariantID m_varType ;

    /** 属性名
    */
    const RCString m_name ;
};

static RCNameToPropID s_nameToPropID[] =
{
    { RCCoderPropID::kBlockSize,        RCVariantType::RC_VT_UINT64, _T("C") },
    { RCCoderPropID::kDictionarySize,   RCVariantType::RC_VT_UINT64, _T("D") },
    { RCCoderPropID::kUsedMemorySize,   RCVariantType::RC_VT_UINT64, _T("MEM") },
    
    { RCCoderPropID::kOrder,            RCVariantType::RC_VT_UINT64, _T("O") },
    { RCCoderPropID::kPosStateBits,     RCVariantType::RC_VT_UINT64, _T("PB") },
    { RCCoderPropID::kLitContextBits,   RCVariantType::RC_VT_UINT64, _T("LC") },
    { RCCoderPropID::kLitPosBits,       RCVariantType::RC_VT_UINT64, _T("LP") },
    { RCCoderPropID::kEndMarker,        RCVariantType::RC_VT_BOOL,   _T("eos") },

    { RCCoderPropID::kNumPasses,        RCVariantType::RC_VT_UINT64, _T("Pass") },
    { RCCoderPropID::kNumFastBytes,     RCVariantType::RC_VT_UINT64, _T("fb") },
    { RCCoderPropID::kMatchFinderCycles,RCVariantType::RC_VT_UINT64, _T("mc") },
    { RCCoderPropID::kAlgorithm,        RCVariantType::RC_VT_UINT64, _T("a") },
    { RCCoderPropID::kMatchFinder,      RCVariantType::RC_VT_STRING, _T("mf") },
    { RCCoderPropID::kNumThreads,       RCVariantType::RC_VT_UINT64, _T("mt") },
    
    { RCCoderPropID::kDefaultProp,      RCVariantType::RC_VT_UINT64, _T("") }
};

static bool ConvertProperty(const RCVariant& srcProp, 
                            RCVariantType::RCVariantID varType, 
                            RCVariant& destProp)
{
    if (varType == srcProp.which())
    {
        destProp = srcProp ;
        return true;
    }
    else if( IsInteger64Type(srcProp) && 
             (varType == RCVariantType::RC_VT_UINT64) &&
             (varType == RCVariantType::RC_VT_INT64) )
    {
        if(varType == RCVariantType::RC_VT_UINT64)
        {
            destProp = static_cast<uint64_t>(GetInteger64Value(srcProp)) ;
        }
        else
        {
            destProp = static_cast<int64_t>(GetInteger64Value(srcProp)) ;
        }
    }
    else if (varType == RCVariantType::RC_VT_BOOL)
    {
        bool res = false ;
        if (RCParseProperties::SetBoolProperty(res, srcProp) != RC_S_OK)
        {
            return false ;
        }
        destProp = res ;
        return true ;
    }
    return false;
}

static int32_t FindPropIdExact(const RCString& name)
{
    for (int32_t i = 0; i < sizeof(s_nameToPropID) / sizeof(s_nameToPropID[0]); i++)
    {
        if (RCStringUtil::CompareNoCase(name, s_nameToPropID[i].m_name) == 0)
        {
            return i;
        }
    }
    return -1;
}

static int32_t FindPropIdStart(const RCString& name)
{
    for (int32_t i = 0; i < sizeof(s_nameToPropID) / sizeof(s_nameToPropID[0]); i++)
    {
        const RCString& t = s_nameToPropID[i].m_name;
        if (RCStringUtil::CompareNoCase(t, RCStringUtil::Left(name, (int32_t)t.size())) == 0)
        {
            return i ;
        }
    }
    return -1;
}

void SetOneMethodProp(RCOneMethodInfo& oneMethodInfo, 
                      RCPropertyID propID,
                      const RCVariant& value)
{
    for (int32_t j = 0; j < (int32_t)oneMethodInfo.m_properties.size(); j++)
    {
        if (oneMethodInfo.m_properties[j].m_id == propID)
        {
            return;
        }
    }
    RCArchiveProp property;
    property.m_id = propID;
    property.m_value = value;
    oneMethodInfo.m_properties.push_back(property);
}

static void SetOneMethodPropOverWrite(RCOneMethodInfo& oneMethodInfo, 
                                      RCPropertyID propID,
                                      const RCVariant& value)
{
    for (int32_t j = 0; j < (int32_t)oneMethodInfo.m_properties.size(); j++)
    {
        if (oneMethodInfo.m_properties[j].m_id == propID)
        {
            oneMethodInfo.m_properties[j].m_value = value;
            return;
        }
    }
}

static void SplitParams(const RCString& srcString, RCVector<RCString>& subStrings)
{
    subStrings.clear();
    RCString name;
    int32_t len = (int32_t)srcString.size();
    if (len == 0)
    {
        return;
    }
    for (int32_t i = 0; i < len; i++)
    {
        char_t c = srcString[i];
        if (c == L':')
        {
            subStrings.push_back(name);
            name.clear();
        }
        else
        {
            name += c;
        }
    }
    if(!name.empty())
    {
        subStrings.push_back(name);
    }
}

static void SplitParam(const RCString& param, RCString& name, RCString& value)
{
    RCString::size_type eqPos = param.find(_T('='));
    if (eqPos != RCString::npos)
    {
        name =  RCStringUtil::Left(param, (int32_t)eqPos);
        value = RCStringUtil::Mid(param, (int32_t)eqPos + 1);
        return;
    }
    for(int32_t i = 0; i < (int32_t)param.size(); i++)
    {
        char_t c = param[i];
        if (c >= _T('0') && c <= _T('9') )
        {
            name = RCStringUtil::Left(param, i);
            value = RCStringUtil::Mid(param, i);
            return;
        }
    }
    name = param;
}

RCArchiveOutHandler::RCArchiveOutHandler()
{
    Init();
}

RCArchiveOutHandler::~RCArchiveOutHandler()
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

void RCArchiveOutHandler::Init()
{
    m_removeSfxBlock = false;
    m_compressHeaders = true;
    m_encryptHeadersSpecified = false;
    m_encryptHeaders = false;

    m_writeCTime = false;
    m_writeATime = false;
    m_writeMTime = true;

#ifdef COMPRESS_MT
    m_numThreads = RCSystemUtils::GetNumberOfProcessors();
#else
    m_numThreads = 1 ;
#endif

    m_level = 5;
    m_autoFilter = true;
    m_volumeMode = false;
    m_crcSize = 4;
    InitSolid();
    m_hasExtFilters = false ;
    m_hasFileFilters = false ;
}

void RCArchiveOutHandler::InitSolid()
{
    InitSolidFiles();
    InitSolidSize();
    m_solidExtension = false;
    m_numSolidBytesDefined = false;
}

void RCArchiveOutHandler::InitSolidFiles()
{
    m_numSolidFiles = (uint64_t)(int64_t)(-1);
}

void RCArchiveOutHandler::InitSolidSize()
{
    m_numSolidBytes = (uint64_t)(int64_t)(-1);
}

void RCArchiveOutHandler::BeforeSetProperty()
{
    Init();
#ifdef COMPRESS_MT
    m_numProcessors = RCSystemUtils::GetNumberOfProcessors();
#endif

    m_mainDicSize = 0xFFFFFFFF;
    m_mainDicMethodIndex = 0xFFFFFFFF;
    m_minNumber = 0;
    m_crcSize = 4;
}

HResult RCArchiveOutHandler::SetProperty(const RCString& nameSpec, const RCVariant& value)
{
    RCString name = nameSpec;
    RCStringUtil::MakeUpper(name);
    if (name.empty())
    {
        return RC_E_INVALIDARG;
    }

    if (name[0] == _T('X'))
    {
        name.erase(name.begin());
        m_level = 9;
        return RCParseProperties::ParsePropValue(name, value, m_level);
    }

    if (name[0] == _T('S'))
    {
        name.erase(name.begin());
        if (name.empty())
        {
            return SetSolidSettings(value);
        }
        if (!IsEmptyType(value))
        {
            return RC_E_INVALIDARG;
        }
        return SetSolidSettings(name);
    }

    if (name == _T("CRC"))
    {
        m_crcSize = 4;
        name.erase(0,3);
        return RCParseProperties::ParsePropValue(name, value, m_crcSize);
    }

    uint32_t number;
    int32_t index = RCParseProperties::ParseStringToUInt32(name, number);
    RCString realName = RCStringUtil::Mid(name,index);
    if (index == 0)
    {
        if( RCStringUtil::CompareNoCase(RCStringUtil::Left(name,2),_T("MT")) == 0)
        {
#ifdef COMPRESS_MT
            HResult hr = RCParseProperties::ParseMtProp(RCStringUtil::Mid(name,2), value, m_numProcessors, m_numThreads) ;
            if(!IsSuccess(hr))
            {
                return hr ;
            }
#endif
            return RC_S_OK;
        }
        if(RCStringUtil::CompareNoCase(name,_T("RSFX")) == 0)
        {
            return RCParseProperties::SetBoolProperty(m_removeSfxBlock, value);
        }
        else if(RCStringUtil::CompareNoCase(name,_T("F")) == 0)
        {
            return RCParseProperties::SetBoolProperty(m_autoFilter, value);
        }
        else if(RCStringUtil::CompareNoCase(name,_T("HC")) == 0)
        {
            return RCParseProperties::SetBoolProperty(m_compressHeaders, value);
        }
        else if(RCStringUtil::CompareNoCase(name,_T("HCF")) == 0)
        {
            bool compressHeadersFull = true;
            HResult hr = RCParseProperties::SetBoolProperty(compressHeadersFull, value) ;
            if(!IsSuccess(hr))
            {
                return hr ;
            }
            if (!compressHeadersFull)
            {
                return RC_E_INVALIDARG;
            }
            return RC_S_OK;
        }
        else if(RCStringUtil::CompareNoCase(name,_T("HE")) == 0)
        {
            HResult hr = RCParseProperties::SetBoolProperty(m_encryptHeaders, value) ;
            if(!IsSuccess(hr))
            {
                return hr ;
            }
            m_encryptHeadersSpecified = true;
            return RC_S_OK ;
        }
        else if(RCStringUtil::CompareNoCase(name,_T("TC")) == 0)
        {
            return RCParseProperties::SetBoolProperty(m_writeCTime, value);
        }
        else if(RCStringUtil::CompareNoCase(name,_T("TA")) == 0)
        {
            return RCParseProperties::SetBoolProperty(m_writeATime, value);
        }
        else if(RCStringUtil::CompareNoCase(name,_T("TM")) == 0)
        {
            return RCParseProperties::SetBoolProperty(m_writeMTime, value);
        }
        else if(RCStringUtil::CompareNoCase(name,_T("V")) == 0)
        {
            return RCParseProperties::SetBoolProperty(m_volumeMode, value);
        }
        else if (RCStringUtil::CompareNoCase(name, _T("EF")) == 0)
        {
            s_extFilters.clear() ;
            m_hasExtFilters = true ;

            bool last = false;
            RCString valueString = GetStringValue(value);

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

                RCMethodID methodID;
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

                RCString::value_type* exeExts[] =
                {
                    _T("DLL"),
                    _T("EXE"),
                    _T("OCX"),
                    _T("SFX"),
                    _T("SYS")
                };

                bool isExeExt = false;
                for (uint32_t i = 0; i < sizeof(exeExts)/sizeof(exeExts[0]); i++)
                {
                    if (RCStringUtil::CompareNoCase(ext, exeExts[i]) == 0)
                    {
                        isExeExt = true;
                        break;
                    }
                }

                if (isExeExt)
                {
                    continue;
                }

                if (level < 0)
                {
                    level = 5;
                }

                RC7zExtFilterPtr item(new RC7zExtFilter);
                if (RCStringUtil::CompareNoCase(name, L"COPY") == 0)
                {
                    methodID = RCMethod::ID_COMPRESS_COPY;
                }
                else if (RCStringUtil::CompareNoCase(name, L"LZMA") == 0)
                {
                    methodID = RCMethod::ID_COMPRESS_LZMA;

                    item->m_lzma.m_dicSize = (level >= 9 ? s_lzmaDicSizeX9 :
                        (level >= 7 ? s_lzmaDicSizeX7 :
                        (level >= 5 ? s_lzmaDicSizeX5 :
                        (level >= 3 ? s_lzmaDicSizeX3 :
                        s_lzmaDicSizeX1))));

                    item->m_lzma.m_algo = (level >= 5 ? s_lzmaAlgoX5 : s_lzmaAlgoX1);

                    item->m_lzma.m_fastBytes = (level >= 7 ? s_lzmaFastBytesX7 : s_lzmaFastBytesX1);

                    item->m_lzma.m_matchFinder = (level >= 5 ? s_lzmaMatchFinderX5 : s_lzmaMatchFinderX1);
                }
                else if (RCStringUtil::CompareNoCase(name, L"DEFLATE") == 0)
                {
                    methodID = RCMethod::ID_COMPRESS_DEFLATE_COM;

                    item->m_deflate.m_fastBytes = (level >= 9 ? s_deflateFastBytesX9 :
                        (level >= 7 ? s_deflateFastBytesX7 :
                        s_deflateFastBytesX1));

                    item->m_deflate.m_numPasses = (level >= 9 ? s_deflatePassesX9 :
                        (level >= 7 ? s_deflatePassesX7 :
                        s_deflatePassesX1));

                    item->m_deflate.m_algo = (level >= 5 ? s_deflateAlgoX5 : s_deflateAlgoX1);
                }
                else if (RCStringUtil::CompareNoCase(name, L"BZIP2") == 0)
                {
                    methodID = RCMethod::ID_COMPRESS_BZIP2;

                    item->m_bzip2.m_numPasses = (level >= 9 ? s_bzip2NumPassesX9 :
                        (level >= 7 ? s_bzip2NumPassesX7 :
                        s_bzip2NumPassesX1));

                    item->m_bzip2.m_dicSize =  (level >= 5 ? s_bzip2DicSizeX5 :
                        (level >= 3 ? s_bzip2DicSizeX3 :
                        s_bzip2DicSizeX1));
                }
                else if (RCStringUtil::CompareNoCase(name, L"PPMD") == 0)
                {
                    methodID = RCMethod::ID_COMPRESS_PPMD;

                    item->m_ppmd.m_useMemSize = (level >= 9 ? s_ppmdMemSizeX9 :
                        (level >= 7 ? s_ppmdMemSizeX7 :
                        (level >= 5 ? s_ppmdMemSizeX5 :
                        s_ppmdMemSizeX1)));

                    item->m_ppmd.m_order = (level >= 9 ? s_ppmdOrderX9 :
                        (level >= 7 ? s_ppmdOrderX7 :
                        (level >= 5 ? s_ppmdOrderX5 :
                        s_ppmdOrderX1)));
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

            return RC_S_OK;
        }
        else if (RCStringUtil::CompareNoCase(name, _T("FF")) == 0)
        {
            s_fileFilter.m_fileList.erase();
            m_hasFileFilters = true ;

            RCString name;
            RCString valueString = GetStringValue(value);

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
                methodID = RCMethod::ID_COMPRESS_COPY;
            }
            else if (RCStringUtil::CompareNoCase(name, L"LZMA") == 0)
            {
                methodID = RCMethod::ID_COMPRESS_LZMA;

                s_fileFilter.m_lzma.m_dicSize = (level >= 9 ? s_lzmaDicSizeX9 :
                    (level >= 7 ? s_lzmaDicSizeX7 :
                    (level >= 5 ? s_lzmaDicSizeX5 :
                    (level >= 3 ? s_lzmaDicSizeX3 :
                    s_lzmaDicSizeX1))));

                s_fileFilter.m_lzma.m_algo = (level >= 5 ? s_lzmaAlgoX5 : s_lzmaAlgoX1);

                s_fileFilter.m_lzma.m_fastBytes = (level >= 7 ? s_lzmaFastBytesX7 : s_lzmaFastBytesX1);

                s_fileFilter.m_lzma.m_matchFinder = (level >= 5 ? s_lzmaMatchFinderX5 : s_lzmaMatchFinderX1);
            }
            else if (RCStringUtil::CompareNoCase(name, L"DEFLATE") == 0)
            {
                methodID = RCMethod::ID_COMPRESS_DEFLATE_COM;

                s_fileFilter.m_deflate.m_fastBytes = (level >= 9 ? s_deflateFastBytesX9 :
                    (level >= 7 ? s_deflateFastBytesX7 :
                    s_deflateFastBytesX1));

                s_fileFilter.m_deflate.m_numPasses = (level >= 9 ? s_deflatePassesX9 :
                    (level >= 7 ? s_deflatePassesX7 :
                    s_deflatePassesX1));

                s_fileFilter.m_deflate.m_algo = (level >= 5 ? s_deflateAlgoX5 : s_deflateAlgoX1);
            }
            else if (RCStringUtil::CompareNoCase(name, L"BZIP2") == 0)
            {
                methodID = RCMethod::ID_COMPRESS_BZIP2;

                s_fileFilter.m_bzip2.m_numPasses = (level >= 9 ? s_bzip2NumPassesX9 :
                    (level >= 7 ? s_bzip2NumPassesX7 :
                    s_bzip2NumPassesX1));

                s_fileFilter.m_bzip2.m_dicSize =  (level >= 5 ? s_bzip2DicSizeX5 :
                    (level >= 3 ? s_bzip2DicSizeX3 :
                    s_bzip2DicSizeX1));
            }
            else if (RCStringUtil::CompareNoCase(name, L"PPMD") == 0)
            {
                methodID = RCMethod::ID_COMPRESS_PPMD;

                s_fileFilter.m_ppmd.m_useMemSize = (level >= 9 ? s_ppmdMemSizeX9 :
                    (level >= 7 ? s_ppmdMemSizeX7 :
                    (level >= 5 ? s_ppmdMemSizeX5 :
                    s_ppmdMemSizeX1)));

                s_fileFilter.m_ppmd.m_order = (level >= 9 ? s_ppmdOrderX9 :
                    (level >= 7 ? s_ppmdOrderX7 :
                    (level >= 5 ? s_ppmdOrderX5 :
                    s_ppmdOrderX1)));
            }
            else
            {
                return RC_E_INVALIDARG;
            }

            s_fileFilter.m_methodID = methodID;
            s_fileFilter.m_level = level;
            s_fileFilter.m_fileList = valueString;

            return RC_S_OK;
        }
        number = 0;
    }
    if (number > 10000)
    {
        return RC_E_FAIL;
    }
    if (number < m_minNumber)
    {
        return RC_E_INVALIDARG;
    }
    number -= m_minNumber ;
    for(int32_t j = (int32_t)m_methods.size(); j <= (int32_t)number; j++)
    {
        RCOneMethodInfo oneMethodInfo;
        m_methods.push_back(oneMethodInfo);
    }

    RCOneMethodInfo& oneMethodInfo = m_methods[number];

    if (realName.empty())
    {
        if(!IsStringType(value))
        {
            return RC_E_INVALIDARG;
        }
        HResult hr = SetParams(oneMethodInfo, GetStringValue(value)) ;
        if(!IsSuccess(hr))
        {
            return hr ;
        }
    }
    else
    {
        int32_t index = FindPropIdStart(realName);
        if (index < 0)
        {
            return RC_E_INVALIDARG;
        }
        const RCNameToPropID& nameToPropID = s_nameToPropID[index];
        RCArchiveProp property;
        property.m_id = nameToPropID.m_propID;
        if (property.m_id == RCCoderPropID::kBlockSize ||
            property.m_id == RCCoderPropID::kDictionarySize ||
            property.m_id == RCCoderPropID::kUsedMemorySize)
        {
            uint32_t dicSize = 0 ;
            HResult hr = RCParseProperties::ParsePropDictionaryValue(RCStringUtil::Mid(realName, (int32_t)nameToPropID.m_name.size()), value, dicSize) ;
            if(!IsSuccess(hr))
            {
                return hr ;
            }
            property.m_value = (uint64_t)dicSize;
            if (number <= m_mainDicMethodIndex)
            {
                m_mainDicSize = dicSize;
            }
        }
        else
        {
            int32_t index = FindPropIdExact(realName);
            if (index < 0)
            {
                return RC_E_INVALIDARG ;
            }

            const RCNameToPropID& nameToPropID = s_nameToPropID[index];
            property.m_id = nameToPropID.m_propID;

            if (!ConvertProperty(value, nameToPropID.m_varType, property.m_value))
            {
                return RC_E_INVALIDARG;
            }
        }
        oneMethodInfo.m_properties.push_back(property);
    }
    return RC_S_OK;
}

HResult RCArchiveOutHandler::SetSolidSettings(const RCString& s)
{
    RCString s2 = s;
    RCStringUtil::MakeUpper(s2);
    for (int32_t i = 0; i < (int32_t)s2.size();)
    {
        const RCString::value_type* start = ((const RCString::value_type*)s2.c_str()) + i;
        const RCString::value_type* end = NULL ;
        uint64_t v = RCStringUtil::ConvertStringToUInt64(start, &end) ;
        if (start == end)
        {
            if (s2[i++] != _T('E'))
            {
                return RC_E_INVALIDARG;
            }
            m_solidExtension = true ;
            continue;
        }
        i += (int32_t)(end - start);
        if (i == (int32_t)s2.size())
        {
            return RC_E_INVALIDARG;
        }
        RCString::value_type c = s2[i++];
        switch(c)
        {
        case _T('F'):
            if (v < 1)
            {
                v = 1;
            }
            m_numSolidFiles = v;
            break;
        case _T('B'):
            m_numSolidBytes = v;
            m_numSolidBytesDefined = true;
            break;
        case _T('K'):
            m_numSolidBytes = (v << 10);
            m_numSolidBytesDefined = true;
            break;
        case _T('M'):
            m_numSolidBytes = (v << 20);
            m_numSolidBytesDefined = true;
            break;
        case _T('G'):
            m_numSolidBytes = (v << 30);
            m_numSolidBytesDefined = true;
            break;
        default:
            return RC_E_INVALIDARG;
        }
    }
    return RC_S_OK;
}

HResult RCArchiveOutHandler::SetSolidSettings(const RCVariant& value)
{
    bool isSolid = false ;
    switch(value.which())
    {
    case RCVariantType::RC_VT_EMPTY:
        isSolid = true ;
        break;
    case RCVariantType::RC_VT_BOOL:
        isSolid = GetBooleanValue(value) ;
        break;
    case RCVariantType::RC_VT_STRING:
        if(RCParseProperties::StringToBool(GetStringValue(value), isSolid))
        {
            break;
        }
        return SetSolidSettings(GetStringValue(value)) ;
    default:
        return RC_E_INVALIDARG ;
    }
    if (isSolid)
    {
        InitSolid() ;
    }
    else
    {
        m_numSolidFiles = 1 ;
    }
    return RC_S_OK;
}

HResult RCArchiveOutHandler::SetParam(RCOneMethodInfo& oneMethodInfo, const RCString& name, const RCString& value)
{
    RCArchiveProp property;
    int32_t index = FindPropIdExact(name);
    if (index < 0)
    {
        return RC_E_INVALIDARG;
    }
    const RCNameToPropID& nameToPropID = s_nameToPropID[index];
    property.m_id = nameToPropID.m_propID ;
    
    if (property.m_id == RCCoderPropID::kBlockSize ||
        property.m_id == RCCoderPropID::kDictionarySize ||
        property.m_id == RCCoderPropID::kUsedMemorySize)
    {
        uint32_t dicSize = 0 ;
        HResult hr = RCParseProperties::ParsePropDictionaryValue(value, dicSize) ;
        if(!IsSuccess(hr))
        {
            return hr ;
        }
        property.m_value = uint64_t(dicSize) ;
    }
    else
    {
        RCVariant propValue ;
        if (nameToPropID.m_varType == RCVariantType::RC_VT_STRING)
        {
            propValue = value ;
        }
        else if (nameToPropID.m_varType == RCVariantType::RC_VT_BOOL)
        {
            bool res = false ;
            if (!RCParseProperties::StringToBool(value, res))
            {
                return RC_E_INVALIDARG ;
            }
            propValue = res ;
        }
        else
        {
            uint32_t number = 0 ;
            if (RCParseProperties::ParseStringToUInt32(value, number) == (int32_t)value.size())
            {
                propValue = uint64_t(number) ;
            }
            else
            {
                propValue = value;
            }
        }

        if (!ConvertProperty(propValue, nameToPropID.m_varType, property.m_value))
        {
            return RC_E_INVALIDARG;
        }
    }
    oneMethodInfo.m_properties.push_back(property);
    return RC_S_OK ;
}

HResult RCArchiveOutHandler::SetParams(RCOneMethodInfo& oneMethodInfo, const RCString& srcString)
{
    RCVector<RCString> params ;
    SplitParams(srcString, params) ;
    if (!params.empty())
    {
        oneMethodInfo.m_methodName = params[0];
    }
    for (int32_t i = 1; i < (int32_t)params.size(); i++)
    {
        const RCString& param = params[i];
        RCString name ;
        RCString value ;
        SplitParam(param, name, value);
        HResult hr = SetParam(oneMethodInfo, name, value) ;
        if(!IsSuccess(hr))
        {
            return hr ;
        }
    }
    return RC_S_OK;
}

void RCArchiveOutHandler::SetCompressionMethod2(RCOneMethodInfo& oneMethodInfo, uint32_t numThreads, uint64_t totalSize)
{
    uint32_t level = m_level;
    if (oneMethodInfo.m_methodName.empty())
    {
        oneMethodInfo.m_methodName = s_defaultMethodName;
    }

    if (oneMethodInfo.IsLzma())
    {
        uint32_t dicSize =  (level >= 9 ? s_lzmaDicSizeX9 :
                            (level >= 7 ? s_lzmaDicSizeX7 :
                            (level >= 5 ? s_lzmaDicSizeX5 :
                            (level >= 3 ? s_lzmaDicSizeX3 :
                            s_lzmaDicSizeX1))));
        if ((totalSize > 0) &&
            (dicSize > s_lzmaDicSizeX1) &&
            (totalSize < dicSize/2))
        {
            if (totalSize > s_lzmaDicSizeX3)
            {
                //压缩总大小大于1M以1M为基
                dicSize = (uint32_t)(totalSize*2 & (((uint64_t)1<<32)-s_lzmaDicSizeX3));
            }
            else
            {
                //压缩总大小大于64K以64K为基
                dicSize  = (uint32_t)totalSize*2 & (((uint64_t)1<<32)-s_lzmaDicSizeX1);
            }
            SetOneMethodPropOverWrite(oneMethodInfo, RCCoderPropID::kDictionarySize, uint64_t(dicSize));
        }

        uint32_t algo = (level >= 5 ? s_lzmaAlgoX5 : s_lzmaAlgoX1);

        uint32_t fastBytes = (level >= 7 ? s_lzmaFastBytesX7 : s_lzmaFastBytesX1);

        const RCString::value_type* matchFinder = (level >= 5 ? s_lzmaMatchFinderX5 : s_lzmaMatchFinderX1);

        SetOneMethodProp(oneMethodInfo, RCCoderPropID::kDictionarySize, uint64_t(dicSize));
        SetOneMethodProp(oneMethodInfo, RCCoderPropID::kAlgorithm, uint64_t(algo));
        SetOneMethodProp(oneMethodInfo, RCCoderPropID::kNumFastBytes, uint64_t(fastBytes));
        SetOneMethodProp(oneMethodInfo, RCCoderPropID::kMatchFinder, RCString(matchFinder));
#ifdef COMPRESS_MT
        SetOneMethodProp(oneMethodInfo, RCCoderPropID::kNumThreads, uint64_t(numThreads));
#endif
    }
    else if (oneMethodInfo.IsDeflate())
    {
        uint32_t fastBytes = (level >= 9 ? s_deflateFastBytesX9 :
                             (level >= 7 ? s_deflateFastBytesX7 :
                              s_deflateFastBytesX1));

        uint32_t numPasses = (level >= 9 ? s_deflatePassesX9 :
                             (level >= 7 ? s_deflatePassesX7 :
                              s_deflatePassesX1));

        uint32_t algo = (level >= 5 ? s_deflateAlgoX5 : s_deflateAlgoX1);

        SetOneMethodProp(oneMethodInfo, RCCoderPropID::kAlgorithm, uint64_t(algo));
        SetOneMethodProp(oneMethodInfo, RCCoderPropID::kNumFastBytes, uint64_t(fastBytes));
        SetOneMethodProp(oneMethodInfo, RCCoderPropID::kNumPasses, uint64_t(numPasses));
    }
    else if (oneMethodInfo.IsBZip2())
    {
        uint32_t numPasses = (level >= 9 ? s_bzip2NumPassesX9 :
                             (level >= 7 ? s_bzip2NumPassesX7 :
                              s_bzip2NumPassesX1));

        uint32_t dicSize =  (level >= 5 ? s_bzip2DicSizeX5 :
                            (level >= 3 ? s_bzip2DicSizeX3 :
                             s_bzip2DicSizeX1));

        SetOneMethodProp(oneMethodInfo, RCCoderPropID::kNumPasses, uint64_t(numPasses));
        SetOneMethodProp(oneMethodInfo, RCCoderPropID::kDictionarySize, uint64_t(dicSize));
#ifdef COMPRESS_MT
        SetOneMethodProp(oneMethodInfo, RCCoderPropID::kNumThreads, uint64_t(numThreads));
#endif
    }
    else if (oneMethodInfo.IsPPMd())
    {
        uint32_t useMemSize = (level >= 9 ? s_ppmdMemSizeX9 :
                              (level >= 7 ? s_ppmdMemSizeX7 :
                              (level >= 5 ? s_ppmdMemSizeX5 :
                               s_ppmdMemSizeX1)));

        uint32_t order = (level >= 9 ? s_ppmdOrderX9 :
                         (level >= 7 ? s_ppmdOrderX7 :
                         (level >= 5 ? s_ppmdOrderX5 :
                          s_ppmdOrderX1)));

        if ((totalSize > 0) &&
            (useMemSize > s_ppmdMemSizeX1) &&
            (totalSize < useMemSize/2))
        {
            //压缩总大小大于4M以4M为基
            useMemSize = (uint32_t)totalSize*2 & (((uint64_t)1<<32)-s_ppmdMemSizeX1);

            if (useMemSize < s_ppmdMinMemSize)
            {
                useMemSize = s_ppmdMinMemSize;
            }

            if (useMemSize > s_ppmdMaxMemBlockSize)
            {
                useMemSize = s_ppmdMaxMemBlockSize;
            }

            SetOneMethodPropOverWrite(oneMethodInfo, RCCoderPropID::kUsedMemorySize, uint64_t(useMemSize));
        }

        SetOneMethodProp(oneMethodInfo, RCCoderPropID::kUsedMemorySize, uint64_t(useMemSize));
        SetOneMethodProp(oneMethodInfo, RCCoderPropID::kOrder, uint64_t(order));
    }
}

END_NAMESPACE_RCZIP
