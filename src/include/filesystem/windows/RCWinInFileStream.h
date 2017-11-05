/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCWinInFileStream_h_
#define __RCWinInFileStream_h_ 1

#include "interface/IStream.h"
#include "interface/IUnknownImpl.h"
#include "filesystem/windows/RCWinInFile.h"

BEGIN_NAMESPACE_RCZIP

class RCWinInFileStream:
    public IUnknownImpl4<IInStream,
                         IStreamGetSize,
                         ICloneStream,
                         IStreamGetFileName>
{
public:

    /** 默认构造函数
    */
    RCWinInFileStream() ;
    
    /** 默认析构函数
    */
    ~RCWinInFileStream() ;
    
public:
    
    /** 打开文件
    @param [in] fileName 文件名
    @return 成功返回true, 失败返回false
    */
    bool Open(const RCString& fileName);
    
    /** 共享方式打开文件
    @param [in] fileName 文件名
    @param [in] shareForWrite 是否设置共享写属性
    @return 成功返回true, 失败返回false
    */
    bool OpenShared(const RCString& fileName, bool shareForWrite);
    
    /** 读取数据 
    @param [out] data 存放数据的缓冲区
    @param [in] size 缓冲区大小
    @param [out] processedSize 实际读取的大小
    @return 成功返回RC_S_OK, 失败返回错误码
    */
    virtual HResult Read(void* data, uint32_t size, uint32_t* processedSize) ;
    
    /** 调整指针位置
    @param [in] offset 需要调整的偏移量
    @param [in] seekOrigin 调整的基准位置
    @param [out] newPosition 调整后的文件偏移
    @return 成功返回RC_S_OK, 失败返回错误码
    */
    virtual HResult Seek(int64_t offset, RC_STREAM_SEEK seekOrigin, uint64_t* newPosition) ;

    /** 获取长度
    @param [out] size 返回流的长度
    @return 成功返回RC_S_OK, 失败返回错误码
    */
    virtual HResult GetSize(uint64_t* size) ;

    /**  Clone 数据流 
    @return Clone 的数据流，如果失败返回NULL
    */
    virtual IUnknown* Clone(void) ;
    
    /** 获取流对应的文件名，含完整路径
    @param [in] fileName 文件名
    @return 成功返回RC_S_OK, 失败返回错误码
    */
    virtual HResult GetFileName(RCString& fileName) ;
    
private:
    
    /** 文件内容读取实现对象
    */
    RCWinInFile m_file ;
};

END_NAMESPACE_RCZIP

#endif //__RCWinInFileStream_h_
