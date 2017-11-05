/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "format/wim/RCWimUtils.h"
#include "filesystem/RCStreamUtils.h"
#include "format/wim/RCWimItem.h"
#include "common/RCStringUtil.h"
#include "filesystem/RCFileName.h"
#include "filesystem/RCSequentialOutStreamImp2.h"
#include "format/wim/RCWimUnpacker.h"
#include "common/RCStringBuffer.h"
#include "algorithm/CpuArch.h"
#include <functional>

#define Get16(p) GetUi16(p)
#define Get32(p) GetUi32(p)
#define Get64(p) GetUi64(p)

/////////////////////////////////////////////////////////////////
//RCWimUtils class implementation

BEGIN_NAMESPACE_RCZIP

static const uint32_t s_kWimSignatureSize = 8;
static const byte_t s_kWimSignature[s_kWimSignatureSize] = { 'M', 'S', 'W', 'I', 'M', 0, 0, 0 };

class CompareStreamsByPos:
    public std::binary_function<RCWimStreamInfo, RCWimStreamInfo ,bool>
{
public:
    bool operator()(const RCWimStreamInfo& p1, const RCWimStreamInfo& p2) const
    {
        if(p1.m_partNumber < p2.m_partNumber)
        {
            return true ;
        }
        else if(p1.m_partNumber > p2.m_partNumber)
        {
            return false ;
        }
        if(p1.m_resource.m_offset < p2.m_resource.m_offset)
        {
            return true ;
        }
        else
        {
            return false ;
        }
    }
};

class CompareHashRefs:
    public std::binary_function<int32_t, int32_t ,bool>
{
public:
    CompareHashRefs(const RCVector<RCWimStreamInfo>* pWimStreamInfo):
        m_pWimStreamInfo(pWimStreamInfo)
    {
    }
    
    bool operator()(const int32_t p1, const int32_t p2) const
    {
        const RCVector<RCWimStreamInfo>& streams = *m_pWimStreamInfo ;
        return memcmp(streams[p1].m_hash, streams[p2].m_hash, s_kWimHashSize) < 0 ;
    }
    
private:
    const RCVector<RCWimStreamInfo>* m_pWimStreamInfo ;
};

class CompareItems:
    public std::binary_function<RCWimItem, RCWimItem ,bool>
{
public:
    bool operator()(const RCWimItem& i1, const RCWimItem& i2) const
    {
        if (i1.isDir() != i2.isDir())
        {
            return (i1.isDir()) ? false : true ;
        }
        if (i1.isDir())
        {
            return RCStringUtil::CompareNoCase(i1.m_name, i2.m_name) > 0 ? true : false ;
        }
        if(i1.m_streamIndex < i2.m_streamIndex)
        {
            return true ;
        }
        else if(i1.m_streamIndex > i2.m_streamIndex)
        {
            return false ;
        }
        return RCStringUtil::CompareNoCase(i1.m_name, i2.m_name) < 0 ? true : false ;
    }
};

RCWimUtils::RCWimUtils(ICompressCodecsInfo* compressCodecsInfo):
    m_compressCodecsInfo(compressCodecsInfo)
{
    
}

HResult RCWimUtils::ReadHeader(IInStream* inStream, RCWimHeader& header)
{
    const uint32_t kHeaderSizeMax = 0xD0;
    byte_t p[kHeaderSizeMax];
    HResult hr = RCStreamUtils::ReadStream_FALSE(inStream, p, kHeaderSizeMax) ;
    if (memcmp(p, s_kWimSignature, s_kWimSignatureSize) != 0)
    {
        return RC_S_FALSE;
    }
    return header.Parse(p);
}

HResult RCWimUtils::OpenArchive(IInStream* inStream,
                                const RCWimHeader& header, 
                                RCByteBuffer& xml,
                                RCWimDatabase& database)

