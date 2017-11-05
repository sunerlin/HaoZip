/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "format/chm/RCChmIn.h"
#include "format/chm/RCChmHeader.h"
#include "format/chm/RCChmDefs.h"
#include "filesystem/RCStreamUtils.h"
#include "filesystem/RCLimitedSequentialInStream.h"
#include "common/RCStringConvert.h"

/////////////////////////////////////////////////////////////////
//RCChmIn class implementation

BEGIN_NAMESPACE_RCZIP

static char GetHex(byte_t value)
{
    return (char)((value < 10) ? ('0' + value) : ('A' + (value - 10)));
}

static void PrintByte(byte_t b, RCStringA& s)
{
    s += GetHex(b >> 4);
    s += GetHex(b & 0xF);
}

byte_t RCChmIn::ReadByte()
{
    byte_t b;
    if (!m_inBuffer.ReadByte(b))
    {
        _ThrowCode(RC_E_ReadStreamError) ;
    }
    return b;
}

void RCChmIn::Skip(size_t size)
{
    while (size-- != 0)
    {
        ReadByte();
    }
}

void RCChmIn::ReadBytes(byte_t* data, uint32_t size)
{
    for (uint32_t i = 0; i < size; i++)
    {
        data[i] = ReadByte();
    }
}

uint16_t RCChmIn::ReadUInt16()
{
    uint16_t value = 0;
    for (int32_t i = 0; i < 2; i++)
    {
        value |= ((uint16_t)(ReadByte()) << (8 * i));
    }
    return value;
}

uint32_t RCChmIn::ReadUInt32()
{
    uint32_t value = 0;
    for (int32_t i = 0; i < 4; i++)
    {
        value |= ((uint32_t)(ReadByte()) << (8 * i));
    }
    return value;
}

uint64_t RCChmIn::ReadUInt64()
{
    uint64_t value = 0;
    for (int32_t i = 0; i < 8; i++)
    {
        value |= ((uint64_t)(ReadByte()) << (8 * i));
    }
    return value;
}

uint64_t RCChmIn::ReadEncInt()
{
    uint64_t val = 0;;
    for (int32_t i = 0; i < 10; i++)
    {
        byte_t b = ReadByte();
        val |= (b & 0x7F);
        if (b < 0x80)
        {
            return val;
        }
        val <<= 7;
    }
    _ThrowCode(RC_E_ReadStreamError) ;
}

void RCChmIn::ReadGUID(RC_GUID &g)
{
    g.Data1 = ReadUInt32();
    g.Data2 = ReadUInt16();
    g.Data3 = ReadUInt16();
    ReadBytes(g.Data4, 8);
}

void RCChmIn::ReadString(int32_t size, RCStringA& s)
{
    s.clear();
    while(size-- != 0)
    {
        char c = (char)ReadByte();
        if (c == 0)
        {
            Skip(size);
            return;
        }
        s += c;
    }
}

void RCChmIn::ReadUString(int32_t size, RCString& s)
{
    s.clear();
    while(size-- != 0)
    {
        char_t c = ReadUInt16();
        if (c == 0)
        {
            Skip(2 * size);
            return;
        }
        s += c;
    }
}

HResult RCChmIn::ReadChunk(IInStream* inStream, uint64_t pos, uint64_t size)
{
    HResult hr = inStream->Seek(pos, RC_STREAM_SEEK_SET, NULL);
    if (hr != RC_S_OK)
    {
        return hr;
    }

    RCLimitedSequentialInStream* streamSpec = new RCLimitedSequentialInStream;
    ISequentialInStreamPtr limitedStream(streamSpec);
    streamSpec->SetStream(inStream);
    streamSpec->Init(size);
    m_inBuffer.SetStream(limitedStream.Get());
    m_inBuffer.Init();

    return RC_S_OK;
}

HResult RCChmIn::ReadDirEntry(RCChmDatabase& database)
{
    RCChmItem item;
    uint64_t nameLength = ReadEncInt();
    if (nameLength == 0 || nameLength >= 0x10000000)
    {
        return RC_S_FALSE;
    }

    ReadString((int32_t)nameLength, item.m_name);
    item.m_section = ReadEncInt();
    item.m_offset = ReadEncInt();
    item.m_size = ReadEncInt();
    database.m_items.push_back(item);

    return RC_S_OK;
}

