/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __IStream_h_
#define __IStream_h_ 1

#include "interface/IUnknown.h"
#include "base/RCString.h"

BEGIN_NAMESPACE_RCZIP

/** 接口ID定义
*/
enum
{
    IID_ISequentialInStream     = IID_ISTREAM_BASE,
    IID_ISequentialOutStream,
    IID_IInStream,
    IID_IOutStream,
    IID_IStreamGetSize,
    IID_IOutStreamFlush,
    IID_ICloneStream,
    IID_IStreamGetFileName,
    IID_IMultiStream,
    IID_IMultiVolStream,
    IID_IZipMultiVolStream
};

/** 读取数据接口
*/
class ISequentialInStream:
    public IUnknown
{
public:
    
    /** 接口ID
    */
    enum { IID = IID_ISequentialInStream } ;
    
public:
    
    /** 读取数据 
    @param [out] data 存放数据的缓冲区
    @param [in] size 缓冲区大小
    @param [out] processedSize 实际读取的大小
    @return 成功返回RC_S_OK, 否则返回错误码
    */
    virtual HResult Read(void* data, uint32_t size, uint32_t* processedSize) = 0 ;

protected:
       
    /** 默认析构函数
    */
    ~ISequentialInStream() {} ;
};

/** 写出数据接口
*/
class ISequentialOutStream:
    public IUnknown
{
public:
    
    /** 接口ID
    */
    enum { IID = IID_ISequentialOutStream } ;
    
public:
    
    /** 写数据  
    @param [in] data 存放数据的缓冲区
    @param [in] size 缓冲区大小
    @param [out] processedSize 实际读取的大小
    @return 成功返回RC_S_OK, 否则返回错误码
    */
    virtual HResult Write(const void* data, uint32_t size, uint32_t* processedSize) = 0 ;

protected:
       
    /** 默认析构函数
    */
    ~ISequentialOutStream() {} ;
};

/** 文件偏移调整的相对位置定义 
*/
typedef enum
{
    /** 从文件开始 
    */
    RC_STREAM_SEEK_SET = 0,

    /** 从当前文件偏移处开始 
    */
    RC_STREAM_SEEK_CUR = 1,

    /** 从文件结束
    */
    RC_STREAM_SEEK_END = 2
    
} RC_STREAM_SEEK ;

/** 输入数据接口
*/
class IInStream:
    public ISequentialInStream
{
public:
    
    /** 接口ID
    */
    enum { IID = IID_IInStream } ;
    
public:
    
    /** 调整指针位置
    @param [in] offset 需要调整的偏移量
    @param [in] seekOrigin 调整的基准位置
    @param [out] newPosition 调整后的文件偏移
    @return 成功返回RC_S_OK, 否则返回错误码
    */
    virtual HResult Seek(int64_t offset, RC_STREAM_SEEK seekOrigin, uint64_t* newPosition) = 0 ;

protected:
       
    /** 默认析构函数
    */
    ~IInStream() {} ;
};

/** 输出数据接口
*/
class IOutStream:
    public ISequentialOutStream
{
public:
    
    /** 接口ID
    */
    enum { IID = IID_IOutStream } ;
    
public:
    
    /** 调整指针位置
    @param [in] offset 需要调整的偏移量
    @param [in] seekOrigin 调整的基准位置
    @param [out] newPosition 调整后的文件偏移
    @return 成功返回RC_S_OK, 否则返回错误码
    */
    virtual HResult Seek(int64_t offset, RC_STREAM_SEEK seekOrigin, uint64_t* newPosition) = 0 ;
    
    /** 调整大小
    @param [in] newSize 新的大小 
    @return 成功返回RC_S_OK, 否则返回错误码
    */
    virtual HResult SetSize(uint64_t newSize) = 0 ;

protected:
       
    /** 默认析构函数
    */
    ~IOutStream() {} ;
};

/** 获取流的大小 
*/
class IStreamGetSize:
    public IUnknown
{
public:
    
    /** 接口ID
    */
    enum { IID = IID_IStreamGetSize } ;
    
public:
    
    /** 获取长度
    @param [out] size 返回流的长度
    @return 成功返回RC_S_OK, 否则返回错误码
    */
    virtual HResult GetSize(uint64_t* size) = 0 ;

protected:
       
    /** 默认析构函数
    */
    ~IStreamGetSize() {} ;
};

/** 清空缓冲区，并把缓存数据写入流
*/
class IOutStreamFlush:
    public IUnknown
{
public:
    
    /** 接口ID
    */
    enum { IID = IID_IOutStreamFlush } ;
    
public:
    
    /** 清空缓冲区，并把缓存数据写入流
    @return 成功返回RC_S_OK, 否则返回错误码
    */
    virtual HResult Flush() = 0 ;

protected:
       
    /** 默认析构函数
    */
    ~IOutStreamFlush() {} ;
};

/** Clone数据流接口
*/
class ICloneStream:
    public IUnknown
{
public:
    
    /** 接口ID
    */
    enum { IID = IID_ICloneStream } ;
    
public:
    
    /**  Clone 数据流 
    @return Clone 的数据流，如果失败返回NULL
    */
    virtual IUnknown* Clone(void) = 0 ;

protected:
       
    /** 默认析构函数
    */
    ~ICloneStream() {} ;
};

