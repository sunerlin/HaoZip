/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#include "format/cab/RCCabIn.h"
#include "format/common/RCFindSignature.h"
#include <algorithm>
#include <functional>

BEGIN_NAMESPACE_RCZIP

byte_t RCCabInArchive::s_marker[RCCabHeaderDefs::kMarkerSize] = {'M', 'S', 'C', 'F', 0, 0, 0, 0 };

bool RCCabArchiveFileInfo::ReserveBlockPresent() const 
{ 
    return (m_flags & RCCabHeaderDefs::kReservePresent) != 0; 
}

bool RCCabArchiveFileInfo::IsTherePrev() const 
{ 
    return (m_flags & RCCabHeaderDefs::kPrevCabinet) != 0; 
}

bool RCCabArchiveFileInfo::IsThereNext() const 
{ 
    return (m_flags & RCCabHeaderDefs::kNextCabinet) != 0; 
}

byte_t RCCabArchiveFileInfo::GetDataBlockReserveSize() const 
{ 
    return static_cast<byte_t>(ReserveBlockPresent() ? m_perDataBlockAreaSize : 0); 
}

void RCCabArchiveFileInfo::Clear()
{
    m_perCabinetAreaSize = 0;
    m_perFolderAreaSize = 0;
    m_perDataBlockAreaSize = 0;
}

RCCabArchiveFileInfo::RCCabArchiveFileInfo()
{
    Clear();
}

void RCCabDatabase::Clear()
{
    m_archiveInfo.Clear();
    m_folders.clear();
    m_items.clear();
}

bool RCCabDatabase::IsTherePrevFolder() const
{
    for (int32_t i = 0; i < static_cast<int32_t>(m_items.size()); i++)
    {
        if (m_items[i].ContinuedFromPrev())
        {
            return true;
        }
    }
    return false;
}

int32_t RCCabDatabase::GetNumberOfNewFolders() const
{
    int32_t res = static_cast<int32_t>(m_folders.size());
    if (IsTherePrevFolder())
    {
        res--;
    }
    return res;
}

uint32_t RCCabDatabase::GetFileOffset(int32_t index) const 
{ 
    return m_items[index].m_offset; 
}

uint32_t RCCabDatabase::GetFileSize(int32_t index) const 
{ 
    return m_items[index].m_size; 
}

byte_t RCCabInArchive::Read8()
{
    byte_t b;
    if (!m_inBuffer.ReadByte(b))
    {
        _ThrowCode(RC_E_ReadStreamError) ;
    }
    return b;
}

uint16_t RCCabInArchive::Read16()
{
    uint16_t value = 0;
    for (int32_t i = 0; i < 2; i++)
    {
        byte_t b = Read8();
        value |= (uint16_t(b) << (8 * i));
    }
    return value;
}

uint32_t RCCabInArchive::Read32()
{
    uint32_t value = 0;
    for (int32_t i = 0; i < 4; i++)
    {
        byte_t b = Read8();
        value |= (uint32_t(b) << (8 * i));
    }
    return value;
}

RCStringA RCCabInArchive::SafeReadName()
{
    RCStringA name;
    for(;;)
    {
        byte_t b = Read8();
        if (b == 0)
        {
            return name;
        }
        name += static_cast<char>(b);
    }
}

void RCCabInArchive::Skip(uint32_t size)
{
    while(size-- != 0)
    {
        Read8();
    }
}

void RCCabInArchive::ReadOtherArchive(RCCabOtherArchive &oa)
{
    oa.m_fileName = SafeReadName();
    oa.m_diskName = SafeReadName();
}