HResult RCChmIn::OpenChm(IInStream* inStream, RCChmDatabase& database)
{
    uint32_t headerSize = ReadUInt32();
    if (headerSize != 0x60)
    {
        return RC_S_FALSE;
    }
    uint32_t unknown1 = ReadUInt32();
    if (unknown1 != 0 && unknown1 != 1) // it's 0 in one .sll file
    {
        return RC_S_FALSE;
    }
    /* uint32_t timeStamp = */ ReadUInt32();
    // Considered as a big-endian DWORD, it appears to contain seconds (MSB) and
    // fractional seconds (second byte).
    // The third and fourth bytes may contain even more fractional bits.
    // The 4 least significant bits in the last byte are constant.
    /* uint32_t lang = */ ReadUInt32();
    RC_GUID g;
    ReadGUID(g); // {7C01FD10-7BAA-11D0-9E0C-00A0-C922-E6EC}
    ReadGUID(g); // {7C01FD11-7BAA-11D0-9E0C-00A0-C922-E6EC}
    const int32_t kNumSections = 2;
    uint64_t sectionOffsets[kNumSections];
    uint64_t sectionSizes[kNumSections];
    int32_t i;
    for (i = 0; i < kNumSections; i++)
    {
        sectionOffsets[i] = ReadUInt64();
        sectionSizes[i] = ReadUInt64();
    }
    // if (chmVersion == 3)
    database.m_contentOffset = ReadUInt64();
    /*
    else
    database.ContentOffset = m_startPosition + 0x58
    */

    /*
    // Section 0
    ReadChunk(inStream, sectionOffsets[0], sectionSizes[0]);
    if (sectionSizes[0] != 0x18)
    return RC_S_FALSE;
    ReadUInt32(); // unknown:  01FE
    ReadUInt32(); // unknown:  0
    uint64_t fileSize = ReadUInt64();
    ReadUInt32(); // unknown:  0
    ReadUInt32(); // unknown:  0
    */

    // Section 1: The Directory Listing
    ReadChunk(inStream, sectionOffsets[1], sectionSizes[1]);
    if (ReadUInt32() != RCChmHeader::kItspSignature)
    {
        return RC_S_FALSE;
    }

    if (ReadUInt32() != 1) // version
    {
        return RC_S_FALSE;
    }

    /* uint32_t dirHeaderSize = */ ReadUInt32();
    ReadUInt32(); // 0x0A (unknown)
    uint32_t dirChunkSize = ReadUInt32(); // $1000
    if (dirChunkSize < 32)
    {
        return RC_S_FALSE;
    }
    /* uint32_t density = */ ReadUInt32(); //  "Density" of quickref section, usually 2.
    /* uint32_t depth = */ ReadUInt32(); //  Depth of the index tree: 1 there is no index,
    // 2 if there is one level of PMGI chunks.

    /* uint32_t chunkNumber = */ ReadUInt32(); //  Chunk number of root index chunk, -1 if there is none
    // (though at least one file has 0 despite there being no
    // index chunk, probably a bug.)
    /* uint32_t firstPmglChunkNumber = */ ReadUInt32(); // Chunk number of first PMGL (listing) chunk
    /* uint32_t lastPmglChunkNumber = */ ReadUInt32();  // Chunk number of last PMGL (listing) chunk
    ReadUInt32(); // -1 (unknown)
    uint32_t numDirChunks = ReadUInt32(); // Number of directory chunks (total)
    /* uint32_t windowsLangId = */ ReadUInt32();
    ReadGUID(g);  // {5D02926A-212E-11D0-9DF9-00A0C922E6EC}
    ReadUInt32(); // 0x54 (This is the length again)
    ReadUInt32(); // -1 (unknown)
    ReadUInt32(); // -1 (unknown)
    ReadUInt32(); // -1 (unknown)

    for (uint32_t ci = 0; ci < numDirChunks; ci++)
    {
        uint64_t chunkPos = m_inBuffer.GetProcessedSize();
        if (ReadUInt32() == RCChmHeader::kPmglSignature)
        {
            // The quickref area is written backwards from the end of the chunk.
            // One quickref entry exists for every n entries in the file, where n
            // is calculated as 1 + (1 << quickref density). So for density = 2, n = 5.

            uint32_t quickrefLength = ReadUInt32(); // Length of free space and/or quickref area at end of directory chunk
            if (quickrefLength > dirChunkSize || quickrefLength < 2)
            {
                return RC_S_FALSE;
            }

            ReadUInt32(); // Always 0
            ReadUInt32(); // Chunk number of previous listing chunk when reading
            // directory in sequence (-1 if this is the first listing chunk)
            ReadUInt32(); // Chunk number of next  listing chunk when reading
            // directory in sequence (-1 if this is the last listing chunk)
            int32_t numItems = 0;
            for (;;)
            {
                uint64_t offset = m_inBuffer.GetProcessedSize() - chunkPos;
                uint32_t offsetLimit = dirChunkSize - quickrefLength;
                if (offset > offsetLimit)
                {
                    return RC_S_FALSE;
                }

                if (offset == offsetLimit)
                {
                    break;
                }
                HResult hr = ReadDirEntry(database);
                if (hr != RC_S_OK)
                {
                    return hr;
                }
                numItems++;
            }
            Skip(quickrefLength - 2);
            if (ReadUInt16() != numItems)
            {
                return RC_S_FALSE;
            }
        }
        else
        {
            Skip(dirChunkSize - 4);
        }
    }
    return RC_S_OK;
}

