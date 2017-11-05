/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "format/zip/RCZipHandler.h"
#include "format/common/RCItemNameUtils.h"
#include "filesystem/RCFileTime.h"
#include "filesystem/RCStreamUtils.h"
#include "common/RCStringUtil.h"
#include "common/RCFilterCoder.h"
#include "common/RCCreateCoder.h"
#include "interface/RCMethodDefs.h"
#include "crypto/Zip/RCZipDecoder.h"
#include "crypto/WzAES/RCWzAESDecoder.h"
#include "crypto/ZipStrong/RCZipStrongDecoder.h"
#include "common/RCLocalProgress.h"
#include "format/common/RCOutStreamWithCRC.h"
#include "format/common/RCPropData.h"

#include "format/zip/RCZipDefs.h"
#include "thread/RCThread.h"
#include "thread/RCSynchronization.h"
#include "locked/RCMtExtractProgressMixer.h"
#include "locked/RCMtExtractProgress.h"
#include "locked/RCMtLocalProgress.h"
#include "interface/RCArchiveType.h"

/////////////////////////////////////////////////////////////////
//RCZipHandler class implementation

BEGIN_NAMESPACE_RCZIP

const char_t* s_zipHostOS[] =
{
    _T("FAT"),
    _T("AMIGA"),
    _T("VMS"),
    _T("Unix"),
    _T("VM/CMS"),
    _T("Atari"),
    _T("HPFS"),
    _T("Macintosh"),
    _T("Z-System"),
    _T("CP/M"),
    _T("TOPS-20"),
    _T("NTFS"),
    _T("SMS/QDOS"),
    _T("Acorn"),
    _T("VFAT"),
    _T("MVS"),
    _T("BeOS"),
    _T("Tandem"),
    _T("OS/400"),
    _T("OS/X")
};

const int32_t s_zipNumHostOSes = sizeof(s_zipHostOS) / sizeof(s_zipHostOS[0]);

static const char_t* s_zipUnknownOS = _T("Unknown");

static RCPropData s_zipProps[] =
{
    { RCString(_T("")), RCPropID::kpidPath,      RCVariantType::RC_VT_STRING },
    { RCString(_T("")), RCPropID::kpidIsDir,     RCVariantType::RC_VT_BOOL },
    { RCString(_T("")), RCPropID::kpidSize,      RCVariantType::RC_VT_UINT64 },
    { RCString(_T("")), RCPropID::kpidPackSize,  RCVariantType::RC_VT_UINT64 },
    { RCString(_T("")), RCPropID::kpidMTime,     RCVariantType::RC_VT_UINT64 },
    { RCString(_T("")), RCPropID::kpidCTime,     RCVariantType::RC_VT_UINT64 },
    { RCString(_T("")), RCPropID::kpidATime,     RCVariantType::RC_VT_UINT64 },

    { RCString(_T("")), RCPropID::kpidAttrib,    RCVariantType::RC_VT_UINT64 },

    { RCString(_T("")), RCPropID::kpidEncrypted, RCVariantType::RC_VT_BOOL },
    { RCString(_T("")), RCPropID::kpidComment,   RCVariantType::RC_VT_STRING },

    { RCString(_T("")), RCPropID::kpidCRC,       RCVariantType::RC_VT_UINT64 },

    { RCString(_T("")), RCPropID::kpidMethod,    RCVariantType::RC_VT_STRING },
    { RCString(_T("")), RCPropID::kpidHostOS,    RCVariantType::RC_VT_STRING },
    { RCString(_T("")), RCPropID::kpidCommented, RCVariantType::RC_VT_BOOL}
};

static const char_t* s_zipMethods[] =
{
    _T("Store"),
    _T("Shrink"),
    _T("Reduced1"),
    _T("Reduced2"),
    _T("Reduced2"),
    _T("Reduced3"),
    _T("Implode"),
    _T("Tokenizing"),
    _T("Deflate"),
    _T("Deflate64"),
    _T("PKImploding"),
    _T("Unknown"),
    _T("BZip2")
};

const int s_zipNumMethods = sizeof(s_zipMethods) / sizeof(s_zipMethods[0]);

static const char_t* s_zipBZip2Method        = _T("BZip2");
static const char_t* s_zipLZMAMethod         = _T("LZMA");
static const char_t* s_zipJpegMethod         = _T("Jpeg");
static const char_t* s_zipWavPackMethod      = _T("WavPack");
static const char_t* s_zipPPMDMethod         = _T("PPMd");
static const char_t* s_zipAESMethod          = _T("AES");
static const char_t* s_zipCryptoMethod       = _T("ZipCrypto");
static const char_t* s_zipStrongCryptoMethod = _T("StrongCrypto");

struct RCZipStrongCryptoPair
{
    /** id
    */
    uint16_t m_id;

    /** 名字
    */
    const char_t* m_name;
};

static RCZipStrongCryptoPair s_zipStrongCryptoPairs[] =
{
    { RCZipStrongCryptoFlags::kDES,      _T("DES") },
    { RCZipStrongCryptoFlags::kRC2old,   _T("RC2a") },
    { RCZipStrongCryptoFlags::k3DES168,  _T("3DES-168") },
    { RCZipStrongCryptoFlags::k3DES112,  _T("3DES-112") },
    { RCZipStrongCryptoFlags::kAES128,   _T("pkAES-128") },
    { RCZipStrongCryptoFlags::kAES192,   _T("pkAES-192") },
    { RCZipStrongCryptoFlags::kAES256,   _T("pkAES-256") },
    { RCZipStrongCryptoFlags::kRC2,      _T("RC2") },
    { RCZipStrongCryptoFlags::kBlowfish, _T("Blowfish") },
    { RCZipStrongCryptoFlags::kTwofish,  _T("Twofish") },
    { RCZipStrongCryptoFlags::kRC4,      _T("RC4") }
};

static RCPropData s_zipArcProps[] =
{
    { RCString(_T("")), RCPropID::kpidBit64,     RCVariantType::RC_VT_BOOL   },
    { RCString(_T("")), RCPropID::kpidComment,   RCVariantType::RC_VT_STRING },
    { RCString(_T("")), RCPropID::kpidCommented, RCVariantType::RC_VT_BOOL },
    { RCString(_T("")), RCPropID::kpidCommentSizePosition, RCVariantType::RC_VT_UINT64 },
    { RCString(_T("")), RCPropID::kpidArchiveType,    RCVariantType::RC_VT_UINT64 }
};

static RCStringA BytesToString(const RCByteBuffer& data)
{
    RCStringA s;

    int32_t size = (int32_t)data.GetCapacity();
    if (size > 0)
    {
        char* p = new char[size+1];
        memcpy(p, data.data(), size);
        p[size] = '\0';
        s = p;
        delete []p;
    }

    return s;
}

HResult RCZipHandler::GetNumberOfProperties(uint32_t& numProperties)
{ 
    numProperties = sizeof(s_zipProps) / sizeof(s_zipProps[0]);

    return RC_S_OK; 
}

HResult RCZipHandler::GetPropertyInfo(uint32_t index, RCString& name, RCPropertyID& propID, RCVariantType::RCVariantID& varType)
{
    if (index >= sizeof(s_zipProps) / sizeof(s_zipProps[0])) 
    {
        return RC_E_INVALIDARG;
    }
    const RCPropData& srcItem = s_zipProps[index];

    propID  = srcItem.m_propID ; 
    varType = srcItem.m_varType ; 
    name    = srcItem.m_propName ;

    return RC_S_OK;
}

