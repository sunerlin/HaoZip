/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCMtCompressProgress_h_
#define __RCMtCompressProgress_h_ 1

#include "interface/ICoder.h"
#include "interface/IUnknownImpl.h"

BEGIN_NAMESPACE_RCZIP

class RCMtCompressProgressMixer ;

class RCMtCompressProgress:
    public IUnknownImpl<ICompressProgressInfo>
{
public:

    /** 默认构造函数
    */
    RCMtCompressProgress() ;
    
    /** 默认析构函数
    */
    ~RCMtCompressProgress() ;
    
public:
    
    /** 初始化
    @param [in] progress 压缩进度接口
    @param [in] index 线程序号
    */
    void Init(RCMtCompressProgressMixer* progress, uint32_t index) ;
    
    /** 使用当前值重新初始化
    */
    void Reinit() ;
    
    /** 设置正在压缩的文件编号
    @param [in] itemIndex 正在压缩的文件编号
    */
    void SetItemIndex(uint32_t itemIndex) ;
    
    /** 设置压缩进度
    @param [in] inSize 输入数据大小
    @param [in] outSize 输出数据大小
    @return 成功返回RC_S_OK, 失败则返回错误码
    */
    virtual HResult SetRatioInfo(uint64_t inSize, uint64_t outSize) ;
    
private:
    
    /** 压缩进度接口
    */
    RCMtCompressProgressMixer* m_progress ;
    
    /** 线程号
    */
    uint32_t m_index ;
    
    /** 正在压缩的文件编号
    */
    uint32_t m_itemIndex ;
};

END_NAMESPACE_RCZIP

#endif //__RCMtCompressProgress_h_