{
    HResult hr = UnpackData(inStream, header.m_xmlResource, header.IsLzxMode(), xml, NULL) ;
    if(!IsSuccess(hr))
    {
        return hr ;
    }
    hr = ReadStreams(inStream, header, database) ;
    if(!IsSuccess(hr))
    {
        return hr ;
    }
    bool needBootMetadata = !header.m_metadataResource.IsEmpty() ;
    if (header.m_partNumber == 1)
    {
        int32_t imageIndex = 1;
        for (int32_t j = 0; j < (int32_t)database.m_streams.size(); ++j)
        {
            const RCWimStreamInfo& si = database.m_streams[j] ;
            if (!si.m_resource.IsMetadata() || si.m_partNumber != header.m_partNumber)
            {
                continue;
            }
            byte_t hash[s_kWimHashSize] ;
            RCByteBuffer metadata ;
            hr = UnpackData(inStream, si.m_resource, header.IsLzxMode(), metadata, hash) ;
            if(!IsSuccess(hr))
            {
                return hr ;
            }
            if (memcmp(hash, si.m_hash, s_kWimHashSize) != 0)
            {
                return RC_S_FALSE ;
            }
            RCString::value_type sz[32];
            RCStringUtil::ConvertUInt64ToString(imageIndex++, sz);
            RCString s = sz ;
            s += RCFileName::GetDirDelimiter() ;
            hr = ParseDir(metadata.data(), metadata.GetCapacity(), s, database.m_items) ;
            if(!IsSuccess(hr))
            {
                return hr ;
            }
            if (needBootMetadata)
            {
                if (header.m_metadataResource.m_offset == si.m_resource.m_offset)
                {
                    needBootMetadata = false;
                }
            }
        }
    }

    if (needBootMetadata)
    {
        RCByteBuffer metadata;
        HResult hr = UnpackData(inStream, header.m_metadataResource, header.IsLzxMode(), metadata, NULL) ;
        if(!IsSuccess(hr))
        {
            return hr ;
        }
        RCString dir = _T("0") ;
        dir += RCFileName::GetDirDelimiter() ;
        hr = ParseDir(metadata.data(), metadata.GetCapacity(),dir, database.m_items) ;
        if(!IsSuccess(hr))
        {
            return hr ;
        }
    }
    return RC_S_OK ;
}

HResult RCWimUtils::SortDatabase(RCWimDatabase& database)
{
    std::sort(database.m_streams.begin(),database.m_streams.end(),CompareStreamsByPos());
    {
        RCVector<int32_t> sortedByHash;
        {
            for (int32_t j = 0; j < (int32_t)database.m_streams.size(); ++j)
            {
                sortedByHash.push_back(j);
            }
            std::sort(sortedByHash.begin(),sortedByHash.end(),CompareHashRefs(&database.m_streams)) ;
        }

        for (int32_t i = 0; i < (int32_t)database.m_items.size(); ++i)
        {
            RCWimItem& item = database.m_items[i] ;
            item.m_streamIndex = -1;
            if (item.HasStream())
            {
                item.m_streamIndex = FindHash(database.m_streams, sortedByHash, item.m_hash);
            }
        }
    }

    {
        RCVector<bool> used ;
        int32_t j = 0 ;
        for (j = 0; j < (int32_t)database.m_streams.size(); ++j)
        {
            const RCWimStreamInfo& s = database.m_streams[j];
            used.push_back(s.m_resource.IsMetadata() && s.m_partNumber == 1);
        }
        for (int32_t i = 0; i < (int32_t)database.m_items.size(); ++i)
        {
            RCWimItem& item = database.m_items[i] ;
            if (item.m_streamIndex >= 0)
            {
                used[item.m_streamIndex] = true;
            }
        }
        for (j = 0; j < (int32_t)database.m_streams.size(); ++j)
        {
            if (!used[j])
            {
                RCWimItem item;
                item.m_streamIndex = j;
                item.m_hasMetadata = false;
                database.m_items.push_back(item);
            }
        }
    }
    std::sort(database.m_items.begin(),database.m_items.end(),CompareItems());
    return RC_S_OK ;
}

HResult RCWimUtils::ReadStreams(IInStream* inStream, const RCWimHeader& h, RCWimDatabase& db)
{
    RCByteBuffer offsetBuf ;
    HResult hr = UnpackData(inStream, h.m_offsetResource, h.IsLzxMode(), offsetBuf, NULL) ;
    if(!IsSuccess(hr))
    {
        return hr ;
    }
    for (size_t i = 0; i + s_kWimStreamInfoSize <= offsetBuf.GetCapacity(); i += s_kWimStreamInfoSize)
    {
        RCWimStreamInfo s;
        GetStream((const byte_t *)offsetBuf.data() + i, s);
        if (s.m_partNumber == h.m_partNumber)
        {
            db.m_streams.push_back(s) ;
        }
    }
    return RC_S_OK ;
}

HResult RCWimUtils::UnpackData(IInStream* inStream, const RCWimResource& resource, bool lzxMode, RCByteBuffer& buf, byte_t* digest) 
{
    size_t size = (size_t)resource.m_unpackSize;
    if (size != resource.m_unpackSize)
    {
        return RC_E_OUTOFMEMORY;
    }
    buf.Free() ;
    buf.SetCapacity(size) ;

    RCSequentialOutStreamImp2 *outStreamSpec = new RCSequentialOutStreamImp2();
    ISequentialOutStreamPtr outStream = outStreamSpec ;
    outStreamSpec->Init((byte_t *)buf.data(), size);

    RCWimUnpacker unpacker(m_compressCodecsInfo.Get());
    return unpacker.Unpack(inStream, resource, lzxMode, outStream.Get(), NULL, digest);
}

