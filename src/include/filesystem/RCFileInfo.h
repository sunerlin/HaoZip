/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCFileInfo_h_
#define __RCFileInfo_h_ 1

#include "base/RCString.h"
#include "filesystem/RCFileDefs.h"

BEGIN_NAMESPACE_RCZIP

class RCFileInfo
{
public:

    /** 默认构造函数
    */
    RCFileInfo() ;
    
    /** 默认析构函数
    */
    ~RCFileInfo() ;
    
public:
    
    /** 设置文件大小
    @param [in] size 文件大小值
    */
    void SetSize(uint64_t size) ;
    
    /** 设置文件创建时间
    @param [in] cTime 文件创建时间值
    */
    void SetCreateTime(const RC_FILE_TIME& cTime) ;
    
    /** 设置文件访问时间
    @param [in] aTime 文件访问时间值
    */
    void SetAccessTime(const RC_FILE_TIME& aTime) ;
    
    /** 设置文件修改时间
    @param [in] mTime 文件修改时间值
    */
    void SetModifyTime(const RC_FILE_TIME& mTime) ;
    
    /** 设置文件属性
    @param [in] attrib 文件属性值
    */
    void SetAttribuite( uint64_t attrib) ;
    
    /** 设置文件名
    @param [in] fileName 文件名，不含路径
    */
    void SetFileName(const RCString& fileName) ;
    
    /** 获取文件大小
    @return 返回文件大小
    */
    uint64_t GetSize(void) const ;
    
    /** 获取文件创建时间
    @return 返回文件创建时间
    */
    const RC_FILE_TIME& GetCreateTime(void) const ;
    
    /** 获取文件访问时间
    @return 返回文件访问时间
    */
    const RC_FILE_TIME& GetAccessTime(void) const ;
    
    /** 获取文件修改时间
    @return 返回文件修改时间
    */
    const RC_FILE_TIME& GetModifyTime(void) const ;
    
    /** 获取文件属性
    @return 返回文件属性
    */
    uint64_t GetAttribuite(void) const ;
    
    /** 获取文件名
    @return 返回文件名
    */
    const RCString& GetFileName(void) const ;
    
    /** 是否具有归档属性
    @return 如果为归档属性，返回true, 否则返回false
    */
    bool IsArchived() const ;
    
    /** 是否具有压缩属性
    @return 如果为压缩属性，返回true, 否则返回false
    */
    bool IsCompressed() const ;
    
    /** 是否为目录
    @return 如果为目录，返回true, 否则返回false
    */
    bool IsDir() const ;
    
    /** 是否为加密属性
    @return 如果为加密属性，返回true, 否则返回false
    */
    bool IsEncrypted() ;
    
    /** 是否为隐藏属性
    @return 如果为隐藏属性，返回true, 否则返回false
    */
    bool IsHidden() const ;
    
    /** 是否为Normal属性
    @return 如果为Normal属性，返回true, 否则返回false
    */
    bool IsNormal() const ;
    
    /** 是否为只读属性
    @return 如果为只读属性，返回true, 否则返回false
    */
    bool IsReadOnly() const ;
    
    /** 是否为系统属性
    @return 如果为系统属性，返回true, 否则返回false
    */
    bool IsSystem() const ;
    
    /** 是否为临时属性
    @return 如果为临时属性，返回true, 否则返回false
    */
    bool IsTemporary() const ;
    
    /** 是否为点目录
    @return 如果为"." 或者 ".."，返回true, 否则返回false
    */
    bool IsDots() const;
    
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
};

END_NAMESPACE_RCZIP

#endif //__RCFileInfo_h_
