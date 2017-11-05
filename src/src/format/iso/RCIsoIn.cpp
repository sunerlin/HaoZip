/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#include "format/iso/RCIsoIn.h"
#include "filesystem/RCStreamUtils.h"

BEGIN_NAMESPACE_RCZIP

bool RCIsoDateTime::NotSpecified() const 
{ 
    return (m_year == 0 && m_month == 0 && m_day == 0 &&
        m_hour == 0 && m_minute == 0 && m_second == 0 && m_gmtOffset == 0); 
}

uint64_t RCIsoBootInitialEntry::GetSize() const
{
    return m_sectorCount * 512;
}

RCString RCIsoBootInitialEntry::GetName() const
{
    RCString s;
    if (m_bootable)
    {
        s += _T("Bootable");
    }
    else
    {
        s += _T("NotBootable");
    }
    s += _T("_");
    if (m_bootMediaType >= RCIsoHeaderDefs::kNumBootMediaTypes)
    {
        char_t name[32];
        RCStringUtil::ConvertUInt64ToString(m_bootMediaType, name);
        s += name;
    }
    else
    {
        s += RCIsoHeaderDefs::kMediaTypes[m_bootMediaType];
    }
    s += _T(".img");
    return s;
}

bool RCIsoVolumeDescriptor::IsJoliet() const
{
    if ((m_volFlags & 1) != 0)
    {
        return false;
    }
    byte_t b = m_escapeSequence[2];
    return (m_escapeSequence[0] == 0x25 && m_escapeSequence[1] == 0x2F &&
        (b == 0x40 || b == 0x43 || b == 0x45));
}

RCIsoInArchive::RCIsoInArchive():
    m_archiveSize(0),
    m_mainVolDescIndex(0),
    m_blockSize(0),
    m_isSusp(false),
    m_suspSkipSize(0),
    m_position(0),
    m_bufferPos(0),
    m_bootIsDefined(false),
    m_openArchiveCallback(0)
{
    
}

bool RCIsoInArchive::IsJoliet() const 
{ 
    return m_volDescs[m_mainVolDescIndex].IsJoliet(); 
}

uint64_t RCIsoInArchive::GetBootItemSize(int32_t index) const
{
    const RCIsoBootInitialEntry &be = m_bootEntries[index];
    uint64_t size = be.GetSize();
    if (be.m_bootMediaType == RCIsoHeaderDefs::k1d2Floppy)
    {
        size = (1200 << 10);
    }
    else if (be.m_bootMediaType == RCIsoHeaderDefs::k1d44Floppy)
    {
        size = (1440 << 10);
    }
    else if (be.m_bootMediaType == RCIsoHeaderDefs::k2d88Floppy)
    {
        size = (2880 << 10);
    }
    uint64_t startPos = be.m_loadRBA * m_blockSize;
    if (startPos < m_archiveSize)
    {
        if (m_archiveSize - startPos < size)
        {
            size = m_archiveSize - startPos;
        }
    }
    return size;
}

byte_t RCIsoInArchive::ReadByte()
{
    if (m_bufferPos >= m_blockSize)
    {
        m_bufferPos = 0;
    }
    if (m_bufferPos == 0)
    {
        size_t processedSize = m_blockSize;
        if (RCStreamUtils::ReadStream(m_stream.Get(), m_buffer, &processedSize) != RC_S_OK)
        {
            _ThrowCode(RC_E_ReadStreamError) ;
        }
        if (processedSize != m_blockSize)
        {
            _ThrowCode(RC_E_ReadStreamError) ;
        }
    }
    byte_t b = m_buffer[m_bufferPos++];
    m_position++;
    return b;
}

void RCIsoInArchive::ReadBytes(byte_t *data, uint32_t size)
{
    for (uint32_t i = 0; i < size; i++)
    {
        data[i] = ReadByte();
    }
}

void RCIsoInArchive::Skip(size_t size)
{
    while (size-- != 0)
    {
        ReadByte();
    }
}

void RCIsoInArchive::SkipZeros(size_t size)
{
    while (size-- != 0)
    {
        byte_t b = ReadByte();
        if (b != 0)
        {
            _ThrowCode(RC_E_ReadStreamError) ;
        }
    }
}

