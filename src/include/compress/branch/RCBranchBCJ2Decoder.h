/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCBranchBCJ2Decoder_h_
#define __RCBranchBCJ2Decoder_h_ 1

#include "interface/ICoder.h"
#include "interface/IUnknownImpl.h"
#include "compress/range/RCRangeDecoder.h"
#include "compress/range/RCRangeBitDecoder.h"
#include "compress/branch/RCBranchBCJ2Defs.h"
#include "common/RCOutBuffer.h"
#include "common/RCInBuffer.h"

BEGIN_NAMESPACE_RCZIP

/** BCJ2 解码器
*/
class RCBranchBCJ2Decoder:
    public IUnknownImpl<ICompressCoder2>
{
public:

    /** 默认构造函数
    */
    RCBranchBCJ2Decoder() ;
    
    /** 默认析构函数
    */
    ~RCBranchBCJ2Decoder() ;
   
public:
    
    /** Flush
    @return 成功返回RC_S_OK,否则返回错误号
    */
    HResult Flush() ;
    
    /** 释放流接口
    */
    void ReleaseStreams() ;
    
    /** 压缩/解压数据
    @param [in] inStreams 输入流信息
    @param [in] outStreams 输出流信息
    @param [in] progress 进度回调接口
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult Code(const std::vector<in_stream_data>& inStreams,
                         const std::vector<out_stream_data>& outStreams, 
                         ICompressProgressInfo* progress) ;

private:
    
    /** 压缩/解压数据
    @param [in] inStreams 输入流信息
    @param [in] outStreams 输出流信息
    @param [in] progress 进度回调接口
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult CodeReal(const std::vector<in_stream_data>& inStreams,
                             const std::vector<out_stream_data>& outStreams, 
                             ICompressProgressInfo* progress) ;

private:

    /** 主输入流
    */
    RCInBuffer m_mainInStream;
    
    /** 调用
    */
    RCInBuffer m_callStream;
    
    /** 跳转
    */
    RCInBuffer m_jumpStream;
    
    /** 序列解码器
    */
    RCRangeDecoder m_rangeDecoder;
    
    /** 解码状态
    */
    RCRangeBitDecoder<s_kNumMoveBits> m_statusDecoder[256 + 2];

    /** 输出流
    */
    RCOutBuffer m_outStream;
};

END_NAMESPACE_RCZIP

#endif //__RCBranchBCJ2Decoder_h_