/** 获取流对应的文件名
*/
class IStreamGetFileName:
    public IUnknown
{
public:
    
    /** 接口ID
    */
    enum { IID = IID_IStreamGetFileName } ;
    
public:
    
    /** 获取流对应的文件名，含完整路径
    @param [out] fileName 返回文件名，含完整路径
    @return 成功返回RC_S_OK, 否则返回错误码
    */
    virtual HResult GetFileName(RCString& fileName) = 0 ;

protected:
       
    /** 默认析构函数
    */
    ~IStreamGetFileName() {} ;
};

/** 分卷流接口,取得某分卷大小
*/
class IMultiStream :
    public IUnknown
{
public:
    
    /** 接口ID
    */
    enum { IID = IID_IMultiStream };
    
public:

    /** 取得某分卷大小
    @param [in] index 分卷序号
    @param [out] volumeSize 卷大小字节数
    @return 成功返回RC_S_OK, 否则返回错误码
    */
    virtual HResult GetVolumeSize(uint32_t index, uint64_t& volumeSize) = 0 ;

    /** 取得从开始到某分卷的所有分卷的总大小
        计算区间 [0, index)
    @param [in] index 分卷序号
    @param [out] volumeTotalSize 总分卷大小字节数
    @return 成功返回RC_S_OK, 否则返回错误码
    */
    virtual HResult GetVolumeTotalSize(uint32_t index, uint64_t& volumeTotalSize) = 0;
    
protected:
    
    /** 默认析构函数
    */
    ~IMultiStream() {} ;
};

/** 分卷输出流接口
*/
class IMultiVolStream :
    public IUnknown
{
public:
    
    /** 接口ID
    */
    enum { IID = IID_IMultiVolStream };
    
public:
    
    /** 取得当前正在输出的卷编号
    @param [out] volIndex 返回正在输出的卷编号
    @return 成功返回RC_S_OK, 否则返回错误码
    */
    virtual HResult GetCurVolIndex(uint32_t& volIndex) = 0;

    /** 取得当前卷可写字节数
    @param [out] freeByte 返回当前卷的可写字节数
    @return 成功返回RC_S_OK, 否则返回错误码
    */
    virtual HResult GetCurVolFreeByte(uint64_t& freeByte) = 0;

    /** 取得当前卷偏移量
    @param [out] position 返回当前卷偏移量
    @return 成功返回RC_S_OK, 否则返回错误码
    */
    virtual HResult GetCurVolPosition(uint64_t& position) = 0;

    /** 检查从某位置开始的一段数据是否跨卷
    @param [in] beginPos 当前位置偏移
    @param [in] dataLen 数据长度
    @param [out] result 如果跨卷返回true, 否则返回false
    @return 成功返回RC_S_OK, 否则返回错误码
    */
    virtual HResult CheckDataAcrossVol(uint64_t beginPos, uint64_t dataLen, bool& result) = 0;

    /** 迫使从新卷开始写文件
    @return 成功返回RC_S_OK, 否则返回错误码
    */
    virtual HResult ForceStartNextVol() = 0;

    /** 取得分卷数量
    @param [out] volCount 返回分卷数量
    @return 成功返回RC_S_OK, 否则返回错误码
    */
    virtual HResult GetVolumeCount(uint32_t& volCount) = 0;

protected:
    
    /** 默认析构函数
    */
    ~IMultiVolStream() {};
};

/** 设置生成zip 分卷接口
*/
class IZipMultiVolStream :
    public IUnknown
{
public:
    
    /** 接口ID
    */
    enum { IID = IID_IZipMultiVolStream };
    
public:
    
    /** 设置是否生成zip 分卷
    @param [in] toZip  true生成zip分卷 false不生成zip分卷
    @return 成功返回RC_S_OK, 否则返回错误码
    */
    virtual HResult SetToZipMultiVolFormat(bool toZip) = 0;
    
protected:
    
    /** 默认析构函数
    */
    ~IZipMultiVolStream() {};
};

/** 智能指针定义
*/

typedef RCComPtr<ISequentialInStream>   ISequentialInStreamPtr ;
typedef RCComPtr<ISequentialOutStream>  ISequentialOutStreamPtr ;
typedef RCComPtr<IInStream>             IInStreamPtr ;
typedef RCComPtr<IOutStream>            IOutStreamPtr ;
typedef RCComPtr<IStreamGetSize>        IStreamGetSizePtr ;
typedef RCComPtr<IOutStreamFlush>       IOutStreamFlushPtr ;
typedef RCComPtr<ICloneStream>          ICloneStreamPtr ;
typedef RCComPtr<IStreamGetFileName>    IStreamGetFileNamePtr ;
typedef RCComPtr<IMultiStream>          IMultiStreamPtr;
typedef RCComPtr<IMultiVolStream>       IMultiVolStreamPtr;
typedef RCComPtr<IZipMultiVolStream>    IZipMultiVolStreamPtr;

END_NAMESPACE_RCZIP

#endif //__IStream_h_
