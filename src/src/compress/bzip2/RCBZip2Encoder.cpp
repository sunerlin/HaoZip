/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "compress/bzip2/RCBZip2Encoder.h"
#include "compress/bzip2/RCBZip2Defs.h"

/////////////////////////////////////////////////////////////////
//RCBZip2Encoder class implementation

BEGIN_NAMESPACE_RCZIP

#define BZIP2_ENCODER_BUFFER_SIZE (1 << 17)

class RCBZip2EncoderFlusher
{
public:
    RCBZip2EncoderFlusher(RCBZip2Encoder *coder):
        _coder(coder), 
        m_needFlush(true)
    {
    }
    ~RCBZip2EncoderFlusher()
    {
        if (m_needFlush)
        {
            _coder->Flush();
        }
        _coder->ReleaseStreams();
    }
    
public:
    bool m_needFlush ;
    
private:
    RCBZip2Encoder* _coder;
};

RCBZip2Encoder::RCBZip2Encoder():
    m_numPasses(1),
    m_blockSizeMult(RCBZip2Defs::s_kBlockSizeMultMax),
    m_optimizeNumTables(false)
{
#ifdef COMPRESS_BZIP2_MT
    m_threadsInfo = 0;
    m_numThreadsPrev = 0;
    m_numThreads = 1;
#endif
}

RCBZip2Encoder::~RCBZip2Encoder()
{
#ifdef COMPRESS_BZIP2_MT
    Free() ;
#endif
}

void RCBZip2Encoder::WriteBytes(const byte_t* data, uint32_t sizeInBits, byte_t lastByte)
{
    uint32_t bytesSize = (sizeInBits / 8);
    for (uint32_t i = 0; i < bytesSize; i++)
    {
        m_outStream.WriteBits(data[i], 8);
    }
    WriteBits(lastByte, (sizeInBits & 7));
}

uint32_t RCBZip2Encoder::ReadRleBlock(byte_t* buffer)
{
    uint32_t i = 0;
    byte_t prevByte;
    if (m_inStream.ReadByte(prevByte))
    {
        uint32_t blockSize = m_blockSizeMult * RCBZip2Defs::s_kBlockSizeStep - 1;
        int32_t numReps = 1;
        buffer[i++] = prevByte;
        while (i < blockSize) // "- 1" to support RLE
        {
            byte_t b;
            if (!m_inStream.ReadByte(b))
            {
                break;
            }
            if (b != prevByte)
            {
                if (numReps >= RCBZip2Defs::s_kRleModeRepSize)
                {
                    buffer[i++] = (byte_t)(numReps - RCBZip2Defs::s_kRleModeRepSize);
                }
                buffer[i++] = b;
                numReps = 1;
                prevByte = b;
                continue;
            }
            numReps++;
            if (numReps <= RCBZip2Defs::s_kRleModeRepSize)
            {
                buffer[i++] = b;
            }
            else if (numReps == RCBZip2Defs::s_kRleModeRepSize + 255)
            {
                buffer[i++] = (byte_t)(numReps - RCBZip2Defs::s_kRleModeRepSize);
                numReps = 0;
            }
        }
        // it's to support original BZip2 decoder
        if (numReps >= RCBZip2Defs::s_kRleModeRepSize)
        {
            buffer[i++] = (byte_t)(numReps - RCBZip2Defs::s_kRleModeRepSize);
        }
    }
    return i;
}

void RCBZip2Encoder::WriteBits(uint32_t value, uint32_t numBits)
{
    m_outStream.WriteBits(value, numBits) ;
}

void RCBZip2Encoder::WriteByte(byte_t b)
{
    WriteBits(b , 8) ;
}

void RCBZip2Encoder::WriteBit(bool v)
{
    WriteBits((v ? 1 : 0), 1);
}

void RCBZip2Encoder::WriteCRC(uint32_t v)
{
    for (int32_t i = 0; i < 4; i++)
    {
        WriteByte(((byte_t)(v >> (24 - i * 8))));
    }
}

HResult RCBZip2Encoder::Flush()
{
    return m_outStream.Flush();
}

void RCBZip2Encoder::ReleaseStreams()
{
    m_inStream.ReleaseStream();
    m_outStream.ReleaseStream();
}

