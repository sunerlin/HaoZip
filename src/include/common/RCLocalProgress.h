/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCLocalProgress_h_
#define __RCLocalProgress_h_ 1

#include "interface/ICoder.h"
#include "interface/IProgress.h"
#include "interface/IUnknownImpl.h"

BEGIN_NAMESPACE_RCZIP

class RCLocalProgress:
    public IUnknownImpl<ICompressProgressInfo>
{
public:

    /** 默认构造函数
    */
    RCLocalProgress() ;
    
    /** 默认析构函数
    */
    ~RCLocalProgress() ;
    
    /** 初始化
    @param [in] progress 进度接口
    @param [in] inSizeIsMain 是否以输入大小计算进度
    */
    void Init(IProgress* progress, bool inSizeIsMain) ;
    
    /** 更新当前处理项的索引并更新回调进度
    @return 成功返回RC_S_OK, 否则返回错误码
    */
    HResult SetCur() ;
    
    /** 设置当前处理的下标值
    @param [in] index 当前下标值
    */
    void SetIndex(uint32_t index) ;
    
    /** 获取进度偏移
    @return 返回进度偏移值
    */
    uint64_t GetProgressOffset() const ;
    
    /** 获取输入大小
    @return 返回输入大小
    */
    uint64_t GetInSize() const ;
    
    /** 获取输出大小
    @return 返回输出大小
    */
    uint64_t GetOutSize() const ;
    
    /** 是否自动触发进度回调
    @return 如果自动触发进度回调返回true, 否则返回false
    */
    bool IsSendRatio() const ;
    
    /** 是否自动触发已完成回调
    @return 如果自动触发已完成回调返回true, 否则返回false
    */
    bool IsSendProgress() const ;
    
    /** 设置当前进度偏移
    @param [in] value 当前进度偏移值
    */
    void SetProgressOffset(uint64_t value) ;
    
    /** 设置输入大小
    @param [in] value 输入大小值
    */
    void SetInSize(uint64_t value) ;
    
    /** 设置输出大小
    @param [in] value 输出大小值
    */
    void SetOutSize(uint64_t value) ;
    
    /** 是否自动触发进度回调
    @param [in] value 是否自动触发进度回调属性值
    */
    void SetSendRatio(bool value) ;
    
    /** 是否自动触发已完成回调
    @param [in] value 是否自动触发已完成回调属性值
    */
    void SetSendProgress(bool value) ;
    
    /** 设置压缩进度
    @param [in] inSize 输入数据大小
    @param [in] outSize 输出数据大小
    @return 成功返回RC_S_OK, 否则返回错误码
    */
    virtual HResult SetRatioInfo(uint64_t inSize, uint64_t outSize) ;
    
private:
    
    /** 压缩整体进度信息
    */
    IProgressPtr m_spProgress;
    
    /** 压缩解压比例进度信息
    */
    ICompressProgressInfoPtr m_spRatioProgress ;
    
    /** 多线程压缩解压比例进度信息
    */
    IMtCompressProgressInfoPtr m_spMtRatioProgress ;
    
    /** 是否使用inSize作为进度回调
    */
    bool m_inSizeIsMain ;
    
    /** 进度偏移
    */
    uint64_t m_progressOffset ;
    
    /** 输入大小
    */
    uint64_t m_inSize ;
    
    /** 输出大小
    */
    uint64_t m_outSize ;
    
    /** 是否发送完成比例信息
    */
    bool m_sendRatio ;
    
    /** 是否发送整体进度信息
    */
    bool m_sendProgress ;
    
    /** 当前处理项的索引
    */
    uint32_t m_curIndex ;
};

END_NAMESPACE_RCZIP

#endif //__RCLocalProgress_h_
