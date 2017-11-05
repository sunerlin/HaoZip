/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCCoderMixer_h_
#define __RCCoderMixer_h_ 1

#include "common/RCVector.h"
#include "interface/ICoder.h"

BEGIN_NAMESPACE_RCZIP

struct RCCoderMixerBindPair
{
    /** 输入索引
    */
    uint32_t m_inIndex;

    /** 输出索引
    */
    uint32_t m_outIndex;
};

struct RCCoderMixserCoderStreamsInfo
{
    /** 输入流个数
    */
    uint32_t m_numInStreams;

    /** 输出流个数
    */
    uint32_t m_numOutStreams;
};

struct RCCoderMixserBindInfo
{
    /** 混合编码器流信息
    */
    RCVector<RCCoderMixserCoderStreamsInfo> m_coders;

    /** 混合编码器绑定信息
    */
    RCVector<RCCoderMixerBindPair> m_bindPairs;

    /** 输入流个数
    */ 
    RCVector<uint32_t> m_inStreams;

    /** 输出流个数
    */
    RCVector<uint32_t> m_outStreams;

    /** 清除
    */
    void Clear() ;

    /** 获取流的个数
    @param [out] numInStreams 返回输入流的个数
    @param [out] numOutStreams 返回输出流的个数
    */
    void GetNumStreams(uint32_t& numInStreams, uint32_t& numOutStreams) const ;

    /** 在输入流中查找绑定信息
    @param [in] inStream 输入流的索引
    @return 返回 绑定信息的索引
    */
    int32_t FindBinderForInStream(uint32_t inStream) const ;    

    /** 在输出流中查找绑定信息
    @param [in] outStream 输入流的索引
    @return 返回绑定信息的索引
    */
    int32_t FindBinderForOutStream(uint32_t outStream) const ;

    /** 获取输入流中的编码器索引
    @param [in] coderIndex 编码器索引
    @return 返回输入流中的编码器索引
    */
    uint32_t GetCoderInStreamIndex(uint32_t coderIndex) const ;

    /** 获取输出流中的编码器索引
    @param [in] coderIndex 编码器索引
    @return 返回输出流中的编码器索引
    */
    uint32_t GetCoderOutStreamIndex(uint32_t coderIndex) const ;

    /** 在输入流中查找
    @param [in] streamIndex 流索引
    @param [in] coderIndex 编码器索引
    @param [out] coderStreamIndex 编码器流索引
    @return 成功返回RC_S_OK，否则返回RC_E_FAIL
    */
    HResult FindInStream(uint32_t streamIndex, 
                         uint32_t& coderIndex,
                         uint32_t& coderStreamIndex) const ;
    /** 在输出流中查找
    @param [in] streamIndex 流索引
    @param [in] coderIndex 编码器索引
    @param [out] coderStreamIndex 编码器流索引
    @return 成功返回RC_S_OK，否则返回RC_E_FAIL
    */
    HResult FindOutStream(uint32_t streamIndex, 
                          uint32_t& coderIndex,
                          uint32_t& coderStreamIndex) const ;
};

class RCCoderMixserBindReverseConverter
{
public:

    /** 构造函数
    */
    RCCoderMixserBindReverseConverter(const RCCoderMixserBindInfo& srcBindInfo);

    /** 创建反向混合编码器绑定信息
    @param [out] destBindInfo 返回混合编码器绑定信息
    */
    void CreateReverseBindInfo(RCCoderMixserBindInfo& destBindInfo);

public:

    /** 输入流个数
    */
    uint32_t m_numSrcInStreams;

    /** 目标输出源输入map
    */
    RCVector<uint32_t> m_destOutToSrcInMap;

private:

    /** 输出流个数
    */
    uint32_t m_numSrcOutStreams;

    /** 源混合编码器绑定信息
    */
    RCCoderMixserBindInfo m_srcBindInfo;

    /** 源输入目标输出map
    */
    RCVector<uint32_t> m_srcInToDestOutMap;

    /** 源输出目标输入map
    */
    RCVector<uint32_t> m_srcOutToDestInMap;

    /** 目标输如源输出map
    */
    RCVector<uint32_t> m_destInToSrcOutMap;
};

struct RCCoderMixserCoderInfo
{
    /** 压缩编码器
    */
    ICompressCoderPtr m_coder;

    /** 压缩编码器
    */
    ICompressCoder2Ptr m_coder2;

    /** 输入流个数
    */
    uint32_t m_numInStreams;

    /** 输出流个数
    */
    uint32_t m_numOutStreams;

    /** 输入大小
    */
    RCVector<uint64_t> m_inSizes;

    /** 输出大小
    */
    RCVector<uint64_t> m_outSizes;

    /** 输入大小指针
    */
    RCVector<const uint64_t*> m_inSizePointers;

    /** 输出大小指针
    */
    RCVector<const uint64_t*> m_outSizePointers;

    /** 构造函数
    */
    RCCoderMixserCoderInfo(uint32_t numInStreams, uint32_t numOutStreams);

    /** 设置编码/解码器信息
    @param [in] inSizes 输入大小
    @param [in] outSizes 输出大小
    */
    void SetCoderInfo(const uint64_t** inSizes, const uint64_t** outSizes);

    /** 根据ID查询接口
    @param [in] iid 被查询的接口ID
    @param [out] outObject 如果查询成功，则保存结果接口指针，并增加引用计数
    @return 如果成功则返回RC_S_OK，否则返回错误号
    */
    HResult QueryInterface(RC_IID iid, void** outObject) const ;
};

class RCCoderMixer
{
public:

    /** 设置混合编码器绑定信息
    @param [in] bindInfo 混合编码器绑定信息
    @return 成功返回RC_S_OK，否则返回错误号
    */
    virtual HResult SetBindInfo(const RCCoderMixserBindInfo& bindInfo) = 0 ;

    /** 重新初始化
    */
    virtual void ReInit() = 0;

    /** 设置编码/解码器信息
    @param [in] coderIndex 编码器索引
    @param [in] inSizes 输入大小
    @param [in] outSizes 输出大小
    */
    virtual void SetCoderInfo(uint32_t coderIndex, const uint64_t** inSizes, const uint64_t** outSizes) = 0 ;

protected:

    /** 默认析构函数
    */
    ~RCCoderMixer() {} ;
};

END_NAMESPACE_RCZIP

#endif //__RCCoderMixer_h_
