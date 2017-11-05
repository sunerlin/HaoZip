/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "format/zip/RCZipUpdate.h"
#include "format/zip/RCZipOut.h"
#include "format/zip/RCZipAddCommon.h"
#include "interface/RCMethodDefs.h"
#include "common/RCCreateCoder.h"
#include "common/RCLocalProgress.h"
#include "filesystem/RCLimitedSequentialInStream.h"

#include "thread/RCThread.h"
#include "thread/RCMutex.h"
#include "thread/RCSynchronization.h"
#include "locked/RCMtCompressProgress.h"
#include "locked/RCOutMemStream.h"
#include "locked/RCMemLockBlocks.h"
#include "locked/RCMemBlockManagerMt.h"
#include "locked/RCMtCompressProgressMixer.h"
#include "locked/RCMtLocalProgress.h"
#include "common/RCStringUtil.h"
#include "format/zip/RCZipHandler.h"
#include "format/common/RCFileFilterManager.h"

/////////////////////////////////////////////////////////////////
//RCZipUpdate class implementation

BEGIN_NAMESPACE_RCZIP

static const byte_t s_hostOS =
  #ifdef RCZIP_OS_WIN
  RCZipHeader::NHostOS::kFAT;
  #else
  RCZipHeader::NHostOS::kUnix;
  #endif

static const byte_t s_madeByHostOS  = s_hostOS;
static const byte_t s_extractHostOS = s_hostOS;

static const byte_t s_methodForDirectory = RCZipHeader::NCompressionMethod::kStored;
static const byte_t s_extractVersionForDirectory = RCZipHeader::NCompressionMethod::kStoreExtractVersion;

static HResult CopyBlockToArchive(ICompressCodecsInfo* codecsInfo,
                                  ISequentialInStream* inStream,
                                  RCZipOut& outArchive,
                                  ICompressProgressInfo* progressInfo)
{
    ISequentialOutStreamPtr outStream;

    outArchive.CreateStreamForCopying(outStream.GetAddress());

    ICompressCoderPtr copyCoder;
    HResult hr = RCCreateCoder::CreateCoder(codecsInfo,
                                            RCMethod::ID_COMPRESS_COPY,
                                            copyCoder,
                                            true);
    if (hr != RC_S_OK)
    {
        return hr;
    }

    return copyCoder.Get()->Code(inStream, outStream.Get(), NULL, NULL, progressInfo);
}

static HResult WriteRange(ICompressCodecsInfo* codecsInfo,
                          IInStream* inStream,
                          RCZipOut& outArchive,
                          const RCZipUpdate::RCZipUpdateRange& updateRange,
                          ICompressProgressInfo* progressInfo)
{
    uint64_t position;
    HResult hr = inStream->Seek(updateRange.m_position, RC_STREAM_SEEK_SET, &position);
    if (hr != RC_S_OK)
    {
        return hr;
    }

    RCLimitedSequentialInStream* streamSpec = new RCLimitedSequentialInStream;
    RCLimitedSequentialInStreamPtr inStreamLimited(streamSpec);
    streamSpec->SetStream(inStream);
    streamSpec->Init(updateRange.m_size);

    hr = CopyBlockToArchive(codecsInfo, inStreamLimited.Get(), outArchive, progressInfo);
    if (hr != RC_S_OK)
    {
        return hr;
    }

    return progressInfo->SetRatioInfo(updateRange.m_size, updateRange.m_size);
}

static void SetFileHeader(RCZipOut& outArchive,
                          const RCZipCompressionMethodMode& options,
                          const RCZipUpdate::RCZipUpdateItem& ui,
                          RCZipItem& item)
{
    item.m_unPackSize = ui.m_size;
    bool isDir = false ;

    item.ClearFlags();

    if (ui.m_newProperties)
    {
        isDir     = ui.m_isDir;
        item.m_name = ui.m_name;
        item.SetUtf8(ui.m_isUtf8);
        item.m_externalAttributes = ui.m_attributes;
        item.m_time      = ui.m_time;
        item.m_ntfsMTime = ui.m_ntfsMTime;
        item.m_ntfsATime = ui.m_ntfsATime;
        item.m_ntfsCTime = ui.m_ntfsCTime;
        item.m_ntfsTimeIsDefined = ui.m_ntfsTimeIsDefined;
    }
    else
    {
        isDir = item.IsDir();
    }

    item.m_diskNumberStart = 0;
    item.m_localHeaderPosition   = outArchive.GetCurrentPosition();
    item.m_madeByVersion.m_hostOS  = s_madeByHostOS;
    item.m_madeByVersion.m_version = RCZipHeader::NCompressionMethod::kMadeByProgramVersion;

    item.m_extractVersion.m_hostOS = s_extractHostOS;

    item.m_internalAttributes    = 0; // test it
    item.SetEncrypted(!isDir && options.m_passwordIsDefined);

    if (isDir)
    {
        item.m_extractVersion.m_version = s_extractVersionForDirectory;
        item.m_compressionMethod        = s_methodForDirectory;
        item.m_packSize = 0;
        item.m_fileCRC  = 0; // test it
    }
}