uint16_t RCIsoInArchive::ReadUInt16Spec()
{
    uint16_t value = 0;
    for (int32_t i = 0; i < 2; i++)
    {
        value |= ((uint16_t)(ReadByte()) << (8 * i));
    }
    return value;
}

uint16_t RCIsoInArchive::ReadUInt16()
{
    byte_t b[4];
    ReadBytes(b, 4);
    uint32_t value = 0;
    for (int32_t i = 0; i < 2; i++)
    {
        if (b[i] != b[3 - i])
        {
            if (m_openArchiveCallback && (m_openArchiveCallback->SetError(RC_E_ReadStreamError) != RC_S_OK))
            {
                _ThrowCode(RC_E_ReadStreamError) ;
            }
        }
        value |= ((uint16_t)(b[i]) << (8 * i));
    }
    return (uint16_t)value;
}

uint32_t RCIsoInArchive::ReadUInt32Le()
{
    uint32_t value = 0;
    for (int32_t i = 0; i < 4; i++)
    {
        value |= ((uint32_t)(ReadByte()) << (8 * i));
    }
    return value;
}

uint32_t RCIsoInArchive::ReadUInt32Be()
{
    uint32_t value = 0;
    for (int32_t i = 0; i < 4; i++)
    {
        value <<= 8;
        value |= ReadByte();
    }
    return value;
}

uint32_t RCIsoInArchive::ReadUInt32()
{
    byte_t b[8];
    ReadBytes(b, 8);
    uint32_t value = 0;
    for (int32_t i = 0; i < 4; i++)
    {
        if (b[i] != b[7 - i])
        {
            if (m_openArchiveCallback && (m_openArchiveCallback->SetError(RC_E_ReadStreamError) != RC_S_OK))
            {
                _ThrowCode(RC_E_ReadStreamError) ;
            }
        }
        value |= ((uint32_t)(b[i]) << (8 * i));
    }
    return value;
}

uint32_t RCIsoInArchive::ReadDigits(int32_t numDigits)
{
    uint32_t res = 0;
    for (int32_t i = 0; i < numDigits; i++)
    {
        byte_t b = ReadByte();
        if (b < '0' || b > '9')
        {
            if (b == 0) // it's bug in some CD's
            {
                b = '0';
            }
            else
            {
                _ThrowCode(RC_E_ReadStreamError) ;
            }
        }
        uint32_t d = (uint32_t)(b - '0');
        res *= 10;
        res += d;
    }
    return res;
}

void RCIsoInArchive::ReadDateTime(RCIsoDateTime &d)
{
    d.m_year = (uint16_t)ReadDigits(4);
    d.m_month = (byte_t)ReadDigits(2);
    d.m_day = (byte_t)ReadDigits(2);
    d.m_hour = (byte_t)ReadDigits(2);
    d.m_minute = (byte_t)ReadDigits(2);
    d.m_second = (byte_t)ReadDigits(2);
    d.m_hundredths = (byte_t)ReadDigits(2);
    d.m_gmtOffset = (signed char)ReadByte();
}

void RCIsoInArchive::ReadBootRecordDescriptor(RCIsoBootRecordDescriptor &d)
{
    ReadBytes(d.m_bootSystemId, sizeof(d.m_bootSystemId));
    ReadBytes(d.m_bootId, sizeof(d.m_bootId));
    ReadBytes(d.m_bootSystemUse, sizeof(d.m_bootSystemUse));
}

void RCIsoInArchive::ReadRecordingDateTime(RCIsoRecordingDateTime &t)
{
    t.m_year = ReadByte();
    t.m_month = ReadByte();
    t.m_day = ReadByte();
    t.m_hour = ReadByte();
    t.m_minute = ReadByte();
    t.m_second = ReadByte();
    t.m_gmtOffset = (signed char)ReadByte();
}

