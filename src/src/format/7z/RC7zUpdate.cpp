/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "format/7z/RC7zUpdate.h"
#include "common/RCStringUtil.h"
#include "common/RCLocalProgress.h"
#include "interface/RCMethodDefs.h"
#include "format/7z/RC7zFolderInStream.h"
#include "filesystem/RCLimitedSequentialInStream.h"
#include "format/common/RCItemNameUtils.h"
#include "common/RCCreateCoder.h"
#include "format/7z/RCArchiveOutHandler.h"
#include "format/common/RCOutStreamWithCRC.h"
#include "interface/IPassword.h"
#include "format/7z/RC7zDecoder.h"
#include "format/common/RCFileFilterManager.h"
#include <algorithm>

#ifdef RCZIP_OS_WIN
    #include "base/RCWindowsDefs.h"
#endif

/////////////////////////////////////////////////////////////////
//常量等定义

BEGIN_NAMESPACE_RCZIP

static const RCString::value_type*  s_k7zMatchFinderForBCJ2_LZMA    = _T("BT2")  ;
static const uint32_t               s_k7zDictionaryForBCJ2_LZMA     = 1 << 20 ;
static const uint32_t               s_k7zAlgorithmForBCJ2_LZMA      = 1 ;
static const uint32_t               s_k7zNumFastBytesForBCJ2_LZMA   = 64 ;

static const uint64_t s_LZMA = 0x030101;
static const uint64_t s_BCJ  = 0x03030103;
static const uint64_t s_BCJ2 = 0x0303011B;
static const uint64_t s_AES  = 0x06F10701;

enum RC7zGroupIndex
{
    COPY_GROUP   = 4,
    LZMA_GROUP   = 5,
    DEFLATE_GROP = 6,
    BZIP2_GROUP  = 7,
    PPMD_GROUP   = 8,
    FILE_FILTER_GROUP = 9
};

static const char* s_k7zExts =
      " lzma 7z ace arc arj bz bz2 deb lzo lzx gz pak rpm sit tgz tbz tbz2 tgz cab ha lha lzh rar zoo"
      " zip jar ear war msi"
      " 3gp avi mov mpeg mpg mpe wmv"
      " aac ape fla flac la mp3 m4a mp4 ofr ogg pac ra rm rka shn swa tta wv wma wav"
      " swf "
      " chm hxi hxs"
      " gif jpeg jpg jp2 png tiff  bmp ico psd psp"
      " awg ps eps cgm dxf svg vrml wmf emf ai md"
      " cad dwg pps key sxi"
      " max 3ds"
      " iso bin nrg mdf img pdi tar cpio xpi"
      " vfd vhd vud vmc vsv"
      " vmdk dsk nvram vmem vmsd vmsn vmss vmtm"
      " inl inc idl acf asa h hpp hxx c cpp cxx rc java cs pas bas vb cls ctl frm dlg def"
      " f77 f f90 f95"
      " asm sql manifest dep "
      " mak clw csproj vcproj sln dsp dsw "
      " class "
      " bat cmd"
      " xml xsd xsl xslt hxk hxc htm html xhtml xht mht mhtml htw asp aspx css cgi jsp shtml"
      " awk sed hta js php php3 php4 php5 phptml pl pm py pyo rb sh tcl vbs"
      " text txt tex ans asc srt reg ini doc docx mcw dot rtf hlp xls xlr xlt xlw ppt pdf"
      " sxc sxd sxi sxg sxw stc sti stw stm odt ott odg otg odp otp ods ots odf"
      " abw afp cwk lwp wpd wps wpt wrf wri"
      " abf afm bdf fon mgf otf pcf pfa snf ttf"
      " dbf mdb nsf ntf wdb db fdb gdb"
      " exe dll ocx vbx sfx sys tlb awx com obj lib out o so "
      " pdb pch idb ncb opt" ;

static RCString::value_type* s_k7zExeExts[] =
{
    _T("dll"),
    _T("exe"),
    _T("ocx"),
    _T("sfx"),
    _T("sys")
};

static RCString::size_type GetReverseSlashPos(const RCString& name)
{    
#ifdef RCZIP_OS_WIN
    RCString::size_type slashPos = name.find_last_of(_T("/\\"));
#else
    RCString::size_type slashPos = name.find_last_of(_T('/'));
#endif
    return slashPos;
}

/** 排序比较函数
*/
template<typename TYPE>
class RC7zUpdateCompare:
    public std::binary_function<TYPE,TYPE,bool>
{
public:

    /** 比较函数原型
    */
    typedef int32_t (*PFNCompareFunction)(const TYPE* p1, const TYPE* p2, void* param) ;

public:

    /** 构造函数
    */
    RC7zUpdateCompare(PFNCompareFunction func, void* param):
      m_func(func),
      m_param(param)
    {
    }

    /** 重载()操作符
    */
    bool operator()(const TYPE& lhs, const TYPE& rhs)
    {
        return (*m_func)(&lhs,&rhs,m_param) < 0 ;
    }

private:

    /** 比较函数指针
    */
    PFNCompareFunction m_func ;
    
    /** 比较函数参数
    */
    void* m_param ;
};

struct RC7zUpdate::RC7zRefItem
{
public:

    /** 更新项
    */
    const RC7zUpdateItemPtr* m_updateItem;

    /** 索引
    */
    uint32_t m_index;

    /** 扩展位置
    */
    uint32_t m_extensionPos;

    /** 名字位置
    */
    uint32_t m_namePos;

    /** 扩展索引
    */
    int32_t m_extensionIndex;

public:

    /** 构造函数
    */
    RC7zRefItem(uint32_t index, const RC7zUpdateItemPtr& ui, bool sortByType) ;
};

class RC7zUpdate::RC7zSolidGroup
{
public:

    /** 压缩模式
    */
    RC7zCompressionMethodMode m_method;

    /** 索引序列
    */
    RCVector<uint32_t> m_indices;
};

class RCFolderOutStream2:
    public IUnknownImpl<ISequentialOutStream>
{
public:

    /** 默认构造函数
    */
    RCFolderOutStream2()
    {
        m_crcStreamSpec = new RCOutStreamWithCRC;
        m_crcStream = m_crcStreamSpec;
    }

    /** 初始化
    @param [in] db 基本数据
    @param [in] startIndex 起始索引
    @param [in] extractStatuses 解压状态
    @param [in] outStream 输出流
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult Init(const RC7zArchiveDatabaseEx* db,
                 uint32_t startIndex,
                 const RCBoolVector* extractStatuses,
                 ISequentialOutStream* outStream);

    /** 是否输出流
    */
    void ReleaseOutStream();

    /** 检查完成状态
    */
    HResult CheckFinishedState() const
    { 
        return (m_currentIndex == (int32_t)m_extractStatuses->size()) ? RC_S_OK: RC_E_FAIL;
    }

    /** 写数据  
    @param [in] data 存放数据的缓冲区
    @param [in] size 缓冲区大小
    @pram [out] processedSize 实际读取的大小
    @return 是返回RC_S_OK，否则返回错误号
    */
    virtual HResult Write(const void* data, uint32_t size, uint32_t* processedSize);

private:

    /** 带CRC的输出流
    */
    RCOutStreamWithCRC* m_crcStreamSpec;

    /** 输出流
    */
    ISequentialOutStreamPtr m_crcStream;

    /** 基本数据
    */
    const RC7zArchiveDatabaseEx* m_db;

    /** 解压状态
    */
    const RCBoolVector* m_extractStatuses;

    /** 输出流
    */
    ISequentialOutStreamPtr m_outStream;

    /** 起始索引
    */
    uint32_t m_startIndex;

    /** 当前索引
    */
    int32_t m_currentIndex;

    /** 文件是否被打开
    */
    bool m_fileIsOpen;

    /** rem
    */
    uint64_t m_rem;

private:

    /** 打开文件
    */
    void OpenFile();

    /** 关闭文件
    */
    void CloseFile();

    /** 关闭文件并设置结果
    */
    HResult CloseFileAndSetResult();

    /** 处理空文件
    */
    HResult ProcessEmptyFiles();
};

HResult RCFolderOutStream2::Init(const RC7zArchiveDatabaseEx* db,
                                 uint32_t startIndex,
                                 const RCBoolVector* extractStatuses,
                                 ISequentialOutStream* outStream)
{
    m_db = db;
    m_startIndex = startIndex;
    m_extractStatuses = extractStatuses;
    m_outStream = outStream;

    m_currentIndex = 0;
    m_fileIsOpen = false;
    return ProcessEmptyFiles();
}

