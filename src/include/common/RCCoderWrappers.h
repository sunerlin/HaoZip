/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCCoderWrappers_h_
#define __RCCoderWrappers_h_ 1

#include "algorithm/Types.h"
#include "interface/IStream.h"
#include "interface/ICoder.h"

BEGIN_NAMESPACE_RCZIP

/** 压缩进度封装
*/
struct RCCompressProgressWrap
{
    /** 压缩进度底层回调接口
    */
    ICompressProgress m_progress ;
    
    /** 压缩进度界面回调接口
    */
    ICompressProgressInfo* m_progressInfo ;
    
    /** 操作结果
    */
    HResult m_hr ;
    
    /** 构造函数
    @param [in] progress 进度回调接口
    */
    RCCompressProgressWrap(ICompressProgressInfo* progress);
};

/** 输入流接口封装
*/
struct RCSeqInStreamWrap
{
    /** 底层回调接口
    */
    ISeqInStream m_seqIn ;
    
    /** 流读取数据接口
    */
    ISequentialInStream* m_inStream ;
    
    /** 操作结果
    */
    HResult m_hr ;
    
    /** 构造函数
    @param [in] stream 读取数据接口指针
    */
    RCSeqInStreamWrap(ISequentialInStream* stream) ;
};

/** 流的位置重定位接口封装
*/
struct RCSeekInStreamWrap
{
    /** 底层重定位接口
    */
    ISeekInStream m_seekIn ;
    
    /** 输入流接口
    */
    IInStream* m_inStream ;
    
    /** 操作结果
    */
    HResult m_hr ;
    
    /** 构造函数
    @param [in] stream 输入流接口指针
    */
    RCSeekInStreamWrap(IInStream* stream) ;
};

/** 输出流接口封装
*/
struct RCSeqOutStreamWrap
{
    /** 底层输出流接口
    */
    ISeqOutStream m_seqOut ;
    
    /** 输出流接口
    */
    ISequentialOutStream* m_outStream ;
    
    /** 操作结果
    */
    HResult m_hr ;
    
    /** 已处理大小
    */
    uint64_t m_processed ;
    
    /** 构造函数
    @param [in] stream 输出流接口指针
    */
    RCSeqOutStreamWrap(ISequentialOutStream* stream) ;
};

/** 结果类型转换
@param [in] res 处理结果
@return 转换后的HResult类型的结果
*/
HResult SResToHResult(result_t res) ;

END_NAMESPACE_RCZIP

#endif //__RCCoderWrappers_h_
