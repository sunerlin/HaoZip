/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __IArchive_h
#define __IArchive_h 1

#include "interface/IProgress.h"
#include "interface/IArchiveInfo.h"
#include "base/RCString.h"
#include "common/RCProperty.h"
#include "archive/extract/RCExtractDefs.h"
#include "common/RCVector.h"

BEGIN_NAMESPACE_RCZIP

/** 压缩包内文件时间格式定义
*/
enum RCArchiveFileTimeType
{
    /** Windows文件时间
    */
    RC_ARCHIVE_FILE_TIME_TYPE_WINDOWS   = 0 , 
    
    /** UNIX文件时间
    */
    RC_ARCHIVE_FILE_TIME_TYPE_UNIX      = 1 ,
    
    /** DOS文件时间
    */ 
    RC_ARCHIVE_FILE_TIME_TYPE_DOS       = 2
};

/** 解压文件操作结果
*/
enum RCArchiveExtractResult
{
    /** 解压成功
    */
    RC_ARCHIVE_EXTRACT_RESULT_OK                    = 0 ,
    
    /** 解压时遇到不支持的算法
    */
    RC_ARCHIVE_EXTRACT_RESULT_UNSUPPORTED_METHOD    = 1 ,
    
    /** 解压时遇到错误数据
    */
    RC_ARCHIVE_EXTRACT_RESULT_DATA_ERROR            = 2 ,
    
    /** 解压时遇到CRC校验错误
    */
    RC_ARCHIVE_EXTRACT_RESULT_CRC_ERROR             = 3
};

/** 解压文件询问模式
*/
enum RCArchiveExtractAskMode
{
    /** 正在解压文件
    */
    RC_ARCHIVE_EXTRACT_ASK_MODE_EXTRACT         = 0 ,
    
    /** 正在测试文件
    */
    RC_ARCHIVE_EXTRACT_ASK_MODE_TEST            = 1 ,
    
    /** 正在跳过文件
    */
    RC_ARCHIVE_EXTRACT_ASK_MODE_SKIP            = 2 ,
    
    /** 没有可解压的文件
    */
    RC_ARCHIVE_EXTRACT_ASK_MODE_NO_FILE         = 3
};

/** 压缩文件操作结果
*/
enum RCArchiveUpdateResult
{
    /** 压缩文件操作成功
    */
    RC_ARCHIVE_UPDATE_RESULT_OK     = 0 ,
    
    /** 压缩文件操作失败
    */
    RC_ARCHIVE_UPDATE_RESULT_ERROR  = 1
};

/** 接口ID定义
*/
enum
{
    IID_IArchiveOpenCallback     = IID_IARCHIVE_BASE,
    IID_IArchiveExtractCallback ,
    IID_IArchiveOpenVolumeCallback ,
    IID_IInArchiveGetStream ,
    IID_IArchiveOpenSetSubArchiveName ,
    IID_IInArchive ,
    IID_IArchiveUpdateCallback ,
    IID_IArchiveUpdateCallback2 ,
    IID_IOutArchive ,
    IID_ISetProperties ,
    IID_IArchiveCodecsInfo ,
    IID_IArchiveVolumeType
};

class IInStream ;
class ISequentialInStream ;
class ISequentialOutStream ;

/** 压缩文件分卷信息
*/
struct RCArchiveVolumeItem
{
    /** 分卷文件名
    */
    RCString m_filename;
    
    /** 卷序号
    */
    unsigned short m_volumeIndex;
    
    /** 是否为第一个卷
    */
    bool m_isFirst;
    
    /** 是否为最后一个卷
    */
    bool m_isEnd;

    /** 比较操作符
    @param [in] item 需要比较的卷信息
    @return 如果当前卷序号小于比较卷序号，返回true, 否则返回false
    */
    bool operator< (const RCArchiveVolumeItem& item) const   
    {   
        return m_volumeIndex < item.m_volumeIndex;
    }
};

