/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#include "format/udf/RCUdfIn.h"
#include "filesystem/RCStreamUtils.h"
#include "common/RCStringUtil.h"
#include "algorithm/CpuArch.h"

#define Get16(p) GetUi16(p)
#define Get32(p) GetUi32(p)
#define Get64(p) GetUi64(p)

BEGIN_NAMESPACE_RCZIP

RCString RCUdfLogVol::GetName() const 
{ 
    return m_id.GetString(); 
}

void RCUdfInArchive::UpdateWithName(RCString &res, const RCString &addString)
{
    if (res.empty())
    {
        res = addString;
    }
    else
    {
        res = addString + WCHAR_PATH_SEPARATOR + res;
    }
}

RCString RCUdfInArchive::GetSpecName(const RCString &name)
{
    RCString name2 = name;
    RCStringUtil::Trim(name2);
    if (name2.empty())
    {
        return _T("[]");
    }
    return name;
}

HResult RCUdfInArchive::Open(IInStream *inStream, RCUdfProgressVirt *progress)
{
    m_progress = progress;
    m_stream = inStream;
    HResult res;
    try { 
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

void RCUdfInArchive::Clear()
{
    m_partitions.clear();
    m_logVols.clear();
    m_items.clear();
    m_files.clear();
    m_fileNameLengthTotal = 0;
    m_numRefs = 0;
    m_numExtents = 0;
    m_inlineExtentsSize = 0;
    m_processedProgressBytes = 0;
}

RCString RCUdfInArchive::GetComment() const
{
    RCString res;
    for (int32_t i = 0; i < static_cast<int32_t>(m_logVols.size()); i++)
    {
        if (i > 0)
        {
            res += _T(" ");
        }
        res += m_logVols[i].GetName();
    }
    return res;
}

RCString RCUdfInArchive::GetItemPath(int32_t volIndex, 
                                     int32_t fsIndex, 
                                     int32_t refIndex, 
                                     bool showVolName, 
                                     bool showFsName) const
{
    const RCUdfLogVol &vol = m_logVols[volIndex];
    const RCUdfFileSet &fs = vol.m_fileSets[fsIndex];
    RCString name;
    for (;;)
    {
        const RCUdfRef &ref = fs.m_refs[refIndex];
        refIndex = ref.m_parent;
        if (refIndex < 0)
        {
            break;
        }
        RCUdfInArchive::UpdateWithName(name, GetSpecName(m_files[ref.m_fileIndex].GetName()));
    }

    if (showFsName)
    {
        char_t s[32];
        RCStringUtil::ConvertUInt64ToString(fsIndex, s);
        RCString newName = _T("File Set ");
        newName += s;
        UpdateWithName(name, newName);
    }

    if (showVolName)
    {
        char_t s[32];
        RCStringUtil::ConvertUInt64ToString(volIndex, s);
        RCString newName = s;
        RCString newName2 = vol.GetName();
        if (newName2.empty())
        {
            newName2 = _T("Volume");
        }
        newName += _T('-');
        newName += newName2;
        UpdateWithName(name, newName);
    }
    return name;
}

bool RCUdfInArchive::CheckItemExtents(int32_t volIndex, const RCUdfItem &item) const
{
    for (int32_t i = 0; i < static_cast<int32_t>(item.m_extents.size()); i++)
    {
        const RCUdfMyExtent &e = item.m_extents[i];
        if (!CheckExtent(volIndex, e.m_partitionRef, e.m_pos, e.GetLen()))
        {
            return false;
        }
    }
    return true;
}

HResult RCUdfInArchive::Read(int32_t volIndex, int32_t partitionRef, uint32_t blockPos, uint32_t len, byte_t *buf)
{
    HResult result;
    if (!CheckExtent(volIndex, partitionRef, blockPos, len))
    {
        return RC_S_FALSE;
    }
    const RCUdfLogVol &vol = m_logVols[volIndex];
    const RCUdfPartition &partition = m_partitions[vol.m_partitionMaps[partitionRef].m_partitionIndex];
    result = m_stream->Seek(((uint64_t)partition.m_pos << m_secLogSize) + 
        (uint64_t)blockPos * vol.m_blockSize, RC_STREAM_SEEK_SET, NULL);
    if (!IsSuccess(result))
    {
        return result;
    }
    return RCStreamUtils::ReadStream_FALSE(m_stream.Get(), buf, len);
}

HResult RCUdfInArchive::Read(int32_t volIndex, const RCUdfLongAllocDesc &lad, byte_t *buf)
{
    return Read(volIndex, lad.m_location.m_partitionRef, lad.m_location.m_pos, lad.GetLen(), (byte_t *)buf);
}

HResult RCUdfInArchive::ReadFromFile(int32_t volIndex, const RCUdfItem &item, RCByteBuffer &buf)
{
    HResult result;
    if (item.m_size >= (uint32_t)1 << 30)
    {
        return RC_S_FALSE;
    }
    if (item.m_isInline)
    {
        buf = item.m_inlineData;
        return RC_S_OK;
    }
    buf.SetCapacity((size_t)item.m_size);
    size_t pos = 0;
    for (int32_t i = 0; i < static_cast<int32_t>(item.m_extents.size()); i++)
    {
        const RCUdfMyExtent &e = item.m_extents[i];
        uint32_t len = e.GetLen();
        result = Read(volIndex, e.m_partitionRef, e.m_pos, len, (byte_t *)buf.data() + pos);
        if (!IsSuccess(result))
        {
            return result;
        }
        pos += len;
    }
    return RC_S_OK;
}

HResult RCUdfInArchive::ReadFileItem(int32_t volIndex, int32_t fsIndex, const RCUdfLongAllocDesc &lad, int32_t numRecurseAllowed)
{
    HResult result;
    if (m_files.size() % 100 == 0)
    {
        result = m_progress->SetCompleted(m_files.size(), m_processedProgressBytes);
        if (!IsSuccess(result))
        {
            return result;
        }
    }
    if (numRecurseAllowed-- == 0)
    {
        return RC_S_FALSE;
    }
    RCUdfFile &file = m_files.back();
    const RCUdfLogVol &vol = m_logVols[volIndex];
    RCUdfPartition &partition = m_partitions[vol.m_partitionMaps[lad.m_location.m_partitionRef].m_partitionIndex];

    uint32_t key = lad.m_location.m_pos;
    uint32_t value;
    const uint32_t kRecursedErrorValue = (uint32_t)(int32_t)-1;
    if (partition.m_map->Find(key, value))
    {
        if (value == kRecursedErrorValue)
        {
            return RC_S_FALSE;
        }
        file.m_itemIndex = value;
    }
    else
    {
        value = static_cast<uint32_t>(m_items.size());
        file.m_itemIndex = (int32_t)value;
        if (partition.m_map->Set(key, kRecursedErrorValue))
        {
            return RC_S_FALSE;
        }
        result = ReadItem(volIndex, fsIndex, lad, numRecurseAllowed);
        if (!IsSuccess(result))
        {
            return result;
        }
        if (!partition.m_map->Set(key, value))
        {
            return RC_S_FALSE;
        }
    }
    return RC_S_OK;
}

HResult RCUdfInArchive::ReadItem(int32_t volIndex, int32_t fsIndex, const RCUdfLongAllocDesc &lad, int32_t numRecurseAllowed)
{
    HResult result;
    if (m_items.size() > RCUdfDefs::s_numItemsMax)
    {
        return RC_S_FALSE;
    }
    //m_items.push_back(RCUdfItem());
    //RCUdfItem &item = m_items.back();
    m_items.push_back(RCUdfItemPtr(new RCUdfItem));
    RCUdfItemPtr item = m_items.back();

    const RCUdfLogVol &vol = m_logVols[volIndex];

    if (lad.GetLen() != vol.m_blockSize)
    {
        return RC_S_FALSE;
    }

    RCByteBuffer buf;
    size_t size = lad.GetLen();
    buf.SetCapacity(size);
    result = Read(volIndex, lad, buf.data());
    if (!IsSuccess(result))
    {
        return result;
    }
    RCUdfTag tag;
    const byte_t *p = buf.data();
    result = tag.Parse(p, size);
    if (!IsSuccess(result))
    {
        return result;
    }

    if (tag.m_id != RCUdfDefs::DESC_TYPE_FILE)
    {
        return RC_S_FALSE;
    }

    item->m_icbTag.Parse(p + 16);
    if (item->m_icbTag.m_fileType != RCUdfDefs::ICB_FILE_TYPE_DIR &&
        item->m_icbTag.m_fileType != RCUdfDefs::ICB_FILE_TYPE_FILE)
    {
        return RC_S_FALSE;
    }

    item->Parse(p);

    m_processedProgressBytes += (uint64_t)item->m_numLogBlockRecorded * vol.m_blockSize + size;

    uint32_t extendedAttrLen = Get32(p + 168);
    uint32_t allocDescriptorsLen = Get32(p + 172);

    if ((extendedAttrLen & 3) != 0)
    {
        return RC_S_FALSE;
    }
    int32_t pos = 176;
    if (extendedAttrLen > size - pos)
    {
        return RC_S_FALSE;
    }
    pos += extendedAttrLen;
    int32_t desctType = item->m_icbTag.GetDescriptorType();
    if (allocDescriptorsLen > size - pos)
    {
        return RC_S_FALSE;
    }
    if (desctType == RCUdfDefs::ICB_DESC_TYPE_INLINE)
    {
        item->m_isInline = true;
        item->m_inlineData.SetCapacity(allocDescriptorsLen);
        memcpy(item->m_inlineData.data(), p + pos, allocDescriptorsLen);
    }
    else
    {
        item->m_isInline = false;
        if (desctType != RCUdfDefs::ICB_DESC_TYPE_SHORT && desctType != RCUdfDefs::ICB_DESC_TYPE_LONG)
        {
            return RC_S_FALSE;
        }
        for (uint32_t i = 0; i < allocDescriptorsLen;)
        {
            RCUdfMyExtent e;
            if (desctType == RCUdfDefs::ICB_DESC_TYPE_SHORT)
            {
                if (i + 8 > allocDescriptorsLen)
                {
                    return RC_S_FALSE;
                }
                RCUdfShortAllocDesc sad;
                sad.Parse(p + pos + i);
                e.m_pos = sad.m_pos;
                e.m_len = sad.m_len;
                e.m_partitionRef = lad.m_location.m_partitionRef;
                i += 8;
            }
            else
            {
                if (i + 16 > allocDescriptorsLen)
                {
                    return RC_S_FALSE;
                }
                RCUdfLongAllocDesc ladNew;
                ladNew.Parse(p + pos + i);
                e.m_pos = ladNew.m_location.m_pos;
                e.m_partitionRef = ladNew.m_location.m_partitionRef;
                e.m_len = ladNew.m_len;
                i += 16;
            }
            item->m_extents.push_back(e);
        }
    }

    if (item->m_icbTag.IsDir())
    {
        if (!item->CheckChunkSizes() || !CheckItemExtents(volIndex, *item))
        {
            return RC_S_FALSE;
        }
        RCByteBuffer buf;
        result = ReadFromFile(volIndex, *item, buf);
        if (!IsSuccess(result))
        {
            return result;
        }
        item->m_size = 0;
        item->m_extents.clear();
        item->m_inlineData.Free();

        const byte_t *p = buf.data();
        size = buf.GetCapacity();
        size_t processedTotal = 0;
        for (; processedTotal < size;)
        {
            size_t processedCur;
            RCUdfFileId fileId;
            result = fileId.Parse(p + processedTotal, size - processedTotal, processedCur);
            if (!IsSuccess(result))
            {
                return result;
            }
            if (!fileId.IsItLinkParent())
            {
                RCUdfFile file;
                file.m_id = fileId.m_id;
                m_fileNameLengthTotal += file.m_id.m_data.GetCapacity();
                if (m_fileNameLengthTotal > RCUdfDefs::s_fileNameLengthTotalMax)
                {
                    return RC_S_FALSE;
                }

                item->m_subFiles.push_back(static_cast<int32_t>(m_files.size()));
                if (m_files.size() > RCUdfDefs::s_numFilesMax)
                {
                    return RC_S_FALSE;
                }
                m_files.push_back(file);
                result = ReadFileItem(volIndex, fsIndex, fileId.m_icb, numRecurseAllowed);
                if (!IsSuccess(result))
                {
                    return result;
                }
            }
            processedTotal += processedCur;
        }
    }
    else
    {
        if ((uint32_t)item->m_extents.size() > RCUdfDefs::s_numExtentsMax - m_numExtents)
        {
            return RC_S_FALSE;
        }
        m_numExtents += static_cast<uint32_t>(item->m_extents.size());

        if (item->m_inlineData.GetCapacity() > RCUdfDefs::s_inlineExtentsSizeMax - m_inlineExtentsSize)
        {
            return RC_S_FALSE;
        }
        m_inlineExtentsSize += item->m_inlineData.GetCapacity();
    }
    return RC_S_OK;
}

HResult RCUdfInArchive::Open2()
{
    HResult result;
    Clear();
    uint64_t fileSize;
    result = m_stream->Seek(0, RC_STREAM_SEEK_END, &fileSize);
    if (!IsSuccess(result))
    {
        return result;
    }
    // Some UDFs contain additional 2 KB of zeros, so we also check 12, corrected to 11.
    const int32_t kSecLogSizeMax = 12;
    byte_t buf[1 << kSecLogSizeMax];
    byte_t kSizesLog[] = { 11, 8, 12 };
    
    for (int32_t i = 0;; i++)
    {
        if (i == sizeof(kSizesLog) / sizeof(kSizesLog[0]))
        {
            return S_FALSE ;
        }
        m_secLogSize = kSizesLog[i] ;
    
        int32_t bufSize = 1 << m_secLogSize;
        if (bufSize > fileSize)
        {
            return RC_S_FALSE;
        }
        result = m_stream->Seek(-bufSize, RC_STREAM_SEEK_END, NULL);
        if (!IsSuccess(result))
        {
            return result;
        }
        result = RCStreamUtils::ReadStream_FALSE(m_stream.Get(), buf, bufSize);
        if (!IsSuccess(result))
        {
            return result;
        }
        RCUdfTag tag;
        if (tag.Parse(buf, bufSize) == RC_S_OK)
        {
            if (tag.m_id == RCUdfDefs::DESC_TYPE_ANCHOR_VOL_PTR)
            {
                break;
            }
        }
    }
    if (m_secLogSize == 12)
    {
        m_secLogSize = 11 ;
    }

    RCUdfExtent extentVDS;
    extentVDS.Parse(buf + 16);

    for (uint32_t location = extentVDS.m_pos; ; location++)
    {
        size_t bufSize = 1 << m_secLogSize;
        size_t pos = 0;
        result = m_stream->Seek((uint64_t)location << m_secLogSize, RC_STREAM_SEEK_SET, NULL);
        if (!IsSuccess(result))
        {
            return result;
        }
        result = RCStreamUtils::ReadStream_FALSE(m_stream.Get(), buf, bufSize);
        if (!IsSuccess(result))
        {
            return result;
        }
        RCUdfTag tag;
        result = tag.Parse(buf + pos, bufSize - pos);
        if (!IsSuccess(result))
        {
            return result;
        }
        if (tag.m_id == RCUdfDefs::DESC_TYPE_TERMINATING)
        {
            break;
        }
        if (tag.m_id == RCUdfDefs::DESC_TYPE_PARTITION)
        {
            if (m_partitions.size() >= RCUdfDefs::s_numPartitionsMax)
            {
                return RC_S_FALSE;
            }
            RCUdfPartition partition;
            partition.m_number = Get16(buf + 22);
            partition.m_pos = Get32(buf + 188);
            partition.m_len = Get32(buf + 192);
            m_partitions.push_back(partition);
        }
        else if (tag.m_id == RCUdfDefs::DESC_TYPE_LOGICAL_VOL)
        {
            if (m_logVols.size() >= RCUdfDefs::s_numLogVolumesMax)
            {
                return RC_S_FALSE;
            }
            RCUdfLogVol vol;
            vol.m_id.Parse(buf + 84);
            vol.m_blockSize = Get32(buf + 212);
            if (vol.m_blockSize < 512 || vol.m_blockSize > ((uint32_t)1 << 30))
            {
                return RC_S_FALSE;
            }
            vol.m_fileSetLocation.Parse(buf + 248);
            uint32_t numPartitionMaps = Get32(buf + 268);
            if (numPartitionMaps > RCUdfDefs::s_numPartitionsMax)
            {
                return RC_S_FALSE;
            }
            size_t pos = 440;
            for (uint32_t i = 0; i < numPartitionMaps; i++)
            {
                if (pos + 2 > bufSize)
                {
                    return RC_S_FALSE;
                }
                RCUdfPartitionMap pm;
                pm.m_type = buf[pos];
                byte_t len = buf[pos + 1];
                if (pos + len > bufSize)
                {
                    return RC_S_FALSE;
                }
                if (pm.m_type == 1)
                {
                    if (pos + 6 > bufSize)
                    {
                        return RC_S_FALSE;
                    }
                    pm.m_partitionNumber = Get16(buf + pos + 4);
                }
                else
                {
                    return RC_S_FALSE;
                }
                pos += len;
                vol.m_partitionMaps.push_back(pm);
            }
            m_logVols.push_back(vol);
        }
    }
    uint64_t totalSize = 0;
    int32_t volIndex;
    for (volIndex = 0; volIndex < static_cast<int32_t>(m_logVols.size()); volIndex++)
    {
        RCUdfLogVol &vol = m_logVols[volIndex];
        for (int32_t pmIndex = 0; pmIndex < static_cast<int32_t>(vol.m_partitionMaps.size()); pmIndex++)
        {
            RCUdfPartitionMap &pm = vol.m_partitionMaps[pmIndex];
            int32_t i;
            for (i = 0; i < static_cast<int32_t>(m_partitions.size()); i++)
            {
                RCUdfPartition &part = m_partitions[i];
                if (part.m_number == pm.m_partitionNumber)
                {
                    if (part.m_volIndex >= 0)
                    {
                        return RC_S_FALSE;
                    }
                    pm.m_partitionIndex = i;
                    part.m_volIndex = volIndex;
                    totalSize += (uint64_t)part.m_len << m_secLogSize;
                    break;
                }
            }
            if (i == (int32_t)m_partitions.size())
            {
                return RC_S_FALSE;
            }
        }
    }

    result = m_progress->SetTotal(totalSize);
    if (!IsSuccess(result))
    {
        return result;
    }

    for (volIndex = 0; volIndex < static_cast<int32_t>(m_logVols.size()); volIndex++)
    {
        RCUdfLogVol &vol = m_logVols[volIndex];

        RCUdfLongAllocDesc nextExtent = vol.m_fileSetLocation;
        {
            if (nextExtent.GetLen() < 512)
            {
                return RC_S_FALSE;
            }
            RCByteBuffer buf;
            buf.SetCapacity(nextExtent.GetLen());
            result = Read(volIndex, nextExtent, buf.data());
            if (!IsSuccess(result))
            {
                return result;
            }
            const byte_t *p = buf.data();
            size_t size = nextExtent.GetLen();
            RCUdfTag tag;
            result = tag.Parse(p, size);
            if (!IsSuccess(result))
            {
                return result;
            }
            if (tag.m_id != RCUdfDefs::DESC_TYPE_FILESET)
            {
                return RC_S_FALSE;
            }

            RCUdfFileSet fs;
            fs.m_recodringTime.Parse(p + 16);
            fs.m_rootDirICB.Parse(p + 400);
            vol.m_fileSets.push_back(fs);
        }

        for (int32_t fsIndex = 0; fsIndex < static_cast<int32_t>(vol.m_fileSets.size()); fsIndex++)
        {
            RCUdfFileSet &fs = vol.m_fileSets[fsIndex];
            int32_t fileIndex = static_cast<int32_t>(m_files.size());
            m_files.push_back(RCUdfFile());
            result = ReadFileItem(volIndex, fsIndex, fs.m_rootDirICB, RCUdfDefs::s_numRecureseLevelsMax);
            if (!IsSuccess(result))
            {
                return result;
            }
            result = FillRefs(fs, fileIndex, -1, RCUdfDefs::s_numRecureseLevelsMax);
            if (!IsSuccess(result))
            {
                return result;
            }
        }
    }
    return RC_S_OK;
}

HResult RCUdfInArchive::FillRefs(RCUdfFileSet &fs, int32_t fileIndex, int32_t parent, int32_t numRecurseAllowed)
{
    HResult result;
    if (m_numRefs % 10000 == 0)
    {
        result = m_progress->SetCompleted();
        if (!IsSuccess(result))
        {
            return result;
        }
    }
    if (numRecurseAllowed-- == 0)
    {
        return RC_S_FALSE;
    }
    if (m_numRefs >= RCUdfDefs::s_numRefsMax)
    {
        return RC_S_FALSE;
    }
    m_numRefs++;
    RCUdfRef ref;
    ref.m_fileIndex = fileIndex;
    ref.m_parent = parent;
    parent = static_cast<int32_t>(fs.m_refs.size());
    fs.m_refs.push_back(ref);
    const RCUdfItemPtr item = m_items[m_files[fileIndex].m_itemIndex];
    for (int32_t i = 0; i < static_cast<int32_t>(item->m_subFiles.size()); i++)
    {
        result = FillRefs(fs, item->m_subFiles[i], parent, numRecurseAllowed);
        if (!IsSuccess(result))
        {
            return result;
        }
    }
    return RC_S_OK;
}

bool RCUdfInArchive::CheckExtent(int32_t volIndex, int32_t partitionRef, uint32_t blockPos, uint32_t len) const
{
    const RCUdfLogVol &vol = m_logVols[volIndex];
    const RCUdfPartition &partition = m_partitions[vol.m_partitionMaps[partitionRef].m_partitionIndex];
    uint64_t offset = ((uint64_t)partition.m_pos << m_secLogSize) + (uint64_t)blockPos * vol.m_blockSize;
    return (offset + len) <= (((uint64_t)partition.m_pos + partition.m_len) << m_secLogSize);
}

END_NAMESPACE_RCZIP
