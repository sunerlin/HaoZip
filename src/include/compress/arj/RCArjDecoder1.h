/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCArjDecoder1_h_
#define __RCArjDecoder1_h_ 1

#include "interface/ICoder.h"
#include "interface/IUnknownImpl.h"
#include "compress/lz/RCLZOutWindow.h"
#include "common/RCBitmDecoder.h"
#include "common/RCInBuffer.h"

BEGIN_NAMESPACE_RCZIP

#define CODE_BIT    16

#define THRESHOLD   3
#define DDICSIZ     26624
#define MAXDICBIT   16
#define MATCHBIT    8
#define MAXMATCH    256
#define NC          (0xFF + MAXMATCH + 2 - THRESHOLD)
#define NP          (MAXDICBIT + 1)
#define CBIT        9
#define NT          (CODE_BIT + 3)
#define PBIT        5
#define TBIT        5

#if NT > NP
#define NPT NT
#else
#define NPT NP
#endif

#define CTABLESIZE  4096
#define PTABLESIZE  256

/** Arj 格式解码1
*/
class RCArjDecoder1:
    public IUnknownImpl<ICompressCoder>
{
public:

    /** 默认构造函数
    */
    RCArjDecoder1() ;
    
    /** 默认析构函数
    */
    ~RCArjDecoder1() ;

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
    
    /** 释放文件流
    */
    void ReleaseStreams() ;
    
    /** 生成表
    @param [in] nchar 
    @param [in] bitLen
    @param [in] tableBits
    @param [in] table
    @param [in] tableSize
    */
    void MakeTable( int32_t nchar, 
                    byte_t* bitLen, 
                    int32_t tableBits, 
                    uint32_t* table, 
                    int32_t tableSize);
  
    /** 读取C_Len
    */
    void ReadCLen();
    
    /** 读取Pt_Len
    @param [in] nn
    @param [in] nbit
    @param [in] iSpecial
    */
    void ReadPtLen(int32_t nn, int32_t nbit, int32_t iSpecial) ;
    
    /** 解码
    @return
    */
    uint32_t DecodeC() ;
    
    /** 解码
    @return
    */
    uint32_t DecodeP() ;
    
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
    
    /** 解码输出
    */
    class CCoderReleaser
    {
    public:
        
        /** 是否需要flush
        */
        bool m_needFlush ;
        
        /** 构造函数
        @param [in] coder 解码器指针
        */
        CCoderReleaser(RCArjDecoder1* coder) ;
        
        /** 默认析构函数
        */
        ~CCoderReleaser() ;
        
    private:
        
        /** 解码器指针
        */
        RCArjDecoder1* m_coder;
    }; 
    
    /** 友元声明
    */
    friend class CCoderReleaser ;
    
private:
    
    /** 输出流
    */
    RCLZOutWindow m_outWindowStream ;
    
    /** 解码输入流
    */
    RCBitmDecoder<RCInBuffer> m_inBitStream;

    /** left缓存
    */
    uint32_t m_left[2 * NC - 1];
    
    /** right缓存
    */
    uint32_t m_right[2 * NC - 1];
    
    /** C_Len缓存
    */
    byte_t m_cLen[NC];
    
    /** Pt_Len缓存
    */
    byte_t m_ptLen[NPT];

    /** C_Table
    */
    uint32_t m_cTable[CTABLESIZE];
    
    /** Pt_Table
    */
    uint32_t m_ptTable[PTABLESIZE];
};

END_NAMESPACE_RCZIP

#endif //__RCArjDecoder1_h_
