/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "format/xar/RCXarHandler.h"
#include "format/common/RCPropData.h"
#include "common/RCStringConvert.h"
#include "common/RCStringUtilA.h"
#include "filesystem/RCFileName.h"
#include "filesystem/RCFileTime.h"
#include "filesystem/RCStreamUtils.h"
#include "common/RCStringBuffer.h"
#include "filesystem/RCLimitedSequentialInStream.h"
#include "filesystem/RCSequentialOutStreamImp2.h"
#include "filesystem/RCLimitedSequentialOutStream.h"
#include "format/common/RCOutStreamWithSha1.h"
#include "common/RCCreateCoder.h"
#include "interface/RCMethodDefs.h"
#include "common/RCLocalProgress.h"
#include "crypto/hash/RCSha1Defs.h"
#include "common/RCXml.h"
#include "interface/RCArchiveType.h"
#include "algorithm/CpuArch.h"

#define Get16(p) GetBe16(p)
#define Get32(p) GetBe32(p)
#define Get64(p) GetBe64(p)

#define XAR_SHOW_RAW 1
#define METHOD_NAME_ZLIB "zlib"

/////////////////////////////////////////////////////////////////
//RCXarHandler class implementation

BEGIN_NAMESPACE_RCZIP

static const uint32_t s_kXarXmlSizeMax = ((uint32_t)1 << 30) - (1 << 14) ;

static RCPropData s_kXarProps[] =
{
    { RCString(), RCPropID::kpidPath,       RCVariantType::RC_VT_STRING},
    { RCString(), RCPropID::kpidSize,       RCVariantType::RC_VT_UINT64},
    { RCString(), RCPropID::kpidPackSize,   RCVariantType::RC_VT_UINT64},
    { RCString(), RCPropID::kpidMTime,      RCVariantType::RC_VT_UINT64},
    { RCString(), RCPropID::kpidCTime,      RCVariantType::RC_VT_UINT64},
    { RCString(), RCPropID::kpidATime,      RCVariantType::RC_VT_UINT64},
    { RCString(), RCPropID::kpidMethod,     RCVariantType::RC_VT_STRING}
};

static RCPropData s_kXarArcProps[] =
{
    { RCString(_T("")), RCPropID::kpidArchiveType,    RCVariantType::RC_VT_UINT64 }
};

RCXarHandler::RCXarHandler():
    m_dataStartPos(0)
{
}

RCXarHandler::~RCXarHandler()
{
}

HResult RCXarHandler::SetCompressCodecsInfo(ICompressCodecsInfo* compressCodecsInfo)
{
    m_compressCodecsInfo = compressCodecsInfo ;
    return RC_S_OK ;
}

HResult RCXarHandler::GetNumberOfProperties(uint32_t& numProperties)
{
    numProperties = sizeof(s_kXarProps) / sizeof(s_kXarProps[0]);
    return RC_S_OK; 
}

HResult RCXarHandler::GetPropertyInfo(uint32_t index, RCString& name, RCPropertyID& propID, RCVariantType::RCVariantID& varType)
{
    if (index >= sizeof(s_kXarProps) / sizeof(s_kXarProps[0])) 
    {
        return RC_E_INVALIDARG ;
    }
    const RCPropData& srcItem = s_kXarProps[index];
    propID  = srcItem.m_propID ; 
    varType = srcItem.m_varType ; 
    name    = srcItem.m_propName ;
    return RC_S_OK;
}

HResult RCXarHandler::GetNumberOfArchiveProperties(uint32_t& numProperties)
{
    numProperties = sizeof(s_kXarArcProps) / sizeof(s_kXarArcProps[0]);
    return RC_S_OK; 
}

HResult RCXarHandler::GetArchivePropertyInfo(uint32_t index, RCString& name, RCPropertyID& propID, RCVariantType::RCVariantID& varType)
{
    if (index >= (sizeof(s_kXarArcProps) / sizeof(s_kXarArcProps[0])))
    {
        return RC_E_INVALIDARG;
    }
    const RCPropData& srcItem = s_kXarArcProps[index];
    propID = srcItem.m_propID;
    varType = srcItem.m_varType;
    name = srcItem.m_propName;
    return RC_S_OK;
}

