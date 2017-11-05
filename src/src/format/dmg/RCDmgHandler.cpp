/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "format/dmg/RCDmgHandler.h"
#include "format/common/RCPropData.h"
#include "format/dmg/RCDmgMethods.h"
#include "common/RCStringUtilA.h"
#include "common/RCStringConvert.h"
#include "filesystem/RCStreamUtils.h"
#include "common/RCStringBuffer.h"
#include "common/RCXml.h"
#include "common/RCLocalProgress.h"
#include "filesystem/RCLimitedSequentialInStream.h"
#include "filesystem/RCLimitedSequentialOutStream.h"
#include "common/RCCreateCoder.h"
#include "interface/RCMethodDefs.h"
#include "RCDmgDefs.h"
#include "interface/RCArchiveType.h"
#include "algorithm/CpuArch.h"

//#define DMG_SHOW_RAW 1

#ifdef RCZIP_OS_WIN
    #define RAW_PREFIX _T("raw\\")
#else
    #define RAW_PREFIX _T("raw/")
#endif

#define Get32(p) GetBe32(p)
#define Get64(p) GetBe64(p)

/////////////////////////////////////////////////////////////////
//RCDmgHandler class implementation

BEGIN_NAMESPACE_RCZIP

static RCPropData s_dmgProps[] =
{
    { RCString(), RCPropID::kpidPath,       RCVariantType::RC_VT_STRING},
    { RCString(), RCPropID::kpidSize,       RCVariantType::RC_VT_UINT64},
    { RCString(), RCPropID::kpidPackSize,   RCVariantType::RC_VT_UINT64},
    { RCString(), RCPropID::kpidComment,    RCVariantType::RC_VT_STRING},
    { RCString(), RCPropID::kpidMethod,     RCVariantType::RC_VT_STRING},
    { RCString(), RCPropID::kpidCommented,  RCVariantType::RC_VT_BOOL}
};

static RCPropData s_dmgArcProps[] =
{
    { RCString(), RCPropID::kpidMethod,     RCVariantType::RC_VT_STRING},
    { RCString(), RCPropID::kpidNumBlocks,  RCVariantType::RC_VT_UINT64},
    { RCString(), RCPropID::kpidArchiveType,RCVariantType::RC_VT_UINT64 }
};

static const uint32_t s_dmgXmlSizeMax = ((uint32_t)1 << 31) - (1 << 14);

RCDmgHandler::RCDmgHandler()
{
}

RCDmgHandler::~RCDmgHandler()
{
}

HResult RCDmgHandler::SetCompressCodecsInfo(ICompressCodecsInfo* compressCodecsInfo)
{
    m_compressCodecsInfo = compressCodecsInfo ;
    return RC_S_OK ;
}

HResult RCDmgHandler::GetNumberOfProperties(uint32_t& numProperties)
{
    numProperties = sizeof(s_dmgProps) / sizeof(s_dmgProps[0]);
    return RC_S_OK; 
}

HResult RCDmgHandler::GetPropertyInfo(uint32_t index, RCString& name, RCPropertyID& propID, RCVariantType::RCVariantID& varType)
{
    if (index >= sizeof(s_dmgProps) / sizeof(s_dmgProps[0])) 
    {
        return RC_E_INVALIDARG ;
    }
    const RCPropData& srcItem = s_dmgProps[index];
    propID  = srcItem.m_propID ; 
    varType = srcItem.m_varType ; 
    name    = srcItem.m_propName ;
    return RC_S_OK;
}

HResult RCDmgHandler::GetNumberOfArchiveProperties(uint32_t& numProperties)
{
    numProperties = sizeof(s_dmgArcProps) / sizeof(s_dmgArcProps[0]);
    return RC_S_OK; 
}

HResult RCDmgHandler::GetArchivePropertyInfo(uint32_t index, RCString& name, RCPropertyID& propID, RCVariantType::RCVariantID& varType)
{
    if (index >= sizeof(s_dmgArcProps) / sizeof(s_dmgArcProps[0])) 
    {
        return RC_E_INVALIDARG ;
    }
    const RCPropData& srcItem = s_dmgArcProps[index];
    propID  = srcItem.m_propID ; 
    varType = srcItem.m_varType ; 
    name    = srcItem.m_propName ;
    return RC_S_OK;
}

