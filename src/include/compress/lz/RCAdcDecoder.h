/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCAdcDecoder_h_
#define __RCAdcDecoder_h_ 1

#include "interface/ICoder.h"
#include "interface/IUnknownImpl.h"
#include "compress/lz/RCLZOutWindow.h"
#include "common/RCInBuffer.h"

BEGIN_NAMESPACE_RCZIP

/** Adc 解码
*/
class RCAdcDecoder:
    public IUnknownImpl<ICompressCoder>
{
public:

    /** 默认构造函数
    */
    RCAdcDecoder() ;
    
    /** 默认析构函数
    */
    ~RCAdcDecoder() ;
    
public:
    
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


public:
    
    /** Flush流中的数据
    */
    void Flush() ;
    
    /** 释放流
    */
    void ReleaseStreams() ;
    
private:
    
    /** 解码
    @param [in] inStream 输入流
    @param [in] outStream 输出流
    @param [in] inSize 输入大小
    @param [in] outSize 输出大小
    @param [in] progress 进度显示接口
    @return 成功返回RC_S_OK,否则返回错误号
    */
    HResult CodeReal(ISequentialInStream* inStream,
                     ISequentialOutStream* outStream, 
                     const uint64_t* inSize, 
                     const uint64_t* outSize,
                     ICompressProgressInfo* progress) ;
    
private:
    
    /** 输出流
    */
    RCLZOutWindow m_outWindowStream ;
    
    /** 输入流
    */
    RCInBuffer m_inStream ;
};

END_NAMESPACE_RCZIP

#endif //__RCAdcDecoder_h_