HResult RCZipHandler::GetNumberOfArchiveProperties(uint32_t& numProperties)
{

    numProperties = sizeof(s_zipArcProps) / sizeof(s_zipArcProps[0]);

    return RC_S_OK;
}

HResult RCZipHandler::GetArchivePropertyInfo(uint32_t index, RCString& name, RCPropertyID& propID, RCVariantType::RCVariantID& varType)
{
    if(index >= sizeof(s_zipArcProps) / sizeof(s_zipArcProps[0])) 
    {
        return RC_E_INVALIDARG;
    }

    const RCPropData& srcItem = s_zipArcProps[index];

    propID  = srcItem.m_propID ; 
    varType = srcItem.m_varType ; 
    name    = srcItem.m_propName ;

    return RC_S_OK; 
}

HResult RCZipHandler::GetArchiveProperty(RCPropertyID propID, RCVariant& value)
{
    ClearVariantValue(value) ;
    RCVariant& prop = value;

    switch(propID)
    {
    case RCPropID::kpidBit64:
        {
#ifdef COMPRESS_MT
            if (m_zipInArchive.size() > 0)
            {
                if (m_zipInArchive[0]->m_isZip64)
                {
                    prop = m_zipInArchive[0]->m_isZip64;
                }
            }
            else
            {
                prop = false;
            }
#else
            if (m_zipInArchive.IsZip64)
            {
                prop = m_zipInArchive.IsZip64;
            }
#endif
        }
        break;

    case RCPropID::kpidComment:
        {
#ifdef COMPRESS_MT
            if (m_zipInArchive.size() > 0)
            {
                prop = RCStringConvert::MultiByteToUnicodeString(BytesToString(m_zipInArchive[0]->m_archiveInfo.m_comment), RC_CP_ACP);
            }
            else
            {
                prop = RCString(_T(""));
            }
#else
            prop = RCStringConvert::MultiByteToUnicodeString(BytesToString(m_zipInArchive.m_archiveInfo.m_comment), RC_CP_ACP);
#endif
        }
        break;

    case RCPropID::kpidCommented:
        {
#ifdef COMPRESS_MT
            if (m_zipInArchive.size() > 0)
            {
                prop = m_zipInArchive[0]->m_archiveInfo.m_comment.GetCapacity() > 0 ? true : false ;
            }
            else
            {
                prop = false ;
            }
#else
            prop = m_zipInArchive.m_archiveInfo.m_comment.GetCapacity() > 0 ? true : false ;
#endif
        }
        break ;

    case RCPropID::kpidCommentSizePosition:
        {
#ifdef COMPRESS_MT
            if (m_zipInArchive.size() > 0)
            {
                prop = m_zipInArchive[0]->m_archiveInfo.m_commentSizePosition ;
            }
            else
            {
                prop = (uint64_t)0;
            }
#else
            prop = m_zipInArchive.m_archiveInfo.m_commentSizePosition ;
#endif
        }
        break ;
    case RCPropID::kpidArchiveType:
        {
            prop = uint64_t(RC_ARCHIVE_ZIP) ;
            break;
        }
    }

    return RC_S_OK;
}

HResult RCZipHandler::GetNumberOfItems(uint32_t& numItems)
{
    numItems = (uint32_t)m_zipItems.size();

    return RC_S_OK;
}

HResult RCZipHandler::GetProperty(uint32_t index, RCPropertyID propID, RCVariant& value)
{
    ClearVariantValue(value) ;
    if(index >= (uint32_t)m_zipItems.size())
    {
        return RC_E_INVALIDARG ;
    }
    RCVariant& prop = value;
    const RCZipItemExPtr &item = m_zipItems[index];

    switch(propID)
    {
    case RCPropID::kpidPath:
        {
            prop = RCItemNameUtils::GetOSName2(item->GetUnicodeString(item->m_name));
        }
        break;

    case RCPropID::kpidIsDir:
        {
            prop = item->IsDir();
        }
        break;

    case RCPropID::kpidSize:
        {
            prop = item->m_unPackSize;
        }
        break;

    case RCPropID::kpidPackSize:
        {
            prop = item->m_packSize;
        }
        break;

    case RCPropID::kpidTimeType:
        {
            
            RC_FILE_TIME ft ;
            uint32_t unixTime = 0 ;
            if (item->m_centralExtra.GetNtfsTime(RCZipHeader::NNtfsExtra::kMTime, ft))
            {
                prop = (uint64_t)RC_ARCHIVE_FILE_TIME_TYPE_WINDOWS ;
            }
            else if (item->m_centralExtra.GetUnixTime(RCZipHeader::NUnixTime::kMTime, unixTime))
            {
                prop = (uint64_t)RC_ARCHIVE_FILE_TIME_TYPE_UNIX ;
            }
            else
            {
                prop = (uint64_t)RC_ARCHIVE_FILE_TIME_TYPE_DOS ;
            }
            break ;
        }
        break;

    case RCPropID::kpidCTime:
        {
            RC_FILE_TIME ft;
            if (item->m_centralExtra.GetNtfsTime(RCZipHeader::NNtfsExtra::kCTime, ft))
            {
                prop = RCFileTime::ConvertFromFileTime(ft);
            }
        }
        break;

    case RCPropID::kpidATime:
        {
            RC_FILE_TIME ft;
            if (item->m_centralExtra.GetNtfsTime(RCZipHeader::NNtfsExtra::kATime, ft))
            {
                prop = RCFileTime::ConvertFromFileTime(ft);
            }
        }
        break;

    case RCPropID::kpidMTime:
        {
            RC_FILE_TIME utc;
            if (!item->m_centralExtra.GetNtfsTime(RCZipHeader::NNtfsExtra::kMTime, utc))
            {
                uint32_t unixTime = 0 ;
                if (item->m_centralExtra.GetUnixTime(RCZipHeader::NUnixTime::kMTime, unixTime))
                {
                    RCFileTime::UnixTimeToFileTime(unixTime, utc) ;
                }
                else
                {
                    RC_FILE_TIME localFileTime ;
                    if (!RCFileTime::DosTimeToFileTime(item->m_time, localFileTime) ||
                        !RCFileTime::LocalFileTimeToFileTime(localFileTime, utc) )
                    {
                        utc.u32HighDateTime = 0;
                        utc.u32LowDateTime  = 0;
                    }
                }
            }
            prop = RCFileTime::ConvertFromFileTime(utc) ;
        }
        break;

    case RCPropID::kpidAttrib:
        {
            prop = (uint64_t)item->GetWinAttributes();
        }
        break;

    case RCPropID::kpidEncrypted:
        {
            prop = item->IsEncrypted();
        }
        break;

    case RCPropID::kpidComment:
        {
            prop = item->GetUnicodeString(BytesToString(item->m_comment));
        }
        break;

    case RCPropID::kpidCommented:
        {
            prop = item->m_comment.GetCapacity() > 0 ? true : false ;
        }
        break;

    case RCPropID::kpidCRC:
        {
            if (item->IsThereCrc())
            {
                prop = (uint64_t)item->m_fileCRC;
            }
        }
        break;

    case RCPropID::kpidMethod:
        {
            uint16_t methodId = item->m_compressionMethod;
            RCString method;

            if (item->IsEncrypted())
            {
                if (methodId == RCZipHeader::NCompressionMethod::kWzAES)
                {
                    method = s_zipAESMethod;
                    RCZipWzAESExtraField aesField;

                    if (item->m_centralExtra.GetWzAESField(aesField))
                    {
                        method += _T('-');
                        char_t s[32];
                        RCStringUtil::ConvertUInt64ToString((aesField.m_strength + 1) * 64 , s);
                        method += s;
                        method += _T(' ');
                        methodId = aesField.m_method;
                    }
                }
                else
                {
                    if (item->IsStrongEncrypted())
                    {
                        RCZipStrongCryptoField f;
                        bool finded = false;

                        if (item->m_centralExtra.GetStrongCryptoField(f))
                        {
                            for (int32_t i = 0; i < sizeof(s_zipStrongCryptoPairs) / sizeof(s_zipStrongCryptoPairs[0]); i++)
                            {
                                const RCZipStrongCryptoPair &pair = s_zipStrongCryptoPairs[i];
                                if (f.m_algId == pair.m_id)
                                {
                                    method += pair.m_name;
                                    finded = true;
                                    break;
                                }
                            }
                        }

                        if (!finded)
                        {
                            method += s_zipStrongCryptoMethod;
                        }
                    }
                    else
                    {
                        method += s_zipCryptoMethod;
                    }

                    method += _T(' ');
                }
            }

            if (methodId < s_zipNumMethods)
            {
                method += s_zipMethods[methodId];
            }
            else switch(methodId)
            {

                case RCZipHeader::NCompressionMethod::kLZMA:
                    {
                        method += s_zipLZMAMethod;
                        if (item->IsLzmaEOS())
                        {
                            method += _T(":EOS");
                        }
                    }
                    break;

                case RCZipHeader::NCompressionMethod::kBZip2:
                    {
                        method += s_zipBZip2Method;
                    }
                    break;

                case RCZipHeader::NCompressionMethod::kJpeg:
                    {
                        method += s_zipJpegMethod;
                    }
                    break;

                case RCZipHeader::NCompressionMethod::kWavPack:
                    {
                        method += s_zipWavPackMethod;
                    }
                    break;

                case RCZipHeader::NCompressionMethod::kPPMd:
                    {
                        method += s_zipPPMDMethod;
                    }
                    break;

                default:
                    {
                        char_t s[32];
                        RCStringUtil::ConvertUInt64ToString(methodId, s);
                        method += s;
                    }
            }
            prop = method;
        }
        break;

    case RCPropID::kpidHostOS:
        {
            prop = RCString((item->m_madeByVersion.m_hostOS < s_zipNumHostOSes) ?
                            (s_zipHostOS[item->m_madeByVersion.m_hostOS]) : s_zipUnknownOS);
        }
        break;
    }

    return RC_S_OK;
}

