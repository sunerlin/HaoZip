/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCMtExtractProgress_h_
#define __RCMtExtractProgress_h_ 1

#include "interface/ICoder.h"
#include "interface/IUnknownImpl.h"

BEGIN_NAMESPACE_RCZIP

class RCMtExtractProgressMixer ;

class RCMtExtractProgress:
    public IUnknownImpl<ICompressProgressInfo>
{
public:

    /** 默认构造函数
    */
    RCMtExtractProgress() ;
    
    /** 默认析构函数
    */
    ~RCMtExtractProgress() ;
    
public:
    
    /** 初始化
    @param [in] progress 线程安全的解压进度接口
    @param [in] index 当前文件序号
    */
    void Init(RCMtExtractProgressMixer* progress, uint32_t index) ;
    
    /** 重新初始化
    */
    void Reinit() ;
    
    /** 设置压缩进度
    @param [in] inSize 输入数据大小
    @param [in] outSize 输出数据大小
    @return 成功返回RC_S_OK, 失败则返回错误码
    */
    virtual HResult SetRatioInfo(uint64_t inSize, uint64_t outSize) ;
    
private:
    
    /** 解压进度接口
    */
    RCMtExtractProgressMixer* m_progress ;
    
    /** 处理的文件序号
    */
    uint32_t m_index ;
};

END_NAMESPACE_RCZIP

#endif //__RCMtExtractProgress_h_
