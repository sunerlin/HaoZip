/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCCodecsManager_h_
#define __RCCodecsManager_h_ 1

#include "interface/ICoder.h"
#include "interface/IUnknownImpl.h"
#include <vector>

BEGIN_NAMESPACE_RCZIP

/** 编解码管理类 
*/
class RCCodecsManager:
    public IUnknownImpl<ICompressCodecsInfo>
{
public:
    
    /** 编解码容器类型
    */
    typedef std::vector<ICodecInfoPtr> container_type ;

public:

    /** 默认构造函数
    */
    RCCodecsManager() ;
    
    /** 默认析构函数
    */
    virtual ~RCCodecsManager() ;
   
public:
    
    /** 获取压缩编码解码器个数
    @param [out] numMethods 返回编码解码器个数
    @return 如果成功则返回RC_S_OK，否则返回错误号
    */
    virtual HResult GetNumberOfMethods(uint32_t& numMethods) const ;

    /** 获取编码解码信息
    @param [in] index 编码解码器下标，从0开始
    @param [out] spCodecInfo 编码解码器信息
    @return 成功返回RC_S_OK，否则返回错误号 
    */
    virtual HResult GetMethod(uint32_t index, ICodecInfoPtr& spCodecInfo) const ;

    /** 创建解码器
    @param [in] index 编码器下标，从0开始
    @param [in] iid 解码器的接口ID
    @param [out] coder 解码器的接口指针，引用计数加1
    @return 成功返回RC_S_OK，否则返回错误号 
    */
    virtual HResult CreateDecoder(uint32_t index, RC_IID iid, void** coder) const ;

    /** 创建编码器
    @param [in] index 编码器下标，从0开始
    @param [in] iid 编码器的接口ID
    @param [out] coder 编码器的接口指针，引用计数加1
    @return 成功返回RC_S_OK，否则返回错误号 
    */
    virtual HResult CreateEncoder(uint32_t index, RC_IID iid, void** coder) const ;

public:
    
    /** 取得容器内容
    @return 返回编解码容器的引用
    */
    const container_type& GetCodecArray(void) const ;
    
protected:
    
    /** 注册编码器
    @param [in] spCodecInfo 需要注册的编解码信息
    */    
    bool RegisterCodec(const ICodecInfoPtr& spCodecInfo) ;
    
    /** 创建编码器/解码器
    @param [in] index 编码器下标，从0开始
    @param [in] iid 编码器的接口ID
    @param [in] isEncoder 如果是true,创建编码器，否则创建解码器
    @param [out] coder 编码器的接口指针，引用计数加1
    @return 成功返回RC_S_OK，否则返回错误号 
    */
    HResult DoCreateCoder(uint32_t index, RC_IID iid, bool isEncoder, void** coder) const ;
    
private:
    
    /** 编码解码容器
    */
    container_type m_codecArray ;
};

/** 智能指针定义
*/
typedef RCComPtr<RCCodecsManager> RCCodecsManagerPtr ;

END_NAMESPACE_RCZIP

#endif //__RCCodecsManager_h_