HResult RCDmgHandler::GetArchiveProperty(RCPropertyID propID, RCVariant& value)
{
    ClearVariantValue(value) ;
    RCVariant& prop = value ;
    switch(propID)
    {
    case RCPropID::kpidMethod:
        {
            RCDmgMethods m ;
            size_t count = m_files.size() ;
            for (size_t i = 0; i < count; ++i)
            {
                m.Update(*m_files[i]);
            }
            prop = m.GetString() ;
            break;
        }
    case RCPropID::kpidNumBlocks:
        {
            uint64_t numBlocks = 0;
            size_t count = m_files.size() ;
            for (size_t i = 0; i < count; ++i)
            {
                numBlocks += m_files[i]->m_blocks.size();
            }
            prop = numBlocks ;
            break;
        }
    case RCPropID::kpidArchiveType:
        prop = uint64_t(RC_ARCHIVE_DMG) ;
        break;
    }
    return RC_S_OK;
}

HResult RCDmgHandler::GetProperty(uint32_t index, RCPropertyID propID, RCVariant& value)
{
    ClearVariantValue(value) ;
    RCVariant& prop = value ;
#ifdef DMG_SHOW_RAW
    if (index == m_fileIndices.size())
    {
        switch(propID)
        {
        case RCPropID::kpidPath:
            prop = RCString(RAW_PREFIX _T("a.xml")) ;
            break ;
        case RCPropID::kpidSize:
        case RCPropID::kpidPackSize:
            prop = (uint64_t)m_xml.size();
            break;
        }
    }
    else if (index > m_fileIndices.size())
    {
        uint32_t rawIndex = index - static_cast<uint32_t>(m_fileIndices.size() + 1);
        switch(propID)
        {
        case RCPropID::kpidPath:
            {
                RCString::value_type s[32] = RAW_PREFIX ;
                RCStringUtil::ConvertUInt64ToString(rawIndex, s + RCStringUtil::StringLen(s));
                prop = RCString(s);
                break;
            }
        case RCPropID::kpidSize:
        case RCPropID::kpidPackSize:
            prop = (uint64_t)m_files[rawIndex]->Raw.GetCapacity() ;
            break;
        }
    }
    else
#endif
    {
        if(index >= m_fileIndices.size())
        {
            return RC_E_INVALIDARG ;
        }
        int32_t itemIndex = m_fileIndices[index] ;
        if(itemIndex >= (int32_t)m_files.size())
        {
            return RC_E_INVALIDARG ;
        }
        const RCDmgFile& item = *m_files[itemIndex];
        switch(propID)
        {
        case RCPropID::kpidMethod:
            {
                RCDmgMethods m ;
                m.Update(item) ;
                RCString resString = m.GetString();
                if (!resString.empty())
                {
                    prop = resString ;
                }
                break;
            }
        case RCPropID::kpidPath:
            {
                RCString name;
                RCString::value_type s[64];
                RCStringUtil::ConvertUInt64ToString(index, s);
                name = s ;
                int32_t num = 10;
                int32_t numDigits = 0 ;
                int32_t indicesCount = (int32_t)m_fileIndices.size() ;
                for (numDigits = 1; num < indicesCount; ++numDigits)
                {
                    //TODO: check overflow, Rhett.Li
                    num *= 10 ;
                }
                while ((int32_t)name.size() < numDigits)
                {
                    name = _T('0') + name ;
                }

                RCStringA subName ;
                RCString::size_type pos1 = item.m_name.find(_T('('));
                if (pos1 != RCString::npos)
                {
                    ++pos1;
                    RCString::size_type pos2 = item.m_name.find(_T(')'), pos1);
                    if (pos2 != RCString::npos)
                    {
                        subName = RCStringUtilA::Mid(item.m_name,(int32_t)pos1, pos2 - pos1);
                        pos1 = subName.find(_T(':'));
                        if (pos1 != RCString::npos)
                        {
                            subName = RCStringUtilA::Left(subName,(int32_t)pos1);
                        }
                    }
                }
                RCStringUtilA::Trim(subName);
                if (!subName.empty())
                {
                    if (subName == "Apple_HFS")
                    {
                        subName = "hfs";
                    }
                    else if (subName == "Apple_HFSX")
                    {
                        subName = "hfsx";
                    }
                    else if (subName == "Apple_Free")
                    {
                        subName = "free";
                    }
                    else if (subName == "DDM")
                    {
                        subName = "ddm";
                    }
                    RCString name2 ;
                    RCStringConvert::ConvertUTF8ToUnicode(subName.c_str(), subName.size(), name2);
                    name += _T('.') ;
                    name += name2 ;
                }
                else
                {
                    RCString name2;
                    RCStringConvert::ConvertUTF8ToUnicode(item.m_name.c_str(), item.m_name.size(), name2);
                    if (!name2.empty())
                    {
                        name += _T(" - ");
                    }
                    name += name2;
                }
                prop = name;
                break;
            }
        case RCPropID::kpidComment:
            {
                RCString name;
                RCStringConvert::ConvertUTF8ToUnicode(item.m_name.c_str(), item.m_name.size(), name);
                prop = name;
                break;
            }
        case RCPropID::kpidCommented:
            {
                prop = item.m_name.empty() ? false : true ;
                break;
            }
        case RCPropID::kpidSize:  
            prop = item.GetUnpackSize(); 
            break;
        case RCPropID::kpidPackSize:  
            prop = item.GetPackSize(); 
            break;
        }
    }
    return RC_S_OK ;
}

