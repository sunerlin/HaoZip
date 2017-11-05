/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCWinStdInFileStream_h_
#define __RCWinStdInFileStream_h_ 1

#include "interface/IStream.h"
#include "interface/IUnknownImpl.h"

BEGIN_NAMESPACE_RCZIP

class RCWinStdInFileStream:
    public IUnknownImpl<ISequentialInStream>
{
public:

    /** 默认构造函数
    */
    RCWinStdInFileStream() ;
    
    /** 默认析构函数
    */
    ~RCWinStdInFileStream() ;
    
public:
    
    /** 读取数据 
    @param [out] data 存放数据的缓冲区
    @param [in] size 缓冲区大小
    @param [out] processedSize 实际读取的大小
    @return 成功返回RC_S_OK, 否则返回错误码
    */
    virtual HResult Read(void* data, uint32_t size, uint32_t* processedSize) ;
};

END_NAMESPACE_RCZIP

#endif //__RCWinStdInFileStream_h_
