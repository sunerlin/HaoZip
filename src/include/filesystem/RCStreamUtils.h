/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCStreamUtils_h_
#define __RCStreamUtils_h_ 1

#include "interface/IStream.h"

BEGIN_NAMESPACE_RCZIP

/** 数据流读写辅助类
*/
class RCStreamUtils
{
public:
    
    /** 从输入流中读取数据
    @param [in] stream 数据流接口
    @param [out] data 数据缓冲区
    @param [in,out]  size 输入期望读取的长度，输出实际读取的长度
    @return 成功返回RC_S_OK, 失败返回错误码
    */
    static HResult ReadStream(ISequentialInStream* stream, void* data, size_t* size);
    
    /** 从输入流中读取数据
    @param [in] stream 数据流接口
    @param [out] data 数据缓冲区
    @param [in]  size 输入期望读取的长度
    @return 成功读取size长度返回RC_S_OK, 若读取不到size长度返回RC_S_FALSE, 失败返回错误码
    */
    static HResult ReadStream_FALSE(ISequentialInStream* stream, void* data, size_t size);
    
    /** 从输入流中读取数据
    @param [in] stream 数据流接口
    @param [out] data 数据缓冲区
    @param [in]  size 输入期望读取的长度
    @return 成功读取size长度返回RC_S_OK, 若读取不到size长度返回RC_E_FAIL, 失败返回错误码
    */
    static HResult ReadStream_FAIL(ISequentialInStream* stream, void* data, size_t size);
    
    /** 向输出流中写入数据
    @param [in] stream 数据流接口
    @param [in] data 数据缓冲区
    @param [in]  size 输入期望读取的长度
    @return 成功返回RC_S_OK, 失败返回错误码
    */
    static HResult WriteStream(ISequentialOutStream* stream, const void* data, size_t size);
    
    /** 创建限制大小的数据流接口
    @param [in] inStream 输入流接口
    @param [in] pos 输入流中的数据起始偏移
    @param [in] size 输入流的长度
    @param [out] resStream 返回创建的流接口指针
    @return 成功返回RC_S_OK, 失败返回错误码
    */
    static HResult CreateLimitedInStream(IInStream* inStream, uint64_t pos, uint64_t size, ISequentialInStream** resStream) ;

private:
    
    /** 默认的数据块大小
    */
    static const uint32_t m_kBlockSize = ((uint32_t)1 << 31) ;
};

END_NAMESPACE_RCZIP

#endif //__RCStreamUtils_h_