HResult RCChmIn::OpenHelp2(IInStream* inStream, RCChmDatabase& database)
{
    if (ReadUInt32() != 1) // version
    {
        return RC_S_FALSE;
    }

    if (ReadUInt32() != 0x28) // Location of header section table
    {
        return RC_S_FALSE;
    }

    uint32_t numHeaderSections = ReadUInt32();
    const int32_t kNumHeaderSectionsMax = 5;
    if (numHeaderSections != kNumHeaderSectionsMax)
    {
        return RC_S_FALSE;
    }

    ReadUInt32(); // Length of post-header table
    RC_GUID g;
    ReadGUID(g);  // {0A9007C1-4076-11D3-8789-0000F8105754}

    // header section table
    uint64_t sectionOffsets[kNumHeaderSectionsMax];
    uint64_t sectionSizes[kNumHeaderSectionsMax];
    uint32_t i;
    for (i = 0; i < numHeaderSections; i++)
    {
        sectionOffsets[i] = ReadUInt64();
        sectionSizes[i] = ReadUInt64();
    }

    // Post-Header
    ReadUInt32(); // 2
    ReadUInt32(); // 0x98: offset to CAOL from beginning of post-header)
    // ----- Directory information
    ReadUInt64(); // Chunk number of top-level AOLI chunk in directory, or -1
    ReadUInt64(); // Chunk number of first AOLL chunk in directory
    ReadUInt64(); // Chunk number of last AOLL chunk in directory
    ReadUInt64(); // 0 (unknown)
    ReadUInt32(); // $2000 (Directory chunk size of directory)
    ReadUInt32(); // Quickref density for main directory, usually 2
    ReadUInt32(); // 0 (unknown)
    ReadUInt32(); // Depth of main directory index tree
    // 1 there is no index, 2 if there is one level of AOLI chunks.
    ReadUInt64(); // 0 (unknown)
    uint64_t numDirEntries = ReadUInt64(); // Number of directory entries
    // ----- Directory Index Information
    ReadUInt64(); // -1 (unknown, probably chunk number of top-level AOLI in directory index)
    ReadUInt64(); // Chunk number of first AOLL chunk in directory index
    ReadUInt64(); // Chunk number of last AOLL chunk in directory index
    ReadUInt64(); // 0 (unknown)
    ReadUInt32(); // $200 (Directory chunk size of directory index)
    ReadUInt32(); // Quickref density for directory index, usually 2
    ReadUInt32(); // 0 (unknown)
    ReadUInt32(); // Depth of directory index index tree.
    ReadUInt64(); // Possibly flags -- sometimes 1, sometimes 0.
    ReadUInt64(); // Number of directory index entries (same as number of AOLL
    // chunks in main directory)

    // (The obvious guess for the following two fields, which recur in a number
    // of places, is they are maximum sizes for the directory and directory index.
    // However, I have seen no direct evidence that this is the case.)

    ReadUInt32(); // $100000 (Same as field following chunk size in directory)
    ReadUInt32(); // $20000 (Same as field following chunk size in directory index)

    ReadUInt64(); // 0 (unknown)
    if (ReadUInt32() != RCChmHeader::kCaolSignature)
    {
        return RC_S_FALSE;
    }

    if (ReadUInt32() != 2) // (Most likely a version number)
    {
        return RC_S_FALSE;
    }

    uint32_t caolLength = ReadUInt32(); // $50 (Length of the CAOL section, which includes the ITSF section)
    if (caolLength >= 0x2C)
    {
        /* uint32_t c7 = */ ReadUInt16(); // Unknown.  Remains the same when identical files are built.
        // Does not appear to be a checksum.  Many files have
        // 'HH' (HTML Help?) here, indicating this may be a compiler ID
        //  field.  But at least one ITOL/ITLS compiler does not set this
        // field to a constant value.
        ReadUInt16(); // 0 (Unknown.  Possibly part of 00A4 field)
        ReadUInt32(); // Unknown.  Two values have been seen -- $43ED, and 0.
        ReadUInt32(); // $2000 (Directory chunk size of directory)
        ReadUInt32(); // $200 (Directory chunk size of directory index)
        ReadUInt32(); // $100000 (Same as field following chunk size in directory)
        ReadUInt32(); // $20000 (Same as field following chunk size in directory index)
        ReadUInt32(); // 0 (unknown)
        ReadUInt32(); // 0 (Unknown)
        if (caolLength == 0x2C)
        {
            database.m_contentOffset = 0;
            database.m_newFormat = true;
        }
        else if (caolLength == 0x50)
        {
            ReadUInt32(); // 0 (Unknown)
            if (ReadUInt32() != RCChmHeader::kItsfSignature)
            {
                return RC_S_FALSE;
            }

            if (ReadUInt32() != 4) // $4 (Version number -- CHM uses 3)
            {
                return RC_S_FALSE;
            }

            if (ReadUInt32() != 0x20) // $20 (length of ITSF)
            {
                return RC_S_FALSE;
            }

            uint32_t unknown = ReadUInt32();
            if (unknown != 0 && unknown != 1) // = 0 for some HxW files, 1 in other cases;
            {
                return RC_S_FALSE;
            }

            database.m_contentOffset = m_startPosition + ReadUInt64();
            /* uint32_t timeStamp = */ 
            ReadUInt32();
            // A timestamp of some sort.
            // Considered as a big-endian DWORD, it appears to contain
            // seconds (MSB) and fractional seconds (second byte).
            // The third and fourth bytes may contain even more fractional
            // bits.  The 4 least significant bits in the last byte are constant.
            /* uint32_t lang = */
            ReadUInt32(); // BE?
        }
        else
        {
            return RC_S_FALSE;
        }
    }

    /*
    // Section 0
    ReadChunk(inStream, m_startPosition + sectionOffsets[0], sectionSizes[0]);
    if (sectionSizes[0] != 0x18)
    return RC_S_FALSE;
    ReadUInt32(); // unknown:  01FE
    ReadUInt32(); // unknown:  0
    uint64_t fileSize = ReadUInt64();
    ReadUInt32(); // unknown:  0
    ReadUInt32(); // unknown:  0
    */

    // Section 1: The Directory Listing
    ReadChunk(inStream, m_startPosition + sectionOffsets[1], sectionSizes[1]);
    if (ReadUInt32() != RCChmHeader::kIfcmSignature)
    {
        return RC_S_FALSE;
    }

    if (ReadUInt32() != 1) // (probably a version number)
    {
        return RC_S_FALSE;
    }

    uint32_t dirChunkSize = ReadUInt32(); // $2000
    if (dirChunkSize < 64)
    {
        return RC_S_FALSE;
    }
    ReadUInt32(); // $100000  (unknown)
    ReadUInt32(); // -1 (unknown)
    ReadUInt32(); // -1 (unknown)
    uint32_t numDirChunks = ReadUInt32();
    ReadUInt32(); // 0 (unknown, probably high word of above)

    for (uint32_t ci = 0; ci < numDirChunks; ci++)
    {
        uint64_t chunkPos = m_inBuffer.GetProcessedSize();
        if (ReadUInt32() == RCChmHeader::kAollSignature)
        {
            uint32_t quickrefLength = ReadUInt32(); // Length of quickref area at end of directory chunk
            if (quickrefLength > dirChunkSize || quickrefLength < 2)
            {
                return RC_S_FALSE;
            }

            ReadUInt64(); // Directory chunk number
            // This must match physical position in file, that is
            // the chunk size times the chunk number must be the
            // offset from the end of the directory header.
            ReadUInt64(); // Chunk number of previous listing chunk when reading
            // directory in sequence (-1 if first listing chunk)
            ReadUInt64(); // Chunk number of next listing chunk when reading
            // directory in sequence (-1 if last listing chunk)
            ReadUInt64(); // Number of first listing entry in this chunk
            ReadUInt32(); // 1 (unknown -- other values have also been seen here)
            ReadUInt32(); // 0 (unknown)

            int32_t numItems = 0;
            for (;;)
            {
                uint64_t offset = m_inBuffer.GetProcessedSize() - chunkPos;
                uint32_t offsetLimit = dirChunkSize - quickrefLength;
                if (offset > offsetLimit)
                {
                    return RC_S_FALSE;
                }

                if (offset == offsetLimit)
                {
                    break;
                }

                if (database.m_newFormat)
                {
                    uint16_t nameLength = ReadUInt16();
                    if (nameLength == 0)
                    {
                        return RC_S_FALSE;
                    }

                    RCString name;
                    ReadUString((int32_t)nameLength, name);
                    RCStringA s;
                    RCStringConvert::ConvertUnicodeToUTF8(name.c_str(), name.length(), s);
                    byte_t b = ReadByte();
                    s += ' ';
                    PrintByte(b, s);
                    s += ' ';
                    uint64_t len = ReadEncInt();
                    // then number of items ?
                    // then length ?
                    // then some data (binary encoding?)
                    while (len-- != 0)
                    {
                        b = ReadByte();
                        PrintByte(b, s);
                    }
                    database.m_newFormatString += s;
                    database.m_newFormatString += "\r\n";
                }
                else
                {
                    HResult hr = ReadDirEntry(database);
                    if (hr != RC_S_OK)
                    {
                        return hr;
                    }
                }
                numItems++;
            }
            Skip(quickrefLength - 2);
            if (ReadUInt16() != numItems)
            {
                return RC_S_FALSE;
            }

            if (numItems > numDirEntries)
            {
                return RC_S_FALSE;
            }

            numDirEntries -= numItems;
        }
        else
        {
            Skip(dirChunkSize - 4);
        }
    }
    return numDirEntries == 0 ? RC_S_OK : RC_S_FALSE;
}