static void SetItemInfoFromCompressingResult(const RCZipCompressingResult& compressingResult,
                                             bool isAesMode,
                                             byte_t aesKeyMode,
                                             RCZipItem& item)
{
    item.m_extractVersion.m_version = compressingResult.m_extractVersion;
    item.m_compressionMethod      = compressingResult.m_method;
    item.m_fileCRC                = compressingResult.m_crc;
    item.m_unPackSize             = compressingResult.m_unpackSize;
    item.m_packSize               = compressingResult.m_packSize;

    item.m_localExtra.Clear();
    item.m_centralExtra.Clear();

    if (isAesMode)
    {
        RCZipWzAESExtraField wzAesField;

        wzAesField.m_strength = aesKeyMode;
        wzAesField.m_method   = compressingResult.m_method;

        item.m_compressionMethod = RCZipHeader::NCompressionMethod::kWzAES;
        item.m_fileCRC           = 0;

        RCZipExtraSubBlock sb;

        wzAesField.SetSubBlock(sb);

        item.m_localExtra.m_subBlocks.push_back(sb);
        item.m_centralExtra.m_subBlocks.push_back(sb);
    }
}

static HResult UpdateItemOldData(ICompressCodecsInfo* codecsInfo,
                                 RCZipOut& outArchive,
                                 IInStream* inStream,
                                 const RCZipUpdate::RCZipUpdateItem& ui,
                                 RCZipItemEx& item,
                                 /* bool izZip64, */
                                 ICompressProgressInfo* progress,
                                 uint64_t& complexity)
{
    if (ui.m_newProperties)
    {
        if (item.HasDescriptor())
        {
            return RC_E_NOTIMPL;
        }

        // use old name size.
        // RCZipUpdateRange range(item.GetLocalExtraPosition(), item.LocalExtraSize + item.PackSize);
        RCZipUpdate::RCZipUpdateRange range(item.GetDataPosition(), item.m_packSize);

        // item.ExternalAttributes = ui.Attributes;
        // Test it
        item.m_name = ui.m_name;
        item.SetUtf8(ui.m_isUtf8);
        item.m_time = ui.m_time;
        item.m_ntfsMTime = ui.m_ntfsMTime;
        item.m_ntfsATime = ui.m_ntfsATime;
        item.m_ntfsCTime = ui.m_ntfsCTime;
        item.m_ntfsTimeIsDefined = ui.m_ntfsTimeIsDefined;

        item.m_centralExtra.RemoveUnknownSubBlocks();
        item.m_localExtra.RemoveUnknownSubBlocks();

        outArchive.PrepareWriteCompressedData2((uint16_t)item.m_name.length(),
                                               item.m_unPackSize,
                                               item.m_packSize,
                                               item.m_localExtra.HasWzAesField());

        item.m_localHeaderPosition = outArchive.GetCurrentPosition();
        outArchive.SeekToPackedDataPosition();

        HResult hr = WriteRange(codecsInfo, inStream, outArchive, range, progress);
        if (hr != RC_S_OK)
        {
            return hr;
        }

        complexity += range.m_size;
        outArchive.WriteLocalHeader(item);
    }
    else
    {
        RCZipUpdate::RCZipUpdateRange range(item.m_localHeaderPosition, item.GetLocalFullSize());

        // set new header position
        item.m_localHeaderPosition = outArchive.GetCurrentPosition();

        HResult hr = WriteRange(codecsInfo, inStream, outArchive, range, progress);
        if (hr != RC_S_OK)
        {
            return hr;
        }

        complexity += range.m_size;
        outArchive.MoveBasePosition(range.m_size);
    }

    return RC_S_OK;
}

static void WriteDirHeader(RCZipOut& outArchive,
                           const RCZipCompressionMethodMode* options,
                           const RCZipUpdate::RCZipUpdateItem& ui,
                           RCZipItemEx& item)
{
    SetFileHeader(outArchive, *options, ui, item);

    outArchive.PrepareWriteCompressedData((uint16_t)item.m_name.size(),
                                          ui.m_size,
                                          options->m_isAesMode);
    outArchive.WriteLocalHeader(item) ;
}


#ifdef COMPRESS_MT

static void ZipEncoderThread(void *threadCoderInfo);

struct RCZipUpdateThreadInfo
{
    /** 编码管理器
    */
    ICompressCodecsInfoPtr m_codecsInfo;

    /** 线程
    */
    RCThreadPtr m_thread;

    /** 压缩事件
    */
    RCAutoResetEvent m_compressEvent;

    /** 压缩完成事件
    */
    RCAutoResetEvent m_compressionCompletedEvent;

    /** 线程是否退出
    */
    bool m_exitThread;

    /** 多线程压缩进度管理器
    */
    RCMtCompressProgress* m_progressSpec;

    /** 压缩进度信息接口
    */
    ICompressProgressInfoPtr m_progress;

    /** 内存输出流
    */
    RCOutMemStream* m_outStreamSpec;

    /** 输出流
    */
    IOutStreamPtr m_outStream;

    /** 输入流
    */
    ISequentialInStreamPtr m_inStream;

    /** 添加
    */
    RCZipAddCommon m_coder;

    /** 结果
    */
    HResult m_result;

    /** 压缩结果
    */
    RCZipCompressingResult m_compressingResult;

    /** 是否是空闲线程
    */
    bool m_isFree;

    /** 更新项索引
    */
    uint32_t m_updateIndex;

    /** 当前压缩文件的序号
    */
    uint32_t m_itemIndex ;

    /** 可选文件扩展名过滤压缩算法
    */
    RCZipExtFilterPtr m_zipExtFilter;

    /** 是否支持文件过滤
    */
    bool m_isFileFilter;

    /** 构造函数
    */
    RCZipUpdateThreadInfo(const RCZipCompressionMethodMode &options):
        m_exitThread(false),
        m_progressSpec(0),
        m_outStreamSpec(0),
        m_coder(options),
        m_result(RC_S_OK),
        m_isFree(true),
        m_updateIndex(0),
        m_itemIndex(0)
    {
    }