HResult RCXarHandler::GetArchiveProperty(RCPropertyID propID, RCVariant& value)
{
    ClearVariantValue(value) ;
    switch(propID)
    {
        case RCPropID::kpidArchiveType:
        {
            value = uint64_t(RC_ARCHIVE_XAR) ;
            break;
        }
    }
    return RC_S_OK;
}

HResult RCXarHandler::GetProperty(uint32_t index, RCPropertyID propID, RCVariant& value)
{
    ClearVariantValue(value) ;
    RCVariant& prop = value ;
#ifdef XAR_SHOW_RAW
    if (index == (uint32_t)m_files.size())
    {
        switch(propID)
        {
        case RCPropID::kpidPath: 
            prop = RCString(_T("[TOC].xml"));
            break;
        case RCPropID::kpidSize:
        case RCPropID::kpidPackSize: 
            prop = (uint64_t)m_xml.size() ;
            break;
        }
    }
    else
#endif
    {
        if(index >= (uint32_t)m_files.size())
        {
            return RC_E_INVALIDARG ;
        }
        const RCXarFile& item = m_files[index];
        switch(propID)
        {
        case RCPropID::kpidMethod:
            {
                RCString name ;
                if(RCStringConvert::ConvertUTF8ToUnicode(item.m_method.c_str(),item.m_method.size(), name))
                {
                    prop = name;
                }
                break;
            }
        case RCPropID::kpidPath:
            {
                RCStringA path;
                int32_t cur = index;
                do
                {
                    const RCXarFile& item = m_files[cur];
                    RCStringA s = item.m_name ;
                    if (s.empty())
                    {
                        s = "unknown";
                    }
                    if (path.empty())
                    {
                        path = s ;
                    }
                    else
                    {
#ifdef RCZIP_OS_WIN
                        path = s + '\\' + path;
#else
                        path = s + '/' + path;
#endif
                    }
                    cur = item.m_parent ;
                }while (cur >= 0);

                RCString name ;
                if (RCStringConvert::ConvertUTF8ToUnicode(path.c_str(),path.size(), name))
                {
                    prop = name;
                }
                break;
            }

        case RCPropID::kpidIsDir:  
            prop = item.m_isDir; 
            break;
        case RCPropID::kpidSize:
            if (!item.m_isDir)
            {
                prop = item.m_size;
            }
            break ;
        case RCPropID::kpidPackSize: 
            if (!item.m_isDir)
            {
                prop = item.m_packSize;
            }
            break;
        case RCPropID::kpidMTime:
            prop = item.m_mTime ;
            break;
        case RCPropID::kpidCTime:
            prop = item.m_cTime ;
            break;
        case RCPropID::kpidATime:
            prop = item.m_aTime ;
            break;
        }
    }
    return RC_S_OK ;
}

HResult RCXarHandler::Open(IInStream* stream, uint64_t maxCheckStartPosition, IArchiveOpenCallback* openArchiveCallback)
{
    try
    {
        Close();
        if (Open2(stream) != RC_S_OK)
        {
            return RC_S_FALSE ;
        }
        m_inStream = stream;
    }
    catch(...)
    {
        return RC_S_FALSE ;
    }
    return RC_S_OK ;
}

