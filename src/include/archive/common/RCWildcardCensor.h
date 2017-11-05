/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCWildcardCensor_h_
#define __RCWildcardCensor_h_ 1

#include "archive/common/RCWildcardPair.h"

BEGIN_NAMESPACE_RCZIP

/** 通配符匹配
*/
class RCWildcardCensor
{
public:

    /** 获取Pair容器
    @return 返回Pair容器引用
    */
    RCVector<RCWildcardPair>& GetPairs(void) ;
    
    /** 获取Pair容器
    @return 返回Pair容器
    */
    const RCVector<RCWildcardPair>& GetPairs(void) const ;
    
    /** 是否都为相对路径
    @return 是返回true,否则返回false
    */
    bool AllAreRelative() const ;
    
    /** 增加匹配目录
    @param [in] include 是否包含
    @param [in] path 路径名称
    @param [in] recursive 是否递归
    */
    void AddItem(bool include, const RCString& path, bool recursive) ;
    
    /** 检查路径是否匹配
    @param [in] path 路径名称
    @param [in] isFile 是否为文件
    @return 是返回true,否则返回false
    */
    bool CheckPath(const RCString& path, bool isFile) const ;
    
    /** 排除
    */
    void ExtendExclude() ;

private:
    
    /** 查找前缀
    @param [in] prefix 前缀
    @return 返回对应的序号
    */
    int32_t FindPrefix(const RCString& prefix) const ;
     
private:
    
    /** 匹配对容器
    */
    RCVector<RCWildcardPair> m_pairs ;
};

END_NAMESPACE_RCZIP

#endif //__RCWildcardCensor_h_