class RCZipProgressImp:
    public RCZipProgressVirt
{
public:

    /** 默认构造函数
    */
    RCZipProgressImp(IArchiveOpenCallback* callback):
      m_callback(callback)
    {
    }

public:

    /** 总体数量
    @param [in] numFiles 总体的数量
    @return 成功返回RC_S_OK, 否则返回错误码
    */
    virtual HResult SetTotal(uint64_t numFiles) ;

    /** 当前完成
    @param [in] numFiles 当前完成的数量
    @return 成功返回RC_S_OK, 否则返回错误码
    */
    virtual HResult SetCompleted(uint64_t numFiles) ;

private:

    /** 文档打开回调接口
    */
    IArchiveOpenCallbackPtr m_callback ;
};

HResult RCZipProgressImp::SetTotal(uint64_t numFiles)
{
    if (m_callback)
    {
        return m_callback->SetTotal(numFiles, static_cast<uint64_t>(-1));
    }
    return RC_S_OK;
}

HResult RCZipProgressImp::SetCompleted(uint64_t numFiles)
{
    if (m_callback)
    {
        return m_callback->SetCompleted(numFiles, static_cast<uint64_t>(-1));
    }
    return RC_S_OK;
}

HResult RCZipHandler::Open(IInStream* stream, uint64_t maxCheckStartPosition, IArchiveOpenCallback* openArchiveCallback)
{
    Close();

    HResult hr = stream->Seek(0, RC_STREAM_SEEK_SET, NULL);
    if (!IsSuccess(hr))
    {
        return hr;
    }

#ifdef COMPRESS_MT
    for (uint32_t i = 0; i < (uint32_t)m_numThreads && i < RCZipDefs::s_maxThreadNums; i++)
    {
        ICloneStreamPtr spCloneStream ;
        hr = stream->QueryInterface(IID_ICloneStream,(void**)spCloneStream.GetAddress()) ;
        if (!IsSuccess(hr) || !spCloneStream)
        {
            return RC_S_FALSE;
        }
        IUnknown* cloneStream = spCloneStream->Clone() ;
        if(NULL == cloneStream)
        {
            return RC_S_FALSE ;
        }
        IInStreamPtr spInStream((IInStream*)cloneStream) ;

        RCZipInPtr archive(new RCZipIn);
        hr = archive->Open(spInStream.Get(), &maxCheckStartPosition);
        if (!IsSuccess(hr))
        {
            return hr;
        }
        m_spInStream.push_back(spInStream);
        m_zipInArchive.push_back(archive);
    }

    if (m_zipInArchive.size() < 1)
    {
        return RC_S_FALSE;
    }
#else
    hr = m_zipInArchive.Open(stream, &maxCheckStartPosition);
    if (!IsSuccess(hr))
    {
        return hr;
    }
#endif

    RCZipProgressImp progressImp(openArchiveCallback);

#ifdef COMPRESS_MT
    for (uint32_t i = 0; i < m_zipInArchive.size(); i++)
    {
        hr = m_zipInArchive[i]->ReadHeaders(m_zipItems, &progressImp);
        if (!IsSuccess(hr))
        {
            return hr;
        }
    }
    return hr;
#else
    return m_zipInArchive.ReadHeaders(m_zipItems, &progressImp);
#endif
}

HResult RCZipHandler::Close()
{
    m_zipItems.clear();

 #ifdef COMPRESS_MT
    for (uint32_t i = 0; i < (uint32_t)m_zipInArchive.size(); i++)
    {
        m_zipInArchive[i]->Close();
    }
    m_spInStream.clear();
#else
    m_zipInArchive.Close();
#endif

    return RC_S_OK;
}

class RCZipPPMDI1Decoder:
    public IUnknownImpl<ICompressCoder>
{
public:

    /** 压缩/解压数据
    @param [in] inStream 数据输入流
    @param [in] outStream 数据输出流
    @param [in] inSize 输入数据大小
    @param [in] outSize 输出数据大小 
    @param [in] progress 进度回调接口
    @return 成功返回RC_S_OK，否则返回错误号 
    */
    virtual HResult Code(ISequentialInStream* inStream,
                         ISequentialOutStream* outStream, 
                         const uint64_t* inSize, 
                         const uint64_t* outSize,
                         ICompressProgressInfo* progress) ;

public:
   
    /** 设置编码解码器管理接口指针
    @param [in] compressCodecsInfo 编码解码器指针
    @return 成功返回RC_S_OK，否则返回错误号 
    */
    HResult SetCompressCodecsInfo(ICompressCodecsInfo* compressCodecsInfo) ;

private:
     
    /** 编码解码器管理接口指针
    */
    ICompressCodecsInfoPtr m_compressCodecsInfo ;

    ICompressCoderPtr m_compressDecoder ;
};