void RCIsoInArchive::ReadDirRecord2(RCIsoDirRecord &r, byte_t len)
{
    r.m_extendedAttributeRecordLen = ReadByte();
    if (r.m_extendedAttributeRecordLen != 0)
    {
        _ThrowCode(RC_E_ReadStreamError) ;
    }
    r.m_extentLocation = ReadUInt32();
    r.m_dataLength = ReadUInt32();
    ReadRecordingDateTime(r.m_dateTime);
    r.m_fileFlags = ReadByte();
    r.m_fileUnitSize = ReadByte();
    r.m_interleaveGapSize = ReadByte();
    r.m_volSequenceNumber = ReadUInt16();
    byte_t idLen = ReadByte();
    r.m_fileId.SetCapacity(idLen);
    ReadBytes((byte_t *)r.m_fileId.data(), idLen);
    int32_t padSize = 1 - (idLen & 1);
    // SkipZeros(1 - (idLen & 1));
    Skip(1 - (idLen & 1)); // it's bug in some cd's. Must be zeros

    int32_t curPos = 33 + idLen + padSize;
    if (curPos > len)
    {
        _ThrowCode(RC_E_ReadStreamError) ;
    }
    int32_t rem = len - curPos;
    r.m_systemUse.SetCapacity(rem);
    ReadBytes((byte_t *)r.m_systemUse.data(), rem);
}

void RCIsoInArchive::ReadDirRecord(RCIsoDirRecord &r)
{
    byte_t len = ReadByte();
    // Some CDs can have incorrect value len = 48 ('0') in VolumeDescriptor.
    // But maybe we must use real "len" for other records.
    len = 34;
    ReadDirRecord2(r, len);
}

void RCIsoInArchive::ReadVolumeDescriptor(RCIsoVolumeDescriptor &d)
{
    d.m_volFlags = ReadByte();
    ReadBytes(d.m_systemId, sizeof(d.m_systemId));
    ReadBytes(d.m_volumeId, sizeof(d.m_volumeId));
    SkipZeros(8);
    d.m_volumeSpaceSize = ReadUInt32();
    ReadBytes(d.m_escapeSequence, sizeof(d.m_escapeSequence));
    d.m_volumeSetSize = ReadUInt16();
    d.m_volumeSequenceNumber = ReadUInt16();
    d.m_logicalBlockSize = ReadUInt16();
    d.m_pathTableSize = ReadUInt32();
    d.m_lPathTableLocation = ReadUInt32Le();
    d.m_lOptionalPathTableLocation = ReadUInt32Le();
    d.m_mPathTableLocation = ReadUInt32Be();
    d.m_mOptionalPathTableLocation = ReadUInt32Be();
    ReadDirRecord(d.m_rootDirRecord);
    ReadBytes(d.m_volumeSetId, sizeof(d.m_volumeSetId));
    ReadBytes(d.m_publisherId, sizeof(d.m_publisherId));
    ReadBytes(d.m_dataPreparerId, sizeof(d.m_dataPreparerId));
    ReadBytes(d.m_applicationId, sizeof(d.m_applicationId));
    ReadBytes(d.m_copyrightFileId, sizeof(d.m_copyrightFileId));
    ReadBytes(d.m_abstractFileId, sizeof(d.m_abstractFileId));
    ReadBytes(d.m_bibFileId, sizeof(d.m_bibFileId));
    ReadDateTime(d.m_cTime);
    ReadDateTime(d.m_mTime);
    ReadDateTime(d.m_expirationTime);
    ReadDateTime(d.m_effectiveTime);
    d.m_fileStructureVersion = ReadByte(); // = 1
    SkipZeros(1);
    ReadBytes(d.m_applicationUse, sizeof(d.m_applicationUse));
    SkipZeros(653);
}

inline bool RCIsoInArchive::CheckSignature(const byte_t *sig, const byte_t *data)
{
    for (int32_t i = 0; i < 5; i++)
    {
        if (sig[i] != data[i])
        {
            return false;
        }
    }
    return true;
}

void RCIsoInArchive::SeekToBlock(uint32_t blockIndex)
{
    if (m_stream->Seek((uint64_t)blockIndex * m_volDescs[m_mainVolDescIndex].m_logicalBlockSize, 
                       RC_STREAM_SEEK_SET, &m_position) != RC_S_OK)
    {
        _ThrowCode(RC_E_SeekStreamError) ;
    }
    m_bufferPos = 0;
}

