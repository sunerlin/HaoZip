/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCParseProperties_h_
#define __RCParseProperties_h_ 1

#include "base/RCString.h"
#include "common/RCVariant.h"

BEGIN_NAMESPACE_RCZIP

class RCParseProperties
{
public:

    /** 转换string到uint32
    @param [in] srcString 字符串
    @param [out] number 返回数值
    @return 返回解析string中的几个字节
    */
    static int32_t ParseStringToUInt32(const RCString& srcString, uint32_t& number) ;

    /** 解析属性值
    @param [in] name 属性名
    @param [in] prop 属性
    @param [out] resValue 返回属性值
    @return 成功返回RC_S_OK，否则返回错误号
    */
    static HResult ParsePropValue(const RCString& name, const RCVariant& prop, uint32_t& resValue);

    /** 解析属性字典大小
    @param [in] srcStringSpec 属性名
    @param [out] dicSize 返回字典大小
    @return 成功返回RC_S_OK，否则返回错误号
    */
    static HResult ParsePropDictionaryValue(const RCString& srcStringSpec, uint32_t& dicSize) ;

    /** 解析属性字典大小
    @param [in] name 属性名
    @param [in] prop 属性
    @param [out] resValue 返回字典大小
    @return 成功返回RC_S_OK，否则返回错误号
    */
    static HResult ParsePropDictionaryValue(const RCString& name, const RCVariant& prop, uint32_t& resValue);

    /** 转换string到bool
    @param [in] s 字符串
    @param [out] res bool数值
    @return 成功转换返回true，否则返回false
    */
    static bool StringToBool(const RCString& s, bool& res) ;

    /** 设置bool属性
    @param [out] dest 返回bool属性
    @param [in] value 值
    @return 成功返回RC_S_OK，否则返回错误号
    */
    static HResult SetBoolProperty(bool& dest, const RCVariant& value) ;

    /** 解析多线程属性
    @param [in] name 属性名
    @param [in] prop 属性
    @param [in] defaultNumThreads 默认线程数
    @param [out] numThreads 线程数
    @return 成功返回RC_S_OK，否则返回错误号
    */
    static HResult ParseMtProp(const RCString& name, const RCVariant& prop, uint32_t defaultNumThreads, uint32_t& numThreads) ;
};

END_NAMESPACE_RCZIP

#endif //__RCParseProperties_h_
