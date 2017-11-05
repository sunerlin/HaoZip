/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCUdfProgressImp_h_
#define __RCUdfProgressImp_h_ 1

#include "base/RCTypes.h"
#include "interface/IArchive.h"
#include "RCUdfIn.h"

BEGIN_NAMESPACE_RCZIP

/** Udf 打开进度实现
*/
class RCUdfProgressImp: 
    public RCUdfProgressVirt
{
public:

    /** 设置总数
    @param [in] numBytes 字节数
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult SetTotal(uint64_t numBytes);

    /** 设置完成数
    @param [in] numFiles 文件数
    @param [in] numBytes 字节数
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult SetCompleted(uint64_t numFiles, uint64_t numBytes);

    /** 设置完成
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult SetCompleted();

    /** 构造函数
    @param [in] callback 打开回调接口
    */
    RCUdfProgressImp(IArchiveOpenCallback *callback);

private:

    /** 打开回调接口
    */
    IArchiveOpenCallbackPtr m_callback;

    /** 文件数
    */
    uint64_t m_numFiles;

    /** 字节数
    */
    uint64_t m_numBytes;
};

END_NAMESPACE_RCZIP

#endif //__RCUdfProgressImp_h_