/** 文件打开回调接口 
*/
class IArchiveOpenCallback:
    public IUnknown
{
public:
    
    /** 接口ID
    */
    enum { IID = IID_IArchiveOpenCallback } ;
    
public:
    
    /** 总数量
    @param [in] files 总的文件个数，如果为 -1, 则表示无法通过文件数表示进度
    @param [in] bytes 总的文件大小，如果为 -1, 则表示无法通过字节数表示进度
    @return 成功返回RC_S_OK, 失败则返回错误码
    */
    virtual HResult SetTotal(uint64_t files, uint64_t bytes) = 0 ;
    
    /** 完成数量
    @param [in] files 已经完成的文件个数
    @param [in] bytes 已经完成的文件大小
    @return 成功返回RC_S_OK, 失败则返回错误码
    */
    virtual HResult SetCompleted(uint64_t files, uint64_t bytes) = 0 ;

    /** 检查是否终止
    @return 若不中止返回RC_S_OK, 失败则返回错误码
    */
    virtual HResult CheckBreak() = 0;

    /** 修复分卷
    @param [in] items 修复前的分卷序列
    @param [out] volumeFirst 第一个分卷
    @param [out] outputFilenames 修复后的分卷序列
    @return 成功返回RC_S_OK, 失败则返回错误码
    */
    virtual HResult VolumeRepair(const RCVector<RCArchiveVolumeItem>& items, RCString& volumeFirst, RCVector<RCString>& outputFilenames) = 0;
    
    /** 设置错误
    @param [in] errorCode 错误号
    @return 成功返回RC_S_OK, 失败则返回错误码
    */
    virtual HResult SetError(int32_t errorCode) = 0 ;
    
protected:
    
    /** 默认析构函数
    */
    ~IArchiveOpenCallback(){} ;
};

/** 文档解压回调接口 
*/
class IArchiveExtractCallback : 
    public IProgress
{
public:
    
    /** 接口ID
    */
    enum { IID = IID_IArchiveExtractCallback } ;
    
public:
    
    /** 获取输出流
    @param [in] index 解压文件或者目录的下标值
    @param [out] outStream 输出流的接口指针，引用计数加1
    @param [in] askExtractMode 解压模式
    @return 成功返回RC_S_OK, 失败则返回错误码
    */
    virtual HResult GetStream(uint32_t index, ISequentialOutStream** outStream, int32_t askExtractMode) = 0 ;
    
    /** 准备解压操作
    @param [in] index 解压文件或者目录的下标值
    @param [in] askExtractMode 解压模式
    @return 成功返回RC_S_OK, 失败则返回错误码
    */
    virtual HResult PrepareOperation(uint32_t index, int32_t askExtractMode) = 0 ;
    
    /** 设置结果
    @param [in] index 解压文件或者目录的下标值
    @param [in] resultEOperationResult 操作结果
    @return 成功返回RC_S_OK, 失败则返回错误码
    */
    virtual HResult SetOperationResult(uint32_t index, int32_t resultEOperationResult) = 0 ;
    
    /** 设置多线程解压标记
    @param [in] isMultiThreads 如果为true，当前为多线程解压模式，否则为单线程解压模式
    @return 成功返回RC_S_OK, 失败则返回错误码
    */
    virtual HResult SetMultiThreadMode(bool isMultiThreads) = 0 ;

    /** 获取解压路径
    @param [out] path 解压路径
    @return 成功返回RC_S_OK, 失败则返回错误码
    */
    virtual HResult GetDestinationDirectory(RCString& path) = 0 ;
    
    /** 获取需要从解压路径中去除的路径部分
    @param [out] removePathParts 需要从解压路径中去除的路径部分
    @return 成功返回RC_S_OK, 失败则返回错误码
    */
    virtual HResult GetRemovePathParts(RCVector<RCString>& removePathParts) = 0 ;

    /** 获取解压路径选项
    @param [out] pathMode 解压路径选项
    @return 成功返回RC_S_OK, 失败则返回错误码
    */
    virtual HResult GetPathMode(RCExractPathMode& pathMode) = 0 ;

    /** 获取解压覆盖选项
    @param [out] overwriteMode 解压覆盖选项
    @return 成功返回RC_S_OK, 失败则返回错误码
    */
    virtual HResult GetOverwriteMode(RCExractOverwriteMode& overwriteMode) = 0 ;

    /** 获取解压更新方式
    @param [out] updateMode 解压更新方式
    @return 成功返回RC_S_OK, 失败则返回错误码
    */
    virtual HResult GetUpdateMode(RCExractUpdateMode& updateMode) = 0 ;
    
    /** 是否保留损坏文件
    return 如果保留损坏文件，返回true, 否则返回false
    */
    virtual bool IsKeepDamageFiles(void) = 0 ;
    
protected:
    
    /** 默认析构函数
    */
    ~IArchiveExtractCallback(){} ;
};


