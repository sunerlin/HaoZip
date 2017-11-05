/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCMtExtractProgressMixer_h_
#define __RCMtExtractProgressMixer_h_ 1

#include "base/RCNonCopyable.h"
#include "interface/ICoder.h"
#include "thread/RCMutex.h"
#include "common/RCVector.h"

BEGIN_NAMESPACE_RCZIP

class RCMtLocalProgress ;

class RCMtExtractProgressMixer:
    private RCNonCopyable
{
public:

    /** 默认构造函数
    */
    RCMtExtractProgressMixer() ;
    
    /** 默认析构函数
    */
    ~RCMtExtractProgressMixer() ;

public:
    
    /** 初始化
    @param [in] numItems 进度项目个数
    @param [in] progress 进度接口
    */
    void Init(uint32_t numItems, RCMtLocalProgress* progress);
    
    /** 对进度项重新初始化
    @param [in] index 进度项序号
    */
    void Reinit(uint32_t index);
    
    /** 设置进度
    @param [in] index 进度项序号
    @param [in] inSize 输入数据大小
    @param [in] outSize 输出数据大小
    @return 成功返回RC_S_OK, 失败则返回错误码
    */
    HResult SetRatioInfo(uint32_t index, uint64_t inSize, uint64_t outSize);
    
    /** 设置输入数据大小
    @param [in] value 输入数据大小值
    */
    void SetInSize(uint64_t value) ;
    
    /** 设置输出数据大小
    @param [in] value 输出数据大小值
    */
    void SetOutSize(uint64_t value) ;
    
    /** 以当前数据回调进度
    @return 成功返回RC_S_OK, 失败则返回错误码
    */
    HResult SetCur() ;
    
private:
    
    /** 解压进度管理接口
    */
    RCMtLocalProgress* m_progressSpec ;
    
    /** 解压进度接口
    */
    ICompressProgressInfoPtr m_progress ;
    
    /** 输入数据大小列表
    */
    RCVector<uint64_t> m_inSizes ;
    
    /** 输出数据大小列表
    */
    RCVector<uint64_t> m_outSizes ;
    
    /** 输入数据总大小
    */
    uint64_t m_totalInSize ;
    
    /** 输出数据总大小
    */
    uint64_t m_totalOutSize ;
    
    /** 多线程同步锁
    */
    RCMutex m_lock ;
};

END_NAMESPACE_RCZIP

#endif //__RCMtExtractProgressMixer_h_