void RCFolderOutStream2::ReleaseOutStream()
{
    m_outStream.Release();
    m_crcStreamSpec->ReleaseStream();
}

void RCFolderOutStream2::OpenFile()
{
    m_crcStreamSpec->SetStream((*m_extractStatuses)[m_currentIndex] ? m_outStream.Get() : NULL);
    m_crcStreamSpec->Init(true);
    m_fileIsOpen = true;
    m_rem = m_db->m_files[m_startIndex + m_currentIndex]->m_size;
}

void RCFolderOutStream2::CloseFile()
{
    m_crcStreamSpec->ReleaseStream();
    m_fileIsOpen = false;
    m_currentIndex++;
}

HResult RCFolderOutStream2::CloseFileAndSetResult()
{
    const RC7zFileItemPtr file = m_db->m_files[m_startIndex + m_currentIndex];

    CloseFile();

    return (file->m_isDir || !file->m_crcDefined || file->m_crc == m_crcStreamSpec->GetCRC()) ? RC_S_OK: RC_S_FALSE;
}

HResult RCFolderOutStream2::ProcessEmptyFiles()
{
    while (m_currentIndex < (int32_t)m_extractStatuses->size() && m_db->m_files[m_startIndex + m_currentIndex]->m_size == 0)
    {
        OpenFile();
        HResult hr = CloseFileAndSetResult();
        if (!IsSuccess(hr))
        {
            return hr;
        }
    }
    return RC_S_OK;
}

HResult RCFolderOutStream2::Write(const void *data, uint32_t size, uint32_t *processedSize)
{
    if (processedSize != NULL)
    {
        *processedSize = 0;
    }

    while (size != 0)
    {
        if (m_fileIsOpen)
        {
            uint32_t cur = size < m_rem ? size : (uint32_t)m_rem;
            HResult hr = m_crcStream->Write(data, cur, &cur);
            if (!IsSuccess(hr))
            {
                return hr;
            }

            if (cur == 0)
            {
                break;
            }
            data = (const byte_t *)data + cur;
            size -= cur;
            m_rem -= cur;
            if (processedSize != NULL)
            {
                *processedSize += cur;
            }
            if (m_rem == 0)
            {
                hr = CloseFileAndSetResult();
                if (!IsSuccess(hr))
                {
                    return hr;
                }

                hr = ProcessEmptyFiles();
                if (!IsSuccess(hr))
                {
                    return hr;
                }
                continue;
            }
        }
        else
        {
            HResult hr = ProcessEmptyFiles();
            if (!IsSuccess(hr))
            {
                return hr;
            }

            if (m_currentIndex == (int32_t)m_extractStatuses->size())
            {
                // we don't support partial extracting
                return RC_E_FAIL;
            }
            OpenFile();
        }
    }
    return RC_S_OK;
}

class RCThreadDecoder:
    public RCVirtThread
{
public:

    /** 默认构造函数
    */
    RCThreadDecoder()
    {
#ifdef COMPRESS_MT
        m_numThreads = 1;
#endif
        m_fosSpec = new RCFolderOutStream2;
        m_fos = m_fosSpec;
        m_hr = RC_E_FAIL;
    }

    /** 运行
    */
    virtual void Execute();

public:

    /** 返回值
    */
    HResult m_hr;

    /** 输入流
    */
    IInStreamPtr m_inStream;

    /** 文件夹输入流
    */
    RCFolderOutStream2* m_fosSpec;

    /** 输入流
    */
    ISequentialOutStreamPtr m_fos;

    /** 起始位置
    */
    uint64_t m_startPos;

    /** 包大小
    */
    const uint64_t* m_packSizes;

    /** 文件夹
    */
    const RC7zFolder* m_folder;

#ifndef _NO_CRYPTO

     /** 加密管理器
     */
    ICryptoGetTextPasswordPtr m_getTextPassword;

#endif

    /** 编码管理器
    */
    ICompressCodecsInfo* m_codecsInfo;

    /** 解码器
    */
    RC7zDecoder m_decoder;

#ifdef COMPRESS_MT

    /** 线程个数
    */
    uint32_t m_numThreads;

#endif
};

void RCThreadDecoder::Execute()
{
    try
    {
#ifndef _NO_CRYPTO
        bool passwordIsDefined;
#endif
        m_hr = m_decoder.Decode(m_codecsInfo,
                                m_inStream.Get(),
                                m_startPos,
                                m_packSizes,
                                *m_folder,
                                m_fos.Get(),
                                NULL
#ifndef _NO_CRYPTO
                                , m_getTextPassword.Get()
                                , passwordIsDefined
#endif

#ifdef COMPRESS_MT
                                , m_numThreads
#else
                                , 1
#endif
                              );
    }
    catch(...)
    {
        m_hr = RC_E_FAIL;
    }

    if (m_hr == RC_S_OK)
    {
        m_hr = m_fosSpec->CheckFinishedState();
    }

    m_fosSpec->ReleaseOutStream();
}

bool static Is86FilteredFolder(const RC7zFolder& f)
{
    for (int32_t i = 0; i < (int32_t)f.m_coders.size(); i++)
    {
        RCMethodID m = f.m_coders[i].m_methodID;
        if (m == s_BCJ || m == s_BCJ2)
        {
            return true;
        }
    }
    return false;
}

bool static IsEncryptedFolder(const RC7zFolder& f)
{
    for (int32_t i = 0; i < (int32_t)f.m_coders.size(); i++)
    {
        RCMethodID m = f.m_coders[i].m_methodID;
        if (m == s_AES)
        {
            return true;
        }
    }
    return false;
}


#ifndef _NO_CRYPTO

class RCCryptoGetTextPassword:
    public IUnknownImpl<ICryptoGetTextPassword>
{
public:

    /** 获取密码
    @param [out] password 返回密码
    @return 成功返回RC_S_OK，否则返回错误号
    */
    virtual HResult CryptoGetTextPassword(RCString& password);

public:

    /** 密码
    */
    RCString m_password;
};

HResult RCCryptoGetTextPassword::CryptoGetTextPassword(RCString& password)
{
    password = m_password;
    return RC_S_OK;
}

#endif

static const int32_t kNumGroupsMax = 10;

static bool Is86Group(int32_t group)
{ 
    if (group >= COPY_GROUP)
    {
        return false;
    }
    return (group & 1) != 0;
}

static bool IsEncryptedGroup(int32_t group)
{ 
    if (group >= COPY_GROUP)
    {
        return false;
    }
    return (group & 2) != 0; 
}

static bool IsFilterGroup(int32_t group)
{
    if (group == COPY_GROUP || group == LZMA_GROUP || 
        group == DEFLATE_GROP || group == BZIP2_GROUP || 
        group == PPMD_GROUP || group == FILE_FILTER_GROUP )
    {
        return true;
    }
    return false;
}

static int GetGroupIndex(bool encrypted, int32_t bcjFiltered)
{ 
    return (encrypted ? 2 : 0) + (bcjFiltered ? 1 : 0);
}

/////////////////////////////////////////////////////////////////
//RC7zUpdate class implementation

RC7zUpdateItem::RC7zUpdateItem():
    m_isAnti(false),
    m_isDir(false),
    m_attribDefined(false),
    m_cTimeDefined(false),
    m_aTimeDefined(false),
    m_mTimeDefined(false)
{
}
    
bool RC7zUpdateItem::HasStream() const
{
    return !m_isDir && !m_isAnti && m_size != 0 ;
}

void RC7zUpdateItem::SetDirStatusFromAttrib()
{
#ifdef RCZIP_OS_WIN
    m_isDir = ((m_attrib & FILE_ATTRIBUTE_DIRECTORY) != 0) ;
#else
    #error "Not supported"
#endif
};

int32_t RC7zUpdateItem::GetExtensionPos() const
{
    RCString::size_type slashPos = GetReverseSlashPos(m_name);
    RCString::size_type dotPos = m_name.find_last_of(_T('.')) ;
    if (dotPos == RCString::npos || (dotPos < slashPos && slashPos != RCString::npos))
    {
        return (int32_t)m_name.size();
    }
    return (int32_t)dotPos + 1 ;
}

RCString RC7zUpdateItem::GetExtension() const
{
    return RCStringUtil::Mid(m_name,GetExtensionPos()) ;
}

