/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "format/hfs/RCHfsIn.h"
#include "filesystem/RCFileName.h"
#include "common/RCStringUtil.h"
#include "common/RCStringBuffer.h"
#include "filesystem/RCStreamUtils.h"
#include "algorithm/CpuArch.h"
#include <functional>

#define Get16(p) GetBe16(p)
#define Get32(p) GetBe32(p)
#define Get64(p) GetBe64(p)

/////////////////////////////////////////////////////////////////
//RCHfsIn class implementation

BEGIN_NAMESPACE_RCZIP

void RCHfsFork::Parse(const byte_t* p)
{
    m_size = Get64(p);
    m_numBlocks = Get32(p + 0xC);
    for (int32_t i = 0; i < 8; ++i)
    {
        RCHfsExtent& e = m_extents[i] ;
        e.m_pos = Get32(p + 0x10 + i * 8);
        e.m_numBlocks = Get32(p + 0x10 + i * 8 + 4);
    } 
}

bool RCHfsVolHeader::IsHfsX() const
{
    return m_version > 4 ;
}

RCHfsItem::RCHfsItem():
    m_size(0), 
    m_numBlocks(0)
{
}

bool RCHfsItem::IsDir() const
{
    return m_type == RECORD_TYPE_FOLDER;
}   


RCHfsDatabase::RCHfsDatabase()
{
}

RCHfsDatabase::~RCHfsDatabase()
{
}

void RCHfsDatabase::Clear()
{
    m_items.clear();
    m_idToIndexMap.clear();
}

class RCCompareIdToIndex:
    public std::binary_function<RCHfsIdIndexPair, RCHfsIdIndexPair ,bool>
{
public:

    /** 重载()操作符
    @param [in] p2 IdIndexPair一
    @param [in] p2 IdIndexPair二
    */
    bool operator()(const RCHfsIdIndexPair& p1, const RCHfsIdIndexPair& p2) const
    {
        if (p1.m_id < p2.m_id)
        {
            return true ;
        }
        else if (p1.m_id > p2.m_id)
        {
            return false ;
        }

        if (p1.m_index < p2.m_index)
        {
            return true ;
        }
        else
        {
            return false ;
        }
    }
};

bool operator< (const RCHfsIdIndexPair &a1, const RCHfsIdIndexPair &a2) { return (a1.m_id  < a2.m_id); }
bool operator> (const RCHfsIdIndexPair &a1, const RCHfsIdIndexPair &a2) { return (a1.m_id  > a2.m_id); }
bool operator==(const RCHfsIdIndexPair &a1, const RCHfsIdIndexPair &a2) { return (a1.m_id == a2.m_id); }
bool operator!=(const RCHfsIdIndexPair &a1, const RCHfsIdIndexPair &a2) { return (a1.m_id != a2.m_id); }

static RCString GetSpecName(const RCString& name, uint32_t /* id */)
{
    RCString name2 = name;
    RCStringUtil::Trim(name2);
    if (name2.empty())
    {
        return _T("[]") ;
    }
    return name;
}

static HResult ReadExtent(int32_t blockSizeLog, IInStream* inStream, byte_t* buf, const RCHfsExtent& e)
{
    HResult hr = inStream->Seek((uint64_t)e.m_pos << blockSizeLog, RC_STREAM_SEEK_SET, NULL) ;
    if(!IsSuccess(hr))
    {
        return hr ;
    }
    return RCStreamUtils::ReadStream_FALSE(inStream, buf, (size_t)e.m_numBlocks << blockSizeLog);
}

struct RCHfsNodeDescriptor
{
    /** 前节点
    */
    uint32_t m_fLink;

    /** 后节点
    */
    uint32_t m_bLink;

    /** 种类
    */
    byte_t m_kind;

    /** 高度
    */
    byte_t m_height;

    /** 记录个数
    */
    uint16_t m_numRecords;

    /** 解析
    @param [in] p 内存数据
    */
    void Parse(const byte_t *p) ;
};

