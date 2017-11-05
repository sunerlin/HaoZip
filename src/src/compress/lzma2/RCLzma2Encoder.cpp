/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "compress/lzma2/RCLzma2Encoder.h"
#include "common/RCAlloc.h"
#include "filesystem/RCStreamUtils.h"

/////////////////////////////////////////////////////////////////
//RCLzmaEncoder class implementation

BEGIN_NAMESPACE_RCZIP

#define RC_LZMA2_STREAM_STEP_SIZE ((uint32_t)1 << 31)

#define PROGRESS_UNKNOWN_VALUE ((uint64_t)(int64_t)-1)

#define CONVERT_PR_VAL(x) (x == PROGRESS_UNKNOWN_VALUE ? 0 : x)

struct CCompressProgressImp
{
    ICompressProgress m_progress ;
    ICompressProgressInfo* m_progressInfo ;
    HResult m_result ;
} ;

ISzAlloc RCLzma2Encoder::s_bigAlloc = { &RCLzma2Encoder::SzBigAlloc, &RCLzma2Encoder::SzBigFree };

ISzAlloc RCLzma2Encoder::s_alloc = { &RCLzma2Encoder::SzAlloc, &RCLzma2Encoder::SzFree };

RCLzma2Encoder::RCLzma2Encoder()
{
    m_seqInStream.m_seqInStream.Read = &RCLzma2Encoder::ReadData ;
    m_seqOutStream.m_seqOutStream.Write = &RCLzma2Encoder::WriteData ;
    m_encoder = Lzma2Enc_Create(&s_alloc, &s_bigAlloc);
    RCZIP_ASSERT(m_encoder != 0) ;
}

RCLzma2Encoder::~RCLzma2Encoder()
{
    if (m_encoder != 0)
    {
        Lzma2Enc_Destroy(m_encoder);
    }
}

int32_t RCLzma2Encoder::ReadData(void* object, void* data, size_t* size)
{
    uint32_t curSize = ((*size < RC_LZMA2_STREAM_STEP_SIZE) ? (uint32_t)*size : RC_LZMA2_STREAM_STEP_SIZE);
    HResult res = ((CSeqInStream *)object)->m_realStream->Read(data, curSize, &curSize);
    *size = curSize;
    return (int32_t)res;
}

size_t RCLzma2Encoder::WriteData(void* object, const void* data, size_t size)
{
    CSeqOutStream *p = (CSeqOutStream *)object;
    p->m_result = RCStreamUtils::WriteStream(p->m_realStream.Get(), data, size);
    if (p->m_result != RC_S_OK)
    {
        return 0;
    }
    return size; 
}

void* RCLzma2Encoder::SzAlloc(void *, size_t size)
{ 
    return RCAlloc::Instance().MyAlloc(size);
}

void RCLzma2Encoder::SzFree(void *, void *address)
{ 
    RCAlloc::Instance().MyFree(address);
}

void* RCLzma2Encoder::SzBigAlloc(void *, size_t size)
{
    return RCAlloc::Instance().BigAlloc(size);
}

void RCLzma2Encoder::SzBigFree(void *, void *address)
{
    RCAlloc::Instance().BigFree(address);
}

HResult RCLzma2Encoder::SResToHRESULT(result_t res)
{
    switch(res)
    {
        case SZ_OK: 
            return RC_S_OK;
        case SZ_ERROR_MEM: 
            return RC_E_OUTOFMEMORY;
        case SZ_ERROR_PARAM: 
            return RC_E_INVALIDARG;
        case SZ_ERROR_DATA: 
            return RC_S_FALSE;
        default:
            break ;
    }
    return RC_E_FAIL ;
}


result_t RCLzma2Encoder::CompressProgress(void* pp, uint64_t inSize, uint64_t outSize)
{
    CCompressProgressImp* p = (CCompressProgressImp*)pp;
    p->m_result = p->m_progressInfo->SetRatioInfo(CONVERT_PR_VAL(inSize), CONVERT_PR_VAL(outSize));
    return (result_t)p->m_result;
}

RCString::value_type RCLzma2Encoder::GetUpperChar(RCString::value_type c)
{
    if (c >= _T('a') && c <= _T('z'))
    {
        c -= 0x20 ;
    }
    return c ;
}

int32_t RCLzma2Encoder::ParseMatchFinder(const RCString::value_type* s, int32_t* btMode, int32_t* numHashBytes)
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

