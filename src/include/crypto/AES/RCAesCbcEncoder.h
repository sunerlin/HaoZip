/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCAesCbcEncoder_h_
#define __RCAesCbcEncoder_h_ 1

#include "interface/ICoder.h"
#include "interface/IUnknownImpl.h"

BEGIN_NAMESPACE_RCZIP

/** Aes Cbs 编码器
*/
class RCAesCbcEncoder:
    public IUnknownImpl2<ICompressFilter,
                         ICryptoProperties
                        >
{
public:

    /** 默认构造函数
    */
    RCAesCbcEncoder() ;
    
    /** 默认析构函数
    */
    ~RCAesCbcEncoder() ;

public:

    /** 初始化
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult Init() ;
    
    /** 过滤数据
    @param [in,out] data 数据缓冲区
    @param [in] size 数据长度
    @return 实际处理数据的长度
    */
    virtual uint32_t Filter(byte_t* data, uint32_t size) ;
    
    /** 设置加密的键数据
    @param [in] data 数据缓冲区
    @param [in] size 数据长度
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult SetKey(const byte_t* data, uint32_t size) ;
    
    /** 设置加密初始化数据
    @param [in] data 数据缓冲区
    @param [in] size 数据长度
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult SetInitVector(const byte_t *data, uint32_t size) ;

private:

    /** 结构 TAesCbcImpl
    */
    struct TAesCbcImpl ;

    /** 实现
    */
    TAesCbcImpl* m_impl ;
};

END_NAMESPACE_RCZIP

#endif //__RCAesCbcEncoder_h_
