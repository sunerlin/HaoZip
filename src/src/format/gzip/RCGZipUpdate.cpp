/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "format/gzip/RCGZipUpdate.h"
#include "format/gzip/RCGZipDeflateProps.h"
#include "format/gzip/RCGZipItem.h"
#include "interface/RCMethodDefs.h"
#include "format/common/RCOutStreamWithCRC.h"
#include "format/common/RCSequentialInStreamWithCRC.h"
#include "common/RCCreateCoder.h"
#include "common/RCLocalProgress.h"

/////////////////////////////////////////////////////////////////
//RCGZipUpdate class implementation

BEGIN_NAMESPACE_RCZIP

#ifdef RCZIP_OS_WIN
    #define g_hostOS    (RCGZipHeader::NHostOS::kFAT) 
#else
    #define g_hostOS    (RCGZipHeader::NHostOS::kUnix) 
#endif

HResult RCGZipUpdate::UpdateArchive(ICompressCodecsInfo* codecsInfo, 
                                    ISequentialOutStream* outStream,
                                    uint64_t unpackSize,
                                    const RCGZipItem& newItem,
                                    RCGZipDeflateProps& deflateProps,
                                    IArchiveUpdateCallback* updateCallback) 
{
    uint64_t complexity = 0;
    HResult hr = updateCallback->SetTotal(unpackSize) ;
    if(!IsSuccess(hr))
    {
        return hr ;
    }
    hr = updateCallback->SetCompleted(complexity) ;
    if(!IsSuccess(hr))
    {
        return hr ;
    }
    
    ISequentialInStreamPtr fileInStream ;

    hr = updateCallback->GetStream(0, fileInStream.GetAddress()) ;
    if(!IsSuccess(hr))
    {
        return hr ;
    }
    
    RCSequentialInStreamWithCRC* inStreamSpec = new RCSequentialInStreamWithCRC ;
    ISequentialInStreamPtr crcStream(inStreamSpec) ;
    inStreamSpec->SetStream(fileInStream.Get());
    inStreamSpec->Init();

    RCLocalProgress* lps = new RCLocalProgress;
    ICompressProgressInfoPtr progress = lps;
    lps->Init(updateCallback, true) ;

    RCGZipItem item = newItem ;
    item.m_method = RCGZipHeader::NCompressionMethod::kDeflate;
    item.m_extraFlags = deflateProps.IsMaximum() ? RCGZipHeader::NExtraFlags::kMaximum :
                                                 RCGZipHeader::NExtraFlags::kFastest ;

    item.m_hostOS = g_hostOS ;

    hr = item.WriteHeader(outStream) ;
    if(!IsSuccess(hr))
    {
        return hr ;
    }
    
    ICompressCoderPtr deflateEncoder ;
    hr  = RCCreateCoder::CreateCoder(codecsInfo,
                                     RCMethod::ID_COMPRESS_DEFLATE_COM,
                                     deflateEncoder,
                                     true);
    if(!IsSuccess(hr))
    {
        return hr ;
    }
    if (!deflateEncoder)
    {
        return RC_E_NOTIMPL ;
    }
    ICompressSetCoderPropertiesPtr setCoderProperties;
    hr = deflateEncoder.QueryInterface(ICompressSetCoderProperties::IID, setCoderProperties.GetAddress());
    if(!IsSuccess(hr))
    {
        return hr ;
    }
    if(setCoderProperties != NULL)
    {
        hr = deflateProps.SetCoderProperties(setCoderProperties.Get()) ;
        if(!IsSuccess(hr))
        {
            return hr ;
        }
    }
    hr = deflateEncoder->Code(crcStream.Get(), outStream, NULL, NULL, progress.Get()) ;
    item.m_crc = inStreamSpec->GetCRC() ;
    item.m_size32 = (uint32_t)inStreamSpec->GetSize() ;
    hr = item.WriteFooter(outStream) ;
    if(!IsSuccess(hr))
    {
        return hr ;
    }
    return updateCallback->SetOperationResult(0, RC_ARCHIVE_UPDATE_RESULT_OK);
}

END_NAMESPACE_RCZIP