void RCIsoInArchive::ReadDir(RCIsoDir &d, int32_t level)
{
    if (!d.IsDir())
    {
        return;
    }
    SeekToBlock(d.m_extentLocation);
    uint64_t startPos = m_position;

    bool firstItem = true;
    for (;;)
    {
        uint64_t offset = m_position - startPos;
        if (offset >= d.m_dataLength)
        {
            break;
        }
        byte_t len = ReadByte();
        if (len == 0)
        {
            continue;
        }
        RCIsoDir subItem;
        ReadDirRecord2(subItem, len);
        if (firstItem && level == 0)
        {
            m_isSusp = subItem.CheckSusp(m_suspSkipSize);
        }
        if (!subItem.IsSystemItem())
        {
            d.m_subItems.push_back(subItem);
        }
        firstItem = false;
    }

    for (int32_t i = 0; i < static_cast<int32_t>(d.m_subItems.size()); i++)
    {
        ReadDir(d.m_subItems[i], level + 1);
    }
}

void RCIsoInArchive::CreateRefs(RCIsoDir &d)
{
    if (!d.IsDir())
    {
        return;
    }
    for (int32_t i = 0; i < static_cast<int32_t>(d.m_subItems.size()); i++)
    {
        RCIsoRef ref;
        RCIsoDir &subItem = d.m_subItems[i];
        subItem.m_parent = &d;
        ref.m_dir = &d;
        ref.m_index = i;
        m_refs.push_back(ref);
        CreateRefs(subItem);
    }
}

void RCIsoInArchive::ReadBootInfo()
{
    if (!m_bootIsDefined)
    {
        return;
    }
    if (memcmp(m_bootDesc.m_bootSystemId, RCIsoHeaderDefs::kElToritoSpec, sizeof(m_bootDesc.m_bootSystemId)) != 0)
    {
        return;
    }
    const byte_t *p = (const byte_t *)m_bootDesc.m_bootSystemUse;
    uint32_t blockIndex = p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
    SeekToBlock(blockIndex);
    byte_t b = ReadByte();
    if (b != RCIsoHeaderDefs::kValidationEntry)
    {
        return;
    }
    {
        RCIsoBootValidationEntry e;
        e.m_platformId = ReadByte();
        if (ReadUInt16Spec() != 0)
        {
            _ThrowCode(RC_E_ReadStreamError) ;
        }
        ReadBytes(e.m_id, sizeof(e.m_id));
        /* uint16_t checkSum = */ 
        ReadUInt16Spec();
        if (ReadByte() != 0x55)
        {
            _ThrowCode(RC_E_ReadStreamError) ;
        }
        if (ReadByte() != 0xAA)
        {
            _ThrowCode(RC_E_ReadStreamError) ;
        }
    }
    b = ReadByte();
    if (b == RCIsoHeaderDefs::kInitialEntryBootable || b == RCIsoHeaderDefs::kInitialEntryNotBootable)
    {
        RCIsoBootInitialEntry e;
        e.m_bootable = (b == RCIsoHeaderDefs::kInitialEntryBootable);
        e.m_bootMediaType = ReadByte();
        e.m_loadSegment = ReadUInt16Spec();
        e.m_systemType = ReadByte();
        if (ReadByte() != 0)
        {
            _ThrowCode(RC_E_ReadStreamError) ;
        }
        e.m_sectorCount = ReadUInt16Spec();
        e.m_loadRBA = ReadUInt32Le();
        if (ReadByte() != 0)
        {
            _ThrowCode(RC_E_ReadStreamError) ;
        }
        m_bootEntries.push_back(e);
    }
    else
    {
        return;
    }
}

const byte_t RCIsoInArchive::s_sigCD001[5] = { 'C', 'D', '0', '0', '1' };
const byte_t RCIsoInArchive::s_sigNSR02[5] = { 'N', 'S', 'R', '0', '2' };
const byte_t RCIsoInArchive::s_sigNSR03[5] = { 'N', 'S', 'R', '0', '3' };
const byte_t RCIsoInArchive::s_sigBEA01[5] = { 'B', 'E', 'A', '0', '1' };
const byte_t RCIsoInArchive::s_sigTEA01[5] = { 'T', 'E', 'A', '0', '1' };

