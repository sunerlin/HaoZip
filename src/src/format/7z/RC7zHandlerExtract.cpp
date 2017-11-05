/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "format/7z/RC7zHandler.h"
#include "format/7z/RC7zDefs.h"
#include "common/RCVector.h"
#include "format/7z/RC7zDecoder.h"
#include "format/7z/RC7zFolderOutStream.h"
#include "common/RCLocalProgress.h"

/////////////////////////////////////////////////////////////////
//RC7zHandler class implementation for Extract

BEGIN_NAMESPACE_RCZIP

class RC7zExtractFolderInfo
{
public:

    /** 构造函数
    */
    RC7zExtractFolderInfo(
#ifdef _7Z_VOL
        int32_t volumeIndex,
#endif
        RC7zNum fileIndex, RC7zNum folderIndex):
#ifdef _7Z_VOL
        m_volumeIndex(volumeIndex),
#endif
        m_fileIndex(fileIndex),
        m_folderIndex(folderIndex),
        m_unpackSize(0)
    {
        if (fileIndex != RC7zDefs::s_7zNumNoIndex)
        {
            m_extractStatuses.reserve(1);
            m_extractStatuses.push_back(true);
        }
    };

public:

#ifdef _7Z_VOL

    /** 分卷索引
    */
    int32_t m_volumeIndex;

#endif

    /** 文件索引
    */
    RC7zNum m_fileIndex;

    /** 文件夹索引
    */
    RC7zNum m_folderIndex;

    /** 解压状态
    */
    RCBoolVector m_extractStatuses;

    /** 解压大小
    */
    uint64_t m_unpackSize;
};