RC7zUpdate::RC7zRefItem::RC7zRefItem(uint32_t index, const RC7zUpdateItemPtr& ui, bool sortByType):
    m_updateItem(&ui),
    m_index(index),
    m_extensionPos(0),
    m_namePos(0),
    m_extensionIndex(0)
{
    if (sortByType)
    {
        int32_t slashPos = (int32_t)GetReverseSlashPos(ui->m_name);
        m_namePos = ((slashPos >= 0) ? (slashPos + 1) : 0);
        int32_t dotPos = static_cast<int32_t>( (uint32_t)ui->m_name.rfind(_T('.')) );
        if (dotPos < 0 || (dotPos < slashPos && slashPos >= 0))
        {
            m_extensionPos = (uint32_t)ui->m_name.size();
        }
        else
        {
            m_extensionPos = dotPos + 1;
            RCString us = RCStringUtil::Mid(ui->m_name, m_extensionPos);
            if (!us.empty())
            {
                RCStringUtil::MakeLower(us);
                int32_t i = 0 ;
                RCStringA s;
                int32_t len = (int32_t)us.size() ;
                for (i = 0; i < len; i++)
                {
                    wchar_t c = wchar_t(us[i]) ;
                    if (c >= 0x80)
                    {
                        break;
                    }
                    s += char(c) ;
                }
                if (i == len)
                {
                    m_extensionIndex = GetExtIndex(s.c_str());
                }
                else
                {
                    m_extensionIndex = 0;
                }
            }
        }
    }
}

