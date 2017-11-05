/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCNsisDecode_h_
#define __RCNsisDecode_h_ 1

#include "interface/ICoder.h"
#include "interface/IStream.h"

BEGIN_NAMESPACE_RCZIP

namespace RCNsisMethodType
{
    enum EEnum
    {
        kCopy,
        kDeflate,
        kBZip2,
        kLZMA
    };
}

class RCNsisDecode
{
public:

    /** 初始化
    @param [in] codecsInfo 编码解码器指针
    @param [in] inStream 输入
    @param [in] method 压缩方法
    @param [in] thereIsFilterFlag 过滤标志
    @param [out] useFilter 返回是否过滤
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult Init(ICompressCodecsInfo* codecsInfo,
                 IInStream* inStream,
                 RCNsisMethodType::EEnum method,
                 bool thereIsFilterFlag,
                 bool& useFilter) ;

    /** 读取数据
    @param [out] data 存放读取结果的缓冲区地址
    @param [in] processedSize 返回实际读到的大小
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult Read(void* data, size_t* processedSize) ;

    /** 释放
    */
    void Release()
    {
        m_filterInStream.Release() ;
        m_codecInStream.Release() ;
        m_decoderInStream.Release() ;
    }
    
private:

    /** 压缩方法
    */
    RCNsisMethodType::EEnum m_method ;

    /** 过滤输入流
    */
    ISequentialInStreamPtr m_filterInStream ;

    /** 编码输入流
    */
    ISequentialInStreamPtr m_codecInStream ;

    /** 解码输入流
    */
    ISequentialInStreamPtr m_decoderInStream ;
};

END_NAMESPACE_RCZIP

#endif //__RCNsisDecode_h_
