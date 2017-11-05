/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCWinOutFileStream_h_
#define __RCWinOutFileStream_h_ 1

#include "interface/IStream.h"
#include "interface/IUnknownImpl.h"
#include "filesystem/windows/RCWinOutFile.h"

BEGIN_NAMESPACE_RCZIP

class IOutStreamEvent ;

class RCWinOutFileStream:
    public IUnknownImpl2<IOutStream,
                         IStreamGetFileName>
{
public:

    /** 构造函数
    @param [in] outStreamEvent IO写入事件处理回调接口
    */
    RCWinOutFileStream(IOutStreamEvent* outStreamEvent = NULL) ;
    
    /** 默认析构函数
    */
    ~RCWinOutFileStream() ;

public:
    
    /** 创建文件
    @param [in] fileName 文件名
    @param [in] createAlways 是否始终创建文件
    @return 成功返回true, 失败返回false
    */
    bool Create(const RCString& fileName, bool createAlways) ;
    
    /** 打开文件
    @param [in] fileName 文件名
    @param [in] creationDisposition 创建属性
    @return 成功返回true, 失败返回false
    */
    bool Open(const RCString& fileName, DWORD creationDisposition) ;
    
    /** 关闭文件
    @return 关闭成功返回RC_S_OK, 否则返回错误码
    */
    HResult Close() ;
    
    /** 设置文件时间
    @param [in] cTime 文件创建时间
    @param [in] aTime 文件最后访问时间
    @param [in] mTime 文件修改时间
    @return 成功返回true, 失败返回false
    */
    bool SetTime(const FILETIME* cTime, const FILETIME* aTime, const FILETIME* mTime) ;
    
    /** 设置文件修改时间
    @param [in] mTime 文件修改时间
    @return 成功返回true, 失败返回false
    */
    bool SetMTime(const FILETIME* mTime) ;
    
    /** 写数据  
    @param [in] data 存放数据的缓冲区
    @param [in] size 缓冲区大小
    @param [out] processedSize 实际读取的大小
    @return 成功返回RC_S_OK, 否则返回错误码
    */
    virtual HResult Write(const void* data, uint32_t size, uint32_t* processedSize) ;
    
    /** 调整指针位置
    @param [in] offset 需要调整的偏移量
    @param [in] seekOrigin 调整的基准位置
    @param [out] newPosition 调整后的文件偏移
    @return 成功返回RC_S_OK, 否则返回错误码
    */
    virtual HResult Seek(int64_t offset, RC_STREAM_SEEK seekOrigin, uint64_t* newPosition) ;
    
    /** 调整大小
    @param [in] newSize 新的大小 
    @return 成功返回RC_S_OK, 否则返回错误码
    */
    virtual HResult SetSize(uint64_t newSize) ;

    /** 已处理的的大小
    @return 返回已处理大小
    */
    virtual uint64_t GetProcessedSize(void) const ;
    
    /** 获取流对应的文件名，含完整路径
    @param [out] fileName 返回当前的文件名，含完整路径
    @return 成功返回RC_S_OK, 否则返回错误码
    */
    virtual HResult GetFileName(RCString& fileName) ;
    
private:
    
    /** 写入数据实现
    */
    RCWinOutFile m_file ;
    
    /** 已经写入的字节数
    */
    uint64_t m_processedSize;
};

END_NAMESPACE_RCZIP

#endif //__RCWinOutFileStream_h_
