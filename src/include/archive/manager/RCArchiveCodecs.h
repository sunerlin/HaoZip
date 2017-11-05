/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCArchiveCodecs_h_
#define __RCArchiveCodecs_h_ 1

#include "coder/RCCodecsManager.h"
#include "archive/manager/RCArchiveInfoEx.h"
#include "interface/IArchive.h"
#include "common/RCVector.h"
#include "base/RCIntrusivePtr.h"
#include <map>

BEGIN_NAMESPACE_RCZIP

/** 管理文档格式和编解码信息
*/
class RCArchiveCodecs:
    public RCRefCounted
{
public:
    
    /** 文档格式容器
    */
    typedef std::map<RCArchiveID,RCArchiveInfoEx> TArchiveInfoExMap ;
    
public:

    /** 默认构造函数
    */
    RCArchiveCodecs() ;
    
    /** 默认析构函数
    */
    virtual ~RCArchiveCodecs() ;
    
public:
    
    /** 根据文件名查找对应的可以更新的文档信息，根据路径中的扩展名匹配文件格式中的主扩展名
    @param [in] arcPath 文件名
    @return 返回文档格式ID，如果失败返回 RC_ARCHIVE_UNKNOWN
    */
    RCArchiveID FindFormatForArchiveName(const RCString& arcPath) const ;
    
    /** 根据文件扩展名查找对应的文档信息
    @param [in] ext 文件扩展名,不包含"."
    @return 返回文档格式ID，如果失败返回 RC_ARCHIVE_UNKNOWN
    */
    RCArchiveID FindFormatForExtension(const RCString& ext) const ;
    
    /** 根据文档类型名称查找对应的文档信息
    @param [in] arcType 文档类型名称, 如"7z"等，不区分大小写
    @return 返回文档格式ID，如果失败返回 RC_ARCHIVE_UNKNOWN
    */
    RCArchiveID FindFormatForArchiveType(const RCString& arcType) const ;
    
    /** 根据文档类型名称查找对应的文档信息
    @param [in] arcType 文档类型名称,可以含有多个用"."分隔, 如"tar.gz"等，不区分大小写
    @param [out] formatIndices 返回文档格式ID数组，如果失败返回 RC_ARCHIVE_UNKNOWN
    @return 如果任何一个文档类型没有匹配，返回false，全部匹配返回true
    */
    bool FindFormatForArchiveType(const RCString& arcType, RCVector<RCArchiveID>& formatIndices) const;
    
    /** 获取编码解码器信息
    @return 返回编码解码器信息接口指针
    */
    ICompressCodecsInfo* GetCompressCodecs(void) const ;
    
    /** 创建输入文档接口
    @param [in] formatIndex 文档格式ID
    @param [out] archive 输入文档接口
    @return 成功返回RC_S_OK，否则返回错误码
    */
    HResult CreateInArchive(RCArchiveID formatIndex, IInArchivePtr& archive) const ;
    
    /** 创建输出文档接口
    @param [in] formatIndex 文档格式ID
    @param [out] archive 输出文档接口
    @return 成功返回RC_S_OK，否则返回错误码
    */
    HResult CreateOutArchive(RCArchiveID formatIndex, IOutArchivePtr& archive) const ;
    
    /** 根据文档格式名称查找支持输出的文件格式
    @param [in] name 文档格式名称,不区分大小写
    @return 文档格式ID
    */
    RCArchiveID FindOutFormatFromName(const RCString& name) const ;
    
    /** 获取文档信息数据
    @param [in] formatIndex 文档格式ID
    @return 返回文档信息数据, 如果ID不存在，返回空的文档信息数据
    */
    const RCArchiveInfoEx& GetArchiveInfoEx(RCArchiveID formatIndex) const ;
    
    /** 返回文档信息数据容器
    @return 返回容器引用
    */
    const TArchiveInfoExMap& GetArchiveInfoExMap(void) const ;
    
protected:
    
    /** 清空数据
    */
    void Clear(void) ;
    
    /** 增加新的文档格式
    @return 成功返回true,否则返回false
    */
    bool AddArchiveInfo(IArchiveInfoPtr spArchiveInfo) ;
       
protected:
    
    /** 压缩编解码管理器
    */
    RCCodecsManagerPtr m_spCodecs ;

private:       
    
    /** 文档格式管理容器
    */
    TArchiveInfoExMap m_archiveInfoMap ;
};

/** 智能指针定义
*/
typedef RCIntrusivePtr<RCArchiveCodecs> RCArchiveCodecsPtr ;

END_NAMESPACE_RCZIP

#endif //__RCArchiveCodecs_h_
