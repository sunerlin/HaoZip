/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "compress/ppmd/RCPPMDEncoder.h"
#include "filesystem/RCStreamUtils.h"
#include "compress/ppmd/RCPPMDType.h"

/////////////////////////////////////////////////////////////////
//RCPPMDEncoder class implementation

BEGIN_NAMESPACE_RCZIP

const uint32_t kMinMemSize = (1 << 11);
const uint32_t kMinOrder = 2;
const uint32_t kUsedMemorySizeDefault = (1 << 24);
const int32_t  kOrderDefault = 6 ;

class RCPPMDEncoderFlusher
{
  public:
    RCPPMDEncoderFlusher(RCPPMDEncoder *encoder):
        _encoder(encoder)
    {
    }
    ~RCPPMDEncoderFlusher()
    {
      _encoder->Flush() ;
      _encoder->ReleaseStreams() ;
    }
private:
    RCPPMDEncoder* _encoder ;
};

RCPPMDEncoder::RCPPMDEncoder():
    m_usedMemorySize(kUsedMemorySizeDefault),
    m_order(kOrderDefault)
{
}

RCPPMDEncoder::~RCPPMDEncoder()
{
}

HResult RCPPMDEncoder::Flush()
{
    m_rangeEncoder.FlushData();
    return m_rangeEncoder.FlushStream();
}

void RCPPMDEncoder::ReleaseStreams()
{
    m_inStream.ReleaseStream();
    m_rangeEncoder.ReleaseStream();
}

HResult RCPPMDEncoder::Code(ISequentialInStream* inStream,
                            ISequentialOutStream* outStream, 
                            const uint64_t* inSize, 
                            const uint64_t* outSize,
                            ICompressProgressInfo* progress)
{
    try
    {
        return CodeReal(inStream, outStream, inSize, outSize, progress);
    }
    catch(HResult errorCode)
    {
        return errorCode ;
    }
    catch(...)
    {
        return RC_E_FAIL ;
    }
}

HResult RCPPMDEncoder::CodeReal(ISequentialInStream* inStream,
                                ISequentialOutStream* outStream, 
                                const uint64_t* inSize, 
                                const uint64_t* outSize,
                                ICompressProgressInfo* progress)
{
    if (!m_inStream.Create(1 << 20))
    {
        return RC_E_OUTOFMEMORY;
    }
    if (!m_rangeEncoder.Create(1 << 20))
    {
        return RC_E_OUTOFMEMORY;
    }
    if (!m_info.m_subAllocator.StartSubAllocator(m_usedMemorySize))
    {
        return RC_E_OUTOFMEMORY;
    }

    m_inStream.SetStream(inStream);
    m_inStream.Init();

    m_rangeEncoder.SetStream(outStream);
    m_rangeEncoder.Init();

    RCPPMDEncoderFlusher flusher(this);

    m_info.m_maxOrder = 0;
    m_info.StartModelRare(m_order);

    for (;;)
    {
        uint32_t size = (1 << 18);
        do
        {
            byte_t symbol;
            if (!m_inStream.ReadByte(symbol))
            {
                // here we can write End Mark for stream version.
                // In current version this feature is not used.
                // m_info.EncodeSymbol(-1, &m_rangeEncoder);
                return RC_S_OK;
            }
            m_info.EncodeSymbol(symbol, &m_rangeEncoder);
        }
        while (--size != 0) ;
        if (progress != NULL)
        {
            uint64_t inSize = m_inStream.GetProcessedSize();
            uint64_t outSize = m_rangeEncoder.GetProcessedSize();
            HResult hr = progress->SetRatioInfo(inSize, outSize) ;
            if(!IsSuccess(hr))
            {
                return hr ;
            }
        }
    }
    return RC_S_FALSE ;
}

HResult RCPPMDEncoder::SetCoderProperties(const RCPropertyIDPairArray& propertyArray)
{
    RCPropertyIDPairArray::const_iterator pos = propertyArray.begin() ;
    for(; pos != propertyArray.end(); ++pos)
    {
        const RCPropertyIDPair& idPair = *pos ;
        RCPropertyID propID = idPair.first ;
        const RCVariant& propVariant = idPair.second ;
        switch (propID)
        {
            case RCCoderPropID::kUsedMemorySize:
                {
                    if (!IsInteger64Type(propVariant))
                    {
                        return RC_E_INVALIDARG ;
                    }
                    uint32_t value = static_cast<uint32_t>( GetInteger64Value(propVariant) ) ;                    
                    if (value < kMinMemSize || value > RCPPMDSubAlloc::s_kMaxMemBlockSize)
                    {
                        return RC_E_INVALIDARG;
                    }
                    m_usedMemorySize = value ;
                }
                break ;
            case RCCoderPropID::kOrder:
                {
                    if (!IsInteger64Type(propVariant))
                    {
                        return RC_E_INVALIDARG ;
                    }
                    uint32_t value = static_cast<uint32_t>( GetInteger64Value(propVariant) ) ;
                    if (value < kMinOrder || value > kMaxOrderCompress)
                    {
                        return RC_E_INVALIDARG ;
                    }
                    m_order = (byte_t)value ;
                }
                break;
            default:
                return RC_E_INVALIDARG ;
        }
    }
    return RC_S_OK;
}

HResult RCPPMDEncoder::WriteCoderProperties(ISequentialOutStream* outStream)
{
    const uint32_t kPropSize = 5;
    byte_t properties[kPropSize];
    properties[0] = m_order;
    for (int i = 0; i < 4; i++)
    {
        properties[1 + i] = byte_t(m_usedMemorySize >> (8 * i));
    }
    return RCStreamUtils::WriteStream(outStream, properties, kPropSize);
}

END_NAMESPACE_RCZIP
