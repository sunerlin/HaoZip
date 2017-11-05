/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCDirItems_h_
#define __RCDirItems_h_ 1

#include "archive/common/RCDirItem.h"
#include "base/RCNonCopyable.h"
#include "common/RCVector.h"
#include "base/RCString.h"
#include "filesystem/RCFileInfo.h"

BEGIN_NAMESPACE_RCZIP

/** 目录项集合
*/
class RCDirItems:
    private RCNonCopyable
{
public:

    /** 默认构造函数
    */
    RCDirItems() ;
    
    /** 默认析构函数
    */
    ~RCDirItems() ;
    
public:
    
    /** 通配符
    */
    static const RCString::value_type s_kAnyStringWildcard;
        
    /** 目录分割符
    */
    static const RCString::value_type s_kDirDelimiter;

    /** 添加目录项
    @param [in] phyParent 物理父节点
    @param [in] logParent 逻辑父节点
    @param [in] fi 文件信息
    @param [out] dirItems 目录项集合
    */
    static void AddDirFileInfo(int32_t phyParent, 
                               int32_t logParent, 
                               const RCFileInfo& fi, 
                               RCVector<RCDirItemPtr>& dirItems);

        
    /** 判断文件是否可读
    @param [in] fileName 文件名
    @param [out] errorCode 错误号
    @return 可读返回true,否则返回false
    */
    static bool IsReadableFile(const RCString& fileName, uint64_t& errorCode) ;
    
public:
    
    /** 返回目录项集合
    @return 返回目录项集合
    */
    const RCVector<RCDirItemPtr>& GetItems() const ;
    
    /** 返回目录项集合引用
    @return 返回目录项集合引用
    */
    RCVector<RCDirItemPtr>& GetItems() ;

    /** 返回目录数量
    @return 返回目录数量
    */
    int32_t GetNumFolders() const;
    
    /** 返回物理路径
    @param [in] index 序号
    @return 返回该序号的物理路径
    */
    RCString GetPhyPath(int32_t index) const;
    
    /** 返回逻辑路径
    @param [in] index 序号
    @return 返回该序号的逻辑路径
    */
    RCString GetLogPath(int32_t index) const;

    /** 增加前缀
    @param [in] phyParent 物理父节点
    @param [in] logParent 逻辑父节点
    @param [in] prefix 前缀
    @return 返回添加编号
    */
    int32_t AddPrefix(int32_t phyParent, int32_t logParent, const RCString &prefix);

    /** 删除最后一个前缀
    */
    void DeleteLastPrefix();

    /** 枚举目录
    @param [in] phyParent 物理父节点
    @param [in] logParent 逻辑父节点
    @param [in] phyPrefix 物理前缀
    @param [out] errorPaths 错误路径
    @param [out] errorCodes 错误编号
    */
    void EnumerateDirectory(int32_t phyParent,
                            int32_t logParent, 
                            const RCString& phyPrefix,
                            RCVector<RCString>& errorPaths, 
                            RCVector<uint64_t>& errorCodes) ;

    /** 枚举目录项
    @param [in] phyPrefix 物理前缀
    @param [in] logPrefix 逻辑前缀
    @param [in] filePaths 文件列表
    @param [out] errorPaths 错误路径列表
    @param [out] errorCodes 错误编号列表
    */
    void EnumerateDirItems2(const RCString& phyPrefix, 
                            const RCString& logPrefix,
                            const RCVector<RCString>& filePaths,
                            RCVector<RCString>& errorPaths, 
                            std::vector<uint64_t>& errorCodes);

    /** 反向排序
    */
    void ReserveDown();

    /** 获取文件的修改时间
    @param [in] index 项序号
    @return 返回修改时间
    */
    RC_FILE_TIME GetModifyTime(int32_t index) const ;
    
private:
    
    /** 返回前缀路径
    @param [in] parents 父节点列表
    @param [in] index  序号
    @param [in] name 名称
    @return 返回前缀路径
    */
    RCString GetPrefixesPath(const RCIntVector& parents, int32_t index, const RCString &name) const;

private:
    
    /** 前缀列表
    */
    RCVector<RCString> m_prefixes ;
    
    /** 物理父节点列表
    */
    RCIntVector m_phyParents ;
    
    /** 逻辑父节点列表
    */
    RCIntVector m_logParents ;    
    
    /** 目录项列表
    */
    RCVector<RCDirItemPtr> m_items ;
};

END_NAMESPACE_RCZIP

#endif //__RCDirItems_h_
