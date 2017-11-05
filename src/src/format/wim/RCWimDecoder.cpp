/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "format/wim/RCWimDecoder.h"

/////////////////////////////////////////////////////////////////
//RCWimDecoder class implementation

BEGIN_NAMESPACE_RCZIP

class RCWimDecoderFlusher
{
public:
    RCWimDecoderFlusher(RCWimDecoder *decoder):
        m_Decoder(decoder), 
        NeedFlush(true)
    {
    }
    
    ~RCWimDecoderFlusher()
    {
        if (NeedFlush)
        {
            m_Decoder->Flush();
        }
        m_Decoder->ReleaseStreams() ;
    }
public:
    bool NeedFlush ;
    
private:
    RCWimDecoder* m_Decoder ;
};

RCWimDecoder::RCWimDecoder()
{
}

RCWimDecoder::~RCWimDecoder()
{
}

void RCWimDecoder::ReleaseStreams()
{
    m_outWindowStream.ReleaseStream();
    m_inBitStream.ReleaseStream();
}

HResult RCWimDecoder::Flush()
{
    return m_outWindowStream.Flush();
}

HResult RCWimDecoder::Code(ISequentialInStream* inStream, ISequentialOutStream* outStream, uint32_t outSize)
{
    try
    {
        return CodeReal(inStream, outStream, outSize);
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

HResult RCWimDecoder::CodeSpec(uint32_t outSize)
{
    {
        byte_t levels[s_kWimMainTableSize];
        for (int32_t i = 0; i < s_kWimMainTableSize; i += 2)
        {
            byte_t b = m_inBitStream.DirectReadByte();
            levels[i] = b & 0xF;
            levels[i + 1] = b >> 4;
        }
        if (!m_mainDecoder.SetCodeLengths(levels))
        {
            return RC_S_FALSE;
        }
    }

    while (outSize > 0)
    {
        uint32_t number = m_mainDecoder.DecodeSymbol(&m_inBitStream);
        if (number < 256)
        {
            m_outWindowStream.PutByte((byte_t)number);
            outSize--;
        }
        else
        {
            if (number >= s_kWimMainTableSize)
            {
                return RC_S_FALSE;
            }
            uint32_t posLenSlot = number - 256;
            uint32_t posSlot = posLenSlot / s_kWimNumLenSlots;
            uint32_t len = posLenSlot % s_kWimNumLenSlots;
            uint32_t distance = (1 << posSlot) - 1 + m_inBitStream.ReadBits(posSlot);

            if (len == s_kWimNumLenSlots - 1)
            {
                len = m_inBitStream.DirectReadByte();
                if (len == 0xFF)
                {
                    len = m_inBitStream.DirectReadByte();
                    len |= (uint32_t)m_inBitStream.DirectReadByte() << 8;
                }
                else
                {
                    len += s_kWimNumLenSlots - 1;
                }
            }

            len += s_kWimMatchMinLen;
            uint32_t locLen = (len <= outSize ? len : outSize);

            if (!m_outWindowStream.CopyBlock(distance, locLen))
            {
                return RC_S_FALSE;
            }

            len -= locLen;
            outSize -= locLen;
            if (len != 0)
            {
                return RC_S_FALSE;
            }
        }
    }
    return RC_S_OK;
}

HResult RCWimDecoder::CodeReal(ISequentialInStream *inStream, ISequentialOutStream *outStream, uint32_t outSize)
{
    if (!m_outWindowStream.Create(s_kWimDictSize) || !m_inBitStream.Create(1 << 16))
    {
        return RC_E_OUTOFMEMORY ;
    }

    RCWimDecoderFlusher flusher(this);

    m_inBitStream.SetStream(inStream);
    m_outWindowStream.SetStream(outStream);
    m_inBitStream.Init();
    m_outWindowStream.Init(false);

    HResult hr = CodeSpec(outSize) ;
    if(!IsSuccess(hr))
    {
        return hr ;
    }

    flusher.NeedFlush = false;
    return Flush() ;
}

END_NAMESPACE_RCZIP
