/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "compress/arj/RCArjDecoder1.h"

/////////////////////////////////////////////////////////////////
//RCArjDecoder1 class implementation

BEGIN_NAMESPACE_RCZIP

#define HISTORY_SIZE    26624
#define MATCH_MAX_LEN    256
#define MATCH_MIN_LEN    3

RCArjDecoder1::CCoderReleaser::CCoderReleaser(RCArjDecoder1* coder):
    m_coder(coder), 
    m_needFlush(true) 
{    
}

RCArjDecoder1::CCoderReleaser::~CCoderReleaser()
{
    if (m_needFlush)
    {
        m_coder->m_outWindowStream.Flush() ;
    }
    m_coder->ReleaseStreams() ;
}

RCArjDecoder1::RCArjDecoder1()
{
}

RCArjDecoder1::~RCArjDecoder1()
{
}

void RCArjDecoder1::ReleaseStreams()
{
    m_outWindowStream.ReleaseStream();
    m_inBitStream.ReleaseStream();
}

void RCArjDecoder1::MakeTable( int32_t nchar, 
                               byte_t* bitlen, 
                               int32_t tablebits, 
                               uint32_t* table, 
                               int32_t tablesize)
{
    uint32_t count[17] ;
    uint32_t weight[17] ;
    uint32_t start[18] ;
    uint32_t* p = NULL ;
    uint32_t i = 0 ;
    uint32_t k = 0 ;
    uint32_t len = 0 ;
    uint32_t ch = 0 ;
    uint32_t jutbits = 0 ;
    uint32_t avail = 0 ;
    uint32_t nextcode = 0 ;
    uint32_t mask = 0 ;

    for (i = 1; i <= 16; i++)
    {
        count[i] = 0;
    }
    for (i = 0; (int32_t)i < nchar; i++)
    {
        count[bitlen[i]]++;
    }

    start[1] = 0;
    for (i = 1; i <= 16; i++)
    {
        start[i + 1] = start[i] + (count[i] << (16 - i));
    }
    if (start[17] != (uint32_t) (1 << 16))
    {
        _ThrowCode(RC_E_DataError) ;
    }

    jutbits = 16 - tablebits;
    for (i = 1; (int32_t)i <= tablebits; i++)
    {
        start[i] >>= jutbits;
        weight[i] = 1 << (tablebits - i);
    }
    while (i <= 16)
    {
        weight[i] = 1 << (16 - i);
        i++;
    }

    i = start[tablebits + 1] >> jutbits;
    if (i != (uint32_t) (1 << 16))
    {
        k = 1 << tablebits;
        while (i != k)
        {
            table[i++] = 0;
        }
    }

    avail = nchar;
    mask = 1 << (15 - tablebits);
    for (ch = 0; (int32_t)ch < nchar; ch++)
    {
        if ((len = bitlen[ch]) == 0)
        {
            continue;
        }
        k = start[len];
        nextcode = k + weight[len];
        if ((int32_t)len <= tablebits)
        {
            if (nextcode > (uint32_t)tablesize)
            {
                _ThrowCode(RC_E_DataError) ;
            }
            for (i = start[len]; i < nextcode; i++)
            {
                table[i] = ch;
            }
        }
        else
        {
            p = &table[k >> jutbits];
            i = len - tablebits;
            while (i != 0)
            {
                if (*p == 0)
                {
                    m_right[avail] = m_left[avail] = 0;
                    *p = avail++;
                }
                if (k & mask)
                {
                    p = &m_right[*p];
                }
                else
                {
                    p = &m_left[*p];
                }
                k <<= 1;
                i--;
            }
            *p = ch;
        }
        start[len] = nextcode;
    }
}

void RCArjDecoder1::ReadCLen()
{
    int32_t i = 0 ;
    int32_t c = 0 ;
    int32_t n = 0 ;
    uint32_t mask = 0 ;

    n = m_inBitStream.ReadBits(CBIT);
    if (n == 0)
    {
        c = m_inBitStream.ReadBits(CBIT);
        for (i = 0; i < NC; i++)
        {
            m_cLen[i] = 0;
        }
        for (i = 0; i < CTABLESIZE; i++)
        {
            m_cTable[i] = c;
        }
    }
    else
    {
        i = 0;
        while (i < n)
        {
            uint32_t bitBuf = m_inBitStream.GetValue(16);
            c = m_ptTable[bitBuf >> (8)];
            if (c >= NT)
            {
                mask = 1 << (7);
                do
                {
                    if (bitBuf & mask)
                    {
                        c = m_right[c];
                    }
                    else
                    {
                        c = m_left[c];
                    }
                    mask >>= 1;
                } while (c >= NT);
            }
            m_inBitStream.MovePos((int32_t)(m_ptLen[c]));
            if (c <= 2)
            {
                if (c == 0)
                {
                    c = 1;
                }
                else if (c == 1)
                {
                    c = m_inBitStream.ReadBits(4) + 3;
                }
                else
                {
                    c = m_inBitStream.ReadBits(CBIT) + 20;
                }
                while (--c >= 0)
                {
                    m_cLen[i++] = 0;
                }
            }
            else
            {
                m_cLen[i++] = (byte_t)(c - 2);
            }
        }
        while (i < NC)
        {
            m_cLen[i++] = 0;
        }
        MakeTable(NC, m_cLen, 12, m_cTable, CTABLESIZE);
    }
}