HResult RCLzma2Encoder::SetCoderProperties(const RCPropertyIDPairArray& propertyArray)
{
    CLzma2EncProps props;
    Lzma2EncProps_Init(&props);
    RCPropertyIDPairArray::const_iterator pos = propertyArray.begin() ;
    for(; pos != propertyArray.end(); ++pos)
    {
        const RCPropertyIDPair& idPair = *pos ;
        RCPropertyID propID = idPair.first ;
        const RCVariant& propVariant = idPair.second ;
        switch (propID)
        {
            case RCCoderPropID::kBlockSize:
                if (!IsInteger64Type(propVariant))
                {
                    return RC_E_INVALIDARG;
                }
                props.blockSize = static_cast<int32_t>( GetInteger64Value(propVariant) ); 
                break;
            case RCCoderPropID::kNumThreads:
                if (!IsInteger64Type(propVariant))
                {
                    return RC_E_INVALIDARG;
                }
                props.numTotalThreads = static_cast<int32_t>( GetInteger64Value(propVariant) );
                break;
            case RCCoderPropID::kNumFastBytes:
                if (!IsInteger64Type(propVariant))
                {
                    return RC_E_INVALIDARG ;
                }
                props.lzmaProps.fb = static_cast<int32_t>( GetInteger64Value(propVariant) ) ;
                break;
            case RCCoderPropID::kMatchFinderCycles:
                if (!IsInteger64Type(propVariant))
                {
                    return RC_E_INVALIDARG ;
                }
                props.lzmaProps.mc = static_cast<uint32_t>( GetInteger64Value(propVariant) ) ;
                break;
            case RCCoderPropID::kAlgorithm:
                if (!IsInteger64Type(propVariant))
                {
                    return RC_E_INVALIDARG ;
                }
                props.lzmaProps.algo = static_cast<int32_t>( GetInteger64Value(propVariant) ) ;
                break;
            case RCCoderPropID::kDictionarySize:
                if (!IsInteger64Type(propVariant))
                {
                    return RC_E_INVALIDARG ;
                }
                props.lzmaProps.dictSize = static_cast<uint32_t>( GetInteger64Value(propVariant) ) ;
                break;
            case RCCoderPropID::kPosStateBits:
                if (!IsInteger64Type(propVariant))
                {
                    return RC_E_INVALIDARG ;
                }
                props.lzmaProps.pb = static_cast<int32_t>( GetInteger64Value(propVariant) ) ;
                break;
            case RCCoderPropID::kLitPosBits:
                if (!IsInteger64Type(propVariant))
                {
                    return RC_E_INVALIDARG ;
                }
                props.lzmaProps.lp = static_cast<int32_t>( GetInteger64Value(propVariant) ) ;
                break;
            case RCCoderPropID::kLitContextBits:
                if (!IsInteger64Type(propVariant))
                {
                    return RC_E_INVALIDARG ;
                }
                props.lzmaProps.lc = static_cast<int32_t>( GetInteger64Value(propVariant) ) ;
                break;
            case RCCoderPropID::kEndMarker:
                if (!IsBooleanType(propVariant))
                {
                    return RC_E_INVALIDARG ;
                }
                props.lzmaProps.writeEndMark = ( GetBooleanValue(propVariant) ) ? 1 : 0 ;
                break ;
            case RCCoderPropID::kMatchFinder:
                {
                    if (!IsStringType(propVariant))
                    {
                        return RC_E_INVALIDARG ;
                    }
                    RCString matchFinder = GetStringValue(propVariant) ;
                    if (!ParseMatchFinder(matchFinder.c_str(), &props.lzmaProps.btMode, &props.lzmaProps.numHashBytes /* , &_matchFinderBase.skipModeBits */))
                    {
                        return RC_E_INVALIDARG ;
                    }
                }
                break;
            default:
                return RC_E_INVALIDARG;
        }
    }
    return SResToHRESULT(Lzma2Enc_SetProps(m_encoder, &props));
}

HResult RCLzma2Encoder::WriteCoderProperties(ISequentialOutStream* outStream)
{
    byte_t prop = Lzma2Enc_WriteProperties(m_encoder);

    return RCStreamUtils::WriteStream(outStream, &prop, 1);
}

HResult RCLzma2Encoder::Code(ISequentialInStream* inStream,
                             ISequentialOutStream* outStream, 
                             const uint64_t* inSize, 
                             const uint64_t* outSize,
                             ICompressProgressInfo* progress)
{
    CCompressProgressImp progressImp;
    progressImp.m_progress.Progress = CompressProgress;
    progressImp.m_progressInfo = progress;
    progressImp.m_result = SZ_OK;

    m_seqInStream.m_realStream = inStream;
    SetOutStream(outStream);

    result_t res = Lzma2Enc_Encode(m_encoder,
                                   &m_seqOutStream.m_seqOutStream,
                                   &m_seqInStream.m_seqInStream,
                                   progress ? &progressImp.m_progress : NULL);

    ReleaseOutStream();

    if (res == SZ_ERROR_WRITE && m_seqOutStream.m_result != RC_S_OK)
    {
        return m_seqOutStream.m_result ;
    }
    if (res == SZ_ERROR_PROGRESS && progressImp.m_result != RC_S_OK)
    {
        return progressImp.m_result;
    }
    return SResToHRESULT(res);
}

HResult RCLzma2Encoder::SetOutStream(ISequentialOutStream* outStream) 
{
    m_seqOutStream.m_realStream = outStream ;
    m_seqOutStream.m_result = RC_S_OK ;
    return RC_S_OK ;
}

HResult RCLzma2Encoder::ReleaseOutStream()
{
    m_seqOutStream.m_realStream.Release();
    return RC_S_OK ;
}

END_NAMESPACE_RCZIP