HResult RCZipPPMDI1Decoder::Code(ISequentialInStream* inStream,
                               ISequentialOutStream* outStream, 
                               const uint64_t* inSize, 
                               const uint64_t* outSize,
                               ICompressProgressInfo* progress)
{
    if (!m_compressDecoder)
    {
        HResult hr = RCCreateCoder::CreateCoder(m_compressCodecsInfo.Get(),
                                                RCMethod::ID_COMPRESS_PPMDI1,
                                                m_compressDecoder,
                                                false);
        if (!IsSuccess(hr))
        {
            return hr ;
        }
            
        if (!m_compressDecoder)
        {
            return RC_E_NOTIMPL;
        }
    }

    uint16_t ppmd_set;
    byte_t buf[5];

    HResult hr = RCStreamUtils::ReadStream_FAIL(inStream, &ppmd_set, sizeof(ppmd_set));
    if (!IsSuccess(hr)) 
    {
        return hr;
    }
    buf[0] = (byte_t)(ppmd_set & 0xF) + 1;
    byte_t tt = (byte_t)((ppmd_set & 0x0FF0) >> 4) + 1;
    buf[1] = tt;
    buf[2] = 0;
    buf[3] = (byte_t)((ppmd_set & 0xF000) >> 12);
    

    ICompressSetDecoderProperties2Ptr setDecoderProperties;
    m_compressDecoder->QueryInterface(IID_ICompressSetDecoderProperties2, (void **)setDecoderProperties.GetAddress());
    if (setDecoderProperties)
    {
        hr = setDecoderProperties->SetDecoderProperties2(buf, 5);
        if (!IsSuccess(hr))
        {
            return hr ;
        }
    }
    return m_compressDecoder->Code(inStream, outStream, NULL, outSize, progress);
}

HResult RCZipPPMDI1Decoder::SetCompressCodecsInfo(ICompressCodecsInfo* compressCodecsInfo)
{
    if (!compressCodecsInfo)
    {
        return RC_S_FALSE;
    }
    m_compressCodecsInfo = compressCodecsInfo;
    return RC_S_OK;
}

class RCZipLzmaDecoder:
    public IUnknownImpl<ICompressCoder>
{
public:

    /** 压缩/解压数据
    @param [in] inStream 数据输入流
    @param [in] outStream 数据输出流
    @param [in] inSize 输入数据大小
    @param [in] outSize 输出数据大小 
    @param [in] progress 进度回调接口
    @return 成功返回RC_S_OK，否则返回错误号 
    */
    virtual HResult Code(ISequentialInStream* inStream,
                         ISequentialOutStream* outStream, 
                         const uint64_t* inSize, 
                         const uint64_t* outSize,
                         ICompressProgressInfo* progress) ;

public:
   
    /** 设置编码解码器管理接口指针
    @param [in] compressCodecsInfo 编码解码器指针
    @return 成功返回RC_S_OK，否则返回错误号 
    */
    HResult SetCompressCodecsInfo(ICompressCodecsInfo* compressCodecsInfo) ;

private:
     
    /** 编码解码器管理接口指针
    */
    ICompressCodecsInfoPtr m_compressCodecsInfo ;

    /** 编码解码器
    */
    ICompressCoderPtr m_compressDecoder ;
};

HResult RCZipLzmaDecoder::Code(ISequentialInStream* inStream,
                               ISequentialOutStream* outStream, 
                               const uint64_t* inSize, 
                               const uint64_t* outSize,
                               ICompressProgressInfo* progress)
{
    if (!m_compressDecoder)
    {
        HResult hr = RCCreateCoder::CreateCoder(m_compressCodecsInfo.Get(),
                                                RCMethod::ID_COMPRESS_LZMA,
                                                m_compressDecoder,
                                                false);
        if (!IsSuccess(hr))
        {
            return hr ;
        }
            
        if (!m_compressDecoder)
        {
            return RC_E_NOTIMPL;
        }
    }

    byte_t buf[9];
    HResult hr = RCStreamUtils::ReadStream_FALSE(inStream, buf, 9);
    if (buf[2] != 5 || buf[3] != 0)
    {
        return RC_E_NOTIMPL;
    }

    ICompressSetDecoderProperties2Ptr setDecoderProperties;
    m_compressDecoder->QueryInterface(IID_ICompressSetDecoderProperties2, (void **)setDecoderProperties.GetAddress());
    if (setDecoderProperties)
    {
        hr = setDecoderProperties->SetDecoderProperties2(buf + 4, 5);
        if (!IsSuccess(hr))
        {
            return hr ;
        }
    }

    return m_compressDecoder->Code(inStream, outStream, NULL, outSize, progress);
}

HResult RCZipLzmaDecoder::SetCompressCodecsInfo(ICompressCodecsInfo* compressCodecsInfo)
{
    if (!compressCodecsInfo)
    {
        return RC_S_FALSE;
    }

    m_compressCodecsInfo = compressCodecsInfo;

    return RC_S_OK;
}

struct RCZipMethodItem
{
    /** 方法
    */
    uint16_t m_zipMethod;

    /** 压缩器
    */
    ICompressCoderPtr m_coder;
};

class RCZipAddDecoder
{
public:

    /** 默认构造函数
    */
    RCZipAddDecoder(): 
      m_zipCryptoDecoderSpec(0),
      m_pkAesDecoderSpec(0),
      m_wzAesDecoderSpec(0),
      m_filterStreamSpec(0) {}

public:

    /** 解压
    @param [in] codecsInfo 编码管理器
    @param [in] archive 文档
    @param [in] item 解压项信息
    @param [in] realOutStream 输出流
    @param [in] extractCallback 解压回调接口
    @param [in] compressProgress 进度回调接口
    @param [in] numThreads 线程数
    @param [in] res 解压结果
    */
    HResult Decode(ICompressCodecsInfo* codecsInfo,
                   RCZipIn& archive,
                   const RCZipItemExPtr& item,
                   ISequentialOutStream* realOutStream,
                   IArchiveExtractCallback* extractCallback,
                   ICompressProgressInfo* compressProgress,
                   uint32_t numThreads,
                   int32_t &res);

private:

    /** Zip解码器
    */
    RCZipDecoder* m_zipCryptoDecoderSpec;

    /** ZipStrong解码器
    */
    RCZipStrongDecoder* m_pkAesDecoderSpec;

    /** WzAES解码器
    */
    RCWzAESDecoder* m_wzAesDecoderSpec;

    /** zip过滤器
    */
    ICompressFilterPtr m_zipCryptoDecoder;

    /** pkAes过滤器
    */
    ICompressFilterPtr m_pkAesDecoder;

    /** wzAes过滤器
    */
    ICompressFilterPtr m_wzAesDecoder;

    /** 过滤器
    */
    RCFilterCoder* m_filterStreamSpec;

    /** 过滤流
    */
    ISequentialInStreamPtr m_filterStream;

    /** 密码管理器
    */
    ICryptoGetTextPasswordPtr m_getTextPassword;

    /** 模式
    */
    RCVector<RCZipMethodItem> m_methodItems;
};

