/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCQuantumDecoder_h_
#define __RCQuantumDecoder_h_ 1

#include "interface/ICoder.h"
#include "interface/IUnknownImpl.h"
#include "compress/lz/RCLZOutWindow.h"
#include "compress/quantum/RCQuantumRangeDecoder.h"
#include "compress/quantum/RCQuantumModelDecoder.h"
#include "compress/quantum/RCQuantumDefs.h"

BEGIN_NAMESPACE_RCZIP

/** Quantum 解码器
*/
class RCQuantumDecoder:
    public IUnknownImpl4<ICompressCoder,
                         ICompressSetInStream,
                         ICompressSetOutStreamSize,
                         ICompressSetCoderProperties
                        >
{
public:

    /** 默认构造函数
    */
    RCQuantumDecoder() ;
    
    /** 默认析构函数
    */
    ~RCQuantumDecoder() ;
    
public:

    /**
    */
    void Init() ;

    /**
    @return 成功返回RC_S_OK,否则返回错误号
    */
    HResult CodeSpec(uint32_t size) ;

    /**
    */
    void ReleaseStreams() ;

    /**
    @return 成功返回RC_S_OK,否则返回错误号
    */
    HResult Flush() ;
            
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
    
    /** 设置输入流
    @param [in] inStream 输入流接口指针
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult SetInStream(ISequentialInStream* inStream) ;
    
    /** 释放输入流
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult ReleaseInStream() ;
    
    /** 设置输出流大小
    @param [in] outSize 输出流大小
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult SetOutStreamSize(const uint64_t* outSize) ;
    
    /** 设置压缩编码属性
    @param [in] propertyArray 压缩编码属性列表
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult SetCoderProperties(const RCPropertyIDPairArray& propertyArray) ;
    
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

    /** 设置参数
    @param [in] numDictBits 字典位数
    */
    void SetParams(int32_t numDictBits) ;

    /** 设置是否保留历史
    @param [in] keepHistory 是否保留历史
    */
    void SetKeepHistory(bool keepHistory) ;
        
private:

    /** 输出流
    */
    RCLZOutWindow m_outWindowStream;

    /** Range解码器
    */
    RCQuantumRangeDecoder m_rangeDecoder;

    /** 输出大小
    */
    uint64_t m_outSize;

    /** 保留长度
    */
    int32_t m_remainLen; // -1 means end of stream. // -2 means need Init

    /** 重复0
    */
    uint32_t m_rep0;

    /** 字典位数
    */
    int32_t m_numDictBits;

    /** 字典大小
    */
    uint32_t m_dictionarySize;

    /** 解码器
    */
    RCQuantumModelDecoder m_selector ;

    /** 解码器
    */
    RCQuantumModelDecoder m_literals[RCQuantumDefs::s_kNumLitSelectors] ;

    /** 解码器
    */
    RCQuantumModelDecoder m_posSlot[RCQuantumDefs::s_kNumMatchSelectors] ;

    /** 解码器
    */
    RCQuantumModelDecoder m_lenSlot ;

    /** 是否保留历史
    */
    bool m_keepHistory ;
};

END_NAMESPACE_RCZIP

#endif //__RCQuantumDecoder_h_
