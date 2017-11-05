/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCDirItem_h_
#define __RCDirItem_h_ 1

#include "base/RCString.h"
#include "base/RCWindowsDefs.h"
#include "filesystem/RCFileDefs.h"
#include "base/RCSmartPtr.h"

BEGIN_NAMESPACE_RCZIP

/** 目录项
*/
class RCDirItem
{
public:

    /** 默认构造函数
    */
    RCDirItem() ;
    
    /** 默认析构函数
    */
    ~RCDirItem() ;

private:
        
    /** 拷贝构造函数
    @param [in] rhs 拷贝参数
    */
    RCDirItem(const RCDirItem& rhs) ;
    
    /** 赋值操作符
    @param [in] rhs 复制参数
    */
    RCDirItem& operator= (const RCDirItem& rhs) ;
    
public:
    
    /** 设置文件大小
    @param [in] size 项大小
    */
    void SetSize(uint64_t size) ;
    
    /** 设置文件创建时间
    @param [in] cTime 创建时间
    */
    void SetCreateTime(const RC_FILE_TIME& cTime) ;
    
    /** 设置文件访问时间
    @param [in] aTime 访问时间
    */
    void SetAccessTime(const RC_FILE_TIME& aTime) ;
    
    /** 设置文件修改时间
    @param [in] mTime 修改时间
    */
    void SetModifyTime(const RC_FILE_TIME& mTime) ;
    
    /** 设置文件属性
    @param [in] attrib 项属性
    */
    void SetAttribuite( uint64_t attrib) ;
    
    /** 设置文件名
    @param [in] fileName 目录项名称
    */
    void SetFileName(const RCString& fileName) ;
    
    /** 设置物理父节点
    @param [in] phyParent 物理父节点
    */
    void SetPhyParent(int32_t phyParent) ;
    
    /** 设置逻辑父节点
    @param [in] logParent 逻辑父节点
    */
    void SetLogParent(int32_t logParent) ;
    
    /** 获取文件大小
    @return 返回文件大小
    */
    uint64_t GetSize(void) const ;
    
    /** 获取文件创建时间
    @return 返回创建时间
    */
    const RC_FILE_TIME& GetCreateTime(void) const ;
    
    /** 获取文件访问时间
    @return 返回访问时间
    */
    const RC_FILE_TIME& GetAccessTime(void) const ;
    
    /** 获取文件修改时间
    @return 返回修改时间
    */
    const RC_FILE_TIME& GetModifyTime(void) const ;
    
    /** 获取文件属性
    @return 返回属性
    */
    uint64_t GetAttribuite(void) const ;
    
    /** 获取文件名
    @return 返回文件名
    */
    const RCString& GetFileName(void) const ;
    
    /** 获取物理父节点
    @return 返回物理父节点
    */
    int32_t GetPhyParent(void) const ;
    
    /** 获取逻辑父节点
    @return 返回逻辑父节点
    */
    int32_t GetLogParent(void) const ;
    
    /** 判断是否为目录
    @return 是目录返回true,否则返回false
    */
    bool IsDir() const ;
    
private:
   
    /** 文件大小
    */
    uint64_t m_size ;
    
    /** 文件创建时间
    */
    RC_FILE_TIME m_cTime ;
    
    
    /** 文件访问时间
    */
    RC_FILE_TIME m_aTime ;
    
    /** 文件修改时间
    */
    RC_FILE_TIME m_mTime ;
    
    /** 文件属性
    */
    uint64_t m_attrib ;
    
    /** 文件名
    */
    RCString m_fileName ;
    
    /** 物理父节点
    */
    int32_t m_phyParent ;
    
    /** 逻辑父节点
    */
    int32_t m_logParent;
};

typedef RCSharedPtr<RCDirItem> RCDirItemPtr ; 

END_NAMESPACE_RCZIP

#endif //__RCDirItem_h_
