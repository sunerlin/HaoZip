/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCWildcardPair_h_
#define __RCWildcardPair_h_ 1

#include "archive/common/RCWildcardCensorNode.h"

BEGIN_NAMESPACE_RCZIP

/** 通配符匹配对
*/
class RCWildcardPair
{
public:

    /** 构造函数
    @param [in] prefix 前缀
    */
    explicit RCWildcardPair(const RCString& prefix) ;

    /** 获取prefix
    @return 返回前缀引用
    */
    RCString& GetPrefix(void) ;
    
    /** 获取prefix
    @return 返回前缀
    */
    const RCString& GetPrefix(void) const ;
    
    /** 获取head
    @return 返回节点引用
    */
    RCWildcardCensorNode& GetHead(void) ;
    
    /** 获取head
    @return 返回通配节点
    */
    const RCWildcardCensorNode& GetHead(void) const ;
    
private:
    
    /** 前缀
    */
    RCString m_prefix ;
    
    /** 通配节点
    */
    RCWildcardCensorNode m_head ;
};

END_NAMESPACE_RCZIP

#endif //__RCWildcardPair_h_
