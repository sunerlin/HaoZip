/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCXml_h_
#define __RCXml_h_ 1

#include "base/RCNonCopyable.h"
#include "base/RCString.h"
#include "common/RCVector.h"

BEGIN_NAMESPACE_RCZIP

/** XML 节点属性
*/
struct RCXmlProp
{
    /** 属性名称
    */
    RCStringA m_name ;
    
    /** 属性值
    */
    RCStringA m_value ;
};

/** XML节点类
*/
class RCXmlItem
{
public:
    
    /** 默认构造函数
    */
    RCXmlItem() ;
    
    /** 默认析构函数
    */
    ~RCXmlItem() ;
    
public:

    /** 解析XML文件内容
    @param [in] text XML文本内容
    @param [in,out] pos  文本解析开始偏移量
    @param [in] numAllowedLevels 解析的XML文件节点层次
    @return 成功返回true, 否则返回false
    */
    bool ParseItem(const RCStringA& text, int32_t& pos, int32_t numAllowedLevels) ;
  
    /** 判断是否为指定节点
    @param [in] tag 节点名称
    @return 如果为有效节点名称，返回true, 否则返回false
    */
    bool IsTagged(const RCStringA& tag) const ;
    
    /** 查找属性
    @param [in] propName 属性名称
    @return 返回属性的下标值，如果失败返回 -1
    */
    int32_t FindProperty(const RCStringA& propName) const ;
    
    /** 获取属性值
    @param [in] propName 属性名称
    @return 返回属性的值，如果失败返回空串
    */
    RCStringA GetPropertyValue(const RCStringA& propName) const ;
    
    /** 获取子串
    @return 返回字串名称，如果有多个子节点，返回空串
    */
    RCStringA GetSubString() const ;
    
    /** 查找子节点
    @param [in] tag 子节点的名称
    @return 返回子节点的下标值
    */
    int32_t FindSubTag(const RCStringA& tag) const;
    
    /** 根据节点名查找字串
    @param [in] tag 子节点的名称
    @return 返回字串名称，失败则返回空串
    */
    RCStringA GetSubStringForTag(const RCStringA& tag) const ;
    
    /** 获取名称
    @return 返回节点名称
    */
    const RCStringA& GetName() const ;
    
    /** 判断是否为Tag
    @return 如果为Tag返回true, 否则返回false
    */
    bool IsTag() const ;
    
    /** 获取属性
    @return 属性值列表
    */
    const RCVector<RCXmlProp>& GetProps() const ;
    
    /** 获取子节点
    @return 子节点值列表
    */
    const RCVector<RCXmlItem>& GetSubItems() const ;
        
    /** 是否为空格字符
    @param [in] c 需要判断的字符
    @return 如果为空格字符返回true, 否则返回false
    */
    static bool IsSpaceChar(char c) ;
  
private:
    
    /** 解析
    @param [in] s XML文本内容
    @param [in,out] pos  文本解析开始偏移量
    @param [in] numAllowedLevels 解析的XML文件节点层次
    @return 成功返回true, 否则返回false
    */
    bool ParseItems(const RCStringA& s, int32_t& pos, int32_t numAllowedLevels) ;
    
    /** 是否为有效字符
    @param [in] c 需要判断的字符
    @return 如果为有效字符返回true, 否则返回false
    */
    static bool IsValidChar(char c) ;  
    
    /** 解析并读取属性值
    @param [in] s XML文本内容
    @param [in,out] pos  文本解析开始偏移量
    @param [out] prop 返回属性值
    @return 成功返回true, 否则返回false
    */
    static bool ReadProperty(const RCStringA& s, int32_t& pos, RCXmlProp& prop) ;
    
private:
    
    /** 名称
    */
    RCStringA m_name ;
    
    /** Tag标记
    */
    bool m_isTag ;
    
    /** 属性
    */
    RCVector<RCXmlProp> m_props ;
    
    /** 子节点
    */
    RCVector<RCXmlItem> m_subItems ;
};

/** XML文件内容解析类
*/
class RCXml:
    private RCNonCopyable
{
public:

    /** 默认构造函数
    */
    RCXml() ;
    
    /** 默认析构函数
    */
    ~RCXml() ;
    
    /** 解析文件文本内容
    @param [in] text XML格式的文本内容
    @return 成功返回true, 失败返回false
    */
    bool Parse(const RCStringA& text) ;
    
    /** 获取根节点
    @return 返回根节点
    */
    const RCXmlItem& GetRoot() const ;
    
private:
    
    /** 跳过XML文件头
    @param [in] s XML文本
    @param [in,out] pos 文本解析开始偏移量
    @param [in] startString 开始字符串
    @param [in] endString 结束字符串
    @return 成功返回true, 否则返回false
    */
    bool SkipHeader(const RCStringA& s, 
                    int32_t& pos,
                    const RCStringA& startString, 
                    const RCStringA& endString) ;
    
private:
    
    /** 根节点
    */
    RCXmlItem m_root ;
};

END_NAMESPACE_RCZIP

#endif //__RCXml_h_