HResult RC7zUpdate::Update(ICompressCodecsInfo* codecsInfo,
                           IInStream *inStream,
                           const RC7zArchiveDatabaseEx* db,
                           const RCVector<RC7zUpdateItemPtr>& updateItems,
                           RC7zOutArchive& archive,
                           RC7zArchiveDatabase& newDatabase,
                           ISequentialOutStream *seqOutStream,
                           IArchiveUpdateCallback *updateCallback,
                           const RC7zUpdateOptions& options)
{
    uint64_t numSolidFiles = options.m_numSolidFiles;
    if (numSolidFiles == 0)
    {
        numSolidFiles = 1;
    }

    uint64_t startBlockSize = (db != NULL) ? db->m_archiveInfo.m_startPosition: 0;
    if (startBlockSize > 0 && !options.m_removeSfxBlock)
    {
        HResult hr = WriteRange(codecsInfo, inStream, seqOutStream, 0, startBlockSize, NULL) ;
        if(!IsSuccess(hr))
        {
            return hr ;
        }
    }

    RCVector<int32_t> fileIndexToUpdateIndexMap;
    RCVector<RC7zFolderRepack> folderRefs;
    uint64_t complexity = 0;
    uint64_t inSizeForReduce2 = 0;
    bool needEncryptedRepack = false;

    if (db != NULL)
    {
        fileIndexToUpdateIndexMap.reserve(db->m_files.size());
        int32_t i;
        for (i = 0; i < (int32_t)db->m_files.size(); i++)
        {
            fileIndexToUpdateIndexMap.push_back(-1);
        }

        for(i = 0; i < (int32_t)updateItems.size(); i++)
        {
            int32_t index = updateItems[i]->m_indexInArchive;
            if (index != -1)
            {
                fileIndexToUpdateIndexMap[index] = i;
            }
        }

        for(i = 0; i < (int32_t)db->m_folders.size(); i++)
        {
            RC7zNum indexInFolder = 0;
            RC7zNum numCopyItems = 0;
            RC7zNum numUnpackStreams = db->m_numUnpackStreamsVector[i];
            uint64_t repackSize = 0;

            if (i < (int32_t)db->m_folderStartFileIndex.size())
            {
                for (RC7zNum fi = db->m_folderStartFileIndex[i]; indexInFolder < numUnpackStreams; fi++)
                {
                    const RC7zFileItemPtr file = db->m_files[fi];
                    if (file->m_hasStream)
                    {
                        indexInFolder++;
                        int32_t updateIndex = fileIndexToUpdateIndexMap[fi];
                        if (updateIndex >= 0 && !updateItems[updateIndex]->m_newData)
                        {
                            numCopyItems++;
                            repackSize += file->m_size;
                        }
                    }
                }
            }

            if (numCopyItems == 0)
            {
                continue;
            }

            RC7zFolderRepack rep;
            rep.m_folderIndex = i;
            rep.m_numCopyFiles = numCopyItems;
            const RC7zFolder &f = db->m_folders[i];
            bool isEncrypted = IsEncryptedFolder(f);
            rep.m_group = GetGroupIndex(isEncrypted, Is86FilteredFolder(f));
            folderRefs.push_back(rep);
            if (numCopyItems == numUnpackStreams)
            {
                complexity += db->GetFolderFullPackSize(i);
            }
            else
            {
                complexity += repackSize;
                if (repackSize > inSizeForReduce2)
                {
                    inSizeForReduce2 = repackSize;
                }
                if (isEncrypted)
                {
                    needEncryptedRepack = true;
                }
            }
        }

        std::sort(folderRefs.begin(),
                  folderRefs.end(),
                  RC7zUpdateCompare<RC7zFolderRepack>(&RC7zUpdate::CompareFolderRepacks, (void*)db));
    }

    ////////////////////////////

    int32_t i;
    uint64_t inSizeForReduce = 0;
    for(i = 0; i < (int32_t)updateItems.size(); i++)
    {
        const RC7zUpdateItemPtr& ui = updateItems[i];
        if (ui->m_newData)
        {
            complexity += ui->m_size;
            if (numSolidFiles != 1)
            {
                inSizeForReduce += ui->m_size;
            }
            else if (ui->m_size > inSizeForReduce)
            {
                inSizeForReduce = ui->m_size;
            }
        }
    }

    if (inSizeForReduce2 > inSizeForReduce)
    {
        inSizeForReduce = inSizeForReduce2;
    }

    uint32_t kMinReduceSize = (1 << 16);
    if (inSizeForReduce < kMinReduceSize)
    {
        inSizeForReduce = kMinReduceSize;
    }

    HResult hr = updateCallback->SetTotal(complexity) ;
    if(!IsSuccess(hr))
    {
        return hr ;
    }
    complexity = 0;
    hr = updateCallback->SetCompleted(complexity) ;
    if(!IsSuccess(hr))
    {
        return hr ;
    }

    RCLocalProgress *lps = new RCLocalProgress;
    ICompressProgressInfoPtr progress = lps;
    lps->Init(updateCallback, true);
 
    RCThreadDecoder threadDecoder;
    if (!folderRefs.empty())
    {
        threadDecoder.m_codecsInfo = codecsInfo;

        //HResult hr = threadDecoder.Start();
        //if (IsSuccess(hr))
        //{
        //     return hr;
        // }
    }

    RCVector<RC7zSolidGroup> groups;
    for (i = 0; i < kNumGroupsMax; i++)
    {
        groups.push_back(RC7zSolidGroup());
    }

    {
        // ---------- Split files to 2 groups ----------

        bool useFilters = options.m_useFilters;
        const RC7zCompressionMethodMode& method = *options.m_method;

        if (method.m_methods.size() != 1 || method.m_binds.size() != 0)
        {
            useFilters = false;
        }

        RCFileFilterManager fileFilterManager(RCArchiveOutHandler::s_fileFilter.m_fileList);

        for (i = 0; i < (int32_t)updateItems.size(); i++)
        {
            const RC7zUpdateItemPtr ui = updateItems[i];
            if (!ui->m_newData || !ui->HasStream())
            {
                continue;
            }

            //文件算法过滤器
            bool isFileFilter = false;
            if (!RCArchiveOutHandler::s_fileFilter.m_fileList.empty())
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

            if (isFileFilter)
            {
                RCOneMethodInfo oneMethodInfo;
                RC7zArchiveMethodFull methodFull;
                switch (RCArchiveOutHandler::s_fileFilter.m_methodID)
                {
                case RCMethod::ID_COMPRESS_COPY:
                    {
                        methodFull.m_id = RCMethod::ID_COMPRESS_COPY;
                    }
                    break;

                case RCMethod::ID_COMPRESS_LZMA:
                    {
                        if (groups[FILE_FILTER_GROUP].m_indices.size() == 0)
                        {
                            SetOneMethodProp(oneMethodInfo, RCCoderPropID::kDictionarySize, uint64_t(RCArchiveOutHandler::s_fileFilter.m_lzma.m_dicSize));
                            SetOneMethodProp(oneMethodInfo, RCCoderPropID::kAlgorithm, uint64_t(RCArchiveOutHandler::s_fileFilter.m_lzma.m_algo));
                            SetOneMethodProp(oneMethodInfo, RCCoderPropID::kNumFastBytes, uint64_t(RCArchiveOutHandler::s_fileFilter.m_lzma.m_fastBytes));
                            SetOneMethodProp(oneMethodInfo, RCCoderPropID::kMatchFinder, RCString(RCArchiveOutHandler::s_fileFilter.m_lzma.m_matchFinder));
#ifdef COMPRESS_MT
                            SetOneMethodProp(oneMethodInfo, RCCoderPropID::kNumThreads, uint64_t(method.m_numThreads));
#endif
                            methodFull.m_id = RCMethod::ID_COMPRESS_LZMA;
                        }
                    }
                    break;

                case RCMethod::ID_COMPRESS_DEFLATE_COM:
                    {
                        if (groups[FILE_FILTER_GROUP].m_indices.size() == 0)
                        {
                            SetOneMethodProp(oneMethodInfo, RCCoderPropID::kAlgorithm, uint64_t(RCArchiveOutHandler::s_fileFilter.m_deflate.m_algo));
                            SetOneMethodProp(oneMethodInfo, RCCoderPropID::kNumFastBytes, uint64_t(RCArchiveOutHandler::s_fileFilter.m_deflate.m_fastBytes));
                            SetOneMethodProp(oneMethodInfo, RCCoderPropID::kNumPasses, uint64_t(RCArchiveOutHandler::s_fileFilter.m_deflate.m_numPasses));

                            methodFull.m_id = RCMethod::ID_COMPRESS_DEFLATE_COM;
                        }
                    }
                    break;

                case RCMethod::ID_COMPRESS_BZIP2:
                    {
                        if (groups[FILE_FILTER_GROUP].m_indices.size() == 0)
                        {
                            SetOneMethodProp(oneMethodInfo, RCCoderPropID::kNumPasses, uint64_t(RCArchiveOutHandler::s_fileFilter.m_bzip2.m_numPasses));
                            SetOneMethodProp(oneMethodInfo, RCCoderPropID::kDictionarySize, uint64_t(RCArchiveOutHandler::s_fileFilter.m_bzip2.m_dicSize));
#ifdef COMPRESS_MT
                            SetOneMethodProp(oneMethodInfo, RCCoderPropID::kNumThreads, uint64_t(method.m_numThreads));
#endif
                            methodFull.m_id = RCMethod::ID_COMPRESS_BZIP2;
                        }
                    }
                    break;

                case RCMethod::ID_COMPRESS_PPMD:
                    {
                        if (groups[FILE_FILTER_GROUP].m_indices.size() == 0)
                        {
                            SetOneMethodProp(oneMethodInfo, RCCoderPropID::kUsedMemorySize, uint64_t(RCArchiveOutHandler::s_fileFilter.m_ppmd.m_useMemSize));
                            SetOneMethodProp(oneMethodInfo, RCCoderPropID::kOrder, uint64_t(RCArchiveOutHandler::s_fileFilter.m_ppmd.m_order));

                            methodFull.m_id = RCMethod::ID_COMPRESS_PPMD;
                        }
                    }
                    break;
                }

                groups[FILE_FILTER_GROUP].m_indices.push_back(i);
                if (groups[FILE_FILTER_GROUP].m_indices.size() == 1)
                {
                    groups[FILE_FILTER_GROUP].m_method = method;
                    methodFull.m_properties = oneMethodInfo.m_properties;

                    if (method.m_methods.size() > 0)
                    {
                        methodFull.m_numInStreams = method.m_methods[0].m_numInStreams;
                        methodFull.m_numOutStreams = method.m_methods[0].m_numOutStreams;
                    }
                    else
                    {
                        methodFull.m_numInStreams = 1;
                        methodFull.m_numOutStreams = 1;
                    }
                    groups[FILE_FILTER_GROUP].m_method.m_methods.clear();
                    groups[FILE_FILTER_GROUP].m_method.m_methods.push_back(methodFull);
                }
                continue;
            }

            bool filteredGroup = false;
            if ((!isFileFilter) && useFilters)
            {
                RCString::size_type dotPos = ui->m_name.find_last_of(L'.');
                if (dotPos != RCString::npos)
                {
                    if (IsExeFile(RCStringUtil::Mid(ui->m_name, (int32_t)(dotPos + 1))))
                    {
                        groups[GetGroupIndex(method.m_passwordIsDefined, true)].m_indices.push_back(i);
                        continue;
                    }
                }
            }

            //文件扩展名算法过滤器
            if ((!isFileFilter) && (RCArchiveOutHandler::s_extFilters.size() > 0))
            {
                const RCString& name = ui->m_name;
                int32_t dotPos = static_cast<int32_t>( (uint32_t)name.find_last_of(_T('.')) );
                if (dotPos >= 0)
                {
                    int32_t groupIndex = -1;
                    RCOneMethodInfo oneMethodInfo;
                    RC7zArchiveMethodFull methodFull;
                    RCString ext = RCStringUtil::Mid(name,dotPos + 1);
                    for (uint32_t num=0; num < RCArchiveOutHandler::s_extFilters.size(); num++)
                    {
                        if (RCStringUtil::CompareNoCase(ext, RCArchiveOutHandler::s_extFilters[num]->m_fileExt) != 0)
                        {
                            continue;
                        }

                        switch (RCArchiveOutHandler::s_extFilters[num]->m_methodID)
                        {
                        case RCMethod::ID_COMPRESS_COPY:
                            {
                                groupIndex = COPY_GROUP;

                                methodFull.m_id = RCMethod::ID_COMPRESS_COPY;
                            }
                            break;

                        case RCMethod::ID_COMPRESS_LZMA:
                            {
                                groupIndex = LZMA_GROUP;

                                if (groups[LZMA_GROUP].m_indices.size() == 0)
                                {
                                    SetOneMethodProp(oneMethodInfo, RCCoderPropID::kDictionarySize, uint64_t(RCArchiveOutHandler::s_extFilters[num]->m_lzma.m_dicSize));
                                    SetOneMethodProp(oneMethodInfo, RCCoderPropID::kAlgorithm, uint64_t(RCArchiveOutHandler::s_extFilters[num]->m_lzma.m_algo));
                                    SetOneMethodProp(oneMethodInfo, RCCoderPropID::kNumFastBytes, uint64_t(RCArchiveOutHandler::s_extFilters[num]->m_lzma.m_fastBytes));
                                    SetOneMethodProp(oneMethodInfo, RCCoderPropID::kMatchFinder, RCString(RCArchiveOutHandler::s_extFilters[num]->m_lzma.m_matchFinder));
#ifdef COMPRESS_MT
                                    SetOneMethodProp(oneMethodInfo, RCCoderPropID::kNumThreads, uint64_t(method.m_numThreads));
#endif
                                    methodFull.m_id = RCMethod::ID_COMPRESS_LZMA;
                                }
                            }
                            break;

                        case RCMethod::ID_COMPRESS_DEFLATE_COM:
                            {
                                groupIndex = DEFLATE_GROP;

                                if (groups[DEFLATE_GROP].m_indices.size() == 0)
                                {
                                    SetOneMethodProp(oneMethodInfo, RCCoderPropID::kAlgorithm, uint64_t(RCArchiveOutHandler::s_extFilters[num]->m_deflate.m_algo));
                                    SetOneMethodProp(oneMethodInfo, RCCoderPropID::kNumFastBytes, uint64_t(RCArchiveOutHandler::s_extFilters[num]->m_deflate.m_fastBytes));
                                    SetOneMethodProp(oneMethodInfo, RCCoderPropID::kNumPasses, uint64_t(RCArchiveOutHandler::s_extFilters[num]->m_deflate.m_numPasses));

                                    methodFull.m_id = RCMethod::ID_COMPRESS_DEFLATE_COM;
                                }
                            }
                            break;

                        case RCMethod::ID_COMPRESS_BZIP2:
                            {
                                groupIndex = BZIP2_GROUP;

                                if (groups[BZIP2_GROUP].m_indices.size() == 0)
                                {
                                    SetOneMethodProp(oneMethodInfo, RCCoderPropID::kNumPasses, uint64_t(RCArchiveOutHandler::s_extFilters[num]->m_bzip2.m_numPasses));
                                    SetOneMethodProp(oneMethodInfo, RCCoderPropID::kDictionarySize, uint64_t(RCArchiveOutHandler::s_extFilters[num]->m_bzip2.m_dicSize));
#ifdef COMPRESS_MT
                                    SetOneMethodProp(oneMethodInfo, RCCoderPropID::kNumThreads, uint64_t(method.m_numThreads));
#endif
                                    methodFull.m_id = RCMethod::ID_COMPRESS_BZIP2;
                                }
                            }
                            break;

                        case RCMethod::ID_COMPRESS_PPMD:
                            {
                                groupIndex = PPMD_GROUP;

                                if (groups[PPMD_GROUP].m_indices.size() == 0)
                                {
                                    SetOneMethodProp(oneMethodInfo, RCCoderPropID::kUsedMemorySize, uint64_t(RCArchiveOutHandler::s_extFilters[num]->m_ppmd.m_useMemSize));
                                    SetOneMethodProp(oneMethodInfo, RCCoderPropID::kOrder, uint64_t(RCArchiveOutHandler::s_extFilters[num]->m_ppmd.m_order));

                                    methodFull.m_id = RCMethod::ID_COMPRESS_PPMD;
                                }
                            }
                            break;
                        }
                    }

                    if (groupIndex != -1)
                    {
                        groups[groupIndex].m_indices.push_back(i);
                        if (groups[groupIndex].m_indices.size() == 1)
                        {
                            groups[groupIndex].m_method = method;
                            methodFull.m_properties = oneMethodInfo.m_properties;

                            if (method.m_methods.size() > 0)
                            {
                                methodFull.m_numInStreams = method.m_methods[0].m_numInStreams;
                                methodFull.m_numOutStreams = method.m_methods[0].m_numOutStreams;
                            }
                            else
                            {
                                methodFull.m_numInStreams = 1;
                                methodFull.m_numOutStreams = 1;
                            }
                            groups[groupIndex].m_method.m_methods.clear();
                            groups[groupIndex].m_method.m_methods.push_back(methodFull);
                        }
                        continue;
                    }
                }
            }
            groups[GetGroupIndex(method.m_passwordIsDefined, false)].m_indices.push_back(i);
        }
    }

#ifndef _NO_CRYPTO

    RCCryptoGetTextPassword *getPasswordSpec = NULL;
    if (needEncryptedRepack)
    {
        getPasswordSpec = new RCCryptoGetTextPassword;
        threadDecoder.m_getTextPassword = getPasswordSpec;

        if (options.m_method->m_passwordIsDefined)
        {
            getPasswordSpec->m_password = options.m_method->m_password;
        }
        else
        {
            ICryptoGetTextPasswordPtr getDecoderPassword;
                
            hr = updateCallback->QueryInterface(IID_ICryptoGetTextPassword, (void**)getDecoderPassword.GetAddress());
            if (!IsSuccess(hr))
            {
                return hr;
            }

            if (!getDecoderPassword)
            {
                return RC_E_NOTIMPL;
            }

            RCString password;
            HResult hr = getDecoderPassword->CryptoGetTextPassword(password);
            if (!IsSuccess(hr))
            {
                return hr;
            }
            getPasswordSpec->m_password = password;
        }
    }

#endif

    // ---------- Compress ----------

    hr = archive.Create(seqOutStream, false);
    if (!IsSuccess(hr))
    {
        return hr;
    }
    hr = archive.SkipPrefixArchiveHeader();
    if (!IsSuccess(hr))
    {
        return hr;
    }

    int32_t folderRefIndex = 0;
    lps->SetProgressOffset(0);

    for (int32_t groupIndex = 0; groupIndex < kNumGroupsMax; groupIndex++)
    {
        const RC7zSolidGroup &group = groups[groupIndex];

        RC7zCompressionMethodMode method;
        if (Is86Group(groupIndex))
        {
            MakeExeMethod(*options.m_method, options.m_maxFilter, method);
        }
        else if (IsFilterGroup(groupIndex) && (group.m_indices.size() > 0))
        {
            method = group.m_method;
        }
        else
        {
            method = *options.m_method;
        }
        bool needEncrypted = false ;
        if(groupIndex >= COPY_GROUP)
        {
            needEncrypted = method.m_passwordIsDefined ;
        }
        if (needEncrypted || IsEncryptedGroup(groupIndex))
        {
            if (!method.m_passwordIsDefined)
            {
#ifndef _NO_CRYPTO
                if (getPasswordSpec)
                {
                    method.m_password = getPasswordSpec->m_password;
                }
#endif
                method.m_passwordIsDefined = true;
                if(method.m_password.empty())
                {
                    //禁止使用空密码
                    method.m_passwordIsDefined = false ;
                }
            }
        }
        else
        {
            method.m_passwordIsDefined = false;
            method.m_password.clear();
        }

        RC7zEncoder encoder(method);

        for (; folderRefIndex < (int32_t)folderRefs.size(); folderRefIndex++)
        {
            const RC7zFolderRepack& rep = folderRefs[folderRefIndex];
            if (rep.m_group != groupIndex)
            {
                break;
            }
            int32_t folderIndex = rep.m_folderIndex;

            if (rep.m_numCopyFiles == db->m_numUnpackStreamsVector[folderIndex])
            {
                uint64_t packSize = db->GetFolderFullPackSize(folderIndex);
                hr = WriteRange(codecsInfo,
                                inStream,
                                archive.GetSeqOutStream().Get(),
                                db->GetFolderStreamPos(folderIndex, 0),
                                packSize,
                                progress.Get());
                if (!IsSuccess(hr))
                {
                    return hr;
                }
                lps->SetProgressOffset(lps->GetProgressOffset()+ packSize);

                const RC7zFolder& folder = db->m_folders[folderIndex];
                RC7zNum startIndex = db->m_folderStartPackStreamIndex[folderIndex];
                for (int32_t j = 0; j < (int32_t)folder.m_packStreams.size(); j++)
                {
                    newDatabase.m_packSizes.push_back(db->m_packSizes[startIndex + j]);
                    // newDatabase.PackCRCsDefined.Add(db.PackCRCsDefined[startIndex + j]);
                    // newDatabase.PackCRCs.Add(db.PackCRCs[startIndex + j]);
                }
                newDatabase.m_folders.push_back(folder);
            }
            else
            {
                RCStreamBinder sb;
                hr = sb.CreateEvents();
                if (!IsSuccess(hr))
                {
                    return hr;
                }

                ISequentialOutStreamPtr sbOutStream;
                ISequentialInStreamPtr sbInStream;
                sb.CreateStreams(sbInStream.GetAddress(), sbOutStream.GetAddress());
                RCBoolVector extractStatuses;

                RC7zNum numUnpackStreams = db->m_numUnpackStreamsVector[folderIndex];
                RC7zNum indexInFolder = 0;

                for (RC7zNum fi = db->m_folderStartFileIndex[folderIndex]; indexInFolder < numUnpackStreams; fi++)
                {
                    bool needExtract = false;
                    if (db->m_files[fi]->m_hasStream)
                    {
                        indexInFolder++;
                        int updateIndex = fileIndexToUpdateIndexMap[fi];
                        if (updateIndex >= 0 && !updateItems[updateIndex]->m_newData)
                        {
                            needExtract = true;
                        }
                    }
                    extractStatuses.push_back(needExtract);
                }

                hr = threadDecoder.m_fosSpec->Init(db, db->m_folderStartFileIndex[folderIndex], &extractStatuses, sbOutStream.Get());
                if (!IsSuccess(hr))
                {
                    return hr;
                }

                sbOutStream.Release();

                threadDecoder.m_inStream = inStream;
                threadDecoder.m_folder = &db->m_folders[folderIndex];
                threadDecoder.m_startPos = db->GetFolderStreamPos(folderIndex, 0);
                threadDecoder.m_packSizes = &db->m_packSizes[db->m_folderStartPackStreamIndex[folderIndex]];

                threadDecoder.Start();

                int32_t startPackIndex = (int32_t)newDatabase.m_packSizes.size();
                RC7zFolder newFolder;
                hr = encoder.Encode(codecsInfo,
                                    sbInStream.Get(),
                                    NULL,
                                    &inSizeForReduce,
                                    newFolder,
                                    archive.GetSeqOutStream().Get(),
                                    newDatabase.m_packSizes,
                                    progress.Get());
                if (!IsSuccess(hr))
                {
                    threadDecoder.WaitFinish() ;
                    return hr;
                }

                threadDecoder.WaitFinish();

                if (!IsSuccess(threadDecoder.m_hr))
                {
                    return threadDecoder.m_hr;
                }

                for (; startPackIndex < (int32_t)newDatabase.m_packSizes.size(); startPackIndex++)
                {
                    lps->SetOutSize(lps->GetOutSize()+newDatabase.m_packSizes[startPackIndex]);
                }
                lps->SetInSize(lps->GetInSize()+newFolder.GetUnpackSize());

                newDatabase.m_folders.push_back(newFolder);
            }

            newDatabase.m_numUnpackStreamsVector.push_back(rep.m_numCopyFiles);

            RC7zNum numUnpackStreams = db->m_numUnpackStreamsVector[folderIndex];

            RC7zNum indexInFolder = 0;
            for (RC7zNum fi = db->m_folderStartFileIndex[folderIndex]; indexInFolder < numUnpackStreams; fi++)
            {
                RC7zFileItemPtr file(new RC7zFileItem);
                if (!file)
                {
                    return RC_S_FALSE;
                }

                RC7zFileItem2Ptr file2(new RC7zFileItem2);
                if (!file2)
                {
                    return RC_S_FALSE;
                }

                db->GetFile(fi, file, file2);
                if (file->m_hasStream)
                {
                    indexInFolder++;
                    int updateIndex = fileIndexToUpdateIndexMap[fi];
                    if (updateIndex >= 0)
                    {
                        const RC7zUpdateItemPtr ui = updateItems[updateIndex];
                        if (ui->m_newData)
                        {
                            continue;
                        }
                        if (ui->m_newProperties)
                        {
                            RC7zFileItemPtr uf ( new RC7zFileItem) ;
                            FromUpdateItemToFileItem(ui, uf, file2);
                            uf->m_size = file->m_size;
                            uf->m_crc = file->m_crc;
                            uf->m_crcDefined = file->m_crcDefined;
                            uf->m_hasStream = file->m_hasStream;
                            file = uf;
                        }
                        newDatabase.AddFile(file, file2);
                    }
                }
            }
        }

        int32_t numFiles = (int32_t)group.m_indices.size();
        if (numFiles == 0)
        {
            continue;
        }
        RCVector<RC7zRefItem> refItems;
        refItems.reserve(numFiles);
        bool sortByType = (numSolidFiles > 1);
        for (i = 0; i < numFiles; i++)
        {
            refItems.push_back(RC7zRefItem(group.m_indices[i], updateItems[group.m_indices[i]], sortByType));
        }

        std::sort(refItems.begin(),
                  refItems.end(),
                  RC7zUpdateCompare<RC7zRefItem>(&RC7zUpdate::CompareUpdateItems, (void*)&sortByType));

        RCUIntVector indices;
        indices.reserve(numFiles);

        for (i = 0; i < numFiles; i++)
        {
            uint32_t index = refItems[i].m_index;
            indices.push_back(index);
            /*
            const CUpdateItem &ui = updateItems[index];
            CFileItem file;
            if (ui.NewProps)
            FromUpdateItemToFileItem(ui, file);
            else
            file = db.Files[ui.IndexInArchive];
            if (file.IsAnti || file.IsDir)
            return E_FAIL;
            newDatabase.Files.Add(file);
            */
        }

        for (i = 0; i < numFiles;)
        {
            uint64_t totalSize = 0;
            int numSubFiles;
            RCString prevExtension;
            for (numSubFiles = 0; i + numSubFiles < numFiles && numSubFiles < numSolidFiles; numSubFiles++)
            {
                const RC7zUpdateItemPtr ui = updateItems[indices[i + numSubFiles]];
                totalSize += ui->m_size;
                if (totalSize > options.m_numSolidBytes)
                {
                    break;
                }
                if (options.m_solidExtension)
                {
                    RCString ext = ui->GetExtension();
                    if (numSubFiles == 0)
                    {
                        prevExtension = ext;
                    }
                    else
                    {
                        if (RCStringUtil::CompareNoCase(ext, prevExtension) != 0)
                        {
                            break;
                        }
                    }
                }
            }
            if (numSubFiles < 1)
            {
                numSubFiles = 1;
            }

            RC7zFolderInStream* inStreamSpec = new RC7zFolderInStream;
            ISequentialInStreamPtr solidInStream(inStreamSpec);
            inStreamSpec->Init(updateCallback, &indices[i], numSubFiles);

            RC7zFolder folderItem;

            int32_t startPackIndex = (int32_t)newDatabase.m_packSizes.size();
            hr = encoder.Encode(codecsInfo,
                                solidInStream.Get(),
                                NULL,
                                &inSizeForReduce,
                                folderItem,
                                archive.GetSeqOutStream().Get(),
                                newDatabase.m_packSizes,
                                progress.Get());
            if (!IsSuccess(hr))
            {
                return hr;
            }

            for (; startPackIndex < (int32_t)newDatabase.m_packSizes.size(); startPackIndex++)
            {
                lps->SetOutSize(lps->GetOutSize()+newDatabase.m_packSizes[startPackIndex]);
            }

            lps->SetInSize(lps->GetInSize()+folderItem.GetUnpackSize());
            // for ()
            // newDatabase.PackCRCsDefined.Add(false);
            // newDatabase.PackCRCs.Add(0);

            newDatabase.m_folders.push_back(folderItem);

            RC7zNum numUnpackStreams = 0;
            for (int32_t subIndex = 0; subIndex < numSubFiles; subIndex++)
            {
                const RC7zUpdateItemPtr ui = updateItems[indices[i + subIndex]];

                RC7zFileItemPtr file(new RC7zFileItem);
                if (!file)
                {
                    return RC_S_FALSE;
                }

                RC7zFileItem2Ptr file2(new RC7zFileItem2);
                if (!file2)
                {
                    return RC_S_FALSE;
                }

                if (ui->m_newProperties)
                    FromUpdateItemToFileItem(ui, file, file2);
                else
                    db->GetFile(ui->m_indexInArchive, file, file2);

                if (file2->m_isAnti || file->m_isDir)
                {
                    return RC_E_FAIL;
                }

                /*
                CFileItem &file = newDatabase.Files[
                startFileIndexInDatabase + i + subIndex];
                */
                if(subIndex >= (int32_t)inStreamSpec->m_processed.size() )
                {
                    return RC_E_FAIL;
                }
                if (!inStreamSpec->m_processed[subIndex])
                {
                    continue;
                    // file.Name += L".locked";
                }

                file->m_crc = inStreamSpec->m_crcs[subIndex];
                file->m_size = inStreamSpec->m_sizes[subIndex];
                if (file->m_size != 0)
                {
                    file->m_crcDefined = true;
                    file->m_hasStream = true;
                    numUnpackStreams++;
                }
                else
                {
                    file->m_crcDefined = false;
                    file->m_hasStream = false;
                }
                newDatabase.AddFile(file, file2);
            }
            // numUnpackStreams = 0 is very bad case for locked files
            // v3.13 doesn't understand it.
            newDatabase.m_numUnpackStreamsVector.push_back(numUnpackStreams);
            i += numSubFiles;
        }
    }

    if (folderRefIndex != (int32_t)folderRefs.size())
    {
        return RC_E_FAIL;
    }

    /*
    folderRefs.ClearAndFree();
    fileIndexToUpdateIndexMap.ClearAndFree();
    groups.ClearAndFree();
    */

    {
        // ---------- Write Folders & Empty Files ----------

        RCIntVector emptyRefs;
        for (i = 0; i < (int32_t)updateItems.size(); i++)
        {
            const RC7zUpdateItemPtr ui = updateItems[i];
            if (ui->m_newData)
            {
                if (ui->HasStream())
                {
                    continue;
                }
                else
                {
                    //回调接口，保证其他业务逻辑
                    if(updateCallback)
                    {
                        ISequentialInStreamPtr stream ;
                        updateCallback->GetStream(ui->m_indexInClient, stream.GetAddress());
                        if(stream != NULL)
                        {
                            stream.Release() ;
                        }
                        updateCallback->SetOperationResult(ui->m_indexInClient,RC_ARCHIVE_UPDATE_RESULT_OK) ;
                    }
                    
                }
            }
            else if (ui->m_indexInArchive != -1 && db->m_files[ui->m_indexInArchive]->m_hasStream)
            {
                continue;
            }
            emptyRefs.push_back(i);
        }         

        std::sort(emptyRefs.begin(),
                  emptyRefs.end(),
                  RC7zUpdateCompare<int32_t>(&RC7zUpdate::CompareEmptyItems, (void*)&updateItems));

        for (i = 0; i < (int32_t)emptyRefs.size(); i++)
        {
            const RC7zUpdateItemPtr ui = updateItems[emptyRefs[i]];
            RC7zFileItemPtr file(new RC7zFileItem);
            if (!file)
            {
                return RC_S_FALSE;
            }

            RC7zFileItem2Ptr file2(new RC7zFileItem2);
            if (!file2)
            {
                return RC_S_FALSE;
            }

            if (ui->m_newProperties)
            {
                FromUpdateItemToFileItem(ui, file, file2);
            }
            else
            {
                db->GetFile(ui->m_indexInArchive, file, file2);
            }
            newDatabase.AddFile(file, file2);
        }
    }

    {//写注释
        if (updateCallback)
        {
            RCVariant commentVal;
            HResult chr = updateCallback->GetArchiveProperty(RCPropID::kpidComment,commentVal);
            if (IsSuccess(chr) && IsStringType(commentVal))
            {
                newDatabase.m_isCommented = true;
                newDatabase.m_comment = GetStringValue(commentVal);
            }
        }
    }

    newDatabase.ReserveDown();

    return RC_S_OK;
}