    /** 创建事件
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult CreateEvents()
    {
        HResult hr = m_compressEvent.CreateIfNotCreated();
        if (!IsSuccess(hr))
        {
            return hr;
        }
        return m_compressionCompletedEvent.CreateIfNotCreated();
    }

    /** 创建线程
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult CreateThread() 
    { 
        RCThread* thread = new RCThread(&ZipEncoderThread, this);
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

    /** 等待压缩
    */
    void WaitAndCode() ;

    /** 停止并等待关闭
    */
    void StopWaitClose()
    {
        m_exitThread = true;
        if (m_outStreamSpec != 0)
        {
            m_outStreamSpec->StopWriting(RC_E_ABORT);
        }
        if (m_compressEvent.IsCreated())
        {
            m_compressEvent.Set();
        }
        m_thread->Join();
    }

};

void RCZipUpdateThreadInfo::WaitAndCode()
{
    for (;;)
    {
        m_compressEvent.Lock();
        if (m_exitThread)
        {
            return;
        }
        if(m_progressSpec)
        {
            m_progressSpec->SetItemIndex(m_itemIndex) ;
        }
        m_result = m_coder.Compress(m_codecsInfo.Get(),
                                    m_inStream.Get(),
                                    m_outStream.Get(),
                                    m_progress.Get(),
                                    m_compressingResult,
                                    m_zipExtFilter,
                                    RCZipHandler::s_fileFilter,
                                    m_isFileFilter);
        if (m_result == RC_S_OK && m_progress)
        {
            m_result = m_progress->SetRatioInfo(m_compressingResult.m_unpackSize, m_compressingResult.m_packSize);
        }        
        m_compressionCompletedEvent.Set();
    }
}

static void ZipEncoderThread(void *threadCoderInfo)
{
    ((RCZipUpdateThreadInfo *)threadCoderInfo)->WaitAndCode();
    return;
}

class RCZipUpdateThreads
{
public:

    /** 默认析构函数
    */
    ~RCZipUpdateThreads()
    {
        for (int32_t i = 0; i < (int32_t)m_threads.size(); i++)
        {
            m_threads[i].StopWaitClose();
        }
    }

public:

    /** 线程信息
    */
    RCVector<RCZipUpdateThreadInfo> m_threads;
};

struct RCMemBlocks2: public RCMemLockBlocks
{
    /** 压缩结果
    */
    RCZipCompressingResult m_compressingResult ;

    /** 是否定义
    */
    bool m_defined ;

    /** 是否跳过
    */
    bool m_skip ;

    /** 默认构造函数
    */
    RCMemBlocks2(): m_defined(false), m_skip(false) {}
};

class RCZipUpdateMemRefs
{
public:

    /** 默认构造函数
    */
    RCZipUpdateMemRefs(RCMemBlockManagerMt* manager): m_manager(manager) {} ;

    /** 默认析构函数
    */
    ~RCZipUpdateMemRefs()
    {
        for (int32_t i = 0; i < (int32_t)m_refs.size(); i++)
        {
            m_refs[i].FreeOpt(m_manager);
        }
    }

public:

    /** 多线程内存管理
    */
    RCMemBlockManagerMt* m_manager;

    /** 内存块
    */
    RCVector<RCMemBlocks2> m_refs;
};

class RCMtProgressMixer2:
    public IUnknownImpl<ICompressProgressInfo>
{
public:

    /** 创建
    @param [in] progress 进度管理器
    @param [in] inSizeIsMain 输入大小是否在主线程
    */
    void Create(IProgress* progress, bool inSizeIsMain);

    /** 设置进度偏移
    @param [in] progressOffset 进度偏移
    @return 成功返回RC_S_OK, 否则返回错误码
    */
    void SetProgressOffset(uint64_t progressOffset);

    /** 设置压缩进度
    @param [in] index 索引
    @param [in] inSize 输入数据大小
    @param [in] outSize 输出数据大小
    @return 成功返回RC_S_OK, 否则返回错误码
    */
    HResult SetRatioInfo(int index, const uint64_t inSize, const uint64_t outSize);

    /** 设置压缩进度
    @param [in] inSize 输入数据大小
    @param [in] outSize 输出数据大小
    @return 成功返回RC_S_OK, 否则返回错误码
    */
    HResult SetRatioInfo(uint64_t inSize, uint64_t outSize);

public:

    /** 互斥
    */
    RCMutex m_criticalSection;

private:

    /** 进度偏移
    */
    uint64_t m_progressOffset;

    /** 输入大小
    */
    uint64_t m_inSizes[2];

    /** 输出大小
    */
    uint64_t m_outSizes[2];

    /** 进度管理器
    */
    IProgressPtr m_progress;

    /** 压缩进度信息
    */
    ICompressProgressInfoPtr m_ratioProgress;

    /** 输入大小是否在主线程
    */
    bool m_inSizeIsMain;
};

void RCMtProgressMixer2::Create(IProgress* progress, bool inSizeIsMain)
{
    m_progress = progress;
    m_progress.QueryInterface(IID_ICompressProgressInfo, m_ratioProgress.GetAddress());
    m_inSizeIsMain = inSizeIsMain;
    m_progressOffset = m_inSizes[0] = m_inSizes[1] = m_outSizes[0] = m_outSizes[1] = 0;
}

void RCMtProgressMixer2::SetProgressOffset(uint64_t progressOffset)
{
    RCMutexLock spLock(&m_criticalSection) ;
    m_inSizes[1] = m_outSizes[1] = 0;
    m_progressOffset = progressOffset;
}

