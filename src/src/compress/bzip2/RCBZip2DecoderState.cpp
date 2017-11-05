/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "compress/bzip2/RCBZip2DecoderState.h"
#include "compress/bzip2/RCBZip2Decoder.h"
#include "compress/bzip2/RCBZip2Defs.h"
#include "common/RCAlloc.h"

/////////////////////////////////////////////////////////////////
//RCBZip2DecoderState class implementation

BEGIN_NAMESPACE_RCZIP

RCBZip2DecoderState::RCBZip2DecoderState():
    m_counters(NULL)
{
}

RCBZip2DecoderState::~RCBZip2DecoderState()
{
    Free();
}

bool RCBZip2DecoderState::Alloc()
{
    if (m_counters == 0)
    {
        m_counters = (uint32_t *)RCAlloc::Instance().BigAlloc((256 + RCBZip2Defs::s_kBlockSizeMax) * sizeof(uint32_t));
    }
    return (m_counters != NULL);
}

void RCBZip2DecoderState::Free()
{
    RCAlloc::Instance().BigFree(m_counters);
    m_counters = NULL ;
#ifdef COMPRESS_BZIP2_MT
    m_spThread.reset() ;
#endif
}

#ifdef COMPRESS_BZIP2_MT

static void MFThread(void* p) { ((RCBZip2DecoderState *)p)->ThreadFunc(); }
#define RINOK_THREAD(x) { WRes __result_ = (x); if(__result_ != 0) return __result_; }

HResult RCBZip2DecoderState::Create()
{
    RINOK_THREAD(m_streamWasFinishedEvent.CreateIfNotCreated());
    RINOK_THREAD(m_waitingWasStartedEvent.CreateIfNotCreated());
    RINOK_THREAD(m_canWriteEvent.CreateIfNotCreated());
    m_spThread.reset( new RCThread(MFThread, this) ) ;
    if(m_spThread->Start())
    {
        return RC_S_OK ;
    }
    else
    {
        return RC_E_FAIL ;
    }
}

void RCBZip2DecoderState::FinishStream()
{
    m_decoder->m_streamWasFinished1 = true;
    m_streamWasFinishedEvent.Set();
    m_decoder->m_critSection.Lock();
    m_decoder->m_canStartWaitingEvent.Lock();
    m_waitingWasStartedEvent.Set();
}

void RCBZip2DecoderState::ThreadFunc()
{
    for (;;)
    {
        m_decoder->m_canProcessEvent.Lock();
        m_decoder->m_critSection.Lock();
        if (m_decoder->m_isCloseThreads)
        {
            m_decoder->m_critSection.UnLock();
            return;
        }
        if (m_decoder->m_streamWasFinished1)
        {
            FinishStream();
            continue;
        }
        HResult res = RC_S_OK;

        uint32_t blockIndex = m_decoder->m_nextBlockIndex;
        uint32_t nextBlockIndex = blockIndex + 1;
        if (nextBlockIndex == m_decoder->m_numThreads)
        {
            nextBlockIndex = 0;
        }
        m_decoder->m_nextBlockIndex = nextBlockIndex;
        uint32_t crc = 0 ;
        uint64_t packSize = 0;
        uint32_t blockSize = 0 ;
        uint32_t origPtr = 0;
        bool randMode = false;

        try
        {
            bool wasFinished = false ;
            res = m_decoder->ReadSignatures(wasFinished, crc);
            if (res != RC_S_OK)
            {
                m_decoder->m_result1 = res;
                FinishStream();
                continue;
            }
            if (wasFinished)
            {
                m_decoder->m_result1 = res;
                FinishStream();
                continue;
            }

            res = RCBZip2Decoder::ReadBlock(&m_decoder->m_inStream, m_counters, m_decoder->m_blockSizeMax,
                                            m_decoder->m_selectors, m_decoder->m_huffmanDecoders,
                                            &blockSize, &origPtr, &randMode);
            if (res != RC_S_OK)
            {
                m_decoder->m_result1 = res;
                FinishStream();
                continue;
            }
            packSize = m_decoder->m_inStream.GetProcessedSize();
        }
        catch(HResult errorCode)
        {
            res = errorCode ;
            if (res != RC_S_OK)
            {
                res = RC_E_FAIL ; 
            }
        }
        catch(...)
        {
            res = RC_E_FAIL;
        }
        if (res != RC_S_OK)
        {
            m_decoder->m_result1 = res;
            FinishStream();
            continue;
        }

        m_decoder->m_critSection.UnLock();

        RCBZip2Decoder::DecodeBlock1(m_counters, blockSize);

        bool needFinish = true;
        try
        {
            m_decoder->m_states[blockIndex].m_canWriteEvent.Lock();
            needFinish = m_decoder->m_streamWasFinished2;
            if (!needFinish)
            {
                if ((randMode ?
                               RCBZip2Decoder::DecodeBlock2Rand(m_counters + 256, blockSize, origPtr, m_decoder->m_outStream) :
                               RCBZip2Decoder::DecodeBlock2(m_counters + 256, blockSize, origPtr, m_decoder->m_outStream)) == crc)
                {
                    res = m_decoder->SetRatioProgress(packSize);
                }
                else
                {
                    res = RC_S_FALSE;
                }
            }
        }
        catch(HResult errorCode)
        {
            res = errorCode ;
            if (res != RC_S_OK)
            {
                res = RC_E_FAIL ; 
            }
        }
        catch(...)
        {
            res = RC_E_FAIL;
        }
        if (res != RC_S_OK)
        {
            m_decoder->m_result2 = res;
            m_decoder->m_streamWasFinished2 = true;
        }
        m_decoder->m_states[nextBlockIndex].m_canWriteEvent.Set();
        if (res != RC_S_OK || needFinish)
        {
            m_streamWasFinishedEvent.Set();
            m_decoder->m_canStartWaitingEvent.Lock();
            m_waitingWasStartedEvent.Set();
        }
    }
}

#endif //COMPRESS_BZIP2_MT

END_NAMESPACE_RCZIP