/** 打开卷回调接口 
*/
class IArchiveOpenVolumeCallback:
    public IUnknown
{
public:
    
    /** 接口ID
    */
    enum { IID = IID_IArchiveOpenVolumeCallback } ;
    
public:
    
    /** 获取属性
    @param [in] propID 属性ID
    @param [out] value 属性值
    @return 成功返回RC_S_OK，否则返回错误号
    */
    virtual HResult GetProperty(RCPropertyID propID, RCVariant& value) = 0 ;
    
    /** 获取流
    @param [in] name 流的属性名称
    @param [out] inStream 返回输入流接口，引用计数加1
    @return 成功返回RC_S_OK，否则返回错误号
    */
    virtual HResult GetStream(const RCString& name, IInStream** inStream) = 0 ;

    /** 获取路径
    @param [out] folder 流的路径
    @return 成功返回RC_S_OK，否则返回错误号
    */
    virtual HResult GetFolderPrefix(RCString& folder) = 0;
    
protected:
    
    /** 默认析构函数
    */
    ~IArchiveOpenVolumeCallback(){} ;
};

/** 输入文档获取流接口 
*/
class IInArchiveGetStream:
    public IUnknown
{
public:
    
    /** 接口ID
    */
    enum { IID = IID_IInArchiveGetStream } ;
    
public:
    
    /** 获取流
    @param [in] index 流的下标值
    @param [out] stream 返回输入流接口，引用计数加1
    @param [in] openCallback 打开进度回调接口
    @return 成功返回RC_S_OK，否则返回错误号
    */
    virtual HResult GetStream(uint32_t index, ISequentialInStream** stream, IArchiveOpenCallback* openCallback) = 0 ;  

protected:
    
    /** 默认析构函数
    */
    ~IInArchiveGetStream(){} ;
};

/** 设置子文档的名称接口
*/
class IArchiveOpenSetSubArchiveName:
    public IUnknown
{
public:
    
    /** 接口ID
    */
    enum { IID = IID_IArchiveOpenSetSubArchiveName } ;
    
public:
    
    /** 设置名称
    @param [in] name 文档名称
    @return 成功返回RC_S_OK，否则返回错误号
    */
    virtual HResult SetSubArchiveName(const RCString& name) = 0 ;
    
protected:
    
    /** 默认析构函数
    */
    ~IArchiveOpenSetSubArchiveName(){} ;    
};

/** 输入文档接口 
*/
class IInArchive:
    public IUnknown
{
public:
    
    /** 接口ID
    */
    enum { IID = IID_IInArchive } ;
    
public:
    
    /** 打开文档
    @param [in] stream 输入流接口
    @param [in] maxCheckStartPosition 查找文件格式签名的最大偏移位置
    @param [in] openArchiveCallback 打开文档回调接口指针
    @return 成功返回RC_S_OK，否则返回错误号
    */
    virtual HResult Open(IInStream* stream, uint64_t maxCheckStartPosition, IArchiveOpenCallback* openArchiveCallback) = 0;
    
    /** 关闭文档
    @return 成功返回RC_S_OK，否则返回错误号
    */
    virtual HResult Close() = 0 ;
    
    /** 获取内容个数
    @param [out] numItems 返回文档内容个数
    @return 成功返回RC_S_OK，否则返回错误号
    */
    virtual HResult GetNumberOfItems(uint32_t& numItems) = 0 ;
    
    /** 获取属性
    @param [in] index 属性的下标
    @param [in] propID 属性ID
    @param [out] value 属性值
    @return 成功返回RC_S_OK，否则返回错误号
    */
    virtual HResult GetProperty(uint32_t index, RCPropertyID propID, RCVariant& value) = 0 ;
    
    /** 解压
    @param [in] indices 需要解压的Item 下标数组，如果数组内容为空表示解压全部内容
    @param [in] testMode 1为测试模式，0 为正常解压模式
    @param [in] extractCallback 解压回调接口指针
    @return 成功返回RC_S_OK，否则返回错误号
    */
    virtual HResult Extract(const std::vector<uint32_t>& indices, int32_t testMode, IArchiveExtractCallback* extractCallback) = 0;
    
    /** 获取文档属性
    @param [in] propID 属性ID
    @param [out] value 属性值
    @return 成功返回RC_S_OK，否则返回错误号
    */
    virtual HResult GetArchiveProperty(RCPropertyID propID, RCVariant& value) = 0;
    
    /** 获取属性个数
    @param [out] numProperties 返回属性个数
    @return 成功返回RC_S_OK，否则返回错误号
    */
    virtual HResult GetNumberOfProperties(uint32_t& numProperties) = 0;
    
    /** 获取属性信息
    @param [in] index 属性下标值
    @param [out] name 返回属性名称
    @param [out] propID 返回属性ID
    @param [out] varType 返回属性类型
    @return 成功返回RC_S_OK，否则返回错误号
    */
    virtual HResult GetPropertyInfo(uint32_t index, RCString& name, RCPropertyID& propID, RCVariantType::RCVariantID& varType) = 0;
    
    /** 获取文档属性个数
    @param [out] numProperties 返回属性个数
    @return 成功返回RC_S_OK，否则返回错误号
    */
    virtual HResult GetNumberOfArchiveProperties(uint32_t& numProperties) = 0 ;
    
    /** 获取文档属性信息
    @param [in] index 属性下标值
    @param [out] name 返回属性名称
    @param [out] propID 返回属性ID
    @param [out] varType 返回属性类型
    @return 成功返回RC_S_OK，否则返回错误号
    */
    virtual HResult GetArchivePropertyInfo(uint32_t index, RCString& name, RCPropertyID& propID, RCVariantType::RCVariantID& varType) = 0;
    
protected:
    
    /** 默认析构函数
    */
    ~IInArchive(){} ;
};