HResult RCZipAddDecoder::Decode(ICompressCodecsInfo* codecsInfo,
                                RCZipIn& archive,
                                const RCZipItemExPtr& item,
                                ISequentialOutStream* realOutStream,
                                IArchiveExtractCallback* extractCallback,
                                ICompressProgressInfo* compressProgress,
                                uint32_t numThreads,
                                int32_t &res)
{
    res = RC_ARCHIVE_EXTRACT_RESULT_DATA_ERROR;
    RCInStreamReleaser inStreamReleaser;
    bool needCRC = true;
    bool wzAesMode = false;
    bool pkAesMode = false;
    HResult hr;

    uint16_t methodId = item->m_compressionMethod;
    if (item->IsEncrypted())
    {
        if (item->IsStrongEncrypted())
        {
            RCZipStrongCryptoField f;
            if (item->m_centralExtra.GetStrongCryptoField(f))
            {
                pkAesMode = true;
            }
            if (!pkAesMode)
            {
                res = RC_ARCHIVE_EXTRACT_RESULT_UNSUPPORTED_METHOD;
                return RC_S_OK;
            }
        }

        if (methodId == RCZipHeader::NCompressionMethod::kWzAES)
        {
            RCZipWzAESExtraField aesField;
            if (item->m_centralExtra.GetWzAESField(aesField))
            {
                wzAesMode = true;
                needCRC = aesField.NeedCrc();
            }
        }
    }

    RCOutStreamWithCRC *outStreamSpec = new RCOutStreamWithCRC;
    ISequentialOutStreamPtr outStream = outStreamSpec;
    outStreamSpec->SetStream(realOutStream);
    outStreamSpec->Init(needCRC);

    uint64_t authenticationPos;

    ISequentialInStreamPtr inStream;
    {
        uint64_t packSize = item->m_packSize;
        if (wzAesMode)
        {
            if (packSize < RCWzAESDefs::s_kMacSize)
            {
                return RC_S_OK;
            }
            packSize -= RCWzAESDefs::s_kMacSize;
        }
        uint64_t dataPos = item->GetDataPosition();
        inStream.Attach(archive.CreateLimitedStream(dataPos, packSize));
        authenticationPos = dataPos + packSize;
    }

    ICompressFilterPtr cryptoFilter;
    if (item->IsEncrypted())
    {
        if (wzAesMode)
        {
            RCZipWzAESExtraField aesField;
            if (!item->m_centralExtra.GetWzAESField(aesField))
            {
                return RC_S_OK;
            }

            methodId = aesField.m_method;
            if (!m_wzAesDecoder)
            {
                ICompressFilterPtr filter;
                hr = RCCreateCoder::CreateFilter(codecsInfo,
                                                 RCMethod::ID_CRYPTO_WZ_AES,
                                                 filter,
                                                 false);
                if (hr != RC_S_OK)
                {
                    return hr;
                }

                m_wzAesDecoderSpec = dynamic_cast<RCWzAESDecoder*>(filter.Get());
                m_wzAesDecoder     = m_wzAesDecoderSpec;
            }

            cryptoFilter = m_wzAesDecoder;

            byte_t properties = aesField.m_strength;

            hr = m_wzAesDecoderSpec->SetDecoderProperties2(&properties, 1);
            if (hr != RC_S_OK)
            {
                return hr;
            }
        }
        else if (pkAesMode)
        {
            if (!m_pkAesDecoder)
            {
                m_pkAesDecoderSpec = new RCZipStrongDecoder;
                m_pkAesDecoder = m_pkAesDecoderSpec;
            }
            cryptoFilter = m_pkAesDecoder;
        }
        else
        {
            if (!m_zipCryptoDecoder)
            {
                ICompressFilterPtr filter;
                hr = RCCreateCoder::CreateFilter(codecsInfo,
                                                 RCMethod::ID_CRYPTO_ZIP,
                                                 filter,
                                                 false);
                if (hr != RC_S_OK)
                {
                    return hr;
                }

                m_zipCryptoDecoderSpec = dynamic_cast<RCZipDecoder*>(filter.Get());
                m_zipCryptoDecoder = m_zipCryptoDecoderSpec;
            }

            cryptoFilter = m_zipCryptoDecoder;
        }

        ICryptoSetPasswordPtr cryptoSetPassword;

        hr = cryptoFilter.QueryInterface(IID_ICryptoSetPassword, cryptoSetPassword.GetAddress());
        if (hr != RC_S_OK)
        {
            return hr;
        }

        if (!m_getTextPassword)
        {
            extractCallback->QueryInterface(IID_ICryptoGetTextPassword, (void **)m_getTextPassword.GetAddress());
        }

        if (m_getTextPassword)
        {
            RCString password;
            hr = m_getTextPassword->CryptoGetTextPassword(password);
            if (hr != RC_S_OK)
            {
                return hr;
            }

            RCStringA charPassword;
            if (wzAesMode || pkAesMode)
            {
                charPassword = RCStringConvert::UnicodeStringToMultiByte(password, RC_CP_ACP);
            }
            else
            {
                // we use OEM. WinZip/Windows probably use ANSI for some files
                charPassword = RCStringConvert::UnicodeStringToMultiByte(password, RC_CP_OEMCP);
            }
            hr = cryptoSetPassword->CryptoSetPassword((const byte_t *)charPassword.c_str(),
                                                      (uint32_t)charPassword.length());
            if (hr != RC_S_OK)
            {
                return RC_S_OK;
            }
        }
        else
        {
            hr = cryptoSetPassword->CryptoSetPassword(0, 0);
            if (hr != RC_S_OK)
            {
                return RC_S_OK;
            }
        }
    }

    int32_t m;
    for (m = 0; m < (int32_t)m_methodItems.size(); m++)
    {
        if (m_methodItems[m].m_zipMethod == methodId)
        {
            break;
        }
    }

    if (m == (int32_t)m_methodItems.size())
    {
        RCZipMethodItem mi;
        mi.m_zipMethod = methodId;

        RCMethodID szMethodID = 0;
        bool isLzma = false;
        bool isPPMD = false;

        switch (methodId)
        {
        case RCZipHeader::NCompressionMethod::kStored:
            {
                szMethodID = RCMethod::ID_COMPRESS_COPY;
            }
            break;

        case RCZipHeader::NCompressionMethod::kShrunk:
            {
                szMethodID = RCMethod::ID_COMPRESS_SHRINK;
            }
            break;

        case RCZipHeader::NCompressionMethod::kImploded:
            {
                szMethodID =  RCMethod::ID_COMPRESS_IMPLODE;
            }
            break;

        case RCZipHeader::NCompressionMethod::kBZip2:
            {
                szMethodID = RCMethod::ID_COMPRESS_BZIP2;
            }
            break;

        case RCZipHeader::NCompressionMethod::kDeflated:
            {
                szMethodID = RCMethod::ID_COMPRESS_DEFLATE_COM;
            }
            break;

        case RCZipHeader::NCompressionMethod::kDeflated64:
            {
                szMethodID = RCMethod::ID_COMPRESS_DEFLATE_COM_64;
            }
            break;
        case RCZipHeader::NCompressionMethod::kWavPack:
            {
                szMethodID = RCMethod::ID_COMPRESS_WAVPACK;
            }
            break;
        case RCZipHeader::NCompressionMethod::kPPMd:
            {
                //szMethodID = RCMethod::ID_COMPRESS_PPMD;
                isPPMD = true;
            }
            break;

        case RCZipHeader::NCompressionMethod::kLZMA:
            {
                isLzma = true;
            }
            break;
            
            // 没找到对应的解码
        default:
            {
                res = RC_ARCHIVE_EXTRACT_RESULT_UNSUPPORTED_METHOD;
                return RC_S_FALSE;
            }
        }

        if (isLzma)
        {
            RCZipLzmaDecoder* zipLzmaDecoderSpc = new RCZipLzmaDecoder;
            mi.m_coder = zipLzmaDecoderSpc;
            zipLzmaDecoderSpc->SetCompressCodecsInfo(codecsInfo);
        }
        else if (isPPMD)
        {
            RCZipPPMDI1Decoder* zipPpmdDecoderSpc = new RCZipPPMDI1Decoder;
            mi.m_coder = zipPpmdDecoderSpc;
            zipPpmdDecoderSpc->SetCompressCodecsInfo(codecsInfo);
        }
        else
        {
            hr = RCCreateCoder::CreateCoder(codecsInfo,
                                            szMethodID,
                                            mi.m_coder,
                                            false);
            if (hr != RC_S_OK)
            {
                return hr;
            }
        }

        if (mi.m_coder == 0)
        {
            res = RC_ARCHIVE_EXTRACT_RESULT_UNSUPPORTED_METHOD;
            return RC_S_OK;
        }

        m_methodItems.push_back(mi);

        m = (int32_t)m_methodItems.size() - 1;
    }

    ICompressCoder *coder = m_methodItems[m].m_coder.Get();

    {
        ICompressSetDecoderProperties2Ptr setDecoderProperties;
        coder->QueryInterface(IID_ICompressSetDecoderProperties2, (void **)setDecoderProperties.GetAddress());
        if (setDecoderProperties)
        {
            byte_t properties = (byte_t)item->m_flags;

            hr = setDecoderProperties->SetDecoderProperties2(&properties, 1);
            if (hr != RC_S_OK)
            {
                return hr;
            }
        }
    }

#ifdef COMPRESS_MT
    {
        ICompressSetCoderMtPtr setCoderMt;
        coder->QueryInterface(IID_ICompressSetCoderMt, (void **)setCoderMt.GetAddress());
        if (setCoderMt)
        {
            hr = setCoderMt->SetNumberOfThreads(numThreads);
            if (hr != RC_S_OK)
            {
                return hr;
            }
        }
    }
#endif

    {
        HResult hr = RC_S_OK ;

        ISequentialInStreamPtr inStreamNew;
        if (item->IsEncrypted())
        {
            if (!m_filterStream)
            {
                m_filterStreamSpec = new RCFilterCoder;
                m_filterStream = m_filterStreamSpec;
            }

            m_filterStreamSpec->SetFilter(cryptoFilter.Get());
            if (wzAesMode)
            {
                hr = m_wzAesDecoderSpec->ReadHeader(inStream.Get());
                if (hr != RC_S_OK)
                {
                    return hr;
                }
            }
            else if (pkAesMode)
            {
                hr = m_pkAesDecoderSpec->ReadHeader(inStream.Get(), item->m_fileCRC, item->m_unPackSize);
                if (hr == RC_S_OK)
                {
                    bool isPasswordOK;
                    hr = m_pkAesDecoderSpec->CheckPassword(isPasswordOK);
                    if (hr == RC_S_OK && !isPasswordOK)
                    {
                        hr = RC_S_FALSE;
                    }
                }
            }
            else
            {
                hr = m_zipCryptoDecoderSpec->ReadHeader(inStream.Get());
                if (hr != RC_S_OK)
                {
                    return hr;
                }
            }
     
            if (hr == RC_S_OK)
            {
                hr = m_filterStreamSpec->SetInStream(inStream.Get());
                if (hr != RC_S_OK)
                {
                    return hr;
                }

                inStreamReleaser.SetFilterCoder(m_filterStreamSpec);
                inStreamNew = m_filterStream;

                if (wzAesMode)
                {
                    if (!m_wzAesDecoderSpec->CheckPasswordVerifyCode())
                    {
                        return RC_S_OK;
                    }
                }
            }
        }
        else
        {
            inStreamNew = inStream;
        }
    
        if (hr == RC_S_OK)
        {
            hr = coder->Code(inStreamNew.Get(), outStream.Get(), NULL, &item->m_unPackSize, compressProgress);
        }

        if (hr == RC_S_FALSE)
        {
            return RC_S_OK;
        }

        if (hr == RC_E_NOTIMPL)
        {
            res = RC_ARCHIVE_EXTRACT_RESULT_UNSUPPORTED_METHOD;
            return RC_S_OK;
        }

        if (hr == RC_E_ABORT)
        {
            res = RC_ARCHIVE_EXTRACT_RESULT_OK;
            return hr;
        }

        if (hr != RC_S_OK)
        {
            return hr;
        }
    }

    bool crcOK = true;
    bool authOk = true;

    if (needCRC)
    {
        crcOK = (outStreamSpec->GetCRC() == item->m_fileCRC);
    }

    if (wzAesMode)
    {
        inStream.Attach(archive.CreateLimitedStream(authenticationPos, RCWzAESDefs::s_kMacSize));
        if (m_wzAesDecoderSpec->CheckMac(inStream.Get(), authOk) != RC_S_OK)
        {
            authOk = false;
        }
    }

    res = ((crcOK && authOk) ?
        RC_ARCHIVE_EXTRACT_RESULT_OK :
    RC_ARCHIVE_EXTRACT_RESULT_CRC_ERROR);

    return RC_S_OK;
}

