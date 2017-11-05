/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCMtLocalProgress_h_
#define __RCMtLocalProgress_h_ 1

#include "interface/ICoder.h"
#include "interface/IProgress.h"
#include "interface/IUnknownImpl.h"

BEGIN_NAMESPACE_RCZIP

class RCMtLocalProgress:
    public IUnknownImpl<ICompressProgressInfo>
{
public:

    /** 默认构造函数
    */
    RCMtLocalProgress() ;
    
    /** 默认析构函数
    */
    ~RCMtLocalProgress() ;
    
    /** 初始化
    @param [in] progress 进度回调接口
    @param [in] inSizeIsMain true表示按输入数据回调进度，否则按输出数据回调进度
    */
    void Init(IProgress* progress, bool inSizeIsMain) ;
    
    /** 按当前进度数据回调进度接口
    @return 成功返回RC_S_OK, 失败则返回错误码
    */
    HResult SetCur() ;
    
    /** 设置当前处理的下标值
    @param [in] index 当前处理的下标值
    */
    void SetIndex(uint32_t index) ;
    
    /** 获取进度数据偏移
    @return 返回进度数据偏移
    */
    uint64_t GetProgressOffset() const ;
    
    /** 获取输入数据大小
    @return 返回输入数据大小
    */
    uint64_t GetInSize() const ;
    
     /** 获取输出数据大小
    @return 返回输出数据大小
    */
    uint64_t GetOutSize() const ;
    
    /** 获取是否自动发送进度
    @return 如果自动发送进度返回true, 否则返回false
    */
    bool IsSendRatio() const ;
    
    /** 获取在设置进度时自动回调已完成接口
    @return 如果在设置进度时自动回调已完成接口返回true, 否则返回false
    */
    bool IsSendProgress() const ;
    
    /** 设置进度数据偏移
    @param [in] value 进度数据偏移
    */
    void SetProgressOffset(uint64_t value) ;
    
    /** 设置输入数据大小
    @param [in] value 输入数据大小
    */
    void SetInSize(uint64_t value) ;
    
    /** 设置输出数据大小
    @param [in] value 输出数据大小
    */
    void SetOutSize(uint64_t value) ;
    
    /** 设置是否自动发送进度
    @param [in] value true表示是否自动发送进度
    */
    void SetSendRatio(bool value) ;
    
    /** 设置是否自动发送多线程进度
    @param [in] value true表示是否自动发送进度
    */
    void SetSendMtRatio(bool value) ;
    
    /** 设置是否在设置进度时自动回调已完成接口
    @param [in] value true在设置进度时自动回调已完成接口
    */
    void SetSendProgress(bool value) ;
    
    /** 设置压缩进度
    @param [in] inSize 输入数据大小
    @param [in] outSize 输出数据大小
    @return 成功返回RC_S_OK, 失败则返回错误码
    */
    virtual HResult SetRatioInfo(uint64_t inSize, uint64_t outSize) ;
    
private:
    
    /** 进度接口
    */
    IProgressPtr m_progress;
    
    /** 压缩进度接口
    */
    ICompressProgressInfoPtr m_ratioProgress;
    
    /** 多线程压缩解压比例进度信息
    */
    IMtCompressProgressInfoPtr m_spMtRatioProgress ;
    
    /** true表示按输入数据回调进度，否则按输出数据回调进度
    */
    bool m_inSizeIsMain ;
    
    /** 进度数据偏移
    */
    uint64_t m_progressOffset ;
    
    /** 输入数据大小
    */
    uint64_t m_inSize ;
    
    /** 输出数据大小
    */
    uint64_t m_outSize ;
    
    /** 是否自动发送进度
    */
    bool m_sendRatio ;
    
    /** 是否自动发送多线程进度
    */
    bool m_sendMtRatio ;
    
    /** 是否在设置进度时自动回调已完成接口
    */
    bool m_sendProgress ;
    
    /** 当前处理项的索引
    */
    uint32_t m_curIndex ;
};

END_NAMESPACE_RCZIP

#endif //__RCMtLocalProgress_h_