HResult RCMtProgressMixer2::SetRatioInfo(int index, const uint64_t inSize, const uint64_t outSize)
{
     RCMutexLock spLock(&m_criticalSection) ;
    if (index == 0 && m_ratioProgress)
    {
        HResult hr = m_ratioProgress->SetRatioInfo(inSize, outSize);
        if (!IsSuccess(hr))
        {
            return hr;
        }
    }
    if (inSize != 0)
    {
        m_inSizes[index] = inSize;
    }
    if (outSize != 0)
    {
        m_outSizes[index] = outSize;
    }
    uint64_t v = m_progressOffset + (m_inSizeIsMain  ?
        (m_inSizes[0] + m_inSizes[1]) :
        (m_outSizes[0] + m_outSizes[1]));

    return m_progress->SetCompleted(v);
}

HResult RCMtProgressMixer2::SetRatioInfo(const uint64_t inSize, const uint64_t outSize)
{
    return SetRatioInfo(0, inSize, outSize);
}

class RCMtProgressMixer:
    public IUnknownImpl<ICompressProgressInfo>

{
public:

    /** 创建
    @param [in] progress 进度管理器
    @param [in] inSizeIsMain 输入大小是否在主线程
    */
    void Create(IProgress* progress, bool inSizeIsMain);

    /** 设置压缩进度
    @param [in] inSize 输入数据大小
    @param [in] outSize 输出数据大小
    @return 成功返回RC_S_OK, 否则返回错误码
    */
    HResult SetRatioInfo(uint64_t inSize, uint64_t outSize);

public:

    /** 多线程混合进度管理
    */
    RCMtProgressMixer2* m_mixer2;

    /** 压缩进度信息
    */
    ICompressProgressInfoPtr m_ratioProgress;
};

void RCMtProgressMixer::Create(IProgress* progress, bool inSizeIsMain)
{
    m_mixer2 = new RCMtProgressMixer2;
    m_ratioProgress = m_mixer2;
    m_mixer2->Create(progress, inSizeIsMain);
}

HResult RCMtProgressMixer::SetRatioInfo(uint64_t inSize, const uint64_t outSize)
{
    return m_mixer2->SetRatioInfo(1, inSize, outSize);
}

#endif

static HResult UpdateArchive2St(ICompressCodecsInfo* codecsInfo,
                                RCZipOut& outArchive,
                                RCZipIn* inArchive,
                                IInStream* inStream,
                                const RCVector<RCZipItemExPtr>& inputItems,
                                const RCVector<RCZipUpdate::RCZipUpdateItemPtr>& updateItems,
                                const RCZipCompressionMethodMode* options,
                                const RCByteBuffer& comment,
                                IArchiveUpdateCallback* updateCallback)
{
    RCLocalProgress *lps = new RCLocalProgress;
    ICompressProgressInfoPtr progress = lps;
    lps->Init(updateCallback, true);

    RCZipAddCommon compressor(*options);

    RCVector<RCZipItemPtr> items;
    uint64_t unpackSizeTotal = 0;
    uint64_t packSizeTotal   = 0;

    RCFileFilterManager fileFilterManager(RCZipHandler::s_fileFilter.m_fileList);

    for (int32_t itemIndex = 0; itemIndex < (int32_t)updateItems.size(); itemIndex++)
    {
        lps->SetInSize(unpackSizeTotal);
        lps->SetOutSize(packSizeTotal);

        HResult hr = lps->SetCur();
        if (hr != RC_S_OK)
        {
            return hr;
        }

        const RCZipUpdate::RCZipUpdateItemPtr& ui = updateItems[itemIndex];
        RCZipItemExPtr item(new RCZipItemEx);
        if (!ui->m_newProperties || !ui->m_newData)
        {
            item = inputItems[ui->m_indexInArchive];
            if (inArchive->ReadLocalItemAfterCdItemFull(*item) != RC_S_OK)
            {
                return RC_E_NOTIMPL;
            }
        }
        
        //文件算法过滤器
        bool isFileFilter = false;
        if (!RCZipHandler::s_fileFilter.m_fileList.empty())
        {
            RCVariant prop;
            HResult hr = updateCallback->GetProperty(ui->m_indexInClient, RCPropID::kpidAbsolutePath, prop);
            if (IsSuccess(hr) && IsStringType(prop))
            {
                RCString absolutePath = GetStringValue(prop);

                hr = updateCallback->GetProperty(ui->m_indexInClient, RCPropID::kpidRelativePath, prop);
                if (IsSuccess(hr) && IsStringType(prop))
                {
                    RCString relativePath = GetStringValue(prop);;

                    isFileFilter = fileFilterManager.IsFilter(absolutePath, relativePath);
                }
            }
        }

        //文件扩展名算法过滤器
        RCZipExtFilterPtr zipExtFilter;
        if ((!isFileFilter) && (RCZipHandler::s_extFilters.size() > 0))
        {
            RCString::size_type pos = ui->m_name.find_last_of('.');
            if (pos != RCString::npos)
            {
                RCString uName = RCStringConvert::MultiByteToUnicodeString(ui->m_name) ;
                pos = uName.find_last_of(_T('.'));
                RCString ext ;
                if (pos != RCString::npos)
                {
                    ext = RCStringUtil::Mid(uName, (int32_t)pos+1) ;
                }
                for (uint32_t i = 0; i < RCZipHandler::s_extFilters.size(); i++)
                {
                    if (RCStringUtil::CompareNoCase(ext,RCZipHandler::s_extFilters[i]->m_fileExt) == 0)
                    {
                        zipExtFilter = RCZipHandler::s_extFilters[i];
                        break;
                    }
                }
            }
        }

        if (ui->m_newData)
        {
            bool isDir = ((ui->m_newProperties) ? ui->m_isDir : item->IsDir());
            if (isDir)
            {
                WriteDirHeader(outArchive, options, *ui, *item);
            }
            else
            {
                ISequentialInStreamPtr fileInStream;
                hr = updateCallback->GetStream(ui->m_indexInClient, fileInStream.GetAddress());
                if( (hr == RC_S_FALSE) || (fileInStream == NULL) )
                {
                    lps->SetProgressOffset(lps->GetProgressOffset() + ui->m_size);
                    hr = updateCallback->SetOperationResult(ui->m_indexInClient, RC_ARCHIVE_UPDATE_RESULT_OK);
                    if (hr != RC_S_OK)
                    {
                        return hr;
                    }
                    continue;
                }

                if (hr != RC_S_OK)
                {
                    return hr;
                }

                // file Size can be 64-bit !!!
                SetFileHeader(outArchive, *options, *ui, *item);
                outArchive.PrepareWriteCompressedData((uint16_t)item->m_name.length(),
                                                      ui->m_size,
                                                      options->m_isAesMode);

                RCZipCompressingResult compressingResult;
                IOutStreamPtr outStream;
                outArchive.CreateStreamForCompressing(outStream.GetAddress());
                
                hr = compressor.Compress(codecsInfo,
                                         fileInStream.Get(),
                                         outStream.Get(),
                                         progress.Get(),
                                         compressingResult,
                                         zipExtFilter,
                                         RCZipHandler::s_fileFilter,
                                         isFileFilter);
                if (hr != RC_S_OK)
                {
                    return hr;
                }

                SetItemInfoFromCompressingResult(compressingResult,
                                                 options->m_isAesMode,
                                                 options->m_aesKeyMode,
                                                 *item);
                outArchive.WriteLocalHeader(*item);

                hr = updateCallback->SetOperationResult(ui->m_indexInClient, RC_ARCHIVE_UPDATE_RESULT_OK);
                if (hr != RC_S_OK)
                {
                    return hr;
                }

                unpackSizeTotal += item->m_unPackSize;
                packSizeTotal   += item->m_packSize;
            }
        }
        else
        {
            uint64_t complexity = 0;
            lps->SetSendRatio(false);

            hr = UpdateItemOldData(codecsInfo,
                                   outArchive,
                                   inStream,
                                   *ui,
                                   *item,
                                   progress.Get(),
                                   complexity);
            if (hr != RC_S_OK)
            {
                return hr;
            }

            lps->SetSendRatio(true);
            lps->SetProgressOffset(lps->GetProgressOffset() + complexity);
        }
        items.push_back(item);

        lps->SetProgressOffset(lps->GetProgressOffset() + RCZipHeader::kLocalBlockSize);
    }

    outArchive.WriteCentralDir(items, comment);

    return RC_S_OK;
}