HResult RCChmIn::DecompressStream(IInStream* inStream, const RCChmDatabase& database, const RCStringA& name)
{
    int32_t index = database.FindItem(name);
    if (index < 0)
    {
        return RC_S_FALSE;
    }
    const RCChmItem& item = database.m_items[index];
    m_chunkSize = item.m_size;
    return ReadChunk(inStream, database.m_contentOffset + item.m_offset, item.m_size);
}

#define DATA_SPACE "::DataSpace/"
static const char* s_nameList = DATA_SPACE "NameList";
static const char* s_storage = DATA_SPACE "Storage/";
static const char* s_content = "Content";
static const char* s_controlData = "ControlData";
static const char* s_spanInfo = "SpanInfo";
static const char* s_transform = "Transform/";
static const char* s_resetTable = "/InstanceData/ResetTable";
static const char* s_transformList = "List";

static RCStringA GetSectionPrefix(const RCStringA &name)
{
    return RCStringA(s_storage) + name + RCStringA("/");
}

HResult RCChmIn::OpenHighLevel(IInStream* inStream, RCChmFilesDatabase& database)
{
    HResult hr;
    {
        // The NameList file
        hr = DecompressStream(inStream, database, s_nameList);
        if (hr != RC_S_OK)
        {
            return hr;
        }

        /* uint16_t length = */
        ReadUInt16();

        uint16_t numSections = ReadUInt16();
        for (int32_t i = 0; i < numSections; i++)
        {
            RCChmSectionInfo section;
            uint16_t nameLength  = ReadUInt16();
            RCString name;
            ReadUString(nameLength, name);
            if (ReadUInt16() != 0)
            {
                return RC_S_FALSE;
            }

            if (!RCStringConvert::ConvertUnicodeToUTF8(name.c_str(), name.length(), section.m_name))
            {
                return RC_S_FALSE;
            }
            database.m_sections.push_back(section);
        }
    }

    int32_t i;
    for (i = 1; i < (int32_t)database.m_sections.size(); i++)
    {
        RCChmSectionInfo& section = database.m_sections[i];
        RCStringA sectionPrefix = GetSectionPrefix(section.m_name);

        {
            // Content
            int32_t index = database.FindItem(sectionPrefix + s_content);
            if (index < 0)
            {
                return RC_S_FALSE;
            }

            const RCChmItem &item = database.m_items[index];
            section.m_offset = item.m_offset;
            section.m_compressedSize = item.m_size;
        }

        RCStringA transformPrefix = sectionPrefix + s_transform;
        if (database.m_help2Format)
        {
            // Transform List
            hr = DecompressStream(inStream, database, transformPrefix + s_transformList);
            if (hr != RC_S_OK)
            {
                return hr;
            }

            if ((m_chunkSize & 0xF) != 0)
            {
                return RC_S_FALSE;
            }

            int32_t numGuids = (int32_t)(m_chunkSize / 0x10);
            if (numGuids < 1)
            {
                return RC_S_FALSE;
            }

            for (int32_t i = 0; i < numGuids; i++)
            {
                RCChmMethodInfo method;
                ReadGUID(method.m_guid);
                section.m_methods.push_back(method);
            }
        }
        else
        {
            RCChmMethodInfo method;
            method.m_guid = RCChmDefs::s_chmLzxGuid;
            section.m_methods.push_back(method);
        }

        {
            // Control Data
            hr = DecompressStream(inStream, database, sectionPrefix + s_controlData);
            if (hr != RC_S_OK)
            {
                return hr;
            }

            for (int32_t mi = 0; mi < (int32_t)section.m_methods.size(); mi++)
            {
                RCChmMethodInfo& method = section.m_methods[mi];
                uint32_t numDWORDS = ReadUInt32();

                if (method.IsLzx())
                {
                    if (numDWORDS < 5)
                    {
                        return RC_S_FALSE;
                    }

                    if (ReadUInt32() != RCChmHeader::kLzxcSignature)
                    {
                        return RC_S_FALSE;
                    }

                    RCChmLzxInfo &li = method.m_lzxInfo;
                    li.m_version = ReadUInt32();
                    if (li.m_version != 2 && li.m_version != 3)
                    {
                        return RC_S_FALSE;
                    }

                    li.m_resetInterval = ReadUInt32();
                    li.m_windowSize = ReadUInt32();
                    li.m_cacheSize = ReadUInt32();
                    if (
                        li.m_resetInterval != 1 &&
                        li.m_resetInterval != 2 &&
                        li.m_resetInterval != 4 &&
                        li.m_resetInterval != 8 &&
                        li.m_resetInterval != 16 &&
                        li.m_resetInterval != 32 &&
                        li.m_resetInterval != 64)
                    {
                        return RC_S_FALSE;
                    }

                    if (
                        li.m_windowSize != 1 &&
                        li.m_windowSize != 2 &&
                        li.m_windowSize != 4 &&
                        li.m_windowSize != 8 &&
                        li.m_windowSize != 16 &&
                        li.m_windowSize != 32 &&
                        li.m_windowSize != 64)
                    {
                        return RC_S_FALSE;
                    }

                    numDWORDS -= 5;
                    while (numDWORDS-- != 0)
                    {
                        ReadUInt32();
                    }
                }
                else
                {
                    uint32_t numBytes = numDWORDS * 4;
                    method.m_controlData.SetCapacity(numBytes);
                    ReadBytes(method.m_controlData.data(), numBytes);
                }
            }
        }

        {
            // SpanInfo
            hr = DecompressStream(inStream, database, sectionPrefix + s_spanInfo);
            if (hr != RC_S_OK)
            {
                return hr;
            }

            section.m_uncompressedSize = ReadUInt64();
        }

        // read ResetTable for LZX
        for (int32_t mi = 0; mi < (int32_t)section.m_methods.size(); mi++)
        {
            RCChmMethodInfo& method = section.m_methods[mi];
            if (method.IsLzx())
            {
                // ResetTable;
                hr = DecompressStream(inStream, 
                                      database,
                                      transformPrefix + method.GetGuidString() + s_resetTable);
                if (hr != RC_S_OK)
                {
                    return hr;
                }

                RCChmResetTable& rt = method.m_lzxInfo.m_resetTable;
                if (m_chunkSize < 4)
                {
                    if (m_chunkSize != 0)
                    {
                        return RC_S_FALSE;
                    }
                    // ResetTable is empty in .chw files
                    if (section.m_uncompressedSize != 0)
                    {
                        return RC_S_FALSE;
                    }

                    rt.m_uncompressedSize = 0;
                    rt.m_compressedSize = 0;
                    rt.m_blockSize = 0;
                }
                else
                {
                    uint32_t ver = ReadUInt32(); // 2  unknown (possibly a version number)
                    if (ver != 2 && ver != 3)
                    {
                        return RC_S_FALSE;
                    }

                    uint32_t numEntries = ReadUInt32();
                    if (ReadUInt32() != 8) // Size of table entry (bytes)
                    {
                        return RC_S_FALSE;
                    }

                    if (ReadUInt32() != 0x28) // Length of table header
                    {
                        return RC_S_FALSE;
                    }

                    rt.m_uncompressedSize = ReadUInt64();
                    rt.m_compressedSize = ReadUInt64();
                    rt.m_blockSize = ReadUInt64(); //  0x8000 block size for locations below
                    if (rt.m_blockSize != 0x8000)
                    {
                        return RC_S_FALSE;
                    }

                    for (uint32_t i = 0; i < numEntries; i++)
                    {
                        rt.m_resetOffsets.push_back(ReadUInt64());
                    }
                }
            }
        }
    }

    database.SetIndices();
    database.Sort();

    return database.Check() ? RC_S_OK : RC_S_FALSE;
}

