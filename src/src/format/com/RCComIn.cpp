/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#include "format/com/RCComIn.h"
#include "common/RCBuffer.h"
#include "common/RCStringUtil.h"
#include "filesystem/RCStreamUtils.h"
#include "algorithm/CpuArch.h"
#include "algorithm/Alloc.h"

#ifdef RCZIP_OS_WIN
#define WCHAR_PATH_SEPARATOR _T("\\")
#else
#define WCHAR_PATH_SEPARATOR _T("/")
#endif

BEGIN_NAMESPACE_RCZIP

static const uint32_t kSignatureSize = 8;
static const byte_t kSignature[kSignatureSize] = { 0xD0, 0xCF, 0x11, 0xE0, 0xA1, 0xB1, 0x1A, 0xE1 };

#define Get16(p) GetUi16(p)
#define Get32(p) GetUi32(p)

RCUInt32Buf::RCUInt32Buf(): 
    m_buf(0) 
{
}

RCUInt32Buf::~RCUInt32Buf() 
{ 
    Free(); 
}

RCUInt32Buf::operator uint32_t *() const 
{ 
    return m_buf; 
};

void RCUInt32Buf::Free()
{
    MyFree(m_buf);
    m_buf = 0;
}

bool RCUInt32Buf::Allocate(uint32_t numItems)
{
    Free();
    if (numItems == 0)
    {
        return true;
    }
    size_t newSize = (size_t)numItems * sizeof(uint32_t);
    if (newSize / sizeof(uint32_t) != numItems)
    {
        return false;
    }
    m_buf = (uint32_t *)MyAlloc(newSize);
    return (m_buf != 0);
}

bool RCComItem::IsEmpty() const 
{ 
    return m_type == RCComDefs::s_empty; 
}

bool RCComItem::IsDir() const 
{ 
    return m_type == RCComDefs::s_storage || m_type == RCComDefs::s_rootStorage; 
}

void RCComItem::Parse(const byte_t* p, bool mode64bit)
{
    memcpy(m_name, p, RCComDefs::s_nameSizeMax);
    m_type = p[66];
    m_leftDid = Get32(p + 68);
    m_rightDid = Get32(p + 72);
    m_sonDid = Get32(p + 76);
    RCComFuncImpl::GetFileTimeFromMem(p + 100, &m_cTime);
    RCComFuncImpl::GetFileTimeFromMem(p + 108, &m_mTime);
    m_sid = Get32(p + 116);
    m_size = Get32(p + 120);
    if (mode64bit)
    {
        m_size |= ((uint64_t)Get32(p + 124) << 32);
    }
}

HResult RCComFuncImpl::ReadSector(IInStream *inStream, byte_t *buf, int32_t sectorSizeBits, uint32_t sid)
{
    HResult result;
    result = inStream->Seek((((uint64_t)sid + 1) << sectorSizeBits), RC_STREAM_SEEK_SET, NULL);
    if (!IsSuccess(result))
    {
        return result;
    }
    return RCStreamUtils::ReadStream_FALSE(inStream, buf, (uint32_t)1 << sectorSizeBits);
}

HResult RCComFuncImpl::ReadIDs(IInStream *inStream, byte_t *buf, int32_t sectorSizeBits, uint32_t sid, uint32_t *dest)
{
    HResult result;
    result = ReadSector(inStream, buf, sectorSizeBits, sid);
    if (!IsSuccess(result))
    {
        return result;
    }
    uint32_t sectorSize = (uint32_t)1 << sectorSizeBits;
    for (uint32_t t = 0; t < sectorSize; t += 4)
    {
        *dest++ = GetUi32(buf + t);
    }
    return RC_S_OK;
}

void RCComFuncImpl::GetFileTimeFromMem(const byte_t *p, RC_FILE_TIME *ft)
{
    ft->u32LowDateTime = GetUi32(p);
    ft->u32HighDateTime = GetUi32(p + 4);
}

static const uint32_t kNoDid = 0xFFFFFFFF;