HResult RC7zUpdate::WriteRange(ICompressCodecsInfo* codecsInfo,
                               IInStream* inStream, 
                               ISequentialOutStream* outStream,
                               uint64_t position, 
                               uint64_t size, 
                               ICompressProgressInfo* progress)
{
    HResult hr = inStream->Seek(position, RC_STREAM_SEEK_SET, 0) ;
    if(!IsSuccess(hr))
    {
        return hr ;
    }
    RCLimitedSequentialInStream* streamSpec = new RCLimitedSequentialInStream;
    RCLimitedSequentialInStreamPtr inStreamLimited(streamSpec);
    streamSpec->SetStream(inStream);
    streamSpec->Init(size);

    ICompressCoderPtr copyCoder ;
    hr = RCCreateCoder::CreateCoder(codecsInfo, RCMethod::ID_COMPRESS_COPY, copyCoder, true);
    if(!IsSuccess(hr))
    {
        return hr ;
    }
    hr = copyCoder->Code(inStreamLimited.Get(), outStream, NULL, NULL, progress) ;
    if(!IsSuccess(hr))
    {
        return hr ;
    }
    ICompressGetInStreamProcessedSizePtr copyCoderSize;
    hr = copyCoder.QueryInterface(ICompressGetInStreamProcessedSize::IID, copyCoderSize.GetAddress());
    if(!IsSuccess(hr))
    {
        return hr ;
    }
    uint64_t totalSize = 0 ;
    copyCoderSize->GetInStreamProcessedSize(totalSize) ;
    return (totalSize == size ? RC_S_OK : RC_E_FAIL);
}

