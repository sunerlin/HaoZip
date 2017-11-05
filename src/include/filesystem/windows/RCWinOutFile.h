/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCWinOutFile_h_
#define __RCWinOutFile_h_ 1

#include "filesystem/windows/RCWinFileBase.h"

BEGIN_NAMESPACE_RCZIP

class IOutStreamEvent ;

class RCWinOutFile:
    public RCWinFileBase
{
public:

    /** 构造函数
    @param [in] outStreamEvent IO写入事件处理回调接口
    */
    RCWinOutFile(IOutStreamEvent* outStreamEvent = NULL) ;
    
    /** 默认析构函数
    */
    ~RCWinOutFile() ;
    
public:
    
    /** 打开文件
    @param [in] fileName 文件名
    @param [in] creationDisposition 创建属性
    @return 成功返回true, 失败返回false
    */
    bool Open(const RCString& fileName, DWORD creationDisposition) ;
    
    /** 创建文件
    @param [in] fileName 文件名
    @param [in] createAlways 是否始终创建文件
    @return 成功返回true, 失败返回false
    */
    bool Create(const RCString& fileName, bool createAlways);
    
    /** 设置文件时间
    @param [in] cTime 文件创建时间
    @param [in] aTime 文件最后访问时间
    @param [in] mTime 文件修改时间
    @return 成功返回true, 失败返回false
    */
    bool SetTime(const FILETIME* cTime, const FILETIME* aTime, const FILETIME* mTime);
    
    /** 设置文件修改时间
    @param [in] mTime 文件修改时间
    @return 成功返回true, 失败返回false
    */
    bool SetMTime(const FILETIME* mTime);
    
    /** 写入数据
    @param [in] data 数据缓冲区
    @param [in] size 数据长度
    @param [out] processedSize 实际写入的数据长度
    @param [out] result 失败时返回错误码
    @return 成功返回true, 失败返回false
    */
    bool Write(const void* data, uint32_t size, uint32_t& processedSize, HResult* result = NULL) ;
    
    /** 将文件指针定位到文件尾
    @return 成功返回true, 失败返回false
    */
    bool SetEndOfFile();
    
    /** 设置文件长度
    @param [in] length 文件长度
    @return 成功返回true, 失败返回false
    */
    bool SetLength(uint64_t length) ;
   
private:

    /** 写入数据
    @param [in] data 数据缓冲区
    @param [in] size 数据长度
    @param [out] processedSize 实际写入的数据长度
    @return 成功返回RC_S_OK, 失败返回错误码
    */
    HResult WritePart(const void* data, uint32_t size, uint32_t& processedSize) ;    
    
    /** 打开文件
    @param [in] fileName 文件名
    @param [in] shareMode 共享属性
    @param [in] creationDisposition 创建属性
    @param [in] flagsAndAttributes 标记位
    @return 成功返回true, 失败返回false
    */
    bool Open(const RCString& fileName, 
              DWORD shareMode,
              DWORD creationDisposition,
              DWORD flagsAndAttributes) ;
    
private:
    
    /** IO写入事件处理回调接口
    */
    IOutStreamEvent* m_outStreamEvent ;
};

END_NAMESPACE_RCZIP

#endif //__RCWinOutFile_h_
