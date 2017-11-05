/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCWimUnpacker_h_
#define __RCWimUnpacker_h_ 1

#include "common/RCBuffer.h"
#include "format/wim/RCWimDecoder.h"
#include "format/wim/RCWimResource.h"
#include "interface/ICoder.h"
#include "interface/IStream.h"

BEGIN_NAMESPACE_RCZIP

/** Wim Unpacker
*/
class RCWimUnpacker
{
public:

    /** 默认构造函数
    */
    RCWimUnpacker(ICompressCodecsInfo* compressCodecsInfo) ;
    
    /** 默认析构函数
    */
    ~RCWimUnpacker() ;
    
public:

    /** 解压
    @param [in] inStream 输入流
    @param [in] resource WIM资源数据
    @param [in] lzxMode lzx模式
    @param [in] outStream 输出流
    @param [in] progress 进度回调接口
    @param [out] digest 解压数据地址
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult Unpack( IInStream* inStream, 
                    const RCWimResource& resource,
                    bool lzxMode,
                    ISequentialOutStream* outStream, 
                    ICompressProgressInfo* progress,
                    byte_t* digest);
    
    /** 设置编码解码器管理接口指针
    @param [in] compressCodecsInfo 编码解码器指针
    @return 成功返回RC_S_OK，否则返回错误号 
    */
    HResult SetCompressCodecsInfo(ICompressCodecsInfo* compressCodecsInfo) ;
    
private:

    /** 解压
    @param [in] inStream 输入流
    @param [in] resource 
    @param [in] lzxMode lzx模式
    @param [in] outStream 输出流
    @param [in] progress 进度回调接口
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult Unpack(IInStream* inStream, 
                   const RCWimResource& resource, 
                   bool lzxMode,
                   ISequentialOutStream* outStream, 
                   ICompressProgressInfo* progress);
    
private:

    /** copy coder
    */
    ICompressCoderPtr m_copyCoder;

    /** lzx decoder
    */
    ICompressCoderPtr m_lzxDecoder;

    /** xpress decoder
    */
    RCWimDecoder m_xpressDecoder;

    /** 缓存
    */
    RCByteBuffer m_sizesBuf;
    
    /** 编码解码管理器
    */
    ICompressCodecsInfoPtr m_compressCodecsInfo ;
};

END_NAMESPACE_RCZIP

#endif //__RCWimUnpacker_h_