#ifdef COMPRESS_MT

static void ZipDecoderThread(void *threadDecoderInfo);

struct RCZipDecoderThreadInfo
{
    /** 解码管理器
    */
    ICompressCodecsInfo* m_decompressCodecsInfo;

    /** 文档
    */
    RCZipInPtr m_archive;

    /** 项扩展信息
    */
    RCZipItemExPtr m_item;

    /** 输出流
    */
    ISequentialOutStreamPtr m_realOutStream;

    /** 解压回调接口
    */
    IArchiveExtractCallbackPtr m_extractCallback;

    /** 解压进度信息
    */
    ICompressProgressInfoPtr m_decompressProgress;

    /** 线程个数
    */
    uint32_t m_numThreads;

    /** 结果
    */
    int32_t m_result;

    /** 解压结果
    */
    int32_t m_decompressingResult;

    /** 是否是空闲线程
    */
    volatile bool m_isFree;

    /** 线程
    */
    RCThreadPtr m_thread;

    /** 解压事件
    */
    RCAutoResetEvent m_decompressEvent;

    /** 解压完成事件 
    */
    RCAutoResetEvent m_decompressionCompletedEvent;

    /** 线程是否退出
    */
    volatile bool m_exitThread;

    /** 项索引
    */
    uint32_t m_itemIndex ;

    /** 默认构造函数
    */
    RCZipDecoderThreadInfo():
        m_decompressCodecsInfo(NULL),
        m_numThreads(0),
        m_result(0),
        m_decompressingResult(0),
        m_isFree(true),        
        m_exitThread(false),
        m_itemIndex(0)
    {
    }

