/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCWinInFile_h_
#define __RCWinInFile_h_ 1

#include "filesystem/windows/RCWinFileBase.h"

BEGIN_NAMESPACE_RCZIP

class RCWinInFile:
    public RCWinFileBase
{
public:

    /** 默认构造函数
    */
    RCWinInFile() ;
    
    /** 默认析构函数
    */
    ~RCWinInFile() ;
    
public:
    
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
              DWORD flagsAndAttributes);
              
    /** 共享方式打开文件
    @param [in] fileName 文件名
    @param [in] shareForWrite 是否设置共享写属性
    @return 成功返回true, 失败返回false
    */
    bool OpenShared(const RCString& fileName, bool shareForWrite);
    
    /** 打开文件
    @param [in] fileName 文件名
    @return 成功返回true, 失败返回false
    */
    bool Open(const RCString& fileName);
    
    /** 读取文件数据
    @param [out] data 数据缓冲区
    @param [in] size 期望读取的数据大小
    @param [in] processedSize 实际读取的大小
    @return 成功返回true, 失败返回false
    */
    bool Read(void* data, uint32_t size, uint32_t& processedSize);
    
private:
    
    /** 分段读取文件数据
    @param [out] data 数据缓冲区
    @param [in] size 期望读取的数据大小
    @param [in] processedSize 实际读取的大小
    @return 成功返回true, 失败返回false
    */
    bool ReadPart(void* data, uint32_t size, uint32_t& processedSize);
};

END_NAMESPACE_RCZIP

#endif //__RCWinInFile_h_
