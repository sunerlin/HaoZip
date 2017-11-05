/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCCoderMixerMT_h_
#define __RCCoderMixerMT_h_ 1

#include "format/common/RCCoderMixer.h"
#include "locked/RCVirtThread.h"
#include "locked/RCStreamBinder.h"
#include "interface/IStream.h"
#include "interface/ICoder.h"
#include "interface/IUnknownImpl.h"
#include "base/RCSmartPtr.h"

BEGIN_NAMESPACE_RCZIP

struct RCCoderMixerCoder: 
    public RCCoderMixserCoderInfo, 
    public RCVirtThread
{
    /** 结果
    */
    HResult m_result;

    /** 输入流
    */
    RCVector<ISequentialInStreamPtr> m_inStreams;

    /** 输出流
    */
    RCVector<ISequentialOutStreamPtr> m_outStreams;

    /** 输入流指针
    */
    RCVector<ISequentialInStream*> m_inStreamPointers;

    /** 输出流指针
    */
    RCVector<ISequentialOutStream*> m_outStreamPointers;

    /** 构造函数
    */
    RCCoderMixerCoder(uint32_t numInStreams, uint32_t numOutStreams);

    /** 设置编码/解码器信息
    @param [in] inSizes 输入大小
    @param [in] outSizes 输出大小
    */
    void SetCoderInfo(const uint64_t** inSizes, const uint64_t** outSizes);

    /** 运行
    */
    virtual void Execute();

    /** 编码
    @param [in] progress 压缩进度管理器
    */
    void Code(ICompressProgressInfo* progress) ;
};

/** RCCoderMixerCoder智能指针
*/
typedef RCSharedPtr<RCCoderMixerCoder> RCCoderMixerCoderPtr ;

class RCCoderMixerMT:
    public IUnknownImpl<ICompressCoder2>,
    public RCCoderMixer
{
public:

    /** 默认构造函数
    */
    RCCoderMixerMT() ;
    
    /** 默认析构函数
    */
    ~RCCoderMixerMT() ;
    
public:

    /** 设置混合编码器绑定信息
    @param [in] bindInfo 混合编码器绑定信息
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult SetBindInfo(const RCCoderMixserBindInfo& bindInfo) ;

    /** 添加编码器
    @param [in] coder 编码器
    */
    void AddCoder(ICompressCoder* coder) ;

    /** 添加编码器二
    @param [in] coder 编码器二
    */
    void AddCoder2(ICompressCoder2* coder) ;

    /** 设置进度编码索引
    @param [in] coderIndex 编码索引
    */
    void SetProgressCoderIndex(int32_t coderIndex) ;

    /** 重新初始化
    */
    void ReInit() ;

    /** 设置编码/解码器信息
    @param [in] coderIndex 编码器索引
    @param [in] inSizes 输入大小
    @param [in] outSizes 输出大小
    */
    void SetCoderInfo(uint32_t coderIndex, const uint64_t** inSizes, const uint64_t** outSizes) ;

    /** 获取写处理大小
    @param [in] binderIndex 混合编码器绑定索引
    @return 返回处理大小
    */
    uint64_t GetWriteProcessedSize(uint32_t binderIndex) const ;

    /** 压缩/解压数据
    @param [in] inStreams 输入流信息
    @param [in] outStreams 输出流信息
    @param [in] progress 进度回调接口
    @return 成功返回RC_S_OK，否则返回错误号
    */
    virtual HResult Code(const std::vector<in_stream_data>& inStreams,
                         const std::vector<out_stream_data>& outStreams, 
                         ICompressProgressInfo* progress) ;
                             
public:

    /** 混合编码器
    */
    RCVector<RCCoderMixerCoderPtr> m_coders ;
    
private:

    /** 添加编码器公共信息
    */
    void AddCoderCommon() ;

    /** 初始化
    @param [in] inStreams 输入流
    @param [in] outStreams 输出流
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult Init(const std::vector<in_stream_data>& inStreams,
                 const std::vector<out_stream_data>& outStreams) ;

    /** 错误就返回
    @param [in] code 错误码
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult ReturnIfError(HResult code) ;
    
private:

    /** RCStreamBinder智能指针
    */
    typedef RCSharedPtr<RCStreamBinder> RCStreamBinderPtr ;

    /** 混合编码器绑定信息
    */
    RCCoderMixserBindInfo m_bindInfo;

    /** 流的绑定信息
    */
    RCVector<RCStreamBinderPtr> m_streamBinders;

    /** 进度编码索引
    */
    int32_t m_progressCoderIndex ;
};

END_NAMESPACE_RCZIP

#endif //__RCCoderMixerMT_h_