HResult RCBZip2Encoder::Code(ISequentialInStream* inStream,
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

HResult RCBZip2Encoder::CodeReal(ISequentialInStream* inStream,
                                 ISequentialOutStream* outStream, 
                                 const uint64_t* inSize, 
                                 const uint64_t* outSize,
                                 ICompressProgressInfo* progress)
{
#ifdef COMPRESS_BZIP2_MT
    m_progress = progress;
    HResult hr = Create() ;
    if(!IsSuccess(hr))
    {
        return hr ;
    }
    for (uint32_t t = 0; t < m_numThreads; t++)
#endif
    {
#ifdef COMPRESS_BZIP2_MT
        RCBZip2ThreadInfo &ti = m_threadsInfo[t];
        if (m_mtMode)
        {
            hr = ti.m_streamWasFinishedEvent.Reset() ;
            if(!IsSuccess(hr))
            {
                return hr ;
            }
            hr = ti.m_waitingWasStartedEvent.Reset() ;
            if(!IsSuccess(hr))
            {
                return hr ;
            }
            hr = ti.m_canWriteEvent.Reset() ;
            if(!IsSuccess(hr))
            {
                return hr ;
            }
        }
#else
        RCBZip2ThreadInfo& ti = m_threadsInfo ;
        ti.m_encoder = this;
#endif

        ti.m_optimizeNumTables = m_optimizeNumTables;

        if (!ti.Alloc())
        {
            return RC_E_OUTOFMEMORY;
        }
    }


    if (!m_inStream.Create(BZIP2_ENCODER_BUFFER_SIZE))
    {
        return RC_E_OUTOFMEMORY;
    }
    if (!m_outStream.Create(BZIP2_ENCODER_BUFFER_SIZE))
    {
        return RC_E_OUTOFMEMORY;
    }

    m_inStream.SetStream(inStream);
    m_inStream.Init();

    m_outStream.SetStream(outStream);
    m_outStream.Init();

    RCBZip2EncoderFlusher flusher(this);

    m_combinedCRC.Init();
#ifdef COMPRESS_BZIP2_MT
    m_nextBlockIndex = 0;
    m_streamWasFinished = false;
    m_closeThreads = false;
    m_canStartWaitingEvent.Reset();
#endif

    WriteByte(RCBZip2Defs::s_kArSig0);
    WriteByte(RCBZip2Defs::s_kArSig1);
    WriteByte(RCBZip2Defs::s_kArSig2);
    WriteByte((byte_t)(RCBZip2Defs::s_kArSig3 + m_blockSizeMult));

#ifdef COMPRESS_BZIP2_MT
    if (m_mtMode)
    {
        m_threadsInfo[0].m_canWriteEvent.Set();
        m_result = RC_S_OK;
        m_canProcessEvent.Set();
        uint32_t t;
        for (t = 0; t < m_numThreads; t++)
        {
            m_threadsInfo[t].m_streamWasFinishedEvent.Lock() ;
        }
        m_canProcessEvent.Reset();
        m_canStartWaitingEvent.Set();
        for (t = 0; t < m_numThreads; t++)
        {
            m_threadsInfo[t].m_waitingWasStartedEvent.Lock();
        }
        m_canStartWaitingEvent.Reset();
        if(!IsSuccess(m_result))
        {
            return m_result ;
        }
    }
    else
#endif
    {
        for (;;)
        {
            RCBZip2ThreadInfo& ti =
#ifdef COMPRESS_BZIP2_MT
                m_threadsInfo[0];
#else
                m_threadsInfo ;
#endif
            uint32_t blockSize = ReadRleBlock(ti.m_block);
            if (blockSize == 0)
            {
                break;
            }
            HResult hr = ti.EncodeBlock3(blockSize) ;
            if(!IsSuccess(hr))
            {
                return hr ;
            }
            if (progress)
            {
                uint64_t packSize = m_inStream.GetProcessedSize();
                uint64_t unpackSize = m_outStream.GetProcessedSize();
                hr = progress->SetRatioInfo(packSize, unpackSize) ;
                if(!IsSuccess(hr))
                {
                    return hr ;
                }
            }
        }
    }
    WriteByte(RCBZip2Defs::s_kFinSig0);
    WriteByte(RCBZip2Defs::s_kFinSig1);
    WriteByte(RCBZip2Defs::s_kFinSig2);
    WriteByte(RCBZip2Defs::s_kFinSig3);
    WriteByte(RCBZip2Defs::s_kFinSig4);
    WriteByte(RCBZip2Defs::s_kFinSig5);

    WriteCRC(m_combinedCRC.GetDigest());
    return Flush() ;
}

HResult RCBZip2Encoder::SetCoderProperties(const RCPropertyIDPairArray& propertyArray)
{
    RCPropertyIDPairArray::const_iterator pos = propertyArray.begin() ;
    for(; pos != propertyArray.end(); ++pos)
    {
        const RCPropertyIDPair& idPair = *pos ;
        RCPropertyID propID = idPair.first ;
        const RCVariant& propVariant = idPair.second ;
        switch (propID)
        {
            case RCCoderPropID::kNumPasses:
                {
                    if (!IsInteger64Type(propVariant))
                    {
                        return RC_E_INVALIDARG ;
                    }
                    uint32_t numPasses = static_cast<uint32_t>( GetInteger64Value(propVariant) ) ;
                    if (numPasses == 0)
                    {
                        numPasses = 1;
                    }
                    if (numPasses > RCBZip2Defs::s_kNumPassesMax)
                    {
                        numPasses = RCBZip2Defs::s_kNumPassesMax;
                    }
                    m_numPasses = numPasses;
                    m_optimizeNumTables = (m_numPasses > 1);
                }
                break ;
             case RCCoderPropID::kDictionarySize:
                {
                    if (!IsInteger64Type(propVariant))
                    {
                        return RC_E_INVALIDARG ;
                    }
                    uint32_t dictionary = static_cast<uint32_t>( GetInteger64Value(propVariant) )/RCBZip2Defs::s_kBlockSizeStep ;
                    if (dictionary < RCBZip2Defs::s_kBlockSizeMultMin)
                    {
                        dictionary = RCBZip2Defs::s_kBlockSizeMultMin;
                    }
                    else if (dictionary > RCBZip2Defs::s_kBlockSizeMultMax)
                    {
                        dictionary = RCBZip2Defs::s_kBlockSizeMultMax;
                    }
                    m_blockSizeMult = dictionary;
                }
                break ;
             case RCCoderPropID::kNumThreads:
                {
#ifdef COMPRESS_BZIP2_MT 
                    if (!IsInteger64Type(propVariant))
                    {
                        return RC_E_INVALIDARG ;
                    }
                    m_numThreads = static_cast<uint32_t>( GetInteger64Value(propVariant) ) ;
                    if (m_numThreads < 1)
                    {
                        m_numThreads = 1; 
                    }
#endif
                }
                break ;
             default:
                return RC_E_INVALIDARG ;
        }
    }
    return RC_S_OK;
}

#ifdef COMPRESS_BZIP2_MT

HResult RCBZip2Encoder::Create()
{
    WRes res = m_canProcessEvent.CreateIfNotCreated() ;
    if(res != 0)
    {
        return res ;
    }
    res = m_canStartWaitingEvent.CreateIfNotCreated() ;
    if(res != 0)
    {
        return res ;
    }
    if (m_threadsInfo != 0 && m_numThreadsPrev == m_numThreads)
    {
        return RC_S_OK;
    }
    try
    {
        Free();
        m_mtMode = (m_numThreads > 1);
        m_numThreadsPrev = m_numThreads;
        m_threadsInfo = new RCBZip2ThreadInfo[m_numThreads];
        if (m_threadsInfo == NULL)
        {
            return RC_E_OUTOFMEMORY;
        }
    }
    catch(...)
    {
        return RC_E_OUTOFMEMORY;
    }
    for (uint32_t t = 0; t < m_numThreads; t++)
    {
        RCBZip2ThreadInfo& ti = m_threadsInfo[t];
        ti.m_encoder = this;
        if (m_mtMode)
        {
            HResult res = ti.Create();
            if (res != RC_S_OK)
            {
                m_numThreads = t;
                Free();
                return res;
            }
        }
    }
    return RC_S_OK;
}

void RCBZip2Encoder::Free()
{
    if (!m_threadsInfo)
    {
        return;
    }
    m_closeThreads = true;
    m_canProcessEvent.Set();
    for (uint32_t t = 0; t < m_numThreads; t++)
    {
        RCBZip2ThreadInfo& ti = m_threadsInfo[t];
        if (m_mtMode && (ti.m_spThread != NULL) && ti.m_spThread->IsRunning() )
        {
            ti.m_spThread->Join() ;
            ti.m_spThread.reset() ;
        }
        ti.Free() ;
    }
    delete [] m_threadsInfo ;
    m_threadsInfo = NULL ;
}

HResult RCBZip2Encoder::SetNumberOfThreads(uint32_t numThreads)
{
    m_numThreads = numThreads;
    if (m_numThreads < 1)
    {
        m_numThreads = 1 ;
    }
    return RC_S_OK;
}

#endif

END_NAMESPACE_RCZIP