HResult RCDmgHandler::Open(IInStream* stream, uint64_t maxCheckStartPosition, IArchiveOpenCallback* openArchiveCallback)
{
    try
    {
        Close();
        if (Open2(stream) != RC_S_OK)
        {
            return RC_S_FALSE ;
        }
        m_inStream = stream ;
        return RC_S_OK ;
    }
    catch(...)
    {
        return RC_S_FALSE ;
    }
}

HResult RCDmgHandler::Open2(IInStream *stream)
{
    const int32_t HEADER_SIZE = 0x1E0 ;
    uint64_t headerPos = 0 ;
    HResult hr = stream->Seek(-HEADER_SIZE, RC_STREAM_SEEK_END, &headerPos) ;
    if(!IsSuccess(hr))
    {
        return hr ;
    }
    byte_t buf[HEADER_SIZE];
    hr = RCStreamUtils::ReadStream_FALSE(stream, buf, HEADER_SIZE) ;
    if(!IsSuccess(hr))
    {
        return hr ;
    }
    uint64_t address1 = Get64(buf + 0);
    uint64_t address2 = Get64(buf + 0xB8);
    uint64_t size64 = Get64(buf + 0xC0);
    if (address1 != address2 || 
        size64 >= s_dmgXmlSizeMax || 
        size64 == 0 ||
        address1 >= headerPos || 
        address1 + size64 > headerPos)
    {
        return RC_S_FALSE ;
    }
    hr = stream->Seek(address1, RC_STREAM_SEEK_SET, NULL) ;
    if(!IsSuccess(hr))
    {
        return hr ;
    }
    size_t size = (size_t)size64;

    RCStringBufferA stringBuf(&m_xml) ;
    char* ss = stringBuf.GetBuffer((int32_t)size + 1);
    hr = RCStreamUtils::ReadStream_FALSE(stream, ss, size) ;
    if(!IsSuccess(hr))
    {
        return hr ;
    }
    ss[size] = 0;
    stringBuf.ReleaseBuffer();

    RCXml xml;
    if (!xml.Parse(m_xml))
    {
        return RC_S_FALSE ;
    }
    if (xml.GetRoot().GetName() != "plist")
    {
        return RC_S_FALSE ;
    }

    int32_t dictIndex = xml.GetRoot().FindSubTag("dict");
    if (dictIndex < 0)
    {
        return RC_S_FALSE ;
    }

    const RCXmlItem& dictItem = xml.GetRoot().GetSubItems()[dictIndex];
    int32_t rfDictIndex = FindKeyPair(dictItem, "resource-fork", "dict");
    if (rfDictIndex < 0)
    {
        return RC_S_FALSE ;
    }

    const RCXmlItem& rfDictItem = dictItem.GetSubItems()[rfDictIndex];
    int32_t arrIndex = FindKeyPair(rfDictItem, "blkx", "array");
    if (arrIndex < 0)
    {
        return RC_S_FALSE ;
    }

    const RCXmlItem& arrItem = rfDictItem.GetSubItems()[arrIndex] ;

    size_t i = 0 ;
    size_t itemCount = arrItem.GetSubItems().size() ;
    for(i = 0; i < itemCount ; i++)
    {
        const RCXmlItem& item = arrItem.GetSubItems()[i];
        if (!item.IsTagged("dict"))
        {
            continue;
        }
        RCDmgFilePtr spFile(new RCDmgFile) ;
        RCDmgFile& file = *spFile;
        file.m_startPos = 0 ;
        int32_t destLen = 0 ;
        {
            RCStringA dataString ;
            RCStringA name = GetStringFromKeyPair(item, "Name", "string");
            if (name.empty())
            {
                name = GetStringFromKeyPair(item, "CFName", "string");
            }
            file.m_name = name;
            dataString = GetStringFromKeyPair(item, "Data", "data");

            destLen = Base64ToBin(NULL, dataString.c_str(), (int32_t)dataString.size());
            file.m_raw.SetCapacity(destLen);
            Base64ToBin(file.m_raw.data(), dataString.c_str(), (int32_t)dataString.size());
        }

        if (destLen > 0xCC && Get32(file.m_raw.data()) == 0x6D697368)
        {
            //PRF(printf("\n\n index = %d", m_files.Size()));
            const int32_t kRecordSize = 40 ;
            for (int32_t offset = 0xCC; offset + kRecordSize <= destLen; offset += kRecordSize)
            {
                const byte_t* p = (const byte_t*)file.m_raw.data() + offset ;
                RCDmgBlockPtr spDmgBlock(new RCDmgBlock()) ;
                RCDmgBlock& b = *spDmgBlock ;
                b.m_type = Get32(p) ;
                if (b.m_type == RCDmgDefs::METHOD_END)
                {
                    break;
                }
                if (b.m_type == RCDmgDefs::METHOD_DUMMY)
                {
                    continue;
                }

                b.m_unpPos   = Get64(p + 0x08) << 9;
                b.m_unpSize  = Get64(p + 0x10) << 9;
                b.m_packPos  = Get64(p + 0x18);
                b.m_packSize = Get64(p + 0x20);

                /*
                if (startPosIsdefined)
                {
                }
                else
                {
                startPosIsdefined = true;
                startPos = b.PackPos;
                }
                startPos += b.m_packSize;
                */

                file.m_blocks.push_back(spDmgBlock) ;

                //PRF(printf("\nType=%8x  m[1]=%8x  uPos=%8x  uSize=%7x  pPos=%8x  pSize=%7x",
                //    b.Type, Get32(p + 4), (uint32_t)b.UnpPos, (uint32_t)b.UnpSize, (uint32_t)b.PackPos, (uint32_t)b.PackSize));
            }
        }
        m_files.push_back(spFile);
        if (!file.m_blocks.empty())
        {
            m_fileIndices.push_back((int32_t)(m_files.size() - 1));
        }
    }
    // PackPos for each new file is 0 in some DMG files. So we use additional StartPos
    bool allStartAreZeros = true;
    for (i = 0; i < m_files.size(); i++)
    {
        const RCDmgFile& file = *m_files[i];
        if (!file.m_blocks.empty() && file.m_blocks[0]->m_packPos != 0)
        {
            allStartAreZeros = false;
        }
    }
    uint64_t startPos = 0;
    if (allStartAreZeros)
    {
        for (i = 0; i < m_files.size(); i++)
        {
            RCDmgFile& file = *m_files[i];
            file.m_startPos = startPos;
            if (!file.m_blocks.empty())
            {
                startPos += file.m_blocks.back()->GetNextPackOffset() ;
            }
        }
    }
    return RC_S_OK ;
}

