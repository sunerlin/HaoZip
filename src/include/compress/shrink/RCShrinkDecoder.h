/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCShrinkDecoder_h_
#define __RCShrinkDecoder_h_ 1

#include "interface/ICoder.h"
#include "interface/IUnknownImpl.h"

BEGIN_NAMESPACE_RCZIP

/** Shrink 解码器
*/
class RCShrinkDecoder:
    public IUnknownImpl<ICompressCoder>
{
public:
    /** 常量数据定义
    */
    static const int32_t s_kNumMaxBits = 13 ;
    static const uint32_t s_kNumItems = 1 << s_kNumMaxBits ;
    
public:

    /** 默认构造函数
    */
    RCShrinkDecoder() ;
    
    /** 默认析构函数
    */
    ~RCShrinkDecoder() ;

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

    /** parents
    */
    uint16_t m_parents[s_kNumItems] ;

    /** 后缀
    */
    byte_t m_suffixes[s_kNumItems] ;

    /** stack
    */
    byte_t m_stack[s_kNumItems] ;

    /** 是否空闲
    */
    bool m_isFree[s_kNumItems] ;

    /** 是否Parent
    */
    bool m_isParent[s_kNumItems] ;
    
private:

    /** 缓存大小
    */
    static const uint32_t s_kBufferSize = (1 << 20);

    /** 最小数据位
    */
    static const int s_kNumMinBits = 9;
};

END_NAMESPACE_RCZIP

#endif //__RCShrinkDecoder_h_