static HResult UpdateArchive2(ICompressCodecsInfo* codecsInfo,
                              RCZipOut& outArchive,
                              RCZipIn* inArchive,
                              IInStream* inStream,
                              const RCVector<RCZipItemExPtr>& inputItems,
                              const RCVector<RCZipUpdate::RCZipUpdateItemPtr>& updateItems,
                              const RCZipCompressionMethodMode* options,
                              const RCByteBuffer& comment,
                              IArchiveUpdateCallback* updateCallback)
{
    uint64_t complexity         = 0;
    uint64_t numFilesToCompress = 0;
    uint64_t numBytesToCompress = 0;

    int32_t i;
    for(i = 0; i < (int32_t)updateItems.size(); i++)
    {
        const RCZipUpdate::RCZipUpdateItemPtr &ui = updateItems[i];
        if (ui->m_newData)
        {
            complexity += ui->m_size;
            numBytesToCompress += ui->m_size;
            numFilesToCompress++;
            /*
            if (ui.Commented)
            complexity += ui.CommentRange.Size;
            */
        }
        else
        {
            RCZipItemExPtr inputItem = inputItems[ui->m_indexInArchive];
            if (inArchive->ReadLocalItemAfterCdItemFull(*inputItem) != RC_S_OK)
            {
                return RC_E_NOTIMPL;
            }
            complexity += inputItem->GetLocalFullSize();
            // complexity += inputItem.GetCentralExtraPlusCommentSize();
        }
        complexity += RCZipHeader::kLocalBlockSize;
        complexity += RCZipHeader::kCentralBlockSize;
    }

    if (comment.GetCapacity() != 0)
    {
        complexity += comment.GetCapacity();
    }
    complexity++; // end of central
    updateCallback->SetTotal(complexity);

    RCZipAddCommon compressor(*options);

    complexity = 0;

#ifdef COMPRESS_MT

    const size_t kNumMaxThreads = (1 << 10);
    uint32_t numThreads = 1 ;
    if(options)
    {
        numThreads = options->m_numThreads;
    }
    if (numThreads > kNumMaxThreads)
    {
        numThreads = kNumMaxThreads;
    }

    const size_t kMemPerThread = (1 << 25);
    const size_t kBlockSize = 1 << 16;

    RCZipCompressionMethodMode options2;
    if (options != 0)
    {
        options2 = *options;
    }

    bool mtMode = ((options != 0) && (numThreads > 1));

    if (numFilesToCompress <= 1)
    {
        mtMode = false;
    }

    if (mtMode)
    {
        byte_t method = options->m_methodSequence.front();
        if (method == RCZipHeader::NCompressionMethod::kStored && !options->m_passwordIsDefined)
        {
            mtMode = false;
        }

        if (method == RCZipHeader::NCompressionMethod::kBZip2)
        {
            uint64_t averageSize = numBytesToCompress / numFilesToCompress;
            uint32_t blockSize = options->m_dicSize;
            if (blockSize == 0)
            {
                blockSize = 1;
            }
            uint64_t averageNumberOfBlocks = averageSize / blockSize;
            uint32_t numBZip2Threads = 32;
            if (averageNumberOfBlocks < numBZip2Threads)
            {
                numBZip2Threads = (uint32_t)averageNumberOfBlocks;
            }
            if (numBZip2Threads < 1)
            {
                numBZip2Threads = 1;
            }
            numThreads = numThreads / numBZip2Threads;
            options2.m_numThreads = numBZip2Threads;
            if (numThreads <= 1)
            {
                mtMode = false;
            }
        }
    }

    if (!mtMode)
#endif
    return UpdateArchive2St(codecsInfo,
                            outArchive,
                            inArchive,
                            inStream,
                            inputItems,
                            updateItems,
                            options,
                            comment,
                            updateCallback);

#ifdef COMPRESS_MT

    HResult hr = RC_S_OK ;
    if(updateCallback == NULL)
    {
        return RC_E_INVALIDARG ;
    }
        
    ISetMultiThreadModePtr setMultiThreadMode ; 
    if(IsSuccess(updateCallback->QueryInterface(ISetMultiThreadMode::IID,(void**)setMultiThreadMode.GetAddress())))
    {
        if(setMultiThreadMode)
        {
            setMultiThreadMode->SetMultiThread(true) ;
        }
    }
    
    RCVector<RCZipItemPtr> items;   
    RCMtProgressMixer* mtProgressMixerSpec = new RCMtProgressMixer;
    ICompressProgressInfoPtr progress = mtProgressMixerSpec;
    mtProgressMixerSpec->Create(updateCallback, true);
    
    IMtCompressProgressInfoPtr spMtProgress ;
    updateCallback->QueryInterface(IMtCompressProgressInfo::IID, (void**)spMtProgress.GetAddress());

    RCMtCompressProgressMixer mtCompressProgressMixer;
    mtCompressProgressMixer.Init(numThreads, mtProgressMixerSpec->m_ratioProgress.Get(),spMtProgress.Get());

    RCMemBlockManagerMt memManager(kBlockSize);
    RCZipUpdateMemRefs refs(&memManager);

    RCZipUpdateThreads threads;
    RCVector<HANDLE> compressingCompletedEvents;
    RCVector<int32_t> threadIndices;  // list threads in order of updateItems

    {
        hr = memManager.AllocateSpaceAlways((size_t)numThreads * (kMemPerThread / kBlockSize));
        if (!IsSuccess(hr))
        {
            return hr;
        }

        for(i = 0; i < (int32_t)updateItems.size(); i++)
        {
            refs.m_refs.push_back(RCMemBlocks2());
        }

        uint32_t i;
        for (i = 0; i < (uint32_t)numThreads; i++)
        {
            threads.m_threads.push_back(RCZipUpdateThreadInfo(options2));
        }

        for (i = 0; i < numThreads; i++)
        {
            RCZipUpdateThreadInfo &threadInfo = threads.m_threads[i];
            threadInfo.m_codecsInfo = codecsInfo;
            hr = threadInfo.CreateEvents();
            if (!IsSuccess(hr))
            {
                return hr;
            }

            threadInfo.m_outStreamSpec = new RCOutMemStream(&memManager);
            hr = threadInfo.m_outStreamSpec->CreateEvents();
            if (!IsSuccess(hr))
            {
                return hr;
            }

            threadInfo.m_outStream = threadInfo.m_outStreamSpec;
            threadInfo.m_isFree = true;
            threadInfo.m_progressSpec = new RCMtCompressProgress();
            threadInfo.m_progress = threadInfo.m_progressSpec;
            threadInfo.m_progressSpec->Init(&mtCompressProgressMixer, (int)i);
            
            hr = threadInfo.CreateThread();
            if (!IsSuccess(hr))
            {
                return hr;
            }
        }
    }
    int32_t mtItemIndex = 0;

    int32_t itemIndex = 0;
    int32_t lastRealStreamItemIndex = -1;

    RCFileFilterManager fileFilterManager(RCZipHandler::s_fileFilter.m_fileList);

    while (itemIndex < (int32_t)updateItems.size())
    {
        if ((uint32_t)threadIndices.size() < numThreads && mtItemIndex < (int32_t)updateItems.size())
        {
            const RCZipUpdate::RCZipUpdateItemPtr& ui = updateItems[mtItemIndex++];
            if (!ui->m_newData)
            {
                continue;
            }
           
            RCZipItemExPtr item;
            if (ui->m_newProperties)
            {
                if (ui->m_isDir)
                {
                    continue;
                }
            }
            else
            {
                item = inputItems[ui->m_indexInArchive];
                if (inArchive->ReadLocalItemAfterCdItemFull(*item) != RC_S_OK)
                {
                    return RC_E_NOTIMPL;
                }

                if (item->IsDir())
                {
                    continue;
                }
            }
            
            ISequentialInStreamPtr fileInStream;
            {
                RCMutexLock spLock( &(mtProgressMixerSpec->m_mixer2->m_criticalSection) );
                HResult res = updateCallback->GetStream(ui->m_indexInClient, fileInStream.GetAddress());
                if (res == RC_S_FALSE || fileInStream == NULL)
                {
                    complexity += ui->m_size;
                    complexity += RCZipHeader::kLocalBlockSize;
                    mtProgressMixerSpec->m_mixer2->SetProgressOffset(complexity);
                    hr = updateCallback->SetOperationResult(ui->m_indexInClient,RC_ARCHIVE_UPDATE_RESULT_OK);
                    if (!IsSuccess(hr))
                    {
                        return hr;
                    }

                    refs.m_refs[mtItemIndex - 1].m_skip = true;
                    continue;
                }
                if (!IsSuccess(res))
                {
                    return res;
                }
            }

            //文件算法过滤器
            bool isFileFilter = false;
            if (!RCZipHandler::s_fileFilter.m_fileList.empty())
            {
                RCVariant prop;
                HResult hr = updateCallback->GetProperty(ui->m_indexInClient, RCPropID::kpidAbsolutePath, prop);
                if (IsSuccess(hr) && IsStringType(prop))
                {
                    RCString absolutePath = GetStringValue(prop);

                    HResult hr = updateCallback->GetProperty(ui->m_indexInClient, RCPropID::kpidRelativePath, prop);
                    if (IsSuccess(hr) && IsStringType(prop))
                    {
                        RCString relativePath = GetStringValue(prop);;

                        isFileFilter = fileFilterManager.IsFilter(absolutePath, relativePath);
                    }
                }
            }

            //文件扩展名算法过滤器
            RCZipExtFilterPtr zipExtFilter;
            if ((!isFileFilter) && (RCZipHandler::s_extFilters.size() > 0))
            {
                RCString::size_type pos = ui->m_name.find_last_of('.');
                if (pos != RCString::npos)
                {
                    RCString uName = RCStringConvert::MultiByteToUnicodeString(ui->m_name) ;
                    pos = uName.find_last_of(_T('.'));
                    RCString ext ;
                    if (pos != RCString::npos)
                    {
                        ext = RCStringUtil::Mid(uName, (int32_t)pos+1) ;
                    }
                    for (uint32_t i = 0; i < RCZipHandler::s_extFilters.size(); i++)
                    {
                        if (RCStringUtil::CompareNoCase(ext,RCZipHandler::s_extFilters[i]->m_fileExt) == 0)
                        {
                            zipExtFilter = RCZipHandler::s_extFilters[i];
                            break;
                        }
                    }
                }
            }

            for (uint32_t i = 0; i < numThreads; i++)
            {
                RCZipUpdateThreadInfo &threadInfo = threads.m_threads[i];
                if (threadInfo.m_isFree)
                {
                    threadInfo.m_isFree = false;
                    threadInfo.m_inStream = fileInStream;

                    // !!!!! we must release ref before sending event
                    // BUG was here in v4.43 and v4.44. It could change ref counter in two threads in same time
                    fileInStream.Release();

                    threadInfo.m_outStreamSpec->Init();
                    threadInfo.m_progressSpec->Reinit();
                    threadInfo.m_updateIndex = mtItemIndex - 1;
                    threadInfo.m_itemIndex = ui->m_indexInClient ;
                    threadInfo.m_zipExtFilter = zipExtFilter;
                    threadInfo.m_isFileFilter = isFileFilter;
                    threadInfo.m_compressEvent.Set();

                    compressingCompletedEvents.push_back(threadInfo.m_compressionCompletedEvent);
                    threadIndices.push_back(i);
                    break;
                }
            }
            continue;
        }

        if (refs.m_refs[itemIndex].m_skip)
        {
            itemIndex++;
            continue;
        }

        const RCZipUpdate::RCZipUpdateItemPtr& ui = updateItems[itemIndex];

        RCZipItemExPtr item(new RCZipItemEx);
        if (!ui->m_newProperties || !ui->m_newData)
        {
            item = inputItems[ui->m_indexInArchive];
            if (inArchive->ReadLocalItemAfterCdItemFull(*item) != S_OK)
            {
                return RC_E_NOTIMPL;
            }
        }

        if (ui->m_newData)
        {
            bool isDir = ((ui->m_newProperties) ? ui->m_isDir : item->IsDir());
            if (isDir)
            {
                WriteDirHeader(outArchive, options, *ui, *item);
            }
            else
            {
                if (lastRealStreamItemIndex < itemIndex)
                {
                    lastRealStreamItemIndex = itemIndex;
                    SetFileHeader(outArchive, *options, *ui, *item);
                    // file Size can be 64-bit !!!
                    outArchive.PrepareWriteCompressedData((uint16_t)item->m_name.length(), ui->m_size, options->m_isAesMode);
                }

                RCMemBlocks2 &memRef = refs.m_refs[itemIndex];
                if (memRef.m_defined)
                {
                    IOutStreamPtr outStream;
                    outArchive.CreateStreamForCompressing(outStream.GetAddress());
                    memRef.WriteToStream(memManager.GetBlockSize(), outStream.Get());
                    SetItemInfoFromCompressingResult(memRef.m_compressingResult,
                                                     options->m_isAesMode,
                                                     options->m_aesKeyMode,
                                                     *item);
                    SetFileHeader(outArchive, *options, *ui, *item);
                    outArchive.WriteLocalHeader(*item);

                    memRef.FreeOpt(&memManager);
                }
                else
                {
                    {
                        RCZipUpdateThreadInfo &thread = threads.m_threads[threadIndices.front()];
                        if (!thread.m_outStreamSpec->WasUnlockEventSent())
                        {
                            IOutStreamPtr outStream;
                            outArchive.CreateStreamForCompressing(outStream.GetAddress());
                            thread.m_outStreamSpec->SetOutStream(outStream.Get());
                            thread.m_outStreamSpec->SetRealStreamMode();
                        }
                    }

                    uint32_t result = ::WaitForMultipleObjects((uint32_t)compressingCompletedEvents.size(),
                                                               &compressingCompletedEvents.front(),
                                                               FALSE,
                                                               INFINITE);
                    int32_t t = (int32_t)(result - WAIT_OBJECT_0);

                    RCZipUpdateThreadInfo &threadInfo = threads.m_threads[threadIndices[t]];
                    threadInfo.m_inStream.Release();
                    threadInfo.m_isFree = true;
                    hr = threadInfo.m_result;
                    if (!IsSuccess(hr))
                    {
                        return hr;
                    }
                    hr = updateCallback->SetOperationResult(threadInfo.m_itemIndex,RC_ARCHIVE_UPDATE_RESULT_OK);
                    if (!IsSuccess(hr))
                    {
                        return hr;
                    }
                    RCVectorUtils::Delete(threadIndices, t);
                    RCVectorUtils::Delete(compressingCompletedEvents, t);
                    if (t == 0)
                    {
                        hr = threadInfo.m_outStreamSpec->WriteToRealStream();
                        if (!IsSuccess(hr))
                        {
                            return hr;
                        }
                        threadInfo.m_outStreamSpec->ReleaseOutStream();
                        SetItemInfoFromCompressingResult(threadInfo.m_compressingResult,
                                                         options->m_isAesMode,
                                                         options->m_aesKeyMode,
                                                         *item);
                        SetFileHeader(outArchive, *options, *ui, *item);
                        outArchive.WriteLocalHeader(*item);
                    }
                    else
                    {
                        RCMemBlocks2 &memRef = refs.m_refs[threadInfo.m_updateIndex];
                        threadInfo.m_outStreamSpec->DetachData(memRef);
                        memRef.m_compressingResult = threadInfo.m_compressingResult;
                        memRef.m_defined = true;
                        continue;
                    }
                }
            }
        }
        else
        {
            hr = UpdateItemOldData(codecsInfo, outArchive, inStream, *ui, *item, progress.Get(), complexity);
            if (!IsSuccess(hr))
            {
                return hr;
            }
        }
        items.push_back(item);
        complexity += RCZipHeader::kLocalBlockSize;
        mtProgressMixerSpec->m_mixer2->SetProgressOffset(complexity);
        itemIndex++;
    }
    outArchive.WriteCentralDir(items, comment);

    return RC_S_OK;
#endif
}

