/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCZipAddCommon_h_
#define __RCZipAddCommon_h_ 1

#include "base/RCDefs.h"
#include "RCZipCompressionMethodMode.h"
#include "interface/ICoder.h"
#include "interface/IStream.h"
#include "common/RCFilterCoder.h"
#include "compress/copy/RCCopyCoder.h"
#include "crypto/Zip/RCZipEncoder.h"
#include "crypto/WzAES/RCWzAESEncoder.h"
#include "format/zip/RCZipExtFilter.h"
#include "format/zip/RCZipFileFilter.h"

BEGIN_NAMESPACE_RCZIP

struct RCZipCompressingResult
{
    /** 解压大小
    */
    uint64_t m_unpackSize ;

    /** 压缩包大小
    */
    uint64_t m_packSize ;
    
    /** CRC校验码
    */
    uint32_t m_crc ;

    /** 压缩模式
    */
    uint16_t m_method ;

    /** 解压版本
    */
    byte_t m_extractVersion ;
};

class RCZipAddCommon
{
public:

    /** 默认构造函数
    @param [in] options 压缩模式信息
    */
    RCZipAddCommon(const RCZipCompressionMethodMode& options);

public:

    /** 压缩
    @param [in] codecsInfo 编码管理器
    @param [in] inStream 输入流
    @param [in] outStream 输出流
    @param [in] progress 进度管理器
    @param [in] operationResult 压缩结果
    @param [in] zipExtFilter 扩展名过滤器
    @param [in] zipFileFilter 文件过滤器
    @param [in] isFileFilter 是否进行文件过滤
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult Compress(ICompressCodecsInfo* codecsInfo,
                     ISequentialInStream* inStream,
                     IOutStream* outStream,
                     ICompressProgressInfo* progress,
                     RCZipCompressingResult& operationResult,
                     RCZipExtFilterPtr& zipExtFilter,
                     RCZipFileFilter& zipFileFilter,
                     bool isFileFilter);

private:

    /** 压缩模式
    */
    RCZipCompressionMethodMode m_options;

    /** COPY编码器
    */
    RCCopyCoder* m_copyCoderSpec;

    /** COPY编码器
    */
    ICompressCoderPtr m_copyCoder;

    /** LZMA编码器
    */
    ICompressCoderPtr m_zipLzmaEncoder;

    /** SHRINK编码器
    */
    ICompressCoderPtr m_shrinkEncoder;

    /** IMPLODED编码器
    */
    ICompressCoderPtr m_implodedEncoder;

    /** DEFLATED编码器
    */
    ICompressCoderPtr m_deflatedComEncoder;

    /** DEFLATED64编码器
    */
    ICompressCoderPtr m_deflated64Encoder;

    /** BZIP2编码器
    */
    ICompressCoderPtr m_bzip2Encoder;

    /** 加密过滤器
    */
    RCFilterCoder* m_cryptoStreamSpec;

    /** 加密输出流
    */
    ISequentialOutStreamPtr m_cryptoStream;

    /** ZIP加密器
    */
    RCZipEncoder* m_filterSpec;

    /** WZAES加密器
    */
    RCWzAESEncoder* m_filterAesSpec;

    /** ZIP加密器
    */
    ICompressFilterPtr m_zipCryptoFilter;

    /** WZAES加密器
    */
    ICompressFilterPtr m_aesFilter;
};

END_NAMESPACE_RCZIP

#endif //__RCZipAddCommon_h_