/** 文档更新回调接口
*/
class IArchiveUpdateCallback : 
    public IProgress
{
public:
    
    /** 接口ID
    */
    enum { IID = IID_IArchiveUpdateCallback } ;
    
public:
    
    /** 获取更新信息
    @param [in] index 属性下标值
    @param [out] newData 如果是新数据返回1，如果是旧数据返回0
    @param [out] newProperties 如果是新属性返回1，如果是旧属性返回0
    @param [out] indexInArchive 返回输入文档的下标值，如果失败返回-1
    @return 成功返回RC_S_OK，否则返回错误号
    */
    virtual HResult GetUpdateItemInfo(uint32_t index, 
                                      int32_t& newData,
                                      int32_t& newProperties,
                                      uint32_t& indexInArchive ) = 0 ;
    
    /** 获取属性
    @param [in] index 属性下标
    @param [in] propID 属性ID
    @param [out] value 属性值
    @return 成功返回RC_S_OK，否则返回错误号
    */
    virtual HResult GetProperty(uint32_t index, RCPropertyID propID, RCVariant& value) = 0 ;
    
    /** 获取压缩文档属性
    @param [in] propID 属性ID
    @param [out] value 属性值
    @return 成功返回RC_S_OK，否则返回错误号
    */
    virtual HResult GetArchiveProperty(RCPropertyID propID, RCVariant& value) = 0 ;
    
    /** 获取输入流
    @param [in] index 输入流的下标
    @param [out] inStream 输入流的接口指针，引用计数加1
    @return 成功返回RC_S_OK，否则返回错误号
    */
    virtual HResult GetStream(uint32_t index, ISequentialInStream** inStream) = 0 ;
    
    /** 设置结果
    @param [in] index 输入流的下标
    @param [in] operationResult 操作结果
    @return 成功返回RC_S_OK，否则返回错误号
    */
    virtual HResult SetOperationResult(uint32_t index, int32_t operationResult) = 0 ;

protected:
    
    /** 默认析构函数
    */
    ~IArchiveUpdateCallback(){} ;
};

/** 文档更新回调接口
*/
class IArchiveUpdateCallback2 : 
    public IArchiveUpdateCallback
{
public:
    
    /** 接口ID
    */
    enum { IID = IID_IArchiveUpdateCallback2 } ;
    
public:
    
    /** 获取卷大小
    @param [in] index 卷的下标
    @param [out] size 卷的大小
    @return 成功返回RC_S_OK，否则返回错误号
    */
    virtual HResult GetVolumeSize(uint32_t index, uint64_t& size) = 0 ;
    
    /** 获取卷对应的流
    @param [in] index 卷的下标
    @param [out] volumeStream 输出流的接口指针，引用计数加1
    @return 成功返回RC_S_OK，否则返回错误号
    */
    virtual HResult GetVolumeStream(uint32_t index, ISequentialOutStream** volumeStream) = 0 ;

protected:
    
    /** 默认析构函数
    */
    ~IArchiveUpdateCallback2(){} ;
};

/** 输出文档接口
*/
class IOutArchive:
    public IUnknown
{
public:
    
    /** 接口ID
    */
    enum { IID = IID_IOutArchive } ;
    
public:
    
    /** 更新内容
    @param [in] outStream 输出流的接口指针
    @param [in] numItems 文档内容的个数
    @param [in] updateCallback 文档更新接口指针
    @return 成功返回RC_S_OK，否则返回错误号
    */
    virtual HResult UpdateItems(ISequentialOutStream* outStream, 
                                uint32_t numItems, 
                                IArchiveUpdateCallback* updateCallback) = 0 ;
    
    /** 获取文件时间类型
    @param [out] type 时间类型数值
    @return 成功返回RC_S_OK，否则返回错误号
    */
    virtual HResult GetFileTimeType(uint32_t& type) = 0 ;
    
protected:
    
    /** 默认析构函数
    */
    ~IOutArchive(){} ;
};

