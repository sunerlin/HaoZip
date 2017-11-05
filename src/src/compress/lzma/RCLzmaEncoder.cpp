/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "compress/lzma/RCLzmaEncoder.h"
#include "common/RCCoderWrappers.h"
#include "filesystem/RCStreamUtils.h"
#include "common/RCAlloc.h"

/////////////////////////////////////////////////////////////////
//RCLzmaEncoder class implementation

BEGIN_NAMESPACE_RCZIP

ISzAlloc RCLzmaEncoder::s_bigAlloc = { &RCLzmaEncoder::SzBigAlloc, &RCLzmaEncoder::SzBigFree };
ISzAlloc RCLzmaEncoder::s_alloc = { &RCLzmaEncoder::SzAlloc, &RCLzmaEncoder::SzFree };

RCLzmaEncoder::RCLzmaEncoder()
{
    m_encoder = LzmaEnc_Create(&s_alloc);
    RCZIP_ASSERT(m_encoder != 0) ;
}

RCLzmaEncoder::~RCLzmaEncoder()
{
    if (m_encoder != 0)
    {
        LzmaEnc_Destroy(m_encoder, &s_alloc, &s_bigAlloc);
    }
}

void* RCLzmaEncoder::SzBigAlloc(void*, size_t size)
{
    return RCAlloc::Instance().BigAlloc(size);
}

void RCLzmaEncoder::SzBigFree(void* , void* address)
{
    RCAlloc::Instance().BigFree(address);
}

void* RCLzmaEncoder::SzAlloc(void*, size_t size)
{
    return RCAlloc::Instance().MyAlloc(size);
}

void RCLzmaEncoder::SzFree(void*, void *address)
{
    RCAlloc::Instance().MyFree(address);
}

RCString::value_type RCLzmaEncoder::GetUpperChar(RCString::value_type c)
{
    if (c >= _T('a') && c <= _T('z'))
    {
        c -= 0x20 ;
    }
    return c ;
}

int32_t RCLzmaEncoder::ParseMatchFinder(const RCString::value_type* s, int32_t* btMode, int32_t* numHashBytes)
{
    RCString::value_type c = GetUpperChar(*s++);
    if (c == _T('H'))
    {
        if (GetUpperChar(*s++) != _T('C'))
        {
            return 0;
        }
        int32_t numHashBytesLoc = (int32_t)(*s++ - _T('0'));
        if (numHashBytesLoc < 4 || numHashBytesLoc > 4)
        {
            return 0;
        }
        if (*s++ != 0)
        {
            return 0;
        }
        *btMode = 0;
        *numHashBytes = numHashBytesLoc;
        return 1;
    }
    if (c != _T('B'))
    {
        return 0;
    }

    if (GetUpperChar(*s++) != _T('T'))
    {
        return 0;
    }
    int32_t numHashBytesLoc = (int32_t)(*s++ - _T('0'));
    if (numHashBytesLoc < 2 || numHashBytesLoc > 4)
    {
        return 0;
    }
    c = GetUpperChar(*s++);
    if (c != _T('\0'))
    {
        return 0;
    }
    *btMode = 1;
    *numHashBytes = numHashBytesLoc;
    return 1 ;
}

HResult RCLzmaEncoder::SetLzmaProp(RCCoderPropIDEnumType propID, const RCVariant& prop, CLzmaEncProps& ep)
{
    if (propID == RCCoderPropID::kMatchFinder)
    {
        if (!IsStringType(prop))
        {
            return RC_E_INVALIDARG ;
        }
        RCString proValue = prop.GetStringValue() ;
        return ParseMatchFinder(proValue.c_str(), &ep.btMode, &ep.numHashBytes) ? RC_S_OK : RC_E_INVALIDARG ;
    }
    if (!IsInteger64Type(prop))
    {
        return RC_E_INVALIDARG;
    }
    uint32_t v = (uint32_t)(int64_t)GetInteger64Value(prop) ;
    switch (propID)
    {
    case RCCoderPropID::kNumFastBytes: 
        ep.fb = v; 
        break;
    case RCCoderPropID::kMatchFinderCycles: 
        ep.mc = v; 
        break;
    case RCCoderPropID::kAlgorithm: 
        ep.algo = v; 
        break;
    case RCCoderPropID::kDictionarySize: 
        ep.dictSize = v; 
        break;
    case RCCoderPropID::kPosStateBits: 
        ep.pb = v; 
        break;
    case RCCoderPropID::kLitPosBits: 
        ep.lp = v; 
        break;
    case RCCoderPropID::kLitContextBits: 
        ep.lc = v; 
        break;
    default: 
        return RC_E_INVALIDARG;
    }
    return RC_S_OK ;
}

HResult RCLzmaEncoder::Code(ISequentialInStream* inStream,
                            ISequentialOutStream* outStream, 
                            const uint64_t* inSize, 
                            const uint64_t* outSize,
                            ICompressProgressInfo* progress)
{
    RCSeqInStreamWrap inWrap(inStream);
    RCSeqOutStreamWrap outWrap(outStream);
    RCCompressProgressWrap progressWrap(progress);

    result_t res = LzmaEnc_Encode(m_encoder, &outWrap.m_seqOut, &inWrap.m_seqIn, progress ? &progressWrap.m_progress : NULL, &s_alloc, &s_bigAlloc);
    if (res == SZ_ERROR_READ && inWrap.m_hr != RC_S_OK)
    {
        return inWrap.m_hr ;
    }
    if (res == SZ_ERROR_WRITE && outWrap.m_hr != RC_S_OK)
    {
        return outWrap.m_hr;
    }
    if (res == SZ_ERROR_PROGRESS && progressWrap.m_hr != RC_S_OK)
    {
        return progressWrap.m_hr;
    }
    return SResToHResult(res) ;
}

HResult RCLzmaEncoder::SetCoderProperties(const RCPropertyIDPairArray& propertyArray)
{
    CLzmaEncProps props;
    LzmaEncProps_Init(&props);
    RCPropertyIDPairArray::const_iterator pos = propertyArray.begin() ;
    for(; pos != propertyArray.end(); ++pos)
    {
        const RCPropertyIDPair& idPair = *pos ;
        RCPropertyID propID = idPair.first ;
        const RCVariant& prop = idPair.second ;
        switch (propID)
        {
        case RCCoderPropID::kEndMarker:
            if (!IsBooleanType(prop))
            {
                return RC_E_INVALIDARG ;
            }
            props.writeEndMark = GetBooleanValue(prop) ; 
            break;
      case RCCoderPropID::kNumThreads:
            if (!IsInteger64Type(prop))
            {
                return RC_E_INVALIDARG; 
            }
            props.numThreads = (int32_t)GetInteger64Value(prop) ; 
            break;
      default:
            {
                HResult hr = SetLzmaProp((RCCoderPropIDEnumType)propID, prop, props) ;
                if(!IsSuccess(hr))
                {
                    return hr ;
                }
            }
            break ;
        }
    }
    return SResToHResult(LzmaEnc_SetProps(m_encoder, &props));
}

HResult RCLzmaEncoder::WriteCoderProperties(ISequentialOutStream* outStream)
{
    byte_t props[LZMA_PROPS_SIZE];
    size_t size = LZMA_PROPS_SIZE;
    HResult hr = LzmaEnc_WriteProperties(m_encoder, props, &size) ;
    if(!IsSuccess(hr))
    {
        return hr ;
    }
    return RCStreamUtils::WriteStream(outStream, props, size);
}

END_NAMESPACE_RCZIP
