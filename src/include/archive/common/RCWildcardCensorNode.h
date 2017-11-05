/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCWildcardCensorNode_h_
#define __RCWildcardCensorNode_h_ 1

#include "archive/common/RCWildcardItem.h"

BEGIN_NAMESPACE_RCZIP

/** 匹配节点
*/
class RCWildcardCensorNode
{
public:

    /** 默认构造函数
    */
    RCWildcardCensorNode() ;
    
    /** 构造函数
    @param [in] name 名称
    @param [in] parent 父节点
    */
    RCWildcardCensorNode(const RCString& name, RCWildcardCensorNode* parent) ;
    
    /** 默认析构函数
    */
    ~RCWildcardCensorNode() ;
    
public:
    
    /** 查找子节点
    @param [in] name 路径名
    @return 返回子节点的下标, 如果查找不到返回-1
    */
    int32_t FindSubNode(const RCString& name) const ;
    
    /** 增加节点
    @param [in] include 是否包含
    @param [in] item 增加项
    */
    void AddItem(bool include, RCWildcardItem& item) ;
    
    /** 增加节点
    @param [in] include 是否包含
    @param [in] path 路径
    @param [in] recursive 是否递归
    @param [in] forFile 包括文件
    @param [in] forDir 包括目录
    */
    void AddItem(bool include, const RCString& path, bool recursive, bool forFile, bool forDir);
    
    /** 增加节点
    @param [in] include 是否包含
    @param [in] path 路径
    @param [in] recursive 是否递归
    */
    void AddItem2(bool include, const RCString& path, bool recursive) ;
    
    /** 是否需要检查子目录
    @return 需要检查返回true,否则返回false
    */
    bool NeedCheckSubDirs() const ;

    /** 是否包含子项
    @return 包含子项返回true,否则返回false
    */
    bool AreThereIncludeItems() const ;
    
    /** 检查路径
    @param [in] path 路径
    @param [in] isFile 是否为文件
    @param [out] include 是否包括
    @return 匹配返回true,否则返回false
    */
    bool CheckPath(const RCString& path, bool isFile, bool& include) const ;
    
    /** 检查路径
    @param [in] path 路径
    @param [in] isFile 是否为文件
    @return 匹配返回true,否则返回false
    */
    bool CheckPath(const RCString& path, bool isFile) const ;
    
    /** 检查路径一只到根路径
    @param [in] include 是否包含
    @param [in,out] pathParts 各级路径列表
    @param [in] isFile 是否为文件
    @return 匹配返回true,否则返回false
    */
    bool CheckPathToRoot(bool include, RCVector<RCString>& pathParts, bool isFile) const ;
    
    /** 排除
    @param [in] fromNodes 待排除的节点
    */
    void ExtendExclude(const RCWildcardCensorNode& fromNodes) ;
    
    /** 设置父节点
    @param [in] parent 父节点
    */
    void SetParent(RCWildcardCensorNode* parent) ;
    
    /** 返回父节点
    @return 返回父节点指针
    */
    RCWildcardCensorNode* GetParent(void) const ;
    
    /** 返回名称
    @return 返回名称字符串
    */
    const RCString& GetName() const ;
    
    /** 设置名称
    @param [in] name 名称
    */
    void SetName(const RCString& name ) ;
    
    /** 返回子节点列表
    @return 返回子节点列表引用
    */
    RCVector<RCWildcardCensorNode>& GetSubNodes() ;
    
    /** 返回子节点列表
    @return 返回子节点列表
    */
    const RCVector<RCWildcardCensorNode>& GetSubNodes() const ;
    
    /** 返回包含的项
    @return 返回包含的项列表引用
    */
    RCVector<RCWildcardItem>& GetIncludeItems() ;
    
    /** 返回包含的项
    @return 返回包含的项列表
    */
    const RCVector<RCWildcardItem>& GetIncludeItems() const ;
    
    /** 返回排除的项列表
    @return 返回排除的项列表引用
    */
    RCVector<RCWildcardItem>& GetExcludeItems() ;
    
    /** 返回排除项
    @return 返回排除项列表
    */
    const RCVector<RCWildcardItem>& GetExcludeItems() const ;
    
private:
    
    /** 检查当前路径
    @param [in] include 是否包含
    @param [in] pathParts 各级路径列表
    @param [in] isFile 是否是文件
    @return 匹配返回true,否则返回false
    */
    bool CheckPathCurrent(bool include, const RCVector<RCString>& pathParts, bool isFile) const ;
    
    /** 增加匹配项
    @param [in] include 是否包含
    @param [out] item 返回增加的项
    */
    void AddItemSimple(bool include, RCWildcardItem& item) ;
    
    /** 检查路径
    @param [in,out] pathParts 各级路径列表
    @param [in] isFile 是否是文件
    @param [out] include 是否包含
    @return 匹配返回true,否则返回false
    */
    bool CheckPath(RCVector<RCString>& pathParts, bool isFile, bool& include) const ;
    
private:
    
    /** 父节点
    */
    RCWildcardCensorNode* m_parent ;
    
    /** 名称
    */
    RCString m_name;
    
    /** 子节点列表
    */
    RCVector<RCWildcardCensorNode> m_subNodes;
    
    /** 包含的项列表
    */
    RCVector<RCWildcardItem> m_includeItems;
    
    /** 排除的项列表
    */
    RCVector<RCWildcardItem> m_excludeItems;
};

END_NAMESPACE_RCZIP

#endif //__RCWildcardCensorNode_h_