HResult RCXarHandler::Open2(IInStream* stream)
{
    if(stream == NULL)
    {
        return RC_E_INVALIDARG ;
    }
    uint64_t archiveStartPos = 0 ;
    HResult hr = stream->Seek(0, RC_STREAM_SEEK_SET, &archiveStartPos) ;
    if(!IsSuccess(hr))
    {
        return hr ;
    }
    const uint32_t kHeaderSize = 0x1C ;
    byte_t buf[kHeaderSize];
    hr = RCStreamUtils::ReadStream_FALSE(stream, buf, kHeaderSize) ;
    if(!IsSuccess(hr))
    {
        return hr ;
    }
    uint32_t size = Get16(buf + 4);
    if (Get32(buf) != 0x78617221 || size != kHeaderSize)
    {
        return RC_S_FALSE;
    }

    uint64_t packSize = Get64(buf + 8) ;
    uint64_t unpackSize = Get64(buf + 0x10);
    if (unpackSize >= s_kXarXmlSizeMax)
    {
        return RC_S_FALSE;
    }

    m_dataStartPos = archiveStartPos + kHeaderSize + packSize;
    RCStringBufferA xmlBuffer(&m_xml) ;
    char* ss = xmlBuffer.GetBuffer((int32_t)unpackSize + 1);
    
    ICompressCoderPtr zlibCoder ;
    hr = RCCreateCoder::CreateCoder(m_compressCodecsInfo.Get(),
                                    RCMethod::ID_COMPRESS_ZLIB,
                                    zlibCoder,
                                    false);
    if(!IsSuccess(hr) || !zlibCoder)
    {
        return RC_S_FALSE ;
    }

    RCLimitedSequentialInStream* inStreamLimSpec = new RCLimitedSequentialInStream;
    ISequentialInStreamPtr inStreamLim(inStreamLimSpec) ;
    inStreamLimSpec->SetStream(stream);
    inStreamLimSpec->Init(packSize);

    RCSequentialOutStreamImp2 *outStreamLimSpec = new RCSequentialOutStreamImp2;
    ISequentialOutStreamPtr outStreamLim(outStreamLimSpec) ;
    outStreamLimSpec->Init((byte_t*)ss, (size_t)unpackSize);

    hr = zlibCoder->Code(inStreamLim.Get(), outStreamLim.Get(), NULL, NULL, NULL) ;
    if(!IsSuccess(hr))
    {
        return RC_S_FALSE;
    }

    if (outStreamLimSpec->GetPos() != (size_t)unpackSize)
    {
        return RC_S_FALSE;
    }

    ss[(size_t)unpackSize] = 0;
    xmlBuffer.ReleaseBuffer() ;

    RCXml xml;
    if (!xml.Parse(m_xml))
    {
        return RC_S_FALSE;
    }

    if (!xml.GetRoot().IsTagged("xar") || xml.GetRoot().GetSubItems().size() != 1)
    {
        return RC_S_FALSE;
    }
    const RCXmlItem& toc = xml.GetRoot().GetSubItems()[0];
    if (!toc.IsTagged("toc"))
    {
        return RC_S_FALSE;
    }
    if (!AddItem(toc, m_files, -1))
    {
        return RC_S_FALSE ;
    }
    return RC_S_OK ;
}

HResult RCXarHandler::Close() 
{
    m_inStream.Release();
    m_files.clear();
    m_xml.clear();
    return RC_S_OK ;
}

HResult RCXarHandler::GetNumberOfItems(uint32_t& numItems)
{
    
#ifdef XAR_SHOW_RAW
    numItems = (uint32_t)m_files.size() + 1 ;
#else
    numItems = (uint32_t)_files.size() ;
#endif
    return RC_S_OK ;
}

