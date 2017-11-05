/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RC7zDecoder_h_
#define __RC7zDecoder_h_ 1

#include "format/common/RCCoderMixerMT.h"
#include "format/7z/RC7zItem.h"
#include "interface/IPassword.h"
#include "interface/ICoder.h"
#include "interface/IStream.h"

BEGIN_NAMESPACE_RCZIP

struct RC7zBindInfoEx: 
    public RCCoderMixserBindInfo
{
    /** 编码器id
    */
    RCVector<RCMethodID> m_coderMethodIDs;

    /** 清除
    */
    void Clear()
    {
        RCCoderMixserBindInfo::Clear() ;
        m_coderMethodIDs.clear() ;
    }
};

class RC7zDecoder
{
public:

    /** 默认构造函数
    */
    RC7zDecoder() ;
    
    /** 默认析构函数
    */
    ~RC7zDecoder() ;
    
public:

    /** 解码
    @param [in] codecsInfo 编码管理器
    @param [in] inStream 输入流
    @param [in] startPos 起始位置
    @param [in] packSizes 包大小
    @param [in] folderInfo 文件夹信息
    @param [in] outStream 输出流
    @param [in] compressProgress 压缩进度管理器
    @param [in] getTextPassword 密码管理器
    @param [in] passwordIsDefined 是否有密码
    @param [in] numThreads 线程数
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult Decode(ICompressCodecsInfo* codecsInfo,
                   IInStream* inStream,
                   uint64_t startPos,
                   const uint64_t* packSizes,
                   const RC7zFolder& folderInfo,
                   ISequentialOutStream* outStream,
                   ICompressProgressInfo* compressProgress,
                   ICryptoGetTextPassword* getTextPassword, 
                   bool& passwordIsDefined,
                   uint32_t numThreads) ;

private:

    /** 从文件夹信息得到绑定信息
    @param [in] folder 文件夹信息
    @param [out] bindInfo 返回绑定信息
    */
    static void ConvertFolderItemInfoToBindInfo(const RC7zFolder& folder,
                                                RC7zBindInfoEx& bindInfo) ;

    /** 编码是否相等
    @param [in] a1 编码1
    @param [in] a2 编码2
    @return 相等返回true，否则返回false
    */
    static bool AreCodersEqual(const RCCoderMixserCoderStreamsInfo& a1,
                               const RCCoderMixserCoderStreamsInfo& a2) ;

    /** 混合编码绑定信息中的绑定序列是否相等
    @param [in] a1 混合编码绑定信息1
    @param [in] a2 混合编码绑定信息2
    @return 相等返回true，否则返回false
    */
    static bool AreBindPairsEqual(const RCCoderMixerBindPair& a1, 
                                  const RCCoderMixerBindPair& a2) ;

    /** 绑定扩展信息是否相等
    @param [in] a1 绑定扩展信息1
    @param [in] a2 绑定扩展信息2
    @return 相等返回true，否则返回false
    */
    static bool AreBindInfoExEqual(const RC7zBindInfoEx& a1, 
                                   const RC7zBindInfoEx& a2) ;
                                                                     
private:

    /** 是否有绑定信息
    */
    bool m_bindInfoExPrevIsDefined ;

    /** 绑定信息
    */ 
    RC7zBindInfoEx m_bindInfoExPrev ;

    /** 多线程的混合编码
    */
    RCCoderMixerMT* m_mixerCoderMTSpec ;

    /** 混合编码公共信息
    */
    RCCoderMixer* m_mixerCoderCommon ;
  
    /** 混合编码器
    */
    ICompressCoder2Ptr m_mixerCoder;

    /** 解码器
    */
    RCVector<IUnknownPtr> m_decoders;
};

END_NAMESPACE_RCZIP

#endif //__RC7zDecoder_h_