HResult RCComDatabase::AddNode(int32_t parent, uint32_t did)
{
    HResult result;
    if (did == kNoDid)
    {
        return RC_S_OK;
    }
    if (did >= (uint32_t)m_items.size())
    {
        return RC_S_FALSE;
    }
    const RCComItem &item = m_items[did];
    if (item.IsEmpty())
    {
        return RC_S_FALSE;
    }
    RCComRef ref;
    ref.m_parent = parent;
    ref.m_did = did;
    //int32_t index = m_refs.Add(ref);
    m_refs.push_back(ref);
    int32_t index = (int32_t)m_refs.size() - 1;
    if (m_refs.size() > m_items.size())
    {
        return RC_S_FALSE;
    }
    result = AddNode(parent, item.m_leftDid);
    if (!IsSuccess(result))
    {
        return result;
    }
    result = AddNode(parent, item.m_rightDid);
    if (!IsSuccess(result))
    {
        return result;
    }
    if (item.IsDir())
    {
        result = AddNode(index, item.m_sonDid);
        if (!IsSuccess(result))
        {
            return result;
        }
    }
    return RC_S_OK;
}

static const char kCharOpenBracket  = '[';
static const char kCharCloseBracket = ']';

RCString RCComFuncImpl::CompoundNameToFileName(const RCString &s)
{
    RCString res;
    for (int32_t i = 0; i < (int32_t)s.length(); i++)
    {
        wchar_t c = s[i];
        if (c < 0x20)
        {
            res += kCharOpenBracket;
            res += RCStringUtil::ConvertUInt32ToString(c) ;
            res += kCharCloseBracket;
        }
        else
        {
            res += c;
        }
    }
    return res;
}