HResult RC7zHandler::Extract(const std::vector<uint32_t>& indices, 
                             int32_t testModeSpec, 
                             IArchiveExtractCallback* extractCallbackSpec)
{
    try
    {
        uint32_t numItems = (uint32_t)indices.size() ;
        bool testMode = (testModeSpec != 0);
        IArchiveExtractCallbackPtr extractCallback = extractCallbackSpec;
        uint64_t importantTotalUnpacked = 0;

        bool allFilesMode = (numItems == uint32_t(0));
        if (allFilesMode)
        {
            numItems =
#ifdef _7Z_VOL
                        _refs.size();
#else
                        (uint32_t)m_db.m_files.size();
#endif
        }
        if(numItems == 0)
        {
            return RC_S_OK;
        }

        RCVector<RC7zExtractFolderInfo> extractFolderInfoVector;
        for(uint32_t ii = 0; ii < numItems; ii++)
        {
            uint32_t ref2Index = allFilesMode ? ii : indices[ii];
            {
#ifdef _7Z_VOL
                const CRef& ref = _refs[ref2Index];

                int32_t volumeIndex = ref.VolumeIndex;
                const CVolume& volume = _volumes[volumeIndex];
                const RC7zArchiveDatabaseEx& db = volume.Database;
                uint32_t fileIndex = ref.ItemIndex;
#else
                const RC7zArchiveDatabaseEx& db = m_db;
                uint32_t fileIndex = ref2Index;
#endif

                RC7zNum folderIndex = db.m_fileIndexToFolderIndexMap[fileIndex];
                if (folderIndex == RC7zDefs::s_7zNumNoIndex)
                {
                    extractFolderInfoVector.push_back(RC7zExtractFolderInfo(
#ifdef _7Z_VOL
                        volumeIndex,
#endif
                        fileIndex, RC7zDefs::s_7zNumNoIndex));
                    continue;
                }
                if (extractFolderInfoVector.empty() ||
                    folderIndex != extractFolderInfoVector.back().m_folderIndex
#ifdef _7Z_VOL
                    || volumeIndex != extractFolderInfoVector.back().m_volumeIndex
#endif
                    )
                {
                    extractFolderInfoVector.push_back(RC7zExtractFolderInfo(
#ifdef _7Z_VOL
                        volumeIndex,
#endif
                        RC7zDefs::s_7zNumNoIndex, folderIndex));
                    const RC7zFolder& folderInfo = db.m_folders[folderIndex];
                    uint64_t unpackSize = folderInfo.GetUnpackSize();
                    importantTotalUnpacked += unpackSize;
                    extractFolderInfoVector.back().m_unpackSize = unpackSize;
                }

                RC7zExtractFolderInfo& efi = extractFolderInfoVector.back();
                RC7zNum startIndex = db.m_folderStartFileIndex[folderIndex];
                for (RC7zNum index = (RC7zNum)efi.m_extractStatuses.size(); index <= (RC7zNum)(fileIndex - startIndex); index++)
                {
                    efi.m_extractStatuses.push_back(index == fileIndex - startIndex);
                }
            }
        }

        extractCallback->SetTotal(importantTotalUnpacked);

        RC7zDecoder decoder ;
        uint64_t currentTotalPacked = 0;
        uint64_t currentTotalUnpacked = 0;
        uint64_t totalFolderUnpacked;
        uint64_t totalFolderPacked;

        RCLocalProgress *lps = new RCLocalProgress;
        ICompressProgressInfoPtr progress = lps;
        lps->Init(extractCallback.Get(), false);

        for(int32_t i = 0; i < (int32_t)extractFolderInfoVector.size(); i++,
            currentTotalUnpacked += totalFolderUnpacked,
            currentTotalPacked += totalFolderPacked)
        {
            lps->SetOutSize(currentTotalUnpacked) ;
            lps->SetInSize(currentTotalPacked) ;
            HResult hr = lps->SetCur() ;
            if(!IsSuccess(hr))
            {
                return hr ;
            }

            const RC7zExtractFolderInfo& efi = extractFolderInfoVector[i];
            totalFolderUnpacked = efi.m_unpackSize;

            totalFolderPacked = 0;

            RC7zFolderOutStream* folderOutStream = new RC7zFolderOutStream;
            ISequentialOutStreamPtr outStream(folderOutStream);

#ifdef _7Z_VOL
            const CVolume& volume = _volumes[efi.VolumeIndex];
            const RC7zArchiveDatabaseEx& db = volume.Database;
#else
            const RC7zArchiveDatabaseEx& db = m_db;
#endif

            RC7zNum startIndex;
            if (efi.m_fileIndex != RC7zDefs::s_7zNumNoIndex)
            {
                startIndex = efi.m_fileIndex;
            }
            else
            {
                startIndex = db.m_folderStartFileIndex[efi.m_folderIndex];
            }

            HResult result = folderOutStream->Init( &db,
#ifdef _7Z_VOL
                                                    volume.m_startRef2Index,
#else
                                                    0,
#endif
                                                    startIndex,
                                                    &efi.m_extractStatuses,
                                                    extractCallback.Get(),
                                                    testMode,
                                                    m_crcSize != 0);

            if(!IsSuccess(result))
            {
                return result ;
            }
            if (efi.m_fileIndex != RC7zDefs::s_7zNumNoIndex)
            {
                continue;
            }

            RC7zNum folderIndex = efi.m_folderIndex;
            const RC7zFolder& folderInfo = db.m_folders[folderIndex];

            totalFolderPacked = m_db.GetFolderFullPackSize(folderIndex);

            RC7zNum packStreamIndex = db.m_folderStartPackStreamIndex[folderIndex];
            uint64_t folderStartPackPos = db.GetFolderStreamPos(folderIndex, 0);

            ICryptoGetTextPasswordPtr getTextPassword;
            if (extractCallback)
            {
                extractCallback.QueryInterface(IID_ICryptoGetTextPassword, getTextPassword.GetAddress());
            }

            try
            {
                bool passwordIsDefined = false ;
                HResult result = decoder.Decode(m_codecsInfo.Get(),
#ifdef _7Z_VOL
                                                volume.Stream,
#else
                                                m_inStream.Get(),
#endif
                                                folderStartPackPos,
                                                &db.m_packSizes[packStreamIndex],
                                                folderInfo,
                                                outStream.Get(),
                                                progress.Get(), 
                                                getTextPassword.Get(), 
                                                passwordIsDefined,
                                                m_numThreads ) ;

                if (result == RC_S_FALSE)
                {
                    HResult hr = folderOutStream->FlushCorrupted(RC_ARCHIVE_EXTRACT_RESULT_DATA_ERROR) ;
                    if(!IsSuccess(hr))
                    {
                        return hr ;
                    }
                    continue;
                }
                if (result == RC_E_NOTIMPL)
                {
                    HResult hr = folderOutStream->FlushCorrupted(RC_ARCHIVE_EXTRACT_RESULT_UNSUPPORTED_METHOD) ;
                    if(!IsSuccess(hr))
                    {
                        return hr ;
                    }
                    continue;
                }
                if (result != RC_S_OK)
                {
                    return result;
                }
                if (folderOutStream->WasWritingFinished() != RC_S_OK)
                {
                    HResult hr = folderOutStream->FlushCorrupted(RC_ARCHIVE_EXTRACT_RESULT_DATA_ERROR) ;
                    if(!IsSuccess(hr))
                    {
                        return hr ;
                    }
                    continue;
                }
            }
            catch(...)
            {
                HResult hr = folderOutStream->FlushCorrupted(RC_ARCHIVE_EXTRACT_RESULT_DATA_ERROR) ;
                if(!IsSuccess(hr))
                {
                    return hr ;
                }
                continue;
            }
        }
        return RC_S_OK;
    }
    catch(...)
    {
        return RC_S_FALSE ;
    }
}

END_NAMESPACE_RCZIP
