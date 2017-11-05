/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCPPMDEncoder_h_
#define __RCPPMDEncoder_h_ 1

#include "interface/ICoder.h"
#include "interface/IUnknownImpl.h"
#include "common/RCInBuffer.h"
#include "compress/range/RCRangeEncoder.h"
#include "compress/ppmd/RCPPMDEncodeInfo.h"

BEGIN_NAMESPACE_RCZIP

/** PPMD 编码器
*/
class RCPPMDEncoder:
    public IUnknownImpl3<ICompressCoder,
                         ICompressSetCoderProperties,
                         ICompressWriteCoderProperties
                        >
{
public:

    /** 默认构造函数
    */
    RCPPMDEncoder() ;
    
    /** 默认析构函数
    */
    ~RCPPMDEncoder() ;
    
public:
    
    /** Flush 数据
    @return 成功返回RC_S_OK,否则返回错误号
    */
    HResult Flush() ;
    
    /** 是否数据流
    */
    void ReleaseStreams() ;
    
    /** 压缩/解压数据
    @param [in] inStream 数据输入流
    @param [in] outStream 数据输出流
    @param [in] inSize 输入数据大小
    @param [in] outSize 输出数据大小 
    @param [in] progress 进度回调接口
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult Code(ISequentialInStream* inStream,
                         ISequentialOutStream* outStream, 
                         const uint64_t* inSize, 
                         const uint64_t* outSize,
                         ICompressProgressInfo* progress) ; 

    /** 设置压缩编码属性
    @param [in] propertyArray 压缩编码属性列表
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult SetCoderProperties(const RCPropertyIDPairArray& propertyArray) ;

    /** 将压缩编码属性写入输出流
    @param [in] outStream 输出流接口指针
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult WriteCoderProperties(ISequentialOutStream* outStream) ;
            
private:
    
    /** 压缩/解压数据
    @param [in] inStream 数据输入流
    @param [in] outStream 数据输出流
    @param [in] inSize 输入数据大小
    @param [in] outSize 输出数据大小 
    @param [in] progress 进度回调接口
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult CodeReal(ISequentialInStream* inStream,
                             ISequentialOutStream* outStream, 
                             const uint64_t* inSize, 
                             const uint64_t* outSize,
                             ICompressProgressInfo* progress) ; 
    
private:
    
    /** 输入流
    */
    RCInBuffer m_inStream ;
    
    /** 编码器
    */
    RCRangeEncoder m_rangeEncoder ;
    
    /** 信息
    */
    RCPPMDEncodeInfo m_info ;
    
    /** 内存大小
    */
    uint32_t m_usedMemorySize ;
    
    /** 顺序
    */
    byte_t m_order ;
};

END_NAMESPACE_RCZIP

#endif //__RCPPMDEncoder_h_