HResult RCDmgHandler::Close() 
{
    m_inStream.Release();
    m_fileIndices.clear();
    m_files.clear();
    m_xml.clear();
    return RC_S_OK ;
}

HResult RCDmgHandler::GetNumberOfItems(uint32_t& numItems)
{
#ifdef DMG_SHOW_RAW
    numItems = static_cast<uint32_t>(m_fileIndices.size() + m_files.size() + 1) ;
#else
    numItems = static_cast<uint32_t>(m_fileIndices.size()) ;
#endif
    return RC_S_OK ;
}

HResult RCDmgHandler::Extract(const RCVector<uint32_t>& indices, int32_t testMode, IArchiveExtractCallback* extractCallback)
{
    bool isTestMode = (testMode != 0);
    bool allFilesMode = indices.empty() ? true : false ;
    size_t numItems = indices.size() ;
    if (allFilesMode)
    {
        numItems = m_files.size() ;
    }
    if (numItems == 0)
    {
        return RC_S_OK ;
    }
    uint64_t totalSize = 0 ;
    uint32_t i = 0 ;
    for (i = 0; i < numItems; ++i)
    {
        int32_t index = (int32_t)(allFilesMode ? i : indices[i]);
#ifdef DMG_SHOW_RAW
        if (index == m_fileIndices.size())
        {
            totalSize += m_xml.size();
        }
        else if (index > m_fileIndices.size())
        {
            totalSize += m_files[index - (m_fileIndices.size() + 1)].Raw.GetCapacity();
        }
        else
        {
            totalSize += m_files[m_fileIndices[index]]->GetUnpackSize() ;
        }
#else
        totalSize += m_files[m_fileIndices[index]]->GetUnpackSize() ;
#endif
    }
    if(extractCallback)
    {
        extractCallback->SetTotal(totalSize) ;
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
    ICompressCoderPtr bzip2Coder ;
    ICompressCoderPtr zlibCoder ;
    ICompressCoderPtr adcCoder ;

    RCLocalProgress* lps = new RCLocalProgress;
    ICompressProgressInfoPtr progress(lps) ;
    lps->Init(extractCallback, false) ;

    RCLimitedSequentialInStream* streamSpec = new RCLimitedSequentialInStream;
    ISequentialInStreamPtr inStream(streamSpec);
    streamSpec->SetStream(m_inStream.Get());

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
        int32_t askMode = isTestMode ? RC_ARCHIVE_EXTRACT_ASK_MODE_TEST :
                                       RC_ARCHIVE_EXTRACT_ASK_MODE_EXTRACT ;
        int32_t index = allFilesMode ? i : indices[i];
        if(extractCallback)
        {
            hr = extractCallback->GetStream(index, realOutStream.GetAddress(), askMode) ;
            if(!IsSuccess(hr))
            {
                return hr ;
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

        RCLimitedSequentialOutStream* outStreamSpec = new RCLimitedSequentialOutStream ;
        ISequentialOutStreamPtr outStream(outStreamSpec) ;
        outStreamSpec->SetStream(realOutStream.Get());
        realOutStream.Release();

        int32_t opRes = RC_ARCHIVE_EXTRACT_RESULT_OK;
#ifdef DMG_SHOW_RAW
        if (index > m_fileIndices.size())
        {
            const RCByteBuffer& buf = m_files[index - (m_fileIndices.size() + 1)].Raw;
            outStreamSpec->Init(buf.GetCapacity());
            hr = RCStreamUtils::WriteStream(outStream, buf.data(), buf.GetCapacity()) ;
            if(!IsSuccess(hr))
            {
                return hr ;
            }
            currentPackSize = currentUnpSize = buf.GetCapacity() ;
        }
        else if (index == m_fileIndices.size())
        {
            outStreamSpec->Init(m_xml.size());
            hr = RCStreamUtils::WriteStream(outStream,m_xml.c_str(), m_xml.size()));
            if(!IsSuccess(hr))
            {
                return hr ;
            }
            currentPackSize = currentUnpSize = m_xml.size();
        }
        else
#endif
        {
            const RCDmgFile& item = *m_files[m_fileIndices[index]] ;
            currentPackSize = item.GetPackSize();
            currentUnpSize = item.GetUnpackSize();

            uint64_t unpPos = 0;
            uint64_t packPos = 0;
            {
                const size_t blockCount = item.m_blocks.size() ;
                for (size_t j = 0; j < blockCount ; ++j)
                {
                    lps->SetInSize(currentPackTotal + packPos) ;
                    lps->SetOutSize (currentUnpTotal + unpPos) ;
                    hr = lps->SetCur() ;
                    if(!IsSuccess(hr))
                    {
                        return hr ;
                    }

                    const RCDmgBlock& block = *item.m_blocks[j];

                    packPos += block.m_packSize;
                    if (block.m_unpPos != unpPos)
                    {
                        opRes = RC_ARCHIVE_EXTRACT_RESULT_DATA_ERROR;
                        break;
                    }

                    hr = m_inStream->Seek(item.m_startPos + block.m_packPos, RC_STREAM_SEEK_SET, NULL) ;
                    if(!IsSuccess(hr))
                    {
                        return hr ;
                    }
                    streamSpec->Init(block.m_packSize);
                    // uint64_t startSize = outStreamSpec->GetSize();
                    bool realMethod = true;
                    outStreamSpec->Init(block.m_unpSize);
                    HResult res = RC_S_OK ;

                    switch(block.m_type)
                    {
                    case RCDmgDefs::METHOD_ZERO_0:
                    case RCDmgDefs::METHOD_ZERO_2:
                        realMethod = false;
                        if (block.m_packSize != 0)
                        {
                            opRes = RC_ARCHIVE_EXTRACT_RESULT_UNSUPPORTED_METHOD;
                        }
                        break;

                    case RCDmgDefs::METHOD_COPY:
                        if (block.m_unpSize != block.m_packSize)
                        {
                            opRes = RC_ARCHIVE_EXTRACT_RESULT_UNSUPPORTED_METHOD;
                            break;
                        }
                        if(!copyCoder)
                        {
                            res = RCCreateCoder::CreateCoder(m_compressCodecsInfo.Get(),
                                                            RCMethod::ID_COMPRESS_COPY,
                                                            copyCoder,
                                                            false);
                            if(!IsSuccess(res) || !copyCoder)
                            {
                                opRes = RC_ARCHIVE_EXTRACT_RESULT_UNSUPPORTED_METHOD;
                                break ;
                            }
                        }
                        res = copyCoder->Code(inStream.Get(), outStream.Get(), NULL, NULL, progress.Get());
                        break;
                        
                    case RCDmgDefs::METHOD_ADC:
                        if(!adcCoder)
                        {
                            res = RCCreateCoder::CreateCoder(m_compressCodecsInfo.Get(),
                                                            RCMethod::ID_COMPRESS_ADC,
                                                            adcCoder,
                                                            false);
                            if(!IsSuccess(res) || !adcCoder)
                            {
                                opRes = RC_ARCHIVE_EXTRACT_RESULT_UNSUPPORTED_METHOD;
                                break ;
                            }
                        }
                        res = adcCoder->Code(inStream.Get(), outStream.Get(), &block.m_packSize, &block.m_unpSize, progress.Get()) ;
                        break ;
                    case RCDmgDefs::METHOD_ZLIB:
                        {
                            if(!zlibCoder)
                            {
                                res = RCCreateCoder::CreateCoder(m_compressCodecsInfo.Get(),
                                                                RCMethod::ID_COMPRESS_ZLIB,
                                                                zlibCoder,
                                                                false);
                                if(!IsSuccess(res) || !zlibCoder)
                                {
                                    opRes = RC_ARCHIVE_EXTRACT_RESULT_UNSUPPORTED_METHOD;
                                    break ;
                                }
                            }
                            res = zlibCoder->Code(inStream.Get(), outStream.Get(), NULL, NULL, progress.Get());
                            break;
                        }

                    case RCDmgDefs::METHOD_BZIP2:
                        {
                            if(!bzip2Coder)
                            {
                                res = RCCreateCoder::CreateCoder(m_compressCodecsInfo.Get(),
                                                                RCMethod::ID_COMPRESS_BZIP2,
                                                                bzip2Coder,
                                                                false);
                                if(!IsSuccess(res) || !bzip2Coder)
                                {
                                    opRes = RC_ARCHIVE_EXTRACT_RESULT_UNSUPPORTED_METHOD;
                                    break ;
                                }
                            }
                            res = bzip2Coder->Code(inStream.Get(), outStream.Get(), NULL, NULL, progress.Get());
                            if (res == RC_S_OK)
                            {
                                if (streamSpec->GetSize() != block.m_packSize)
                                {
                                    opRes = RC_ARCHIVE_EXTRACT_RESULT_DATA_ERROR;
                                }
                            }
                            break;
                        }

                    default:
                        opRes = RC_ARCHIVE_EXTRACT_RESULT_UNSUPPORTED_METHOD;
                        break;
                    }
                    if (res != RC_S_OK)
                    {
                        if (res != RC_S_FALSE)
                        {
                            return res ;
                        }
                        if (opRes == RC_ARCHIVE_EXTRACT_RESULT_OK)
                        {
                            opRes = RC_ARCHIVE_EXTRACT_RESULT_DATA_ERROR;
                        }
                    }
                    unpPos += block.m_unpSize;
                    if (!outStreamSpec->IsFinishedOK())
                    {
                        if (realMethod && opRes == RC_ARCHIVE_EXTRACT_RESULT_OK)
                        {
                            opRes = RC_ARCHIVE_EXTRACT_RESULT_DATA_ERROR;
                        }
                        while (outStreamSpec->GetRem() != 0)
                        {
                            uint64_t rem = outStreamSpec->GetRem() ;
                            uint64_t size = std::min(rem, (uint64_t)kZeroBufSize);
                            hr = RCStreamUtils::WriteStream(outStream.Get(), zeroBuf.data(), (size_t)size) ;
                            if(!IsSuccess(hr))
                            {
                                return hr ;
                            }
                        }
                    }
                }
            }
        }
        outStream.Release();
        if(extractCallback)
        {
            hr = extractCallback->SetOperationResult(index,opRes) ;
            if(!IsSuccess(hr))
            {
                return hr ;
            }
        }
    }
    return RC_S_OK ;
}

int32_t RCDmgHandler::Base64ToByte(char c)
{
    if (c >= 'A' && c <= 'Z') return c - 'A';
    if (c >= 'a' && c <= 'z') return c - 'a' + 26;
    if (c >= '0' && c <= '9') return c - '0' + 52;
    if (c == '+') return 62;
    if (c == '/') return 63;
    if (c == '=') return 0;
    return -1;
}

int32_t RCDmgHandler::Base64ToBin(byte_t* dest, const char* src, int32_t srcLen)
{
    int32_t srcPos = 0;
    int32_t destPos = 0;
    while (srcPos < srcLen)
    {
        byte_t buf[4];
        int32_t filled = 0;
        while (srcPos < srcLen)
        {
            int n = Base64ToByte(src[srcPos++]);
            if (n >= 0)
            {
                buf[filled++] = (byte_t)n;
                if (filled == 4)
                {
                    break;
                }
            }
        }
        if (filled >= 2) { if (dest) dest[destPos] = (buf[0] << 2) | (buf[1] >> 4); destPos++; }
        if (filled >= 3) { if (dest) dest[destPos] = (buf[1] << 4) | (buf[2] >> 2); destPos++; }
        if (filled >= 4) { if (dest) dest[destPos] = (buf[2] << 6) | (buf[3]     ); destPos++; }
    }
    return destPos;
}

int32_t RCDmgHandler::FindKeyPair(const RCXmlItem& item, const RCStringA& key, const RCStringA& nextTag)
{
    size_t count = item.GetSubItems().size() ; 
    for (size_t i = 0; i + 1 < count; ++i)
    {
        const RCXmlItem& si = item.GetSubItems()[i];
        if( si.IsTagged("key") && 
            (si.GetSubString() == key) && 
            (item.GetSubItems()[i + 1].IsTagged(nextTag)) )
        {
            return (int32_t)i + 1 ;
        }
    }
    return -1 ;
}

RCStringA RCDmgHandler::GetStringFromKeyPair(const RCXmlItem& item, const RCStringA& key, const RCStringA& nextTag)
{
    int32_t index = FindKeyPair(item, key, nextTag);
    if (index >= 0)
    {
        return item.GetSubItems()[index].GetSubString() ;
    }
    return RCStringA();
}

END_NAMESPACE_RCZIP
