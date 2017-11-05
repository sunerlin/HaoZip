/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCArjDecoder2_h_
#define __RCArjDecoder2_h_ 1

#include "interface/ICoder.h"
#include "interface/IUnknownImpl.h"
#include "compress/lz/RCLZOutWindow.h"
#include "common/RCBitmDecoder.h"
#include "common/RCInBuffer.h"

BEGIN_NAMESPACE_RCZIP

/** Arj 格式解码2
*/
class RCArjDecoder2:
    public IUnknownImpl<ICompressCoder>
{
public:

    /** 默认构造函数
    */
    RCArjDecoder2() ;
    
    /** 默认析构函数
    */
    ~RCArjDecoder2() ;

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

private:
    
    /** 压缩/解压数据
    @param [in] inStream 数据输入流
    @param [in] outStream 数据输出流
    @param [in] inSize 输入数据大小
    @param [in] outSize 输出数据大小 
    @param [in] progress 进度回调接口
    @throws 失败抛异常
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult CodeReal(ISequentialInStream* inStream,
                             ISequentialOutStream* outStream, 
                             const uint64_t* inSize, 
                             const uint64_t* outSize,
                             ICompressProgressInfo* progress) ;
                             
private:
    
    /** 释放流
    */
    void ReleaseStreams() ;

private:
    
    /** 解码输出
    */
    class CCoderReleaser
    {
    public:
        
        /** 是否需要flush
        */
        bool m_needFlush ;
        
        /** 构造函数
        @param [in] coder Arj 编码器指针
        */
        CCoderReleaser(RCArjDecoder2* coder) ;
        
        /** 默认析构函数
        */
        ~CCoderReleaser() ;
        
    private:
        
        /** Arj 编码器2指针
        */
        RCArjDecoder2* m_coder;
    }; 
    
    friend class CCoderReleaser ;
       
private:
    
    /** 输出流
    */
    RCLZOutWindow m_outWindowStream;
    
    /** 输入流
    */
    RCBitmDecoder<RCInBuffer> m_inBitStream ;
};

END_NAMESPACE_RCZIP

#endif //__RCArjDecoder2_h_