HResult RCZipUpdate::UpdateArchive(ICompressCodecsInfo* codecsInfo,
                                   const RCVector<RCZipItemExPtr>& inputItems,
                                   const RCVector<RCZipUpdateItemPtr>& updateItems,
                                   ISequentialOutStream* seqOutStream,
                                   RCZipIn* inArchive,
                                   RCZipCompressionMethodMode* compressionMethodMode,
                                   IArchiveUpdateCallback* updateCallback)
{
    IOutStreamPtr outStream;
    HResult hr = seqOutStream->QueryInterface(IID_IOutStream, (void **)outStream.GetAddress());
    if (hr != RC_S_OK)
    {
        return hr;
    }

    if (!outStream)
    {
        return RC_E_NOTIMPL;
    }

    RCZipInArchiveInfo archiveInfo;
    if (inArchive != 0)
    {
        inArchive->GetArchiveInfo(archiveInfo);
        if (archiveInfo.m_base != 0 || !inArchive->m_isOkHeaders)
        {
            return RC_E_NOTIMPL;
        }
    }
    else
    {
        archiveInfo.m_startPosition = 0;
    }

    //取注释
    if(updateCallback)
    {
        RCVariant commentValue ;
        HResult hr = updateCallback->GetArchiveProperty(RCPropID::kpidComment,commentValue) ;
        if(IsSuccess(hr) && IsStringType(commentValue))
        {
            RCStringA commentA = RCStringConvert::UnicodeStringToMultiByte(GetStringValue(commentValue)) ;
            if(!commentA.empty())
            {
                archiveInfo.m_comment.SetCapacity(commentA.size()) ;
                memcpy(archiveInfo.m_comment.data(), commentA.c_str(), commentA.size()) ;
            }
        }
    }

    RCZipOut outArchive;
    outArchive.Create(outStream.Get());
    if (archiveInfo.m_startPosition > 0)
    {
        ISequentialInStreamPtr inStream;
        inStream.Attach(inArchive->CreateLimitedStream(0, archiveInfo.m_startPosition));
        hr = CopyBlockToArchive(codecsInfo, inStream.Get(), outArchive, NULL);
        if (hr != RC_S_OK)
        {
            return hr;
        }

        outArchive.MoveBasePosition(archiveInfo.m_startPosition);
    }

    IInStreamPtr inStream;
    if(inArchive != 0)
    {
        inStream.Attach(inArchive->CreateStream());
    }

    return UpdateArchive2(codecsInfo,
                          outArchive,
                          inArchive,
                          inStream.Get(),
                          inputItems,
                          updateItems,
                          compressionMethodMode,
                          archiveInfo.m_comment,
                          updateCallback);
}

END_NAMESPACE_RCZIP
