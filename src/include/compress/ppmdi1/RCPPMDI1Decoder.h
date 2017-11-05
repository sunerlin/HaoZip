/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCPPMDI1Decoder_h_
#define __RCPPMDI1Decoder_h_ 1

#include "interface/ICoder.h"
#include "interface/IUnknownImpl.h"

BEGIN_NAMESPACE_RCZIP

/** PPMD I rev 1 解码器
*/
class RCPPMDI1Decoder :
    public IUnknownImpl2<ICompressCoder,
                         ICompressSetDecoderProperties2
                         //,
                         //ICompressSetInStream,
                         //ICompressSetOutStreamSize,
                         //ISequentialInStream
                        >
{
public:

    /** 默认构造函数
    */
    RCPPMDI1Decoder();

    /** 默认析构函数
    */
    ~RCPPMDI1Decoder();

public:

    /** 解码
    @param [in] inStream 输入流
    @param [in] outStream 输出流
    @param [in] inSize 输入大小
    @param [in] outSize 输出大小
    @param [in] progress 进度回调接口
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult Code(ISequentialInStream* inStream,
                         ISequentialOutStream* outStream, 
                         const uint64_t* inSize, 
                         const uint64_t* outSize,
                         ICompressProgressInfo* progress) ; 
    
    /** 设置解码器属性
    @param [in] data 数据指针
    @param [in] size 大小
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult SetDecoderProperties2(const byte_t* data, uint32_t size) ;
    
private:

    /** 顺序
    */
    uint32_t m_order;

    /** 使用内存大小
    */
    uint32_t m_memorySize;

    /** 恢复模式
    */
    uint32_t m_restoreMethod;
};

END_NAMESPACE_RCZIP

#endif //__RCPPMDI1Decoder_h_
