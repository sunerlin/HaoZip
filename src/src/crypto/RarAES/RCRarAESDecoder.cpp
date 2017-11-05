/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "crypto/RarAES/RCRarAESDecoder.h"
#include "crypto/hash/RCSha1Context.h"
#include "interface/RCMethodDefs.h"

/////////////////////////////////////////////////////////////////
//RCRarAESDecoder class implementation

BEGIN_NAMESPACE_RCZIP

RCRarAESDecoder::RCRarAESDecoder():
    m_thereIsSalt(false),
    m_needCalculate(true),
    m_rar350Mode(false)
{
    for (int i = 0; i < sizeof(m_salt); ++i)
    {
        m_salt[i] = 0;
    }
}

RCRarAESDecoder::~RCRarAESDecoder()
{
}

void RCRarAESDecoder::Calculate()
{
    if(m_needCalculate)
    {
        const int32_t kSaltSize = 8 ;    
        byte_t rawPassword[s_kMaxPasswordLength + kSaltSize];    
        memcpy(rawPassword, m_buffer.data(), m_buffer.GetCapacity());    
        size_t rawLength = m_buffer.GetCapacity();    
        if (m_thereIsSalt)
        {
            memcpy(rawPassword + rawLength, m_salt, kSaltSize);
            rawLength += kSaltSize;
        }    
        RCSha1Context sha;
        sha.Init();

        // seems rar reverts hash for sha.
        const int32_t hashRounds = 0x40000;
        int32_t i = 0 ;
        for (i = 0; i < hashRounds; ++i)
        {
            sha.Update(rawPassword, rawLength, m_rar350Mode);
            byte_t pswNum[3] = { (byte_t)i, (byte_t)(i >> 8), (byte_t)(i >> 16) };
            sha.Update(pswNum, 3, m_rar350Mode);
            if (i % (hashRounds / 16) == 0)
            {
                RCSha1Context shaTemp = sha;
                byte_t digest[RCSha1Defs::kDigestSize];
                shaTemp.Final(digest);
                m_aesInit[i / (hashRounds / 16)] = (byte_t)digest[4 * 4 + 3];
            }
        }
        /*
        // it's test message for sha
            const char *message = "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
            sha.Update((const byte_t *)message, strlen(message));
        */
        byte_t digest[20];
        sha.Final(digest);
        for (i = 0; i < 4; ++i)
        {
            for (int32_t j = 0; j < 4; ++j)
            {
                m_aesKey[i * 4 + j] = (digest[i * 4 + 3 - j]);
            }
        }
    }
    m_needCalculate = false ;
}

HResult RCRarAESDecoder::Init()
{
    Calculate();
    Aes_SetKeyDecode(&m_aes.aes, m_aesKey, s_kRarAesKeySize);
    AesCbc_Init(&m_aes, m_aesInit);
    return RC_S_OK;
}

uint32_t RCRarAESDecoder::Filter(byte_t* data, uint32_t size)
{
    return (uint32_t)AesCbc_Decode(&m_aes, data, size);
}

HResult RCRarAESDecoder::SetDecoderProperties2(const byte_t* data, uint32_t size)
{
    bool thereIsSaltPrev = m_thereIsSalt;
    m_thereIsSalt = false;
    if (size == 0)
    {
        return RC_S_OK;
    }
    if (size < 8)
    {
        return RC_E_INVALIDARG ;
    }
    m_thereIsSalt = true;
    bool same = false;
    if (m_thereIsSalt == thereIsSaltPrev)
    {
        same = true;
        if (m_thereIsSalt)
        {
            for (int32_t i = 0; i < sizeof(m_salt); ++i)
            if (m_salt[i] != data[i])
            {
                same = false;
                break;
            }
        }
    }
    for (int32_t i = 0; i < sizeof(m_salt); ++i)
    {
        m_salt[i] = data[i];
    }
    if (!m_needCalculate && !same)
    {
        m_needCalculate = true;
    }
     return RC_S_OK;
}

HResult RCRarAESDecoder::CryptoSetPassword(const byte_t* data, uint32_t size)
{
    if (size > s_kMaxPasswordLength)
    {
        size = s_kMaxPasswordLength;
    }
    bool same = false;
    if (size == m_buffer.GetCapacity())
    {
        same = true;
        for (uint32_t i = 0; i < size; i++)
        if (data[i] != m_buffer[i])
        {
            same = false;
            break;
        }
    }
    if (!m_needCalculate && !same)
    {
        m_needCalculate = true;
    }
    m_buffer.SetCapacity(size);
    memcpy(m_buffer.data(), data, size);
    return RC_S_OK ;
}

HResult RCRarAESDecoder::SetCoderProperties(const RCPropertyIDPairArray& propertyArray)
{
    RCPropertyIDPairArray::const_iterator pos = propertyArray.begin() ;
    for(; pos != propertyArray.end(); ++pos)
    {
        const RCPropertyIDPair& pair = *pos ;
        if(pair.first == RCMethodProperty::ID_SET_RAR_350_MODE)
        {
            if(IsBooleanType(pair.second))
            {
                m_rar350Mode = GetBooleanValue(pair.second) ;
            }
            else
            {
                return RC_E_INVALIDARG ;
            }
        }
        else
        {
            return RC_E_INVALIDARG ;
        }
    }
    return RC_S_OK ;
}

END_NAMESPACE_RCZIP