int32_t RC7zUpdate::CompareBuffers(const RCByteBuffer& a1, const RCByteBuffer& a2)
{
    size_t c1 = a1.GetCapacity();
    size_t c2 = a2.GetCapacity();
    int32_t ret = MyCompare(c1, c2) ;
    if(ret != 0)
    {
        return ret ;
    }
    for (size_t i = 0; i < c1; i++)
    {
        ret = MyCompare(a1[i], a2[i]) ;
        if(ret != 0)
        {
            return ret ;
        }
    }
    return 0;
}

int32_t RC7zUpdate::CompareCoders(const RC7zCoderInfo& c1, const RC7zCoderInfo& c2)
{
    int32_t ret = MyCompare(c1.m_numInStreams, c2.m_numInStreams) ;
    if(ret != 0)
    {
        return ret ;
    }
    ret = MyCompare(c1.m_numOutStreams, c2.m_numOutStreams) ;
    if(ret != 0)
    {
        return ret ;
    }
    ret = MyCompare(c1.m_methodID, c2.m_methodID) ;
    if(ret != 0)
    {
        return ret ;
    }
    return CompareBuffers(c1.m_properties, c2.m_properties);
}

int32_t RC7zUpdate::CompareBindPairs(const RC7zBindPair& b1, const RC7zBindPair& b2)
{
    int32_t ret = MyCompare(b1.m_inIndex, b2.m_inIndex) ;
    if(ret != 0)
    {
        return ret ;
    }
    return MyCompare(b1.m_outIndex, b2.m_outIndex) ;
}