void RCArjDecoder1::ReadPtLen(int32_t nn, int32_t nbit, int32_t i_special)
{
    uint32_t n = m_inBitStream.ReadBits(nbit);
    if (n == 0)
    {
        uint32_t c = m_inBitStream.ReadBits(nbit);
        int32_t i = 0 ;
        for (i = 0; i < nn; i++)
        {
            m_ptLen[i] = 0;
        }
        for (i = 0; i < 256; i++)
        {
            m_ptTable[i] = c;
        }
    }
    else
    {
        uint32_t i = 0;
        while (i < n)
        {
            uint32_t bitBuf = m_inBitStream.GetValue(16);
            int32_t c = bitBuf >> 13;
            if (c == 7)
            {
                uint32_t mask = 1 << (12);
                while (mask & bitBuf)
                {
                    mask >>= 1;
                    c++;
                }
            }
            m_inBitStream.MovePos((c < 7) ? 3 : (int32_t)(c - 3));
            m_ptLen[i++] = (byte_t)c;
            if (i == (uint32_t)i_special)
            {
                c = m_inBitStream.ReadBits(2);
                while (--c >= 0)
                {
                    m_ptLen[i++] = 0;
                }
            }
        }
        while (i < (uint32_t)nn)
        {
            m_ptLen[i++] = 0;
        }
        MakeTable(nn, m_ptLen, 8, m_ptTable, PTABLESIZE);
    }
}

uint32_t RCArjDecoder1::DecodeC()
{
    uint32_t j = 0;
    uint32_t mask = 0 ;
    uint32_t bitbuf = m_inBitStream.GetValue(16);
    j = m_cTable[bitbuf >> 4];
    if (j >= NC)
    {
        mask = 1 << (3);
        do
        {
            if (bitbuf & mask)
            {
                j = m_right[j];
            }
            else
            {
                j = m_left[j];
            }
            mask >>= 1;
        }
        while (j >= NC);
    }
    m_inBitStream.MovePos((int32_t)(m_cLen[j]));
    return j;
}

uint32_t RCArjDecoder1::DecodeP()
{
    uint32_t j = 0;
    uint32_t mask = 0 ;
    uint32_t bitbuf = m_inBitStream.GetValue(16);
    j = m_ptTable[bitbuf >> (8)];
    if (j >= NP)
    {
        mask = 1 << (7);
        do
        {
            if (bitbuf & mask)
            {
                j = m_right[j];
            }
            else
            {
                j = m_left[j];
            }
            mask >>= 1;
        }
        while (j >= NP);
    }
    m_inBitStream.MovePos((int32_t)(m_ptLen[j]));
    if (j != 0)
    {
        j--;
        j = (1 << j) + m_inBitStream.ReadBits((int32_t)j);
    }
    return j;
}

HResult RCArjDecoder1::Code(ISequentialInStream* inStream,
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

HResult RCArjDecoder1::CodeReal(ISequentialInStream* inStream,
                                ISequentialOutStream* outStream, 
                                const uint64_t* inSize, 
                                const uint64_t* outSize,
                                ICompressProgressInfo* progress)
{
    if (outSize == NULL)
    {
        return RC_E_INVALIDARG;
    }
    if (!m_outWindowStream.Create(HISTORY_SIZE))
    {
        return RC_E_OUTOFMEMORY ;
    }
    if (!m_inBitStream.Create(1 << 20))
    {
        return RC_E_OUTOFMEMORY;
    }

    int32_t size1 = sizeof(m_cTable) / sizeof(m_cTable[0]);
    for (int32_t i = 0; i < size1; i++)
    {
        if (i % 100 == 0)
        {
            m_cTable[i] = 0;
        }
        m_cTable[i] = 0;
    }

    uint64_t pos = 0;
    m_outWindowStream.SetStream(outStream);
    m_outWindowStream.Init(false);
    m_inBitStream.SetStream(inStream);
    m_inBitStream.Init();

    CCoderReleaser coderReleaser(this);
    uint32_t blockSize = 0;
    while(pos < *outSize)
    {
        if (blockSize == 0)
        {
            if (progress != NULL)
            {
                uint64_t packSize = m_inBitStream.GetProcessedSize();
                HResult hr = progress->SetRatioInfo(packSize,pos) ;
                if(!IsSuccess(hr))
                {
                    return hr ;
                }
            }
            blockSize = m_inBitStream.ReadBits(16);
            ReadPtLen(NT, TBIT, 3);
            ReadCLen();
            ReadPtLen(NP, PBIT, -1);
        }
        blockSize--;

        uint32_t number = DecodeC();
        if (number < 256)
        {
            m_outWindowStream.PutByte((byte_t)number);
            pos++;
            continue;
        }
        else
        {
            uint32_t len = number - 256 + MATCH_MIN_LEN;
            uint32_t distance = DecodeP();
            if (distance >= pos)
            {
                return RC_S_FALSE;
            }
            m_outWindowStream.CopyBlock(distance, len);
            pos += len;
        }
    }
    coderReleaser.m_needFlush = false;
    return m_outWindowStream.Flush();
}


END_NAMESPACE_RCZIP