static char g_MsiChars[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz._";
static const wchar_t *kMsi_ID = L""; // L"{msi}";
static const int32_t kMsiNumBits = 6;
static const uint32_t kMsiNumChars = 1 << kMsiNumBits;
static const uint32_t kMsiCharMask = kMsiNumChars - 1;
static const uint32_t kMsiStartUnicodeChar = 0x3800;
static const uint32_t kMsiUnicodeRange = kMsiNumChars * (kMsiNumChars + 1);

bool RCComFuncImpl::CompoundMsiNameToFileName(const RCString &name, RCString &resultName)
{
    //resultName.Empty();
    resultName.clear();
    for (int32_t i = 0; i < (int32_t)name.length(); i++)
    {
        wchar_t c =  name[i];
        if (c < kMsiStartUnicodeChar || c > kMsiStartUnicodeChar + kMsiUnicodeRange)
        {
            return false;
        }
        if (i == 0)
        {
            resultName += kMsi_ID;
        }
        c -= kMsiStartUnicodeChar;

        uint32_t c0 = c & kMsiCharMask;
        uint32_t c1 = c >> kMsiNumBits;

        if (c1 <= kMsiNumChars)
        {
            resultName += (wchar_t)g_MsiChars[c0];
            if (c1 == kMsiNumChars)
            {
                break;
            }
            resultName += (wchar_t)g_MsiChars[c1];
        }
        else
        {
            resultName += L'!';
        }
    }
    return true;
}

RCString RCComFuncImpl::ConvertName(const byte_t *p)
{
    RCString s;
    for (int32_t i = 0; i < RCComDefs::s_nameSizeMax; i += 2)
    {
        wchar_t c = (p[i] | (wchar_t)p[i + 1] << 8);
        if (c == 0)
        {
            break;
        }
        s += c;
    }
    RCString msiName;
    if (CompoundMsiNameToFileName(s, msiName))
    {
        return msiName;
    }
    return CompoundNameToFileName(s);
}

RCString RCComDatabase::GetItemPath(uint32_t index) const
{
    RCString s;
    while (index != kNoDid)
    {
        const RCComRef &ref = m_refs[index];
        const RCComItem &item = m_items[ref.m_did];
        if (!s.empty())
        {
            s = (RCString)WCHAR_PATH_SEPARATOR + s;
        }
        s = RCComFuncImpl::ConvertName(item.m_name) + s;
        index = ref.m_parent;
    }
    return s;
}

void RCComDatabase::Clear()
{
    m_fat.Free();
    m_miniSids.Free();
    m_mat.Free();
    m_items.clear();
    m_refs.clear();
}

bool RCComDatabase::IsLargeStream(uint64_t size) const
{ 
    return size >= m_longStreamMinSize; 
}

uint64_t RCComDatabase::GetItemPackSize(uint64_t size) const
{
    uint64_t mask = ((uint64_t)1 << (IsLargeStream(size) ? m_sectorSizeBits : m_miniSectorSizeBits)) - 1;
    return (size + mask) & ~mask;
}

bool RCComDatabase::GetMiniCluster(uint32_t sid, uint64_t& res) const
{
    int32_t subBits = m_sectorSizeBits - m_miniSectorSizeBits;
    uint32_t fid = sid >> subBits;
    if (fid >= m_numSectorsInMiniStream)
    {
        return false;
    }
    res = (((uint64_t)m_miniSids[fid] + 1) << subBits) + (sid & ((1 << subBits) - 1)) ;
    return true;
}

uint64_t RCComFuncImpl::RCFileTime2UInt64(const RC_FILE_TIME& tt)
{
    uint64_t ret;
    ret = tt.u32HighDateTime;
    ret = ret << 32;
    ret += tt.u32LowDateTime;
    return ret;
}

HResult RCComDatabase::Open(IInStream *inStream)
{
    HResult rs;
    static const uint32_t kHeaderSize = 512;
    byte_t p[kHeaderSize];
    rs = RCStreamUtils::ReadStream_FALSE(inStream, p, kHeaderSize);
    if (!IsSuccess(rs))
    {
        return rs;
    }
    if (memcmp(p, kSignature, kSignatureSize) != 0)
    {
        return RC_S_FALSE;
    }
    uint16_t majorVer = Get16(p + 0x1A);
    if (majorVer > 4)
    {
        return RC_S_FALSE;
    }
    if (Get16(p + 0x1C) != 0xFFFE)
    {
        return RC_S_FALSE;
    }
    int32_t sectorSizeBits = Get16(p + 0x1E);
    bool mode64bit = (sectorSizeBits >= 12);
    int32_t miniSectorSizeBits = GetUi16(p + 0x20);
    m_sectorSizeBits = sectorSizeBits;
    m_miniSectorSizeBits = miniSectorSizeBits;

    if (sectorSizeBits > 28 || miniSectorSizeBits > 28 ||
        sectorSizeBits < 7 || miniSectorSizeBits < 2 || miniSectorSizeBits > sectorSizeBits)
    {
        return RC_S_FALSE;
    }
    uint32_t numSectorsForFAT = Get32(p + 0x2C);
    m_longStreamMinSize = Get32(p + 0x38);

    uint32_t sectSize = (uint32_t)1 << (int32_t)sectorSizeBits ;

    RCByteBuffer sect;
    sect.SetCapacity(sectSize);

    int32_t ssb2 = (int32_t)(sectorSizeBits - 2);
    uint32_t numSidsInSec = (uint32_t)1 << ssb2;
    uint32_t numFatItems = numSectorsForFAT << ssb2;
    if ((numFatItems >> ssb2) != numSectorsForFAT)
    {
        return RC_S_FALSE;
    }
    m_fatSize = numFatItems;
    {
        RCUInt32Buf bat;
        uint32_t numSectorsForBat = Get32(p + 0x48);
        const uint32_t kNumHeaderBatItems = 109;
        uint32_t numBatItems = kNumHeaderBatItems + (numSectorsForBat << ssb2);
        if (numBatItems < kNumHeaderBatItems || ((numBatItems - kNumHeaderBatItems) >> ssb2) != numSectorsForBat)
        {
            return RC_S_FALSE;
        }
        if (!bat.Allocate(numBatItems))
        {
            return RC_S_FALSE;
        }
        uint32_t i;
        for (i = 0; i < kNumHeaderBatItems; i++)
        {
            bat[i] = Get32(p + 0x4c + i * 4);
        }
        uint32_t sid = Get32(p + 0x44);
        for (uint32_t s = 0; s < numSectorsForBat; s++)
        {
            rs = RCComFuncImpl::ReadIDs(inStream, sect.data(), sectorSizeBits, sid, bat + i);
            if (!IsSuccess(rs))
            {
                return rs;
            }
            i += numSidsInSec - 1;
            sid = bat[i];
        }
        numBatItems = i;
        if (!m_fat.Allocate(numFatItems))
        {
            return RC_S_FALSE;
        }
        uint32_t j = 0;
        for (i = 0; i < numFatItems; j++, i += numSidsInSec)
        {
            if (j >= numBatItems)
            {
                return RC_S_FALSE;
            }
            rs = RCComFuncImpl::ReadIDs(inStream, sect.data(), sectorSizeBits, bat[j], m_fat + i);
            if (!IsSuccess(rs))
            {
                return rs;
            }
        }
    }

    uint32_t numMatItems;
    {
        uint32_t numSectorsForMat = Get32(p + 0x40);
        numMatItems = (uint32_t)numSectorsForMat << ssb2;
        if ((numMatItems >> ssb2) != numSectorsForMat)
        {
            return RC_S_FALSE;
        }
        if (!m_mat.Allocate(numMatItems))
        {
            return RC_S_FALSE;
        }
        uint32_t i;
        uint32_t sid = Get32(p + 0x3C);
        for (i = 0; i < numMatItems; i += numSidsInSec)
        {
            rs = RCComFuncImpl::ReadIDs(inStream, sect.data(), sectorSizeBits, sid, m_mat + i);
            if (!IsSuccess(rs))
            {
                return rs;
            }
            if (sid >= numFatItems)
            {
                return RC_S_FALSE;
            }
            sid = m_fat[sid];
        }
        if (sid != RCComDefs::s_endOfChain)
        {
            return RC_S_FALSE;
        }
    }

    {
        uint32_t sid = Get32(p + 0x30);
        for (;;)
        {
            if (sid >= numFatItems)
            {
                return RC_S_FALSE;
            }
            rs = RCComFuncImpl::ReadSector(inStream, sect.data(), sectorSizeBits, sid);
            if (!IsSuccess(rs))
            {
                return rs;
            }
            for (uint32_t i = 0; i < sectSize; i += 128)
            {
                RCComItem item;
                item.Parse(sect.data() + i, mode64bit);
                m_items.push_back(item);
            }
            sid = m_fat[sid];
            if (sid == RCComDefs::s_endOfChain)
            {
                break;
            }
        }
    }

    RCComItem root = m_items[0];

    {
        uint32_t numSectorsInMiniStream;
        {
            uint64_t numSatSects64 = (root.m_size + sectSize - 1) >> sectorSizeBits;
            if (numSatSects64 > RCComDefs::s_maxValue)
            {
                return RC_S_FALSE;
            }
            numSectorsInMiniStream = (uint32_t)numSatSects64;
        }
        m_numSectorsInMiniStream = numSectorsInMiniStream;
        if (!m_miniSids.Allocate(numSectorsInMiniStream))
        {
            return RC_S_FALSE;
        }
        {
            uint64_t matSize64 = (root.m_size + ((uint64_t)1 << miniSectorSizeBits) - 1) >> miniSectorSizeBits;
            if (matSize64 > RCComDefs::s_maxValue)
            {
                return RC_S_FALSE;
            }
            m_matSize = (uint32_t)matSize64;
            if (numMatItems < m_matSize)
            {
                return RC_S_FALSE;
            }
        }

        uint32_t sid = root.m_sid;
        for (uint32_t i = 0; ; i++)
        {
            if (sid == RCComDefs::s_endOfChain)
            {
                if (i != numSectorsInMiniStream)
                {
                    return RC_S_FALSE;
                }
                break;
            }
            if (i >= numSectorsInMiniStream)
            {
                return RC_S_FALSE;
            }
            m_miniSids[i] = sid;
            if (sid >= numFatItems)
            {
                return RC_S_FALSE;
            }
            sid = m_fat[sid];
        }
    }
    return AddNode(-1, root.m_sonDid);
}

END_NAMESPACE_RCZIP
