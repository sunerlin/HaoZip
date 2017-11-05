/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "crypto/Rar20/RCRar20Crypto.h"
#include "algorithm/7zCrc.h"

/////////////////////////////////////////////////////////////////
//RCRar20Crypto class implementation

#define  rol(x,n)  (((x) << (n)) | ((x) >> (8 * sizeof(x) - (n))))
#define  ror(x,n)  (((x) >> (n)) | ((x) << (8 * sizeof(x) - (n))))

BEGIN_NAMESPACE_RCZIP

#define NUM_OF_ROUNDS 32

static const byte_t InitSubstTable[256] = {
  215, 19,149, 35, 73,197,192,205,249, 28, 16,119, 48,221,  2, 42,
  232,  1,177,233, 14, 88,219, 25,223,195,244, 90, 87,239,153,137,
  255,199,147, 70, 92, 66,246, 13,216, 40, 62, 29,217,230, 86,  6,
   71, 24,171,196,101,113,218,123, 93, 91,163,178,202, 67, 44,235,
  107,250, 75,234, 49,167,125,211, 83,114,157,144, 32,193,143, 36,
  158,124,247,187, 89,214,141, 47,121,228, 61,130,213,194,174,251,
   97,110, 54,229,115, 57,152, 94,105,243,212, 55,209,245, 63, 11,
  164,200, 31,156, 81,176,227, 21, 76, 99,139,188,127, 17,248, 51,
  207,120,189,210,  8,226, 41, 72,183,203,135,165,166, 60, 98,  7,
  122, 38,155,170, 69,172,252,238, 39,134, 59,128,236, 27,240, 80,
  131,  3, 85,206,145, 79,154,142,159,220,201,133, 74, 64, 20,129,
  224,185,138,103,173,182, 43, 34,254, 82,198,151,231,180, 58, 10,
  118, 26,102, 12, 50,132, 22,191,136,111,162,179, 45,  4,148,108,
  161, 56, 78,126,242,222, 15,175,146, 23, 33,241,181,190, 77,225,
    0, 46,169,186, 68, 95,237, 65, 53,208,253,168,  9, 18,100, 52,
  116,184,160, 96,109, 37, 30,106,140,104,150,  5,204,117,112, 84
};

RCRar20Crypto::RCRar20Crypto()
{
}

RCRar20Crypto::~RCRar20Crypto()
{
}

void RCRar20Crypto::Swap(byte_t *b1, byte_t *b2)
{
    byte_t b = *b1;
    *b1 = *b2;
    *b2 = b;
}

uint32_t RCRar20Crypto::GetUInt32FromMemLE(const byte_t *p)
{
    return p[0] | (((uint32_t)p[1]) << 8) | (((uint32_t)p[2]) << 16) | (((uint32_t)p[3]) << 24);
}

void RCRar20Crypto::WriteUInt32ToMemLE(uint32_t v, byte_t *p)
{
    p[0] = (byte_t)v;
    p[1] = (byte_t)(v >> 8);
    p[2] = (byte_t)(v >> 16);
    p[3] = (byte_t)(v >> 24);
}

uint32_t RCRar20Crypto::SubstLong(uint32_t t)
{
    return (uint32_t)m_substTable[(int)t & 255] |
           ((uint32_t)m_substTable[(int)(t >> 8) & 255] << 8) |
           ((uint32_t)m_substTable[(int)(t >> 16) & 255] << 16) |
           ((uint32_t)m_substTable[(int)(t >> 24) & 255] << 24);
}

void RCRar20Crypto::UpdateKeys(const byte_t* data)
{
    for (int i = 0; i < 16; i += 4)
    {
        for (int j = 0; j < 4; j++)
        {
            m_keys[j] ^= g_CrcTable[data[i + j]];
        }
    }
}

void RCRar20Crypto::CryptBlock(byte_t* buf, bool encrypt)
{
    byte_t inBuf[16];
    uint32_t A, B, C, D, T, TA, TB;

    A = GetUInt32FromMemLE(buf + 0) ^ m_keys[0];
    B = GetUInt32FromMemLE(buf + 4) ^ m_keys[1];
    C = GetUInt32FromMemLE(buf + 8) ^ m_keys[2];
    D = GetUInt32FromMemLE(buf + 12) ^ m_keys[3];

    if (!encrypt)
    {
        memcpy(inBuf, buf, sizeof(inBuf));
    }
  
    for(int32_t i = 0; i < NUM_OF_ROUNDS; ++i)
    {
        uint32_t key = m_keys[(encrypt ? i : (NUM_OF_ROUNDS - 1 - i)) & 3];
        T = ((C + rol(D, 11)) ^ key);
        TA = A ^ SubstLong(T);
        T = ((D ^ rol(C, 17)) + key);
        TB = B ^ SubstLong(T);
        A = C;
        B = D;
        C = TA;
        D = TB;
    }

    WriteUInt32ToMemLE(C ^ m_keys[0], buf + 0);
    WriteUInt32ToMemLE(D ^ m_keys[1], buf + 4);
    WriteUInt32ToMemLE(A ^ m_keys[2], buf + 8);
    WriteUInt32ToMemLE(B ^ m_keys[3], buf + 12);

    UpdateKeys(encrypt ? buf : inBuf);
}

void RCRar20Crypto::EncryptBlock(byte_t *buf)
{
    CryptBlock(buf, true);
}

void RCRar20Crypto::DecryptBlock(byte_t *buf)
{
    CryptBlock(buf, false);
}

void RCRar20Crypto::SetPassword(const byte_t *password, uint32_t passwordLength)
{
    m_keys[0] = 0xD3A3B879L;
    m_keys[1] = 0x3F6D12F7L;
    m_keys[2] = 0x7515A235L;
    m_keys[3] = 0xA4E7F123L;
  
    byte_t psw[256];
    memset(psw, 0, sizeof(psw));
  
    memmove(psw, password, passwordLength);
  
    memcpy(m_substTable, InitSubstTable, sizeof(m_substTable));
    for (uint32_t j = 0; j < 256; ++j)
    {
        for (uint32_t i = 0; i < passwordLength; i += 2)
        {
            uint32_t n2 = (byte_t)g_CrcTable[(psw[i + 1] + j) & 0xFF];
            uint32_t n1 = (byte_t)g_CrcTable[(psw[i] - j) & 0xFF];
            for (uint32_t k = 1; (n1 & 0xFF) != n2; n1++, k++)
            {
                Swap(&m_substTable[n1 & 0xFF], &m_substTable[(n1 + i + k) & 0xFF]);
            }
        }
        for (uint32_t i = 0; i < passwordLength; i+= 16)
        {
            EncryptBlock(&psw[i]);
        }
    }
}

END_NAMESPACE_RCZIP
