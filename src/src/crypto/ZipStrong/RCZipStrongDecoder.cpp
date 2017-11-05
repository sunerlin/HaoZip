/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "crypto/ZipStrong/RCZipStrongDecoder.h"
#include "filesystem/RCStreamUtils.h"
#include "crypto/hash/RCSha1Context.h"
#include "crypto/AES/RCAesCbcDecoder.h"
#include "algorithm/7zCrc.h"
#include "algorithm/CpuArch.h"

/////////////////////////////////////////////////////////////////
//RCZipStrongDecoder class implementation

BEGIN_NAMESPACE_RCZIP

static const uint16_t kAES128 = 0x660E;

// DeriveKey* function is similar to CryptDeriveKey() from Windows.
// But MSDN tells that we need such scheme only if
// "the required key length is longer than the hash value"
// but ZipStrong uses it always.

static void DeriveKey2(const byte_t *digest, byte_t c, byte_t *dest)
{
    byte_t buf[64];
    memset(buf, c, 64);
    for (unsigned i = 0; i < RCSha1Defs::kDigestSize; i++)
    {
        buf[i] ^= digest[i];
    }
    RCSha1Context sha;
    sha.Init();
    sha.Update(buf, 64);
    sha.Final(dest);
}
 
static void DeriveKey(RCSha1Context& sha, byte_t* key)
{
    byte_t digest[RCSha1Defs::kDigestSize];
    sha.Final(digest);
    byte_t temp[RCSha1Defs::kDigestSize * 2];
    DeriveKey2(digest, 0x36, temp);
    DeriveKey2(digest, 0x5C, temp + RCSha1Defs::kDigestSize);
    memcpy(key, temp, 32);
}

void RCZipStrongKeyInfo::SetPassword(const byte_t* data, uint32_t size)
{
    if(data == NULL)
    {
        return ;
    }
    RCSha1Context sha;
    sha.Init();
    sha.Update(data, size);
    DeriveKey(sha, m_masterKey);
}

HResult RCZipStrongBaseDecoder::CryptoSetPassword(const byte_t *data, uint32_t size)
{
    m_key.SetPassword(data, size);
    return RC_S_OK;
}

HResult RCZipStrongBaseDecoder::Init()
{
    return RC_S_OK;
}

HResult RCZipStrongDecoder::ReadHeader(ISequentialInStream *inStream, uint32_t crc, uint64_t unpackSize)
{
    byte_t temp[4];
    HResult hr = RCStreamUtils::ReadStream_FALSE(inStream, temp, 2);
    if (!IsSuccess(hr))
    {
        return hr;
    }

    m_ivSize = GetUi16(temp);
    if (m_ivSize == 0)
    {
        return  RC_E_NOTIMPL;
        /*
        SetUi32(m_iv, crc);
        for (int i = 0; i < 8; i++)
        m_iv[4 + i] = (byte_t)(unpackSize >> (8 * i));
        SetUi32(m_iv + 12, 0);
        */
    }
    else if (m_ivSize == 16)
    {
        hr = RCStreamUtils::ReadStream_FALSE(inStream, m_iv, m_ivSize);
        if (!IsSuccess(hr))
        {
            return hr;
        }
    }
    else
    {
        return  RC_E_NOTIMPL;
    }
   
    hr = RCStreamUtils::ReadStream_FALSE(inStream, temp, 4);
    if (!IsSuccess(hr))
    {
        return hr;
    }

    m_remSize = GetUi32(temp);
    if (m_remSize > m_buf.GetCapacity())
    {
        m_buf.Free();
        m_buf.SetCapacity(m_remSize);
    }
    return RCStreamUtils::ReadStream_FALSE(inStream, m_buf.data(), m_remSize);
}

HResult RCZipStrongDecoder::CheckPassword(bool &isPasswordOK)
{
    isPasswordOK = false;
    if (m_remSize < 10)
    {
        return RC_E_NOTIMPL;
    }

    HResult hr;
    byte_t *p = m_buf.data();
    uint16_t format = GetUi16(p);
    if (format != 3)
    {
        return RC_E_NOTIMPL;
    }
    uint16_t algId  = GetUi16(p + 2);
    if (algId < kAES128)
    {
        return RC_E_NOTIMPL;
    }
    algId -= kAES128;
    if (algId > 2)
    {
        return RC_E_NOTIMPL;
    }
    uint16_t bitLen = GetUi16(p + 4);
    uint16_t flags  = GetUi16(p + 6);
    if (algId * 64 + 128 != bitLen)
    {
        return RC_E_NOTIMPL;
    }
    m_key.m_keySize = 16 + algId * 8;
    if ((flags & 1) == 0)
    {
        return RC_E_NOTIMPL;
    }
    uint32_t rdSize = GetUi16(p + 8);
    uint32_t pos = 10;
    byte_t *rd = p + pos;
    pos += rdSize;
    if (pos + 4 > m_remSize)
    {
        return RC_E_NOTIMPL;
    }
    uint32_t reserved = GetUi32(p + pos);
    pos += 4;
    if (reserved != 0)
    {
        return RC_E_NOTIMPL;
    }
    if (pos + 2 > m_remSize)
    {
        return RC_E_NOTIMPL;
    }

    uint32_t validSize = GetUi16(p + pos);
    pos += 2;
    byte_t *validData = p + pos;
    if (pos + validSize != m_remSize)
    {
        return RC_E_NOTIMPL;
    }

    if (!m_aesFilter)
    {
        m_aesFilter = new RCAesCbcDecoder;
    }

    ICryptoPropertiesPtr cp;
    hr = m_aesFilter.QueryInterface(IID_ICryptoProperties, cp.GetAddress());
    if (!IsSuccess(hr))
    {
        return hr;
    }

    {
        hr = cp->SetKey(m_key.m_masterKey, m_key.m_keySize);
        if (!IsSuccess(hr))
        {
            return hr;
        }

        hr = cp->SetInitVector(m_iv, 16);
        if (!IsSuccess(hr))
        {
            return hr;
        }

        m_aesFilter->Init();
        if (m_aesFilter->Filter(rd, rdSize) != rdSize)
        {
            return RC_E_NOTIMPL;
        }
    }

    byte_t fileKey[32];
    RCSha1Context sha;
    sha.Init();
    sha.Update(m_iv, 16);
    sha.Update(rd, rdSize - 16); // we don't use last 16 bytes (PAD bytes)
    DeriveKey(sha, fileKey);

    hr = cp->SetKey(fileKey, m_key.m_keySize);
    if (!IsSuccess(hr))
    {
        return hr;
    }

    hr = cp->SetInitVector(m_iv, 16);
    if (!IsSuccess(hr))
    {
        return hr;
    }

    m_aesFilter->Init();
    if (m_aesFilter->Filter(validData, validSize) != validSize)
    {
        return RC_E_NOTIMPL;
    }

    if (validSize < 4)
    {
        return RC_E_NOTIMPL;
    }

    validSize -= 4;
    if (GetUi32(validData + validSize) != CrcCalc(validData, validSize))
    {
        return RC_S_OK;
    }

    isPasswordOK = true;
    m_aesFilter->Init();
    return RC_S_OK;
}

uint32_t RCZipStrongDecoder::Filter(byte_t* data, uint32_t size)
{
  return m_aesFilter->Filter(data, size);
}

END_NAMESPACE_RCZIP
