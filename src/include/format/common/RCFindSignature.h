/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCFindSignature_h_
#define __RCFindSignature_h_ 1

#include "interface/IStream.h"

BEGIN_NAMESPACE_RCZIP

class RCFindSignature
{
public:
    
    /** 在输入流中查找文件格式签名
    @param [in] stream 输入流
    @param [in] signature 签名数据的开始地址
    @param [in] signatureSize 签名数据长度
    @param [in] limit 限制从输入流匹配数据的最大长度限制
    @param [out] resPos 如果签名查找成功，返回签名在输入流中的偏移
    @return 成功返回RC_S_OK，失败返回错误码
    */
    static HResult FindSignatureInStream(ISequentialInStream* stream,
                                         const byte_t* signature, 
                                         uint32_t signatureSize,
                                         const uint64_t* limit, 
                                         uint64_t& resPos) ;
};

END_NAMESPACE_RCZIP

#endif //__RCFindSignature_h_
