/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCZDecoder_h_
#define __RCZDecoder_h_ 1

#include "interface/ICoder.h"
#include "interface/IUnknownImpl.h"

BEGIN_NAMESPACE_RCZIP

/** Z 解码器
*/
class RCZDecoder:
    public IUnknownImpl2<ICompressCoder,ICompressSetDecoderProperties2>
{
public:

    /** 默认构造函数
    */
    RCZDecoder() ;
    
    /** 默认析构函数
    */
    ~RCZDecoder() ;

public:

    /** 释放空间
    */
    void Free() ;
    
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

    /** 设置解压属性
    @param [in] data 属性数据
    @param [in] size 数据长度
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult SetDecoderProperties2(const byte_t* data, uint32_t size) ;
    
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

    /** 属性
    */
    byte_t m_properties;

    /** 最大位数
    */
    int32_t m_numMaxBits;

    /** parents
    */
    uint16_t* m_parents;

    /** 后缀
    */
    byte_t* m_suffixes;

    /** stack
    */
    byte_t* m_stack;
};

END_NAMESPACE_RCZIP

#endif //__RCZDecoder_h_
