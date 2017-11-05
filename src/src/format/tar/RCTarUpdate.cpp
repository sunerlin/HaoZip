/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#include "format/tar/RCTarUpdate.h"
#include "format/tar/RCTarOut.h"
#include "format/tar/RCTarHeader.h"
#include "common/RCCreateCoder.h"
#include "common/RCLocalProgress.h"
#include "interface/RCMethodDefs.h"
#include "filesystem/RCLimitedSequentialInStream.h"

BEGIN_NAMESPACE_RCZIP

HResult RCTarUpdate::UpdateArchive(IInStream *inStream,
                                   ISequentialOutStream *outStream,
                                   const std::vector<RCTarItemEx> &inputItems,
                                   const std::vector<RCTarUpdateItem> &updateItems,
                                   IArchiveUpdateCallback *updateCallback,
                                   ICompressCodecsInfoPtr compressCodecsInfo)
{
    RCTarOut outArchive;
    outArchive.Create(outStream);

    uint64_t complexity = 0;

    size_t i = 0 ;
    size_t itemCount = updateItems.size() ;
    for(i = 0; i < itemCount; i++)
    {
        const RCTarUpdateItem &ui = updateItems[i];
        if (ui.m_newData)
        {
            complexity += ui.m_size;
        }
        else
        {
            complexity += inputItems[ui.m_indexInArchive].GetFullSize();
        }
    }

    HResult rs = updateCallback->SetTotal(complexity);
    if (rs != RC_S_OK)
    {
        return rs;
    }

    ICompressCoderPtr copyCoder;
    rs = RCCreateCoder::CreateCoder(compressCodecsInfo.Get(), RCMethod::ID_COMPRESS_COPY, copyCoder, true);
    if (rs != RC_S_OK)
    {
        return rs;
    }

    ICompressGetInStreamProcessedSizePtr copyCoderSize;
    rs = copyCoder.QueryInterface(ICompressGetInStreamProcessedSize::IID, copyCoderSize.GetAddress());
    if (rs != RC_S_OK)
    {
        return rs;
    }

    RCLocalProgress *lps = new RCLocalProgress;
    ICompressProgressInfoPtr progress = lps;
    lps->Init(updateCallback, true);

    RCLimitedSequentialInStream *streamSpec = new RCLimitedSequentialInStream;
    RCLimitedSequentialInStreamPtr inStreamLimited(streamSpec);

    streamSpec->SetStream(inStream);

    complexity = 0;

    for(i = 0; i < itemCount; i++)
    {
        lps->SetInSize(complexity);
        lps->SetOutSize(complexity);

        HResult rs = lps->SetCur();
        if (rs != RC_S_OK)
        {
            return rs;
        }

        const RCTarUpdateItem &ui = updateItems[i];
        RCTarItem item;
        if (ui.m_newProps)
        {
            item.m_mode = ui.m_mode;
            item.m_name = ui.m_name;
            item.m_user = ui.m_user;
            item.m_group = ui.m_group;
            if (ui.m_isDir)
            {
                item.m_linkFlag = RCTarHeader::NLinkFlag::s_directory;
                item.m_size = 0;
            }
            else
            {
                item.m_linkFlag = RCTarHeader::NLinkFlag::s_normal;
                item.m_size = ui.m_size;
            }
            item.m_mTime = ui.m_time;
            item.m_deviceMajorDefined = false;
            item.m_deviceMinorDefined = false;
            item.m_uid = 0;
            item.m_gid = 0;
            memmove(item.m_magic, RCTarHeader::NMagic::s_empty, 8);
        }
        else
        {
            item = inputItems[ui.m_indexInArchive] ;
        }
        if (ui.m_newData)
        {
            item.m_size = ui.m_size;
            if (item.m_size == (uint64_t)(int64_t)-1)
            {
                return RC_E_INVALIDARG;
            }
        }
        else
        {
            item.m_size = inputItems[ui.m_indexInArchive].m_size ;
        }

        if (ui.m_newData)
        {
            lps->SetIndex(ui.m_indexInClient) ;
            ISequentialInStreamPtr fileInStream;
            HResult res = updateCallback->GetStream(ui.m_indexInClient, fileInStream.GetAddress());
            if (res != RC_S_FALSE)
            {
                HResult rs = res;
                if (rs != RC_S_OK)
                {
                    return rs;
                }

                rs = outArchive.WriteHeader(item);
                if (rs != RC_S_OK)
                {
                    return rs;
                }

                if (!ui.m_isDir)
                {
                    rs = copyCoder->Code(fileInStream.Get(), outStream, NULL, NULL, progress.Get());
                    if (rs != RC_S_OK)
                    {
                        return rs;
                    }

                    uint64_t u64size;
                    rs = copyCoderSize.Get()->GetInStreamProcessedSize(u64size);
                    if (rs != RC_S_OK)
                    {
                        return rs;
                    }

                    if (u64size != item.m_size)
                    {
                        return RC_E_FAIL;
                    }

                    rs = outArchive.FillDataResidual(item.m_size);
                    if (rs != RC_S_OK)
                    {
                        return rs;
                    }
                }
            }

            complexity += ui.m_size;

            HResult rs = updateCallback->SetOperationResult(ui.m_indexInClient,RC_ARCHIVE_UPDATE_RESULT_OK);
            if (rs != RC_S_OK)
            {
                return rs;
            }
        }
        else
        {
            const RCTarItemEx &existItemInfo = inputItems[ui.m_indexInArchive];
            uint64_t size;
            if (ui.m_newProps)
            {
                HResult rs = outArchive.WriteHeader(item);
                if (rs != RC_S_OK)
                {
                    return rs;
                }

                rs = inStream->Seek(existItemInfo.GetDataPosition(), RC_STREAM_SEEK_SET, NULL);
                if (rs != RC_S_OK)
                {
                    return rs;
                }

                size = existItemInfo.m_size;
            }
            else
            {
                HResult rs = inStream->Seek(existItemInfo.m_headerPosition, RC_STREAM_SEEK_SET, NULL);
                if (rs != RC_S_OK)
                {
                    return rs;
                }

                size = existItemInfo.GetFullSize();
            }
            streamSpec->Init(size);

            HResult rs = copyCoder->Code(inStreamLimited.Get(), outStream, NULL, NULL, progress.Get());
            if (rs != RC_S_OK)
            {
                return rs;
            }

            uint64_t u64size;
            rs = copyCoderSize.Get()->GetInStreamProcessedSize(u64size);
            if (rs != RC_S_OK)
            {
                return rs;
            }

            if (u64size != size)
            {
                return RC_E_FAIL;
            }

            rs = outArchive.FillDataResidual(existItemInfo.m_size);
            if (rs != RC_S_OK)
            {
                return rs;
            }

            complexity += size;
        }
    }
    return outArchive.WriteFinishHeader();
}

END_NAMESPACE_RCZIP