HResult RCIsoInArchive::Open2()
{
    Clear();
    HResult result;
    result = m_stream->Seek(RCIsoHeaderDefs::kStartPos, RC_STREAM_SEEK_CUR, &m_position);
    if (!IsSuccess(result))
    {
        return result;
    }

    m_bufferPos = 0;
    m_blockSize = s_blockSize;
    for (;;)
    {
        byte_t sig[7];
        ReadBytes(sig, 7);
        byte_t ver = sig[6];
        if (!CheckSignature(s_sigCD001, sig + 1))
        {
            return RC_S_FALSE;
        }
        // version = 2 for ISO 9660:1999?
        if (ver > 2)
        {
            _ThrowCode(RC_S_FALSE) ;
        }

        if (sig[0] == RCIsoHeaderDefs::kTerminator)
        {
            break;
            // Skip(0x800 - 7);
            // continue;
        }
        switch(sig[0])
        {
            case RCIsoHeaderDefs::kBootRecord:
                {
                    m_bootIsDefined = true;
                    ReadBootRecordDescriptor(m_bootDesc);
                    break;
                }
            case RCIsoHeaderDefs::kPrimaryVol:
            case RCIsoHeaderDefs::kSupplementaryVol:
               {
                   RCIsoVolumeDescriptor vd;
                   ReadVolumeDescriptor(vd);
                   if (sig[0] == RCIsoHeaderDefs::kPrimaryVol)
                   {
                       // some burners write "Joliet" Escape Sequence to primary volume
                       memset(vd.m_escapeSequence, 0, sizeof(vd.m_escapeSequence));
                   }
                   m_volDescs.push_back(vd);
                   break;
                }
            default:
                break;
        }
    }

    if (m_volDescs.empty())
    {
        return RC_S_FALSE;
    }

    for (m_mainVolDescIndex = (int32_t)m_volDescs.size() - 1; m_mainVolDescIndex > 0; --m_mainVolDescIndex)
    {
        if (m_volDescs[m_mainVolDescIndex].IsJoliet())
        {
            break;
        }
    }
    // MainVolDescIndex = 0; // to read primary volume
    const RCIsoVolumeDescriptor &vd = m_volDescs[m_mainVolDescIndex];
    if (vd.m_logicalBlockSize != s_blockSize)
    {
        return RC_S_FALSE;
    }
    (RCIsoDirRecord &)m_rootDir = vd.m_rootDirRecord;
    ReadDir(m_rootDir, 0);
    CreateRefs(m_rootDir);
    ReadBootInfo();
    return RC_S_OK;
}

HResult RCIsoInArchive::Open(IInStream *inStream, IArchiveOpenCallback* openArchiveCallback)
{
    m_stream = inStream;
    m_openArchiveCallback = openArchiveCallback;
    uint64_t pos;
    HResult result;
    result = m_stream->Seek(0, RC_STREAM_SEEK_CUR, &pos);
    if (!IsSuccess(result))
    {
        return result;
    }

    result = m_stream->Seek(0, RC_STREAM_SEEK_END, &m_archiveSize);
    if (!IsSuccess(result))
    {
        return result;
    }

    result = m_stream->Seek(pos, RC_STREAM_SEEK_SET, &m_position);
    if (!IsSuccess(result))
    {
        return result;
    }

    HResult res = RC_S_FALSE;
    try 
    { 
        res = Open2(); 
    }
    catch(...) 
    { 
        Clear(); 
        res = RC_S_FALSE; 
    }
    m_stream.Release();
    return res;
}

void RCIsoInArchive::Clear()
{
    m_refs.clear();
    m_rootDir.Clear();
    m_volDescs.clear();
    m_bootIsDefined = false;
    m_bootEntries.clear();
    m_suspSkipSize = 0;
    m_isSusp = false;
}

void RCIsoInArchive::Reset()
{
    m_openArchiveCallback = 0;
}

END_NAMESPACE_RCZIP