HResult RCXarHandler::Extract(const std::vector<uint32_t>& indices, int32_t testMode, IArchiveExtractCallback* extractCallback)
{
    bool isTestMode = (testMode != 0);
    bool allFilesMode = indices.empty() ;
    uint32_t numItems = (uint32_t)indices.size() ;
    if (allFilesMode)
    {
        numItems = (uint32_t)m_files.size();
    }
    if (numItems == 0)
    {
        return RC_S_OK ;
    }
    uint64_t totalSize = 0;
    uint32_t i = 0 ;
    for (i = 0; i < numItems; ++i)
    {
        int32_t index = (int32_t)(allFilesMode ? i : indices[i]);
#ifdef XAR_SHOW_RAW
        if (index == (int32_t)m_files.size())
        {
            totalSize += m_xml.size();
        }
        else
#endif
        {
            totalSize += m_files[index].m_size ;
        }
    }
    if(extractCallback)
    {
        HResult hr = extractCallback->SetTotal(totalSize);
        if(!IsSuccess(hr))
        {
            return hr ;
        }
    }

    uint64_t currentPackTotal = 0;
    uint64_t currentUnpTotal = 0;
    uint64_t currentPackSize = 0;
    uint64_t currentUnpSize = 0;

    const uint32_t kZeroBufSize = (1 << 14);
    RCByteBuffer zeroBuf ;
    zeroBuf.SetCapacity(kZeroBufSize);
    memset(zeroBuf.data(), 0, kZeroBufSize);

    ICompressCoderPtr copyCoder ;
    ICompressCoderPtr zlibCoder ;
    ICompressCoderPtr bzip2Coder ;
    
    HResult hRes = RCCreateCoder::CreateCoder(m_compressCodecsInfo.Get(),
                                              RCMethod::ID_COMPRESS_ZLIB,
                                              zlibCoder,
                                              false);
    if(!IsSuccess(hRes) || !zlibCoder)
    {
        return RC_S_FALSE ;
    }
    hRes = RCCreateCoder::CreateCoder(m_compressCodecsInfo.Get(),
                                      RCMethod::ID_COMPRESS_COPY,
                                      copyCoder,
                                      false);
    if(!IsSuccess(hRes) || !copyCoder)
    {
        return RC_S_FALSE ;
    }
    hRes = RCCreateCoder::CreateCoder(m_compressCodecsInfo.Get(),
                                              RCMethod::ID_COMPRESS_BZIP2,
                                              bzip2Coder,
                                              false);
    if(!IsSuccess(hRes) || !bzip2Coder)
    {
        return RC_S_FALSE ;
    }
    RCLocalProgress* lps = new RCLocalProgress ;
    ICompressProgressInfoPtr progress = lps ;
    lps->Init(extractCallback, false) ;

    RCLimitedSequentialInStream *inStreamSpec = new RCLimitedSequentialInStream ;
    ISequentialInStreamPtr inStream(inStreamSpec) ;
    inStreamSpec->SetStream(m_inStream.Get());

    RCLimitedSequentialOutStream* outStreamLimSpec = new RCLimitedSequentialOutStream;
    ISequentialOutStreamPtr outStream(outStreamLimSpec) ;

    RCOutStreamWithSha1* outStreamSha1Spec = new RCOutStreamWithSha1 ;
    {
        ISequentialOutStreamPtr outStreamSha1(outStreamSha1Spec) ;
        outStreamLimSpec->SetStream(outStreamSha1.Get());
    }

    for (i = 0; i < numItems; i++, currentPackTotal += currentPackSize, currentUnpTotal += currentUnpSize)
    {
        lps->SetInSize(currentPackTotal) ;
        lps->SetOutSize(currentUnpTotal) ;
        currentPackSize = 0;
        currentUnpSize = 0;
        HResult hr = lps->SetCur() ;
        if(!IsSuccess(hr))
        {
            return hr ;
        }
        ISequentialOutStreamPtr realOutStream ;
        int32_t askMode = isTestMode ?
                                      RC_ARCHIVE_EXTRACT_ASK_MODE_TEST :
                                      RC_ARCHIVE_EXTRACT_ASK_MODE_EXTRACT;
        int32_t index = allFilesMode ? i : indices[i];
        if(extractCallback)
        {
            hr = extractCallback->GetStream(index, realOutStream.GetAddress(), askMode) ;
            if(!IsSuccess(hr))
            {
                return hr ;
            }
        }

        if (index < (int32_t)m_files.size())
        {
            const RCXarFile& item = m_files[index] ;
            if (item.m_isDir)
            {
                if(extractCallback)
                {
                    hr = extractCallback->PrepareOperation(index,askMode) ;
                    if(!IsSuccess(hr))
                    {
                        return hr ;
                    }
                    hr = extractCallback->SetOperationResult(index,RC_ARCHIVE_EXTRACT_RESULT_OK) ;
                    if(!IsSuccess(hr))
                    {
                        return hr ;
                    }
                }
                continue;
            }
        }

        if (!isTestMode && (!realOutStream))
        {
            continue;
        }
        if(extractCallback)
        {
            hr = extractCallback->PrepareOperation(index,askMode) ;
            if(!IsSuccess(hr))
            {
                return hr ;
            }
        }
        outStreamSha1Spec->SetStream(realOutStream.Get());
        realOutStream.Release();

        int32_t opRes = RC_ARCHIVE_EXTRACT_RESULT_OK;
#ifdef XAR_SHOW_RAW
        if (index == (int32_t)m_files.size())
        {
            outStreamSha1Spec->Init(false);
            outStreamLimSpec->Init(m_xml.size());
            hr = RCStreamUtils::WriteStream(outStream.Get(),m_xml.c_str(), m_xml.size()) ;
            if(!IsSuccess(hr))
            {
                return hr ;
            }
            currentPackSize = currentUnpSize = m_xml.size() ;
        }
        else
#endif
        {
            const RCXarFile& item = m_files[index];
            if (item.m_hasData)
            {
                currentPackSize = item.m_packSize;
                currentUnpSize = item.m_size;

                HResult hr = m_inStream->Seek(m_dataStartPos + item.m_offset, RC_STREAM_SEEK_SET, NULL) ;
                if(!IsSuccess(hr))
                {
                    return hr ;
                }
                inStreamSpec->Init(item.m_packSize);
                outStreamSha1Spec->Init(item.m_sha1IsDefined);
                outStreamLimSpec->Init(item.m_size);
                HResult res = RC_S_OK ;

                ICompressCoderPtr coder;
                if (item.m_method == "octet-stream")
                {
                    if (item.m_packSize == item.m_size)
                    {
                        coder = copyCoder;
                    }
                    else
                    {
                        opRes = RC_ARCHIVE_EXTRACT_RESULT_UNSUPPORTED_METHOD;
                    }
                }
                else if (item.m_method == METHOD_NAME_ZLIB)
                {
                    coder = zlibCoder;
                }
                else if (item.m_method == "bzip2")
                {
                    coder = bzip2Coder;
                }
                else
                {
                    opRes = RC_ARCHIVE_EXTRACT_RESULT_UNSUPPORTED_METHOD;
                }

                if (coder)
                {
                    res = coder->Code(inStream.Get(), outStream.Get(), NULL, NULL, progress.Get());
                }
                if (res != RC_S_OK)
                {
                    if (res != RC_S_FALSE)
                    {
                        return res;
                    }
                    else if (!outStreamLimSpec->IsFinishedOK())
                    {
                        opRes = RC_ARCHIVE_EXTRACT_RESULT_DATA_ERROR;
                    }

                    if (opRes == RC_ARCHIVE_EXTRACT_RESULT_OK)
                    {
                        opRes = RC_ARCHIVE_EXTRACT_RESULT_DATA_ERROR;
                    }
                }

                if (opRes == RC_ARCHIVE_EXTRACT_RESULT_OK)
                {
                    if (outStreamLimSpec->IsFinishedOK() &&
                        outStreamSha1Spec->GetSize() == item.m_size)
                    {
                        if (!outStreamLimSpec->IsFinishedOK())
                        {
                            opRes = RC_ARCHIVE_EXTRACT_RESULT_DATA_ERROR;
                        }
                        else if (item.m_sha1IsDefined)
                        {
                            byte_t digest[RCSha1Defs::kDigestSize];
                            outStreamSha1Spec->Final(digest);
                            if (memcmp(digest, item.m_sha1,RCSha1Defs::kDigestSize) != 0)
                            {
                                opRes = RC_ARCHIVE_EXTRACT_RESULT_CRC_ERROR;
                            }
                        }
                    }
                    else
                    {
                        opRes = RC_ARCHIVE_EXTRACT_RESULT_DATA_ERROR;
                    }
                }
            }
        }
        outStreamSha1Spec->ReleaseStream();
        if(extractCallback)
        {
            HResult hr = extractCallback->SetOperationResult(index,opRes) ;
            if(!IsSuccess(hr))
            {
                return hr ;
            }
        }
    }
    return RC_S_OK ;
}