HResult RCCabInArchive::Open( const uint64_t* searchHeaderSizeLimit, 
                              RCCabDatabaseEx& db)
{
    IInStream* stream = db.m_stream.Get() ;
    db.Clear() ;   
    HResult result = stream->Seek(0, RC_STREAM_SEEK_SET, &db.m_startPosition);
    if (!IsSuccess(result))
    {
        return result;
    }

    result = RCFindSignature::FindSignatureInStream(stream, 
                                                    RCCabInArchive::s_marker,
                                                    RCCabHeaderDefs::kMarkerSize,
                                                    searchHeaderSizeLimit, 
                                                    db.m_startPosition);
    if (!IsSuccess(result))
    {
        return result;
    }

    result = stream->Seek(db.m_startPosition + RCCabHeaderDefs::kMarkerSize, RC_STREAM_SEEK_SET, NULL);
    if (!IsSuccess(result))
    {
        return result;
    }

    if (!m_inBuffer.Create(1 << 17))
    {
        return RC_E_OUTOFMEMORY;
    }
    m_inBuffer.SetStream(stream);
    m_inBuffer.Init();

    RCCabInArchiveInfo& archiveInfo = db.m_archiveInfo;
    archiveInfo.m_size = Read32();
    if (Read32() != 0)
    {
        return RC_S_FALSE;
    }
    archiveInfo.m_fileHeadersOffset = Read32();
    if (Read32() != 0)
    {
        return RC_S_FALSE;
    }

    archiveInfo.m_versionMinor = Read8();
    archiveInfo.m_versionMajor = Read8();
    archiveInfo.m_numFolders = Read16();
    archiveInfo.m_numFiles  = Read16();
    archiveInfo.m_flags = Read16();

    if (archiveInfo.m_flags > 7)
    {
        return RC_S_FALSE;
    }

    archiveInfo.m_setID = Read16();
    archiveInfo.m_cabinetNumber = Read16();

    if (archiveInfo.ReserveBlockPresent())
    {
        archiveInfo.m_perCabinetAreaSize = Read16();
        archiveInfo.m_perFolderAreaSize = Read8();
        archiveInfo.m_perDataBlockAreaSize = Read8();
        Skip(archiveInfo.m_perCabinetAreaSize);
    }

    if (archiveInfo.IsTherePrev())
    {
        ReadOtherArchive(archiveInfo.m_prevArc);
    }
    if (archiveInfo.IsThereNext())
    {
        ReadOtherArchive(archiveInfo.m_nextArc);
    }

    int32_t i;
    for(i = 0; i < archiveInfo.m_numFolders; i++)
    {
        RCCabFolder folder;
        folder.m_dataStart = Read32();
        folder.m_numDataBlocks = Read16();
        folder.m_compressionTypeMajor = Read8();
        folder.m_compressionTypeMinor = Read8();
        Skip(archiveInfo.m_perFolderAreaSize);
        db.m_folders.push_back(folder);
    }

    result = stream->Seek(db.m_startPosition + archiveInfo.m_fileHeadersOffset, RC_STREAM_SEEK_SET, NULL);
    if (!IsSuccess(result))
    {
        return result;
    }

    m_inBuffer.SetStream(stream);
    m_inBuffer.Init();

    for(i = 0; i < archiveInfo.m_numFiles; i++)
    {
        RCCabItem item;
        item.m_size = Read32();
        item.m_offset = Read32();
        item.m_folderIndex = Read16();
        uint16_t pureDate = Read16();
        uint16_t pureTime = Read16();
        item.m_time = ((uint32_t(pureDate) << 16)) | pureTime;
        item.m_attributes = Read16();
        item.m_name = SafeReadName();
        int32_t folderIndex = item.GetFolderIndex(static_cast<int32_t>(db.m_folders.size()));
        if (folderIndex >= static_cast<int32_t>(db.m_folders.size()))
        {
            return RC_S_FALSE;
        }
        db.m_items.push_back(item);
    }
    return RC_S_OK;
}

int32_t RCCabInArchive::CompareMvItems(const RCCabMvItem *p1, const RCCabMvItem *p2, void *param)
{
    const RCCabMvDatabaseEx &mvDb = *static_cast<const RCCabMvDatabaseEx *>(param);
    const RCCabDatabaseEx &db1 = mvDb.m_volumes[p1->m_volumeIndex];
    const RCCabDatabaseEx &db2 = mvDb.m_volumes[p2->m_volumeIndex];
    const RCCabItem &item1 = db1.m_items[p1->m_itemIndex];
    const RCCabItem &item2 = db2.m_items[p2->m_itemIndex];;
    bool isDir1 = item1.IsDir();
    bool isDir2 = item2.IsDir();
    if (isDir1 && !isDir2)
    {
        return -1;
    }
    if (isDir2 && !isDir1)
    {
        return 1;
    }
    int32_t f1 = mvDb.GetFolderIndex(p1);
    int32_t f2 = mvDb.GetFolderIndex(p2);
    int32_t result;
    result = MyCompare(f1, f2);
    if (result != 0)
    {
        return result;
    }

    result = MyCompare(item1.m_offset, item2.m_offset);
    if (result != 0)
    {
        return result;
    }

    result = MyCompare(item1.m_size, item2.m_size);
    if (result != 0)
    {
        return result;
    }
    result = MyCompare(p1->m_volumeIndex, p2->m_volumeIndex) ;
    if (result != 0)
    {
        return result;
    }
    return MyCompare(p1->m_itemIndex, p2->m_itemIndex) ;
}

bool RCCabMvDatabaseEx::AreItemsEqual(int32_t i1, int32_t i2)
{
    const RCCabMvItem *p1 = &m_items[i1];
    const RCCabMvItem *p2 = &m_items[i2];
    const RCCabDatabaseEx &db1 = m_volumes[p1->m_volumeIndex];
    const RCCabDatabaseEx &db2 = m_volumes[p2->m_volumeIndex];
    const RCCabItem &item1 = db1.m_items[p1->m_itemIndex];
    const RCCabItem &item2 = db2.m_items[p2->m_itemIndex];;
    int32_t f1 = GetFolderIndex(p1);
    int32_t f2 = GetFolderIndex(p2);
    if (f1 != f2)
    {
        return false;
    }
    if (item1.m_offset != item2.m_offset)
    {
        return false;
    }
    if (item1.m_size != item2.m_size)
    {
        return false;
    }
    if (item1.m_name != item2.m_name)
    {
        return false;
    }
    return true;
}