/** 设置文档属性 
*/
class ISetProperties:
    public IUnknown
{
public:
    
    /** 接口ID
    */
    enum { IID = IID_ISetProperties } ;
    
public:
    
    /** 设置属性
    @param [in] propertyArray 文档属性数值
    @return 成功返回RC_S_OK，否则返回错误号
    */
    virtual HResult SetProperties(const RCPropertyNamePairArray& propertyArray) = 0;

protected:
    
    /** 默认析构函数
    */
    ~ISetProperties(){} ;
};

typedef RCComPtr<IInArchive>                    IInArchivePtr ;
typedef RCComPtr<IOutArchive>                   IOutArchivePtr ;

/** 文档格式管理接口
*/
class IArchiveCodecsInfo:
    public IUnknown
{
public:
    
    /** 接口ID
    */
    enum { IID = IID_IArchiveCodecsInfo } ;
    
public:
    
    /** 获取支持的文档格式个数
    @param [out] numFormats 文档格式格式
    @return 如果成功则返回RC_S_OK，否则返回错误号
    */
    virtual HResult GetNumberOfFormats(uint32_t& numFormats) const = 0 ;
    
    /** 获取文档信息
    @param [in] index 文档格式下标
    @param [out] spArcInfo 文档格式信息
    @return 如果成功则返回RC_S_OK，否则返回错误号
    */
    virtual HResult GetFormat(uint32_t index, IArchiveInfoPtr& spArcInfo) const = 0 ;
    
    /** 创建输入文档接口
    @param [in] index 文档格式下标
    @param [out] spInArchive 输入文档接口
    @return 如果成功则返回RC_S_OK，否则返回错误号
    */
    virtual HResult CreateInArchive(uint32_t index, IInArchivePtr& spInArchive) const = 0 ;
    
    /** 创建输出文档接口
    @param [in] index 文档格式下标
    @param [out] spOutArchive 输出文档接口
    @return 如果成功则返回RC_S_OK，否则返回错误号
    */
    virtual HResult CreateOutArchive(uint32_t index, IOutArchivePtr& spOutArchive) const = 0 ;
    
protected:
    
    /** 默认析构函数
    */
    ~IArchiveCodecsInfo(){} ; 
};

/** 获取文件格式分卷属性接口
*/
class IArchiveVolumeType:
    public IUnknown
{
public:
    
    /** 接口ID
    */
    enum { IID = IID_IArchiveVolumeType } ;
    
public:
    /** 获取格式属性
    @param [in] inStream 文件输入流
    @param [in] maxCheckStartPosition 查找文件格式签名的最大偏移位置
    @param [out] isVolume 分卷压缩包返回true，否则返回false
    @return 成功返回RC_S_OK，否则返回错误号
    */
    virtual HResult GetVolumeType(IInStream* inStream, uint64_t maxCheckStartPosition, bool& isVolume) = 0 ;
        
protected:
    
    /** 默认析构函数
    */
    ~IArchiveVolumeType(){} ;
};

/** 智能指针定义
*/
typedef RCComPtr<IArchiveOpenCallback>          IArchiveOpenCallbackPtr ;
typedef RCComPtr<IArchiveExtractCallback>       IArchiveExtractCallbackPtr ;
typedef RCComPtr<IArchiveOpenVolumeCallback>    IArchiveOpenVolumeCallbackPtr ;
typedef RCComPtr<IInArchiveGetStream>           IInArchiveGetStreamPtr ; 
typedef RCComPtr<IArchiveOpenSetSubArchiveName> IArchiveOpenSetSubArchiveNamePtr ;
typedef RCComPtr<IInArchive>                    IInArchivePtr ;
typedef RCComPtr<IArchiveUpdateCallback>        IArchiveUpdateCallbackPtr ;
typedef RCComPtr<IArchiveUpdateCallback2>       IArchiveUpdateCallback2Ptr ;
typedef RCComPtr<IOutArchive>                   IOutArchivePtr ;
typedef RCComPtr<ISetProperties>                ISetPropertiesPtr ;
typedef RCComPtr<IArchiveCodecsInfo>            IArchiveCodecsInfoPtr ;
typedef RCComPtr<IArchiveVolumeType>            IArchiveVolumeTypePtr ;

END_NAMESPACE_RCZIP

#endif //__IArchive_h
