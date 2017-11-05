/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCGZipDeflateProps_h_
#define __RCGZipDeflateProps_h_ 1

#include "interface/ICoder.h"
#include "common/RCProperty.h"

BEGIN_NAMESPACE_RCZIP

class RCGZipDeflateProps
{
public:

    /** 默认构造函数
    */
    RCGZipDeflateProps() ;
    
    /** 是否为最好压缩模式
    @return 是否是最好压缩模式
    */
    bool IsMaximum() const ;

    /** 将编码属性应用到编码接口
    @param [in] setCoderProperties 需要应用属性的编码接口
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult SetCoderProperties(ICompressSetCoderProperties* setCoderProperties) ;
    
    /** 设置编码属性
    @param [in] propertyArray 编码属性数据
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult SetProperties(const RCPropertyNamePairArray& propertyArray);

private:

    /** 初始化属性
    */ 
    void Init() ;
    
    /** 初始化属性为默认值
    */
    void Normalize() ;
    
private:

    /** 等级
    */
    uint32_t m_level ;

    /** passes
    */
    uint32_t m_numPasses ;

    /** fb
    */
    uint32_t m_fb ;

    /** algo
    */
    uint32_t m_algo ;

    /** mc
    */
    uint32_t m_mc ;

    /** 是否定义了mc
    */
    bool m_mcDefined ;
};

END_NAMESPACE_RCZIP

#endif //__RCGZipDeflateProps_h_