void RCHfsNodeDescriptor::Parse(const byte_t* p)
{
    m_fLink = Get32(p);
    m_bLink = Get32(p + 4);
    m_kind = p[8];
    m_height = p[9];
    m_numRecords = Get16(p + 10);
}

struct RCHfsHeaderRec
{
    /** 第一个叶节点
    */
    uint32_t m_firstLeafNode;

    /** 节点大小
    */
    int32_t m_nodeSizeLog;

    /** 总节点数
    */
    uint32_t m_totalNodes;

    /** 解析
    @param [in] p 内存数据
    */
    HResult Parse(const byte_t* p);
};

HResult RCHfsHeaderRec::Parse(const byte_t* p)
{
    m_firstLeafNode = Get32(p + 0xA) ;
    uint32_t nodeSize = Get16(p + 0x12);

    int32_t i = 0 ;
    for (i = 9; ((uint32_t)1 << i) != nodeSize; i++)
    {
        if (i == 16)
        {
            return RC_S_FALSE;
        }
    }
    m_nodeSizeLog = i;
    m_totalNodes = Get32(p + 0x16);
    return RC_S_OK;
}

enum ENodeType
{
    NODE_TYPE_LEAF   = 0xFF,
    NODE_TYPE_INDEX  = 0,
    NODE_TYPE_HEADER = 1,
    NODE_TYPE_MODE   = 2
};

RCString RCHfsDatabase::GetItemPath(int32_t index) const
{
    if(index >= (int32_t)m_items.size())
    {
        return RCString() ;
    }
    const RCHfsItem* item = &m_items[index] ;
    RCString name = GetSpecName(item->m_name, item->m_id);
    for (int32_t i = 0; i < 1000; ++i)
    {
        if (item->m_parentID < 16 && item->m_parentID != 2)
        {
            if (item->m_parentID != 1)
            {
                break;
            }
            return name;
        }
        RCHfsIdIndexPair pair;
        pair.m_id = item->m_parentID;
        pair.m_index = 0;
        int32_t indexInMap = RCVectorUtils::FindInSorted(m_idToIndexMap,pair);
        if (indexInMap < 0)
        {
            break;
        }
        item = &m_items[m_idToIndexMap[indexInMap].m_index];
        name = GetSpecName(item->m_name, item->m_id) + RCFileName::GetDirDelimiter() + name ;
    }
    return RCString(_T("Unknown")) + RCFileName::GetDirDelimiter() + name ;
}

HResult RCHfsDatabase::Open(IInStream* inStream, RCHfsProgressVirt* progress)
{
    static const uint32_t kHeaderSize = 1024 + 512;
    byte_t buf[kHeaderSize];
    HResult hr = RCStreamUtils::ReadStream_FALSE(inStream, buf, kHeaderSize) ;
    if(!IsSuccess(hr))
    {
        return hr ;
    }
    int32_t i = 0 ;
    for (i = 0; i < 1024; i++)
    {
        if (buf[i] != 0)
        {
            return RC_S_FALSE;
        }
    }
    const byte_t* p = buf + 1024 ;
    RCHfsVolHeader& h = m_header ;

    h.m_header[0] = p[0];
    h.m_header[1] = p[1];
    if (p[0] != 'H' || (p[1] != '+' && p[1] != 'X'))
    {
        return RC_S_FALSE ;
    }
    h.m_version = Get16(p + 2);
    if (h.m_version < 4 || h.m_version > 5)
    {
        return RC_S_FALSE;
    }
    h.m_cTime = Get32(p + 0x10);
    h.m_mTime = Get32(p + 0x14);
    uint32_t numFiles = Get32(p + 0x20);
    uint32_t numFolders = Get32(p + 0x24);;
    if (progress)
    {
        hr = progress->SetTotal(numFolders + numFiles) ;
        if(!IsSuccess(hr))
        {
            return hr ;
        }
    }

    uint32_t blockSize = Get32(p + 0x28);

    for (i = 9; ((uint32_t)1 << i) != blockSize; i++)
    {
        if (i == 31)
        {
            return RC_S_FALSE ;
        }
    }
    h.m_blockSizeLog = i;

    h.m_numBlocks = Get32(p + 0x2C);
    h.m_numFreeBlocks = Get32(p + 0x30);

    uint64_t endPos = 0 ;
    hr = inStream->Seek(0, RC_STREAM_SEEK_END, &endPos) ;
    if(!IsSuccess(hr))
    {
        return hr ;
    }
    if ((endPos >> h.m_blockSizeLog) < h.m_numBlocks)
    {
        return RC_S_FALSE;
    }

    h.m_extentsFile.Parse( p + 0x70 + 0x50 * 1);
    h.m_catalogFile.Parse( p + 0x70 + 0x50 * 2);

    hr = LoadExtentFile(inStream) ;
    if(!IsSuccess(hr))
    {
        return hr ;
    }
    hr = LoadCatalog(inStream, progress) ;
    if(!IsSuccess(hr))
    {
        return hr ;
    }
    return RC_S_OK ;
}