bool RCXarHandler::AddItem(const RCXmlItem& item, RCVector<RCXarFile>& files, int32_t parent)
{
    if (!item.IsTag())
    {
        return true;
    }
    if (item.GetName() == "file")
    {
        RCXarFile file;
        file.m_parent = parent;
        parent = (int32_t)files.size() ;
        file.m_name = item.GetSubStringForTag("name");
        RCStringA type = item.GetSubStringForTag("type");
        if (type == "directory")
        {
            file.m_isDir = true;
        }
        else if (type == "file")
        {
            file.m_isDir = false;
        }
        else
        {
            return false;
        }

        int32_t dataIndex = item.FindSubTag("data");
        if (dataIndex >= 0 && !file.m_isDir)
        {
            file.m_hasData = true;
            const RCXmlItem& dataItem = item.GetSubItems()[dataIndex];
            if (!ParseUInt64(dataItem, "size", file.m_size))
            {
                return false;
            }
            if (!ParseUInt64(dataItem, "length", file.m_packSize))
            {
                return false;
            }
            if (!ParseUInt64(dataItem, "offset", file.m_offset))
            {
                return false;
            }
            file.m_sha1IsDefined = ParseSha1(dataItem, "extracted-checksum", file.m_sha1);
            int32_t encodingIndex = dataItem.FindSubTag("encoding");
            const RCXmlItem& encodingItem = dataItem.GetSubItems()[encodingIndex];
            if (encodingItem.IsTag())
            {
                RCStringA s = encodingItem.GetPropertyValue("style");
                if(!s.empty())
                {
                    RCStringA appl = "application/";
                    if (RCStringUtilA::Left(s,(int32_t)appl.size()) == appl)
                    {
                        s = RCStringUtilA::Mid(s,(int32_t)appl.size());
                        RCStringA xx = "x-";
                        if (RCStringUtilA::Left(s,(int32_t)xx.size()) == xx)
                        {
                            s = RCStringUtilA::Mid(s,(int32_t)xx.size());
                            if (s == "gzip")
                            {
                                s = METHOD_NAME_ZLIB;
                            }
                        }
                    }
                    file.m_method = s ;
                }
            }
        }

        file.m_cTime = ParseTime(item, "ctime");
        file.m_mTime = ParseTime(item, "mtime");
        file.m_aTime = ParseTime(item, "atime");
        files.push_back(file);
    }
    for (int32_t i = 0; i < (int32_t)item.GetSubItems().size(); ++i)
    {
        if (!AddItem(item.GetSubItems()[i], files, parent))
        {
            return false ;
        }
    }
    return true;
}

