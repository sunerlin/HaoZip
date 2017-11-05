/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "format/common/RCParseProperties.h"
#include "common/RCStringUtil.h"

/////////////////////////////////////////////////////////////////
//RCParseProperties class implementation

BEGIN_NAMESPACE_RCZIP

static const int32_t s_logarithmicSizeLimit = 32;
static const RCString::value_type s_byteSymbol = _T('B') ;
static const RCString::value_type s_kiloByteSymbol = _T('K') ;
static const RCString::value_type s_megaByteSymbol = _T('M');

int32_t RCParseProperties::ParseStringToUInt32(const RCString& srcString, uint32_t& number)
{
    const RCString::value_type* start = srcString.c_str() ;
    const RCString::value_type* end = NULL ;
    uint64_t number64 = RCStringUtil::ConvertStringToUInt64(start, &end);
    if (number64 > 0xFFFFFFFF)
    {
        number = 0 ;
        return 0 ;
    }
    number = (uint32_t)number64;
    return (int32_t)(end - start);
}

HResult RCParseProperties::ParsePropValue(const RCString& name, const RCVariant& prop, uint32_t& resValue)
{
    if (IsInteger64Type(prop))
    {
        if (!name.empty())
        {
            return RC_E_INVALIDARG;
        }
        resValue = static_cast<uint32_t>(GetInteger64Value(prop));
    }
    else if(IsEmptyType(prop))
    {
        if (!name.empty())
        {
            const RCString::value_type* start = name.c_str() ;
            const RCString::value_type* end = NULL ;
            uint64_t v = RCStringUtil::ConvertStringToUInt64(start,&end);
            if (end - start != (int32_t)name.size())
            {
                return RC_E_INVALIDARG;
            }
            resValue = static_cast<uint32_t>(v) ;
        }
    }
    else
    {
        return RC_E_INVALIDARG;
    }
    return RC_S_OK;
}

HResult RCParseProperties::ParsePropDictionaryValue(const RCString& srcStringSpec, uint32_t& dicSize)
{
    RCString srcString = srcStringSpec ;
    RCStringUtil::MakeUpper(srcString) ;
    const RCString::value_type* start = srcString.c_str() ;
    const RCString::value_type* end = NULL ;
    uint64_t number = RCStringUtil::ConvertStringToUInt64(start,&end);
    int32_t numDigits = (int32_t)(end - start);

    if (numDigits == 0 || (int32_t)srcString.size() > numDigits + 1)
    {
        return RC_E_INVALIDARG;
    }

    if ((int32_t)srcString.size() == numDigits)
    {
        if (number >= s_logarithmicSizeLimit)
        {
            return RC_E_INVALIDARG;
        }
        dicSize = (uint32_t)1 << (int32_t)number;
        return RC_S_OK;
    }

    switch (srcString[numDigits])
    {
    case s_byteSymbol:
        if (number >= ((uint64_t)1 << s_logarithmicSizeLimit))
        {
            return RC_E_INVALIDARG;
        }
        dicSize = (uint32_t)number;
        break;
    case s_kiloByteSymbol:
        if (number >= ((uint64_t)1 << (s_logarithmicSizeLimit - 10)))
        {
            return RC_E_INVALIDARG;
        }
        dicSize = (uint32_t)(number << 10);
        break;
    case s_megaByteSymbol:
        if (number >= ((uint64_t)1 << (s_logarithmicSizeLimit - 20)))
        {
            return RC_E_INVALIDARG;
        }
        dicSize = (uint32_t)(number << 20);
        break;
    default:
        return RC_E_INVALIDARG;
    }
    return RC_S_OK;
}

HResult RCParseProperties::ParsePropDictionaryValue(const RCString& name, const RCVariant& prop, uint32_t& resValue)
{
    if (name.empty())
    {
        if (IsInteger64Type(prop))
        {
            uint32_t logDicSize = static_cast<uint32_t>( GetInteger64Value(prop) ) ;
            if (logDicSize >= 32)
            {
                return RC_E_INVALIDARG ;
            }
            resValue = (uint32_t)1 << logDicSize ;
            return RC_S_OK ;
        }

        if (IsStringType(prop))
        {
            return ParsePropDictionaryValue(GetStringValue(prop), resValue) ;
        }
        return RC_E_INVALIDARG ;
    }
    return ParsePropDictionaryValue(name, resValue) ;
}

bool RCParseProperties::StringToBool(const RCString& s, bool& res)
{
    if (s.empty() || 
        RCStringUtil::CompareNoCase(s,_T("ON")) == 0 ||
        RCStringUtil::Compare(s,_T("+")) == 0)
    {
        res = true ;
        return true ;
    }
    if (RCStringUtil::CompareNoCase(s,_T("OFF")) == 0 ||
        RCStringUtil::Compare(s,_T("-")) == 0)
    {
        res = false ;
        return true ;
    }
    return false;
}

HResult RCParseProperties::SetBoolProperty(bool& dest, const RCVariant& value)
{
    if (IsEmptyType(value))
    {
        dest = true;
        return RC_S_OK;
    }
    else if(IsBooleanType(value))
    {
        dest = GetBooleanValue(value) ;
        return RC_S_OK ;
    }
    else if(IsStringType(value))
    {
        return StringToBool(GetStringValue(value), dest) ?  RC_S_OK : RC_E_INVALIDARG;
    }
    return RC_E_INVALIDARG ;
}

HResult RCParseProperties::ParseMtProp(const RCString& name, const RCVariant& prop, uint32_t defaultNumThreads, uint32_t& numThreads)
{
    if (name.empty())
    {
        if (IsInteger64Type(prop))
        {
            numThreads = static_cast<uint32_t>( GetInteger64Value(prop) ) ;
        }
        else
        {
            bool val = false ;
            HResult hr = SetBoolProperty(val, prop) ;
            if (!IsSuccess(hr))
            {
                return hr ;
            }
            numThreads = (val ? defaultNumThreads : 1) ;
        }
    }
    else
    {
        uint32_t number = 0 ;
        int32_t index = ParseStringToUInt32(name, number);
        if (index != (int32_t)name.size())
        {
            return RC_E_INVALIDARG ;
        }
        numThreads = number;
    }
    return RC_S_OK;
}

END_NAMESPACE_RCZIP