int32_t RC7zUpdate::CompareFolders(const RC7zFolder& f1, const RC7zFolder& f2)
{
    int32_t s1 = (int32_t)f1.m_coders.size();
    int32_t s2 = (int32_t)f2.m_coders.size();
    int32_t ret = MyCompare(s1, s2) ;
    if(ret != 0)
    {
        return ret ;
    }
    int32_t i = 0 ;
    for (i = 0; i < s1; i++)
    {
        ret = CompareCoders(f1.m_coders[i], f2.m_coders[i]) ;
        if(ret != 0)
        {
            return ret ;
        }
    }
    s1 = (int32_t)f1.m_bindPairs.size();
    s2 = (int32_t)f2.m_bindPairs.size();
    ret = MyCompare(s1, s2) ;
    if(ret != 0)
    {
        return ret ;
    }
    for (i = 0; i < s1; i++)
    {
        ret = CompareBindPairs(f1.m_bindPairs[i], f2.m_bindPairs[i]) ;
        if(ret != 0)
        {
            return ret ;
        }
    }
    return 0;
}

/*
int32_t RC7zUpdate::CompareFiles(const RC7zFileItemPtr& f1, const RC7zFileItemPtr& f2)
{
    return RCStringUtil::CompareNoCase(f1->Name, f2->Name);
}


int32_t RC7zUpdate::CompareFolderRefs(const int32_t* p1, const int32_t* p2, void* param)
{
    int32_t i1 = *p1;
    int32_t i2 = *p2;
    const RC7zArchiveDatabaseEx& db = *(const RC7zArchiveDatabaseEx *)param;
    int32_t ret = CompareFolders( db.Folders[i1], db.Folders[i2]) ;
    if(ret != 0)
    {
        return ret ;
    }
    ret = MyCompare( db.NumUnpackStreamsVector[i1], db.NumUnpackStreamsVector[i2]) ;
    if(ret != 0)
    {
        return ret ;
    }
    if (db.NumUnpackStreamsVector[i1] == 0)
    {
        return 0;
    }
    return CompareFiles( db.Files[db.FolderStartFileIndex[i1]], db.Files[db.FolderStartFileIndex[i2]]);
}
*/

int32_t RC7zUpdate::CompareFolderRepacks(const RC7zFolderRepack* p1, const RC7zFolderRepack* p2, void* param)
{
    int32_t ret = MyCompare(p1->m_group, p2->m_group);
    if(ret != 0)
    {
        return ret ;
    }

    int32_t i1 = p1->m_folderIndex;
    int32_t i2 = p2->m_folderIndex;

    const RC7zArchiveDatabaseEx &db = *(const RC7zArchiveDatabaseEx *)param;
    ret = CompareFolders(db.m_folders[i1], db.m_folders[i2]);
    if(ret != 0)
    {
        return ret ;
    }

    return MyCompare(i1, i2);
}

int32_t RC7zUpdate::CompareEmptyItems(const int32_t* p1, const int32_t* p2, void* param)
{
    const RCVector<RC7zUpdateItemPtr>& updateItems = *(const RCVector<RC7zUpdateItemPtr> *)param;
    const RC7zUpdateItemPtr& u1 = updateItems[*p1];
    const RC7zUpdateItemPtr& u2 = updateItems[*p2];
    if (u1->m_isDir != u2->m_isDir)
    {
        return (u1->m_isDir) ? 1 : -1 ;
    }
    if (u1->m_isDir)
    {
        if (u1->m_isAnti != u2->m_isAnti)
        {
            return (u1->m_isAnti ? 1 : -1);
        }
        int32_t n = RCStringUtil::CompareNoCase(u1->m_name, u2->m_name);
        return -n ;
    }
    if (u1->m_isAnti != u2->m_isAnti)
    {
        return (u1->m_isAnti ? 1 : -1);
    }
    return RCStringUtil::CompareNoCase(u1->m_name, u2->m_name);
}