    /** 创建事件
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult CreateEvents()
    {
        HResult hr = m_decompressEvent.CreateIfNotCreated();
        if (!IsSuccess(hr))
        {
            return hr;
        }
        return m_decompressionCompletedEvent.CreateIfNotCreated();
    }

    /** 创建线程
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult CreateThread() 
    { 
        RCThread* thread = new RCThread(&ZipDecoderThread, this);
        if (!thread)
        {
            return RC_S_FALSE;
        }

        m_thread.reset(thread);

        if (thread->Start())
        {
            return RC_S_OK;
        }
        return RC_S_FALSE;
    }

    /** 等待解压
    */
    void WaitAndDecode() ;

    /** 停止并等待关闭
    */
    void StopWaitClose()
    {
        m_exitThread = true;
        if (m_decompressEvent.IsCreated())
        {
            m_decompressEvent.Set();
        }
        m_thread->Join();
    }
};

void RCZipDecoderThreadInfo::WaitAndDecode()
{
    for (;;)
    {
        m_decompressEvent.Lock();
        if (m_exitThread)
        {
            break;
        }
        
        RCZipAddDecoder myDecoder;
        m_result = myDecoder.Decode(m_decompressCodecsInfo,
                                    *m_archive.get(),
                                    m_item,
                                    m_realOutStream.Get(),
                                    m_extractCallback.Get(),
                                    m_decompressProgress.Get(),
                                    m_numThreads,
                                    m_decompressingResult);
        if (m_result == RC_E_ABORT)
        {
            //解压被取消，防止弹出诊断框
            break ;
        }
        
        HResult hr = m_extractCallback->SetOperationResult(m_itemIndex,m_decompressingResult);
        if (hr != RC_S_OK)
        {
            break;
        }
        m_decompressionCompletedEvent.Set();
    }
    m_decompressionCompletedEvent.Set();
}

static void ZipDecoderThread(void *threadDecoderInfo)
{
    ((RCZipDecoderThreadInfo *)threadDecoderInfo)->WaitAndDecode();
    return;
}

class RCZipDecoderThreads
{
public:

    /** 默认析构函数
    */
    ~RCZipDecoderThreads()
    {
        for (int32_t i = 0; i < (int32_t)m_threads.size(); i++)
        {
            m_threads[i].StopWaitClose();
        }
    }

public:

    /** 解码线程信息
    */
    RCVector<RCZipDecoderThreadInfo> m_threads;
};

#endif