HResult RCHfsDatabase::LoadExtentFile(IInStream* inStream)
{
    RCByteBuffer extents;
    HResult res = ReadFile(m_header.m_extentsFile, extents, inStream) ;
    if(!IsSuccess(res))
    {
        return res ;
    }
    const byte_t *p = (const byte_t*)extents.data() ;
    RCHfsHeaderRec hr ;
    res = hr.Parse(p + 14) ;
    if(!IsSuccess(res))
    {
        return res ;
    }
    uint32_t node = hr.m_firstLeafNode ;
    if (node != 0)
    {
        return RC_S_FALSE;
    }
    return RC_S_OK ;
}

HResult RCHfsDatabase::LoadCatalog(IInStream* inStream, RCHfsProgressVirt* progress) 
{
    m_items.clear();
    m_idToIndexMap.clear();

    RCByteBuffer catalogBuf;
    HResult res = ReadFile(m_header.m_catalogFile, catalogBuf, inStream) ;
    if(!IsSuccess(res))
    {
        return res ;
    }
    const byte_t* p = (const byte_t*)catalogBuf.data();
    RCHfsHeaderRec hr;
    hr.Parse(p + 14);
    if ((catalogBuf.GetCapacity() >> hr.m_nodeSizeLog) < hr.m_totalNodes)
    {
        return RC_S_FALSE;
    }

    RCByteBuffer usedBuf;
    usedBuf.SetCapacity(hr.m_totalNodes);
    for (uint32_t i = 0; i < hr.m_totalNodes; ++i)
    {
        usedBuf.data()[i] = 0;
    }

    uint32_t node = hr.m_firstLeafNode;
    while (node != 0)
    {
        if (node >= hr.m_totalNodes)
        {
            return RC_S_FALSE;
        }
        if (usedBuf[node])
        {
            return RC_S_FALSE;
        }
        usedBuf.data()[node] = 1;
        size_t nodeOffset = (size_t)node << hr.m_nodeSizeLog;
        RCHfsNodeDescriptor desc;
        desc.Parse(p + nodeOffset);
        if (desc.m_kind != NODE_TYPE_LEAF)
        {
            return RC_S_FALSE;
        }
        for (int32_t i = 0; i < desc.m_numRecords; ++i)
        {
            uint32_t nodeSize = (1 << hr.m_nodeSizeLog);
            uint32_t offs = Get16(p + nodeOffset + nodeSize - (i + 1) * 2);
            uint32_t offsNext = Get16(p + nodeOffset + nodeSize - (i + 2) * 2);
            uint32_t recSize = offsNext - offs;
            if (offsNext >= nodeSize || offsNext < offs || recSize < 6)
            {
                return RC_S_FALSE;
            }

            RCHfsItem item ;

            const byte_t *r = p + nodeOffset + offs;
            uint32_t keyLength = Get16(r);
            item.m_parentID = Get32(r + 2);
            RCString name;
            if (keyLength < 6 || (keyLength & 1) != 0 || keyLength + 2 > recSize)
            {
                return RC_S_FALSE;
            }
            r += 6;
            recSize -= 6;
            keyLength -= 6;

            int nameLength = Get16(r);
            if (nameLength * 2 != (int32_t)keyLength)
            {
                return RC_S_FALSE;
            }
            r += 2;
            recSize -= 2;
            RCStringBuffer nameBuffer(&name) ;
            RCString::value_type* pp = nameBuffer.GetBuffer(nameLength + 1);

            int32_t j = 0 ;
            for (j = 0; j < nameLength; j++)
            {
                pp[j] = ((RCString::value_type)r[j * 2] << 8) | r[j * 2 + 1];
            }
            pp[j] = 0;
            nameBuffer.ReleaseBuffer();
            r += j * 2;
            recSize -= j * 2;

            if (recSize < 2)
            {
                return RC_S_FALSE;
            }
            item.m_type = Get16(r);

            if (item.m_type != RECORD_TYPE_FOLDER && item.m_type != RECORD_TYPE_FILE)
            {
                continue;
            }
            if (recSize < 0x58)
            {
                return RC_S_FALSE;
            }
            item.m_id = Get32(r + 8);
            item.m_cTime = Get32(r + 0xC);
            item.m_mTime = Get32(r + 0x10);
            item.m_aTime = Get32(r + 0x18);
            item.m_name = name ;

            if (item.IsDir())
            {
                RCHfsIdIndexPair pair;
                pair.m_id = item.m_id;
                pair.m_index = (int32_t)m_items.size();
                m_idToIndexMap.push_back(pair) ;
            }
            else
            {
                RCHfsFork fd;
                recSize -= 0x58;
                r += 0x58;
                if (recSize < 0x50 * 2)
                {
                    return RC_S_FALSE;
                }
                fd.Parse(r);
                item.m_size = fd.m_size;
                item.m_numBlocks = fd.m_numBlocks;
                uint32_t curBlock = 0;
                for (int32_t j = 0; j < 8; j++)
                {
                    if (curBlock >= fd.m_numBlocks)
                    {
                        break;
                    }
                    const RCHfsExtent& e = fd.m_extents[j];
                    item.m_extents.push_back(e) ;
                    curBlock += e.m_numBlocks;
                }
            }
            m_items.push_back(item);
            if (progress && m_items.size() % 100 == 0)
            {
                HResult res = progress->SetCompleted(m_items.size()) ;
                if(!IsSuccess(res))
                {
                    return res ;
                }
            }
        }
        node = desc.m_fLink ;
    }
    std::sort(m_idToIndexMap.begin(), m_idToIndexMap.end(), RCCompareIdToIndex()) ;
    return RC_S_OK ;
}