class RCCabMvItemCompare :
    public std::binary_function<const RCCabMvItem& ,const RCCabMvItem& ,bool>
{
public:

    /** 构造函数
    @param [in] databaseEx cabMv基本数据
    */
    RCCabMvItemCompare(RCCabMvDatabaseEx* databaseEx) :
      m_mvDatabaseEx(databaseEx)
    {
    }

    /** 重载()操作符
    @param [in] it1 cabMv项一
    @param [in] it2 cabMv项二
    @return 相等返回true，否则返回false
    */
    bool operator()(const RCCabMvItem& it1, const RCCabMvItem& it2)
    {
        return (RCCabInArchive::CompareMvItems(&it1, &it2, static_cast<void*>(m_mvDatabaseEx)) < 0);
    }

private:

    /** cabMv基本数据
    */
    RCCabMvDatabaseEx* m_mvDatabaseEx;
};

int32_t RCCabMvDatabaseEx::GetFolderIndex(const RCCabMvItem *mvi) const
{
    const RCCabDatabaseEx &db = m_volumes[mvi->m_volumeIndex];
    return m_startFolderOfVol[mvi->m_volumeIndex] +
        db.m_items[mvi->m_itemIndex].GetFolderIndex(static_cast<int32_t>(db.m_folders.size()));
}

void RCCabMvDatabaseEx::Clear()
{
    m_volumes.clear();
    m_items.clear();
    m_startFolderOfVol.clear();
    m_folderStartFileIndex.clear();
}

void RCCabMvDatabaseEx::FillSortAndShrink()
{
    m_items.clear();
    m_startFolderOfVol.clear();
    m_folderStartFileIndex.clear();
    int32_t offset = 0;
    for (int32_t v = 0; v < static_cast<int32_t>(m_volumes.size()); v++)
    {
        const RCCabDatabaseEx &db = m_volumes[v];
        int32_t curOffset = offset;
        if (db.IsTherePrevFolder())
        curOffset--;
        m_startFolderOfVol.push_back(curOffset);
        offset += db.GetNumberOfNewFolders();

        RCCabMvItem mvItem;
        mvItem.m_volumeIndex = v;
        for (int32_t i = 0 ; i < static_cast<int32_t>(db.m_items.size()); i++)
        {
            mvItem.m_itemIndex = i;
            m_items.push_back(mvItem);
        }
    }

    //Items.Sort(CompareMvItems, (void *)this);
    std::sort(m_items.begin(), m_items.end(), RCCabMvItemCompare(this));

    int32_t j = 1;
    int32_t i;

    for (i = 1; i < static_cast<int32_t>(m_items.size()); i++)
    {
        if (!AreItemsEqual(i, i -1))
        {
            m_items[j++] = m_items[i];
        }
    }

    //Items.DeleteFrom(j);
    RCVectorUtils::Delete(m_items, j, static_cast<int32_t>(m_items.size() - j));

    for (i = 0; i < static_cast<int32_t>(m_items.size()); i++)
    {
        int32_t folderIndex = GetFolderIndex(&m_items[i]);
        if (folderIndex >= static_cast<int32_t>(m_folderStartFileIndex.size()))
        {
            m_folderStartFileIndex.push_back(i);
        }
    }
}

bool RCCabMvDatabaseEx::Check()
{
    for (int32_t v = 1; v < static_cast<int32_t>(m_volumes.size()); v++)
    {
        const RCCabDatabaseEx &db1 = m_volumes[v];
        if (db1.IsTherePrevFolder())
        {
            const RCCabDatabaseEx &db0 = m_volumes[v - 1];
            if (db0.m_folders.empty() || db1.m_folders.empty())
            {
                return false;
            }
            const RCCabFolder &f0 = db0.m_folders.back();
            const RCCabFolder &f1 = db1.m_folders.front();
            if (f0.m_compressionTypeMajor != f1.m_compressionTypeMajor ||
                f0.m_compressionTypeMinor != f1.m_compressionTypeMinor)
            {
                return false;
            }
        }
    }

    uint32_t beginPos = 0 ;
    uint64_t endPos = 0 ;
    int32_t prevFolder = -2;
    for(int32_t i = 0; i < static_cast<int32_t>(m_items.size()); i++)
    {
        const RCCabMvItem &mvItem = m_items[i];
        int32_t fIndex = GetFolderIndex(&mvItem);
        if (fIndex >= static_cast<int32_t>(m_folderStartFileIndex.size()))
        {
            return false;
        }
        const RCCabItem &item = m_volumes[mvItem.m_volumeIndex].m_items[mvItem.m_itemIndex];
        if (item.IsDir())
        {
            continue;
        }
        int32_t folderIndex = GetFolderIndex(&mvItem);
        if (folderIndex != prevFolder)
        {
            prevFolder = folderIndex;
        }
        else if (item.m_offset < endPos &&
                (item.m_offset != beginPos || item.GetEndOffset() != endPos))
        {
            return false ;
        }
        beginPos = item.m_offset ;
        endPos = item.GetEndOffset() ;
    }
    return true;
}

END_NAMESPACE_RCZIP