HResult RCZipHandler::Extract(const std::vector<uint32_t>& indices,
                              int32_t testMode,
                              IArchiveExtractCallback* extractCallback)
{
    if(extractCallback)
    {
        extractCallback->SetMultiThreadMode(false) ;
    }
    RCZipAddDecoder myDecoder;

    bool btestMode = (testMode != 0);
    uint64_t totalUnPacked = 0;
    uint64_t totalPacked   = 0;
    uint32_t numItems = (uint32_t)indices.size();

    bool allFilesMode = (numItems == uint32_t(0));

    if (allFilesMode)
    {
        numItems = (uint32_t)m_zipItems.size();
    }

    if (numItems == 0)
    {
        return RC_S_OK;
    }

    uint32_t i;
    for(i = 0; i < numItems; i++)
    {
        const RCZipItemExPtr &item = m_zipItems[allFilesMode ? i : indices[i]];

        totalUnPacked += item->m_unPackSize;
        totalPacked   += item->m_packSize;
    }

    HResult hr = extractCallback->SetTotal(totalUnPacked);
    if (hr != RC_S_OK)
    {
        return hr;
    }

#ifdef COMPRESS_MT
    //是否有输入流
    if (m_zipInArchive.size() < 1)
    {
        hr = RC_S_FALSE;
    }

    uint32_t numThreads = m_numThreads;
    if (numThreads > RCZipDefs::s_maxThreadNums)
    {
        numThreads = RCZipDefs::s_maxThreadNums;
    }

    bool mtMode = (m_numThreads > 1);

    if (mtMode)
    {
        //解压包内文件数小于2，用单线程
        if (numItems <= 1)
        {
            mtMode = false;
        }
    }
   
    if (mtMode)
    {
        //1)解压包内文件都是存储模式压缩，用单线程
        //2)解压包内文件有超过2个是PPMD压缩的，用单线程
        int nPPMd = 0;
        bool isAllStored = true;;
        for (i = 0;  i < numItems; i++)
        {
            int32_t index = allFilesMode ? i : indices[i];

            RCZipItemExPtr item = m_zipItems[index];
            if (item->m_compressionMethod == RCZipHeader::NCompressionMethod::kPPMd)
            {
                if (++nPPMd >= 2)
                {
                    break;
                }
                isAllStored = false;
            }
            else if (isAllStored && item->m_compressionMethod != RCZipHeader::NCompressionMethod::kStored)
            {
                isAllStored = false;
            }
        }

        if (nPPMd >= 2)
        {
            mtMode = false;
        }
        else if (isAllStored)
        {
            mtMode = false;
        }
    }
#endif

#ifdef COMPRESS_MT
    if (!mtMode)
    {
#endif
    //单线程解压
    uint64_t currentTotalUnPacked = 0 ;
    uint64_t currentTotalPacked = 0 ;
    uint64_t currentItemUnPacked = 0 ;
    uint64_t currentItemPacked = 0 ;
    RCLocalProgress* lps = new RCLocalProgress;
    ICompressProgressInfoPtr progress = lps;
    lps->Init(extractCallback, false);
    for (i = 0; i < numItems; i++,
                              currentTotalUnPacked += currentItemUnPacked,
                              currentTotalPacked += currentItemPacked)
    {
        currentItemUnPacked = 0;
        currentItemPacked = 0;

        lps->SetInSize(currentTotalPacked);
        lps->SetOutSize(currentTotalUnPacked);

        hr = lps->SetCur();
        if (hr != RC_S_OK)
        {
            return hr;
        }

        ISequentialOutStreamPtr realOutStream;

        int32_t askMode = btestMode ? RC_ARCHIVE_EXTRACT_ASK_MODE_TEST : RC_ARCHIVE_EXTRACT_ASK_MODE_EXTRACT;
        int32_t index = allFilesMode ? i : indices[i];

        hr = extractCallback->GetStream(index, realOutStream.GetAddress(), askMode);
        if (hr != RC_S_OK)
        {
            return hr;
        }

        RCZipItemExPtr item = m_zipItems[index];
        if (!item->m_fromLocal)
        {
#ifdef COMPRESS_MT
            hr = m_zipInArchive[0]->ReadLocalItemAfterCdItem(*item);
#else
            hr = m_zipInArchive.ReadLocalItemAfterCdItem(*item);
#endif
            if (hr == RC_S_FALSE)
            {
                if (item->IsDir() || realOutStream || btestMode)
                {
                    hr = extractCallback->PrepareOperation(index,askMode);
                    if (hr != RC_S_OK)
                    {
                        return hr;
                    }

                    realOutStream.Release();

                    hr = extractCallback->SetOperationResult(index,RC_ARCHIVE_EXTRACT_RESULT_UNSUPPORTED_METHOD);
                    if (hr != RC_S_OK)
                    {
                        return hr;
                    }
                }
                continue;
            }

            if (hr != RC_S_OK)
            {
                return hr;
            }
        }

        if (item->IsDir() || item->IgnoreItem())
        {
            // if (!btestMode)
            {
                hr = extractCallback->PrepareOperation(index,askMode);
                if (hr != RC_S_OK)
                {
                    return hr;
                }

                realOutStream.Release();

                hr = extractCallback->SetOperationResult(index,RC_ARCHIVE_EXTRACT_RESULT_OK);
                if (hr != RC_S_OK)
                {
                    return hr;
                }
            }
            continue;
        }

        currentItemUnPacked = item->m_unPackSize;
        currentItemPacked = item->m_packSize;

        if (!btestMode && (!realOutStream))
        {
            continue;
        }

        hr = extractCallback->PrepareOperation(index,askMode);
        if (hr != RC_S_OK)
        {
            return hr;
        }

        int32_t res = 0 ;
        hr = myDecoder.Decode(m_compressCodecsInfo.Get(),
#ifdef COMPRESS_MT
                              *m_zipInArchive[0].get(),
#else
                              m_zipInArchive,
#endif
                              item,
                              realOutStream.Get(),
                              extractCallback,
                              progress.Get(),
                              m_numThreads,
                              res);
        if (hr != RC_S_OK)
        {
            return hr;
        }
        realOutStream.Release();

        hr = extractCallback->SetOperationResult(index,res);
        if (hr != RC_S_OK)
        {
            return hr;
        }
    }
    //单线程解压完成，返回
    return RC_S_OK;
#ifdef COMPRESS_MT
    }
#endif

#ifdef COMPRESS_MT
    {
        //多线程解压
        RCMtLocalProgress* lps = new RCMtLocalProgress ;
        ICompressProgressInfoPtr progress = lps ;
        lps->Init(extractCallback, false);
        
        RCMtExtractProgressMixer extractProgressMT ;
        extractProgressMT.Init(numItems,lps) ;
    
        if (extractCallback)
        {
            extractCallback->SetMultiThreadMode(true) ;
        }
        RCZipDecoderThreads threads;
        RCVector<HANDLE> decompressingCompletedEvents;
        RCVector<int32_t> threadIndices; 

        uint32_t i = 0 ;
        for (i = 0; i < (uint32_t)numThreads; i++)
        {
            threads.m_threads.push_back(RCZipDecoderThreadInfo());
        }

        for (i = 0; i < numThreads; i++)
        {
            RCZipDecoderThreadInfo& threadInfo = threads.m_threads[i];
            threadInfo.m_decompressCodecsInfo = m_compressCodecsInfo.Get();
            hr = threadInfo.CreateEvents();
            if (!IsSuccess(hr))
            {
                return hr;
            }
            threadInfo.m_isFree = true;

            hr = threadInfo.CreateThread();
            if (!IsSuccess(hr))
            {
                return hr;
            }
        }

        uint32_t whichThread = 0;

        for (i = 0; i < numItems; ++i)
        {
            if ((uint32_t)threadIndices.size() < numThreads)
            {
                for (uint32_t threadIndex = 0; threadIndex < numThreads; threadIndex++)
                {
                    RCZipDecoderThreadInfo &threadInfo = threads.m_threads[threadIndex];
                    if (threadInfo.m_isFree)
                    {
                        whichThread = threadIndex ;
                        break;
                    }
                }
            }
            ISequentialOutStreamPtr realOutStream;

            int32_t askMode = btestMode ? RC_ARCHIVE_EXTRACT_ASK_MODE_TEST : RC_ARCHIVE_EXTRACT_ASK_MODE_EXTRACT;
            int32_t index = allFilesMode ? i : indices[i];

            hr = extractCallback->GetStream(index, realOutStream.GetAddress(), askMode);
            if (hr != RC_S_OK)
            {
                return hr;
            }

            RCZipItemExPtr item = m_zipItems[index];
            if (!item->m_fromLocal)
            {
                hr = m_zipInArchive[whichThread]->ReadLocalItemAfterCdItem(*item);
                if (hr == RC_S_FALSE)
                {
                    if (item->IsDir() || realOutStream || btestMode)
                    {
                        hr = extractCallback->PrepareOperation(index,askMode);
                        if (hr != RC_S_OK)
                        {
                            return hr;
                        }

                        realOutStream.Release();

                        hr = extractCallback->SetOperationResult(index,RC_ARCHIVE_EXTRACT_RESULT_UNSUPPORTED_METHOD);
                        if (hr != RC_S_OK)
                        {
                            return hr;
                        }
                    }
                    continue;
                }

                if (hr != RC_S_OK)
                {
                    return hr;
                }
            }

            if (item->IsDir() || item->IgnoreItem())
            {
                {
                    hr = extractCallback->PrepareOperation(index,askMode);
                    if (hr != RC_S_OK)
                    {
                        return hr;
                    }

                    realOutStream.Release();

                    hr = extractCallback->SetOperationResult(index,RC_ARCHIVE_EXTRACT_RESULT_OK);
                    if (hr != RC_S_OK)
                    {
                        return hr;
                    }
                }
                continue;
            }

            if (!btestMode && (!realOutStream))
            {
                continue;
            }

            hr = extractCallback->PrepareOperation(index,askMode);
            if (hr != RC_S_OK)
            {
                return hr;
            }


            if ((uint32_t)threadIndices.size() < numThreads)
            {
                RCZipDecoderThreadInfo &threadInfo = threads.m_threads[whichThread];
                if (threadInfo.m_isFree)
                {
                    threadInfo.m_isFree = false;
                    threadInfo.m_archive = m_zipInArchive[whichThread];
                    threadInfo.m_item = item;
                    threadInfo.m_itemIndex = index ;
                    threadInfo.m_extractCallback = extractCallback;
                    threadInfo.m_realOutStream = realOutStream;
                    
                    RCMtExtractProgress* mtProgress = new RCMtExtractProgress ;
                    ICompressProgressInfoPtr spCompressProgressInfo( mtProgress ) ;
                    mtProgress->Init(&extractProgressMT,index) ;
                    mtProgress->Reinit() ;
                    threadInfo.m_decompressProgress = spCompressProgressInfo ;

                    threadInfo.m_decompressEvent.Set();

                    decompressingCompletedEvents.push_back(threadInfo.m_decompressionCompletedEvent);
                    threadIndices.push_back(whichThread);
                    if ((uint32_t)threadIndices.size() < numThreads)
                    {
                        continue;
                    }
                }
            }

            uint32_t result = ::WaitForMultipleObjects((uint32_t)decompressingCompletedEvents.size(),
                                                        &decompressingCompletedEvents.front(),
                                                        FALSE,
                                                        INFINITE);
            int32_t t = (int32_t)(result - WAIT_OBJECT_0);

            RCZipDecoderThreadInfo& threadInfo = threads.m_threads[threadIndices[t]];
            threadInfo.m_isFree = true;
            hr = threadInfo.m_result;
            if (!IsSuccess(hr))
            {
                return hr;
            }
            RCVectorUtils::Delete(threadIndices, t);
            RCVectorUtils::Delete(decompressingCompletedEvents, t);

            if (hr != RC_S_OK)
            {
                return hr;
            }
            realOutStream.Release() ;
        }

       if (decompressingCompletedEvents.size() > 0)
       {
           ::WaitForMultipleObjects((uint32_t)decompressingCompletedEvents.size(),
                                     &decompressingCompletedEvents.front(),
                                     TRUE,
                                     INFINITE);
        }
    }
    //多线程解压完成, 返回
    return RC_S_OK;
#endif
}

END_NAMESPACE_RCZIP
