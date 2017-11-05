/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCGZipUpdate_h_
#define __RCGZipUpdate_h_ 1

#include "interface/IStream.h"
#include "interface/IArchive.h"

BEGIN_NAMESPACE_RCZIP

class RCGZipItem ;
class RCGZipDeflateProps ;
class ICompressCodecsInfo ;

class RCGZipUpdate
{
public:

    /** 压缩数据
    @param [in] codecs 编码管理器
    @param [in] outStream 输出流
    @param [in] unpackSize 解压大小
    @param [in] newItem 新项
    @param [in] deflateProps 默认属性
    @param [in] updateCallback 压缩回调函数指针
    @return 成功返回RC_S_OK,否则返回错误号
    */
    static HResult UpdateArchive(ICompressCodecsInfo* codecsInfo,
                                 ISequentialOutStream* outStream,
                                 uint64_t unpackSize,
                                 const RCGZipItem& newItem,
                                 RCGZipDeflateProps& deflateProps,
                                 IArchiveUpdateCallback* updateCallback) ;
};

END_NAMESPACE_RCZIP

#endif //__RCGZipUpdate_h_
