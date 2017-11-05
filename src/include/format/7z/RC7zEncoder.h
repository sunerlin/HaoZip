/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RC7zEncoder_h_
#define __RC7zEncoder_h_ 1

#include "format/common/RCCoderMixerMT.h"
#include "format/7z/RC7zItem.h"
#include "format/7z/RC7zCompressionMode.h"

BEGIN_NAMESPACE_RCZIP

class RC7zEncoder
{
public:

    /** 构造函数
    */
    RC7zEncoder(const RC7zCompressionMethodMode &options) ;

    /** 默认析构函数
    */
    ~RC7zEncoder() ;
    
public:

    /** 构造编码
    */
    HResult EncoderConstr() ;

    /** 编码
    @param [in] codecsInfo 编码管理器
    @param [in] inStream 输入流
    @param [in] inStreamSize 输入流大小
    @param [in] inSizeForReduce 输入大小
    @param [in] folderItem 文件夹项
    @param [in] outStream 输出流
    @param [in] packSizes 包的大小
    @param [in] compressProgress 压缩进度管理器
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult Encode( ICompressCodecsInfo* codecsInfo,
                    ISequentialInStream* inStream,
                    const uint64_t* inStreamSize, 
                    const uint64_t* inSizeForReduce,
                    RC7zFolder& folderItem,
                    ISequentialOutStream* outStream,
                    RCVector<uint64_t>& packSizes,
                    ICompressProgressInfo* compressProgress) ;
    
private:

    /** 创建混合编码
    @param [in] codecsInfo 编码管理器
    @param [in] inSizeForReduce 输入大小
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult CreateMixerCoder(ICompressCodecsInfo* codecsInfo,
                             const uint64_t* inSizeForReduce) ;

private:

    /** 从混合编码绑定信息得到文件夹信息
    @param [in] bindInfo 混合编码绑定信息
    @param [in] decompressionMethods 解码id
    @param [out] folder 输出文件夹文件
    */
    static void ConvertBindInfoToFolderItemInfo(RCCoderMixserBindInfo& bindInfo,
                                                const RCVector<RCMethodID>& decompressionMethods,
                                                RC7zFolder& folder) ;
private:

    /** 多线程的混合编码
    */
    RCCoderMixerMT* m_mixerCoderSpec ;

    /** 混合编码器
    */
    ICompressCoder2Ptr m_mixerCoder ;

    /** 编码信息
    */
    RCVector<RC7zCoderInfo> m_codersInfo;  

    /** 压缩模式
    */
    RC7zCompressionMethodMode m_options;

    /** 混合编码绑定信息
    */
    RCCoderMixserBindInfo m_bindInfo;

    /** 解码的混合编码绑定信息
    */
    RCCoderMixserBindInfo m_decompressBindInfo;

    /** 解码的混合编码绑定信息转换器
    */
    RCCoderMixserBindReverseConverter* m_bindReverseConverter;

    /** 解码模式
    */
    RCVector<RCMethodID> m_decompressionMethods;

    /** 是否已构造
    */
    bool m_constructed;
};

END_NAMESPACE_RCZIP

#endif //__RC7zEncoder_h_
