/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCTarOut_h_
#define __RCTarOut_h_ 1

#include "interface/IStream.h"
#include "RCTarItem.h"

BEGIN_NAMESPACE_RCZIP

/** Tar 输出
*/
class RCTarOut
{
public: 

    /** 创建
    @param [in] outStream 输出流
    */
    void Create(ISequentialOutStream *outStream);

    /** 写入文件项
    @param [in] item 文件项信息
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult WriteHeaderReal(const RCTarItem &item);

    /** 写入文件项
    @param [in] item 文件项信息
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult WriteHeader(const RCTarItem &item);

    /** 填写文件数据
    @param [in] dataSize 数据大小
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult FillDataResidual(uint64_t dataSize);

    /** 写完成头结构
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult WriteFinishHeader();

private:

    /** 写入字节数据 
    @param [in] buffer 缓存
    @param [in] size 大小
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult WriteBytes(const void *buffer, uint32_t size);

private:

    /** 输出流
    */
    RCComPtr<ISequentialOutStream> m_stream;
};

END_NAMESPACE_RCZIP

#endif //__RCTarOut_h_