void RC7zUpdate::FromUpdateItemToFileItem(const RC7zUpdateItemPtr& ui,
                                          RC7zFileItemPtr& file, 
                                          RC7zFileItem2Ptr& file2)
{
    file->m_name = RCItemNameUtils::MakeLegalName(ui->m_name);
    if (ui->m_attribDefined)
    {
        file->SetAttrib(ui->m_attrib);
    }
    
    file2->m_cTime = ui->m_cTime;  
    file2->m_cTimeDefined = ui->m_cTimeDefined;
    file2->m_aTime = ui->m_aTime;
    file2->m_aTimeDefined = ui->m_aTimeDefined;
    file2->m_mTime = ui->m_mTime;
    file2->m_mTimeDefined = ui->m_mTimeDefined;
    file2->m_isAnti = ui->m_isAnti;
    file2->m_startPosDefined = false;
    
    file->m_size = ui->m_size;
    file->m_isDir = ui->m_isDir;
    file->m_hasStream = ui->HasStream();
}

int32_t RC7zUpdate::GetExtIndex(const char *ext)
{
    int32_t extIndex = 1;
    const char *p = s_k7zExts;
    for (;;)
    {
        char c = *p++;
        if (c == 0)
        {
            return extIndex;
        }
        if (c == ' ')
        {
            continue;
        }
        int32_t pos = 0;
        for (;;)
        {
            char c2 = ext[pos++];
            if (c2 == 0 && (c == 0 || c == ' '))
            {
                return extIndex;
            }
            if (c != c2)
            {
                break;
            }
            c = *p++;
        }
        extIndex++;
        for (;;)
        {
            if (c == 0)
            {
                return extIndex;
            }
            if (c == ' ')
            {
                break;
            }
            c = *p++;
        }
    }
}

int32_t RC7zUpdate::MyStringCompareNoCase(const RCString::value_type* s1, const RCString::value_type* s2)
{
    for (;;)
    {
        RCString::value_type c1 = *s1++;
        RCString::value_type c2 = *s2++;
        if (c1 != c2)
        {
            RCString::value_type u1 = RCStringUtil::CharToUpper(c1);
            RCString::value_type u2 = RCStringUtil::CharToUpper(c2);
            if (u1 < u2)
            {
                return -1;
            }
            if (u1 > u2)
            {
                return 1;
            }
        }
        if (c1 == 0)
        {
            return 0;
        }
    }
}

int32_t RC7zUpdate::CompareUpdateItems(const RC7zRefItem* p1, const RC7zRefItem* p2, void* param)
{
    const RC7zRefItem& a1 = *p1;
    const RC7zRefItem& a2 = *p2;
    const RC7zUpdateItemPtr& u1 = *a1.m_updateItem;
    const RC7zUpdateItemPtr& u2 = *a2.m_updateItem;
    int32_t n = 0 ;
    if (u1->m_isDir != u2->m_isDir)
    {
        return (u1->m_isDir) ? 1 : -1;
    }
    if (u1->m_isDir)
    {
        if (u1->m_isAnti != u2->m_isAnti)
        {
            return (u1->m_isAnti ? 1 : -1);
        }
        n = RCStringUtil::CompareNoCase(u1->m_name, u2->m_name);
        return -n;
    }
    bool sortByType = *(bool *)param;
    if (sortByType)
    {
        int32_t ret = MyCompare(a1.m_extensionIndex, a2.m_extensionIndex) ;
        if(ret != 0)
        {
            return ret ;
        }
        ret = MyStringCompareNoCase(u1->m_name.c_str() + std::min(a1.m_extensionPos,(uint32_t)u1->m_name.size()),
                                    u2->m_name.c_str() + std::min(a2.m_extensionPos,(uint32_t)u2->m_name.size()) ) ;
        if(ret != 0)
        {
            return ret ;
        }
        ret = MyStringCompareNoCase(u1->m_name.c_str() + std::min(a1.m_namePos,(uint32_t)u1->m_name.size()) , 
                                    u2->m_name.c_str() + std::min(a2.m_namePos,(uint32_t)u2->m_name.size()) ) ;
        if(ret != 0)
        {
            return ret ;
        }
        
        if (!u1->m_mTimeDefined && u2->m_mTimeDefined)
        {
            return 1;
        }
        if (u1->m_mTimeDefined && !u2->m_mTimeDefined)
        {
            return -1;
        }
        if (u1->m_mTimeDefined && u2->m_mTimeDefined)
        {
            ret = MyCompare(u1->m_mTime, u2->m_mTime) ;
            if(ret != 0)
            {
                return ret ;
            }
        }
        ret = MyCompare(u1->m_size, u2->m_size) ;
        if(ret != 0)
        {
            return ret ;
        }
    }
    return RCStringUtil::CompareNoCase(u1->m_name, u2->m_name) ;
}

bool RC7zUpdate::IsExeFile(const RCString& ext)
{
    for (int32_t i = 0; i < sizeof(s_k7zExeExts) / sizeof(s_k7zExeExts[0]); i++)
    {
        if (RCStringUtil::CompareNoCase(ext,s_k7zExeExts[i]) == 0)
        {
            return true;
        }
    }
    return false;
}

bool RC7zUpdate::GetMethodFull(uint64_t methodID,
                               uint32_t numInStreams, 
                               RC7zArchiveMethodFull& methodResult)
{
    methodResult.m_id = methodID;
    methodResult.m_numInStreams = numInStreams;
    methodResult.m_numOutStreams = 1;
    return true;
}

bool RC7zUpdate::MakeExeMethod(const RC7zCompressionMethodMode& method,
                               bool bcj2Filter, 
                               RC7zCompressionMethodMode& exeMethod)
{
    exeMethod = method;
    if (bcj2Filter)
    {
        RC7zArchiveMethodFull methodFull;
        if (!GetMethodFull(RCMethod::ID_COMPRESS_BRANCH_BCJ_2, 4, methodFull))
        {
            return false;
        }
        exeMethod.m_methods.insert(exeMethod.m_methods.begin(), methodFull);
        if (!GetMethodFull(RCMethod::ID_COMPRESS_LZMA, 1, methodFull))
        {
            return false;
        }
        {
            RCArchiveProp property;
            property.m_id = RCCoderPropID::kAlgorithm;
            property.m_value = uint64_t(s_k7zAlgorithmForBCJ2_LZMA) ;
            methodFull.m_properties.push_back(property);
        }
        {
            RCArchiveProp property;
            property.m_id = RCCoderPropID::kMatchFinder;
            property.m_value = RCString(s_k7zMatchFinderForBCJ2_LZMA) ;
            methodFull.m_properties.push_back(property);
        }
        {
            RCArchiveProp property;
            property.m_id = RCCoderPropID::kDictionarySize;
            property.m_value = uint64_t(s_k7zDictionaryForBCJ2_LZMA);
            methodFull.m_properties.push_back(property);
        }
        {
            RCArchiveProp property;
            property.m_id = RCCoderPropID::kNumFastBytes;
            property.m_value = uint64_t(s_k7zNumFastBytesForBCJ2_LZMA);
            methodFull.m_properties.push_back(property);
        }

        exeMethod.m_methods.push_back(methodFull);
        exeMethod.m_methods.push_back(methodFull);
        RC7zBind bind;

        bind.m_outCoder = 0;
        bind.m_inStream = 0;

        bind.m_inCoder = 1;
        bind.m_outStream = 0;
        exeMethod.m_binds.push_back(bind);

        bind.m_inCoder = 2;
        bind.m_outStream = 1;
        exeMethod.m_binds.push_back(bind);

        bind.m_inCoder = 3;
        bind.m_outStream = 2;
        exeMethod.m_binds.push_back(bind);
    }
    else
    {
        RC7zArchiveMethodFull methodFull;
        if (!GetMethodFull(RCMethod::ID_COMPRESS_BRANCH_BCJ, 1, methodFull))
        {
            return false;
        }
        exeMethod.m_methods.insert(exeMethod.m_methods.begin(), methodFull);
        RC7zBind bind;
        bind.m_outCoder = 0;
        bind.m_inStream = 0;
        bind.m_inCoder = 1;
        bind.m_outStream = 0;
        exeMethod.m_binds.push_back(bind);
    }
    return true;
}

END_NAMESPACE_RCZIP
