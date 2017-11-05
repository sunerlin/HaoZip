/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "crypto/7zAES/RC7zAESKeyInfo.h"
#include "algorithm/Sha256.h"

/////////////////////////////////////////////////////////////////
//RC7zAESKeyInfo class implementation

BEGIN_NAMESPACE_RCZIP

RC7zAESKeyInfo::RC7zAESKeyInfo()
{
    Init();
}

void RC7zAESKeyInfo::Init()
{
    m_numCyclesPower = 0;
    m_saltSize = 0;
    for (int i = 0; i < sizeof(m_salt); i++)
    {
        m_salt[i] = 0;
    }
    for (int i = 0; i < s_kKeySize; i++)
    {
        m_key[i] = 0;
    }
}

bool RC7zAESKeyInfo::IsEqualTo(const RC7zAESKeyInfo &a) const
{
    if (m_saltSize != a.m_saltSize || m_numCyclesPower != a.m_numCyclesPower)
    {
        return false;
    }
    for (uint32_t i = 0; i < m_saltSize; i++)
    {
        if (m_salt[i] != a.m_salt[i])
        {
            return false;
        }
    }
    return (m_password == a.m_password);
}

void RC7zAESKeyInfo::CalculateDigest()
{
    if (m_numCyclesPower == 0x3F)
    {
        uint32_t pos;
        for (pos = 0; pos < m_saltSize; pos++)
        {
            m_key[pos] = m_salt[pos];
        }
        for (uint32_t i = 0; i < m_password.GetCapacity() && pos < s_kKeySize; i++)
        {
            m_key[pos++] = m_password[i];
        }
        for (; pos < s_kKeySize; pos++)
        {
            m_key[pos] = 0;
        }
    }
    else
    {
        CSha256 sha;
        Sha256_Init(&sha);
        const uint64_t numRounds = uint64_t(1) << (m_numCyclesPower);
        byte_t temp[8] = { 0,0,0,0,0,0,0,0 };
        for (uint64_t round = 0; round < numRounds; round++)
        {
            Sha256_Update(&sha, m_salt, (size_t)m_saltSize);
            Sha256_Update(&sha, m_password.data(), m_password.GetCapacity());
            Sha256_Update(&sha, temp, 8);
            for (int i = 0; i < 8; i++)
            {
                if (++(temp[i]) != 0)
                {
                    break;
                }
            }
        }
        Sha256_Final(&sha, m_key);
    }
}

END_NAMESPACE_RCZIP