bool RCXarHandler::ParseUInt64(const RCXmlItem& item, const char* name, uint64_t& res)
{
    RCStringA s = item.GetSubStringForTag(name);
    const char* end = NULL ;
    res = RCStringUtilA::ConvertStringToUInt64(s.c_str(), &end);
    return (end - (const char *)s.c_str() == (int32_t)s.size()) ;
}

uint64_t RCXarHandler::ParseTime(const RCXmlItem& item, const char* name)
{
    RCStringA s = item.GetSubStringForTag(name);
    if (s.size() < 20)
    {
        return 0;
    }
    const char* p = s.c_str() ;
    if (p[ 4] != '-' || p[ 7] != '-' || p[10] != 'T' ||
        p[13] != ':' || p[16] != ':' || p[19] != 'Z')
    {
        return 0;
    }
    uint32_t year = 0 ;
    uint32_t month = 0 ;
    uint32_t day = 0 ;
    uint32_t hour = 0 ;
    uint32_t min = 0 ;
    uint32_t sec = 0 ;
    if (!ParseNumber(p,      4, year )) return 0;
    if (!ParseNumber(p + 5,  2, month)) return 0;
    if (!ParseNumber(p + 8,  2, day  )) return 0;
    if (!ParseNumber(p + 11, 2, hour )) return 0;
    if (!ParseNumber(p + 14, 2, min  )) return 0;
    if (!ParseNumber(p + 17, 2, sec  )) return 0;

    uint64_t numSecs = 0 ;
    if (!RCFileTime::GetSecondsSince1601(year, month, day, hour, min, sec, numSecs))
    {
        return 0 ;
    }
    return numSecs * 10000000 ;
}

bool RCXarHandler::ParseNumber(const char* s, int32_t size, uint32_t& res) 
{
    const char* end = NULL ;
    res = (uint32_t)RCStringUtilA::ConvertStringToUInt64(s, &end);
    return (end - s == size) ;
}

bool RCXarHandler::ParseSha1(const RCXmlItem& item, const char* name, byte_t* digest)
{
    int32_t index = item.FindSubTag(name);
    if (index  < 0)
    {
        return false;
    }
    const RCXmlItem& checkItem = item.GetSubItems()[index];
    RCStringA style = checkItem.GetPropertyValue("style");
    if (style == "SHA1")
    {
        RCStringA s = checkItem.GetSubString();
        if (s.size() != 40)
        {
            return false;
        }
        for (size_t i = 0; i < s.size(); i += 2)
        {
            byte_t b0, b1;
            if (!HexToByte(s[i], b0) || !HexToByte(s[i + 1], b1))
            {
                return false;
            }
            digest[i / 2] = (b0 << 4) | b1 ;
        }
        return true;
    }
    return false;
}

bool RCXarHandler::HexToByte(char c, byte_t& res)
{
    if(c >= '0' && c <= '9')
    {
        res = c - '0';
    }
    else if (c >= 'A' && c <= 'F')
    {
        res = c - 'A' + 10;
    }
    else if (c >= 'a' && c <= 'f')
    {
        res = c - 'a' + 10;
    }
    else
    {
        return false;
    }
    return true;
}

END_NAMESPACE_RCZIP