HResult RCHfsDatabase::ReadFile(const RCHfsFork& fork, RCByteBuffer& buf, IInStream* inStream)
{
    if (fork.m_numBlocks >= m_header.m_numBlocks)
    {
        return RC_S_FALSE;
    }
    size_t totalSize = (size_t)fork.m_numBlocks << m_header.m_blockSizeLog;
    if ((totalSize >> m_header.m_blockSizeLog) != fork.m_numBlocks)
    {
        return RC_S_FALSE;
    }
    buf.SetCapacity(totalSize);
    uint32_t curBlock = 0;
    for (int32_t i = 0; i < 8; ++i)
    {
        if (curBlock >= fork.m_numBlocks)
        {
            break;
        }
        const RCHfsExtent& e = fork.m_extents[i];
        if (fork.m_numBlocks - curBlock < e.m_numBlocks || e.m_pos >= m_header.m_numBlocks)
        {
            return RC_S_FALSE;
        }
        HResult res = ReadExtent(m_header.m_blockSizeLog, 
                                 inStream,
                                (byte_t *)buf.data() + ((size_t)curBlock << m_header.m_blockSizeLog), 
                                e) ;
        if(!IsSuccess(res))
        {
            return res ;
        }
        curBlock += e.m_numBlocks;
    }
    return RC_S_OK ;
}

END_NAMESPACE_RCZIP
