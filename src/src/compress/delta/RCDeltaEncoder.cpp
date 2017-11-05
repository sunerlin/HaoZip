/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "compress/delta/RCDeltaEncoder.h"
#include "filesystem/RCStreamUtils.h"

/////////////////////////////////////////////////////////////////
//RCDeltaEncoder class implementation

BEGIN_NAMESPACE_RCZIP

HResult RCDeltaEncoder::Init()
{
    DeltaInit();
    return RC_S_OK;
}

uint32_t RCDeltaEncoder::Filter(byte_t* data, uint32_t size)
{
    Delta_Encode(m_state, m_delta, data, size);
    return size;
}

HResult RCDeltaEncoder::SetCoderProperties(const RCPropertyIDPairArray& propertyArray)
{
    uint32_t delta = m_delta;

    RCPropertyIDPairArray::const_iterator pos = propertyArray.begin() ;
    for(; pos != propertyArray.end(); ++pos)
    {
        const RCPropertyIDPair& idPair = *pos ;
        RCPropertyID propID = idPair.first ;
        const RCVariant& propVariant = idPair.second ;
        if (!IsInteger64Type(propVariant))
        {
            delta = static_cast<int32_t>( GetInteger64Value(propVariant) );
            if (delta < 1 || delta > 256)
            {
                return RC_E_INVALIDARG;
            }
        }
        else
        {
            return RC_E_INVALIDARG ;
        }
    }

    m_delta = delta;

    return RC_S_OK;
}

HResult RCDeltaEncoder::WriteCoderProperties(ISequentialOutStream* outStream)
{
    byte_t prop = (byte_t)(m_delta - 1);

    return RCStreamUtils::WriteStream(outStream, &prop, 1);
}

END_NAMESPACE_RCZIP
