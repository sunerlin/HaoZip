/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCBZip2Update_h_
#define __RCBZip2Update_h_ 1

#include "interface/IArchive.h"
#include "interface/ICoder.h"

BEGIN_NAMESPACE_RCZIP

class RCBZip2Update
{
public:

    /** 压缩数据
    @param [in] codecsInfo 编码管理器
    @param [in] unpackSize 解压后大小
    @param [in] outStream 输出流
    @param [in] indexInClient client中的索引
    @param [in] dictionary 字典大小
    @param [in] numPasses passes
    @param [in] numThreads 线程数
    @param [in] updateCallback 更新管理器回调接口
    @return 成功返回RC_S_OK，否则返回错误号
    */
    static HResult UpdateArchive(ICompressCodecsInfo* codecsInfo,
                                 uint64_t unpackSize,
                                 ISequentialOutStream* outStream,
                                 int32_t indexInClient,
                                 uint32_t dictionary,
                                 uint32_t numPasses,
                                 uint32_t numThreads,
                                 IArchiveUpdateCallback* updateCallback) ;

};

END_NAMESPACE_RCZIP

#endif //__RCBZip2Update_h_
