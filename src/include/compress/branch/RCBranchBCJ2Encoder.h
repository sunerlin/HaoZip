/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCBranchBCJ2Encoder_h_
#define __RCBranchBCJ2Encoder_h_ 1

#include "interface/ICoder.h"
#include "interface/IUnknownImpl.h"
#include "compress/range/RCRangeEncoder.h"
#include "compress/range/RCRangeBitEncoder.h"
#include "compress/branch/RCBranchBCJ2Defs.h"

BEGIN_NAMESPACE_RCZIP

/** BCJ2 编码器
*/
class RCBranchBCJ2Encoder:
    public IUnknownImpl<ICompressCoder2>
{
public:

    /** 默认构造函数
    */
    RCBranchBCJ2Encoder() ;
    
    /** 默认析构函数
    */
    ~RCBranchBCJ2Encoder() ;

public:
    
    /** 主文件流
    */
    RCOutBuffer m_mainStream ;
    
    /** 调用
    */
    RCOutBuffer m_callStream ;
    
    /** 跳转
    */
    RCOutBuffer m_jumpStream ;
    
    /** 序列编码器
    */
    RCRangeEncoder m_rangeEncoder ;
    
    /** 编码状态
    */
    RCRangeBitEncoder<s_kNumMoveBits> m_statusEncoder[256 + 2] ;
    
public:
    
    /** 创建
    */
    bool Create() ;
    
    /** Flush
    @return 成功返回RC_S_OK,否则返回错误号
    */
    HResult Flush() ;
    
    /** 释放流
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
    
    /** 缓存
    */
    byte_t* m_buffer ;
};

END_NAMESPACE_RCZIP

#endif //__RCBranchBCJ2Encoder_h_