void RCWimUtils::GetStream(const byte_t* p, RCWimStreamInfo& s)
{
    s.m_resource.Parse(p);
    s.m_partNumber = Get16(p + 24);
    s.m_refCount = Get32(p + 26);
    memcpy(s.m_hash, p + 30, s_kWimHashSize) ;
}

void RCWimUtils::GetFileTimeFromMem(const byte_t* p, RC_FILE_TIME* ft)
{
    ft->u32LowDateTime  = Get32(p);
    ft->u32HighDateTime = Get32(p + 4);
}

HResult RCWimUtils::ParseDirItem(const byte_t* base, 
                                 size_t pos, 
                                 size_t size,
                                 const RCString& prefix, 
                                 RCVector<RCWimItem>& items)
{
    for (;;)
    {
        if (pos + 8 > size)
        {
            return RC_S_FALSE ;
        }
        const byte_t* p = base + pos ;
        uint64_t length = Get64(p) ;
        if (length == 0)
        {
            return RC_S_OK ;
        }
        if (pos + 102 > size || pos + length + 8 > size || length > ((uint64_t)1 << 62))
        {
            return RC_S_FALSE;
        }
        RCWimItem item ;
        item.m_attrib = Get32(p + 8);
        uint64_t subdirOffset = Get64(p + 0x10);
        GetFileTimeFromMem(p + 0x28, &item.m_cTime);
        GetFileTimeFromMem(p + 0x30, &item.m_aTime);
        GetFileTimeFromMem(p + 0x38, &item.m_mTime);
        memcpy(item.m_hash, p + 0x40, s_kWimHashSize);

        uint16_t fileNameLen = Get16(p + 100);

        size_t tempPos = pos + 102;
        if (tempPos + fileNameLen > size)
        {
            return RC_S_FALSE;
        }
        RCStringBuffer nameBuffer(&item.m_name) ;
        RCString::value_type* sz = nameBuffer.GetBuffer( (int32_t)(prefix.size() + fileNameLen / 2 + 1) );
        RCStringUtil::StringCopy(sz,prefix.c_str());
        sz += prefix.size();
        for (uint16_t i = 0; i + 2 <= fileNameLen; i += 2)
        {
            *sz++ = Get16(base + tempPos + i);
        }
        *sz++ = _T('\0') ;
        nameBuffer.ReleaseBuffer();
        if (fileNameLen == 0 && item.isDir() && !item.HasStream())
        {
            item.m_attrib = 0x10; // some swm archives have system/hidden attributes for root
            item.m_name.erase(item.m_name.size() - 1,1);
        }
        items.push_back(item);
        pos += (size_t)length;
        if (item.isDir() && (subdirOffset != 0))
        {
            if (subdirOffset >= size)
            {
                return RC_S_FALSE;
            }
            HResult hr = ParseDirItem(base, (size_t)subdirOffset, size, item.m_name + RCFileName::GetDirDelimiter(), items) ;
            if(!IsSuccess(hr))
            {
                return hr ;
            }
        }
    }
}

HResult RCWimUtils::ParseDir(const byte_t* base, 
                             size_t size,
                             const RCString& prefix, 
                             RCVector<RCWimItem>& items)
{
    size_t pos = 0;
    if (pos + 8 > size)
    {
        return RC_S_FALSE ;
    }
    const byte_t* p = base + pos;
    uint32_t totalLength = Get32(p) ;
    pos = totalLength;
    
    return ParseDirItem(base, pos, size, prefix, items);
}

int32_t RCWimUtils::FindHash(const RCVector<RCWimStreamInfo>& streams,
                             const RCVector<int32_t>& sortedByHash, 
                             const byte_t* hash) 
{
    int32_t left = 0 ;
    int32_t right = (int32_t)streams.size();
    while (left != right)
    {
        int mid = (left + right) / 2 ;
        int streamIndex = sortedByHash[mid];
        uint32_t i = 0 ;
        const byte_t* hash2 = streams[streamIndex].m_hash;
        for (i = 0; i < s_kWimHashSize; i++)
        {
            if (hash[i] != hash2[i])
            {
                break;
            }
        }
        if (i == s_kWimHashSize)
        {
            return streamIndex;
        }
        if (hash[i] < hash2[i])
        {
            right = mid;
        }
        else
        {
            left = mid + 1;
        }
    }
    return -1 ;
}
                                            
END_NAMESPACE_RCZIP