HResult RCChmIn::Open2(IInStream* inStream,
                       const uint64_t* searchHeaderSizeLimit,
                       RCChmFilesDatabase& database)
{
    database.Clear();

    HResult hr = inStream->Seek(0, RC_STREAM_SEEK_CUR, &m_startPosition);
    if (hr != RC_S_OK)
    {
        return hr;
    }

    database.m_help2Format = false;
    const uint32_t chmVersion = 3;
    {
        if (!m_inBuffer.Create(1 << 14))
        {
            return RC_E_OUTOFMEMORY;
        }

        m_inBuffer.SetStream(inStream);
        m_inBuffer.Init();
        uint64_t value = 0;
        const int32_t kSignatureSize = 8;
        uint64_t hxsSignature = RCChmHeader::GetHxsSignature();
        uint64_t chmSignature = ((uint64_t)chmVersion << 32)| RCChmHeader::kItsfSignature;
        uint64_t limit = 1 << 18;
        if (searchHeaderSizeLimit)
        {
            if (limit > *searchHeaderSizeLimit)
            {
                limit = *searchHeaderSizeLimit;
            }
        }

        for (;;)
        {
            byte_t b;
            if (!m_inBuffer.ReadByte(b))
            {
                return RC_S_FALSE;
            }

            value >>= 8;
            value |= ((uint64_t)b) << ((kSignatureSize - 1) * 8);
            if (m_inBuffer.GetProcessedSize() >= kSignatureSize)
            {
                if (value == chmSignature)
                {
                    break;
                }
                if (value == hxsSignature)
                {
                    database.m_help2Format = true;
                    break;
                }
                if (m_inBuffer.GetProcessedSize() > limit)
                {
                    return RC_S_FALSE;
                }
            }
        }
        m_startPosition += m_inBuffer.GetProcessedSize() - kSignatureSize;
    }

    if (database.m_help2Format)
    {
        hr = OpenHelp2(inStream, database);
        if (hr != RC_S_OK)
        {
            return hr;
        }

        if (database.m_newFormat)
        {
            return RC_S_OK;
        }
    }
    else
    {
        hr = OpenChm(inStream, database);
        if (hr != RC_S_OK)
        {
            return hr;
        }
    }

#ifndef CHM_LOW
    hr = OpenHighLevel(inStream, database);
    if (hr == RC_S_FALSE)
    {
        database.HighLevelClear();
        return RC_S_OK;
    }

    if (hr != RC_S_OK)
    {
        return hr;
    }

    database.m_lowLevel = false;
#endif

    return RC_S_OK;
}

HResult RCChmIn::Open(IInStream* inStream,
                      const uint64_t* searchHeaderSizeLimit,
                      RCChmFilesDatabase& database)
{

    HResult hr = Open2(inStream, searchHeaderSizeLimit, database);
    m_inBuffer.ReleaseStream();
    return hr;
}

END_NAMESPACE_RCZIP
