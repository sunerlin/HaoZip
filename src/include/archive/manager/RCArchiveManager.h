/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCArchiveManager_h_
#define __RCArchiveManager_h_ 1

#include "interface/IArchiveInfo.h"
#include "interface/IArchive.h"
#include "interface/IUnknownImpl.h"
#include <vector>

BEGIN_NAMESPACE_RCZIP

/** 创建输入文档接口的函数声明 
*/
typedef IInArchive * (*CreateInArchiveP)();

/** 创建输出文档接口的函数声明 
*/
typedef IOutArchive * (*CreateOutArchiveP)();

/** 文档格式管理器 
*/
class RCArchiveManager:
    public IUnknownImpl<IArchiveCodecsInfo>
{
public:
    
    /** 文档格式管理容器
    */
    typedef std::vector<IArchiveInfoPtr> container_type ;

public:

    /** 默认构造函数
    */
    RCArchiveManager() ;
    
    /** 默认析构函数
    */
    virtual ~RCArchiveManager() ;
   
public:
    
    /** 获取支持的文档格式个数
    @param [out] numFormats 文档格式格式
    @return 如果成功则返回RC_S_OK，否则返回错误号
    */
    virtual HResult GetNumberOfFormats(uint32_t& numFormats) const ;
    
    /** 获取文档信息
    @param [in] index 文档格式下标
    @param [out] spArcInfo 文档格式信息
    @return 如果成功则返回RC_S_OK，否则返回错误号
    */
    virtual HResult GetFormat(uint32_t index, IArchiveInfoPtr& spArcInfo) const ;
    
    /** 创建输入文档接口
    @param [in] index 文档格式下标
    @param [out] spInArchive 输入文档接口
    @return 如果成功则返回RC_S_OK，否则返回错误号
    */
    virtual HResult CreateInArchive(uint32_t index, IInArchivePtr& spInArchive) const ;
    
    /** 创建输出文档接口
    @param [in] index 文档格式下标
    @param [out] spOutArchive 输出文档接口
    @return 如果成功则返回RC_S_OK，否则返回错误号
    */
    virtual HResult CreateOutArchive(uint32_t index, IOutArchivePtr& spOutArchive) const ;

public:
    
    /** 取得容器内容
    @return 文档格式容器引用
    */
    const container_type& GetArchiveArray(void) const ;

protected:
    
    /** 注册文档格式信息
    @param [out] spArcInfo 文档格式信息
    @return 成功返回true,否则返回false
    */    
    bool RegisterArchive(const IArchiveInfoPtr& spArcInfo) ;
    
private:
    
    /** 编码解码容器
    */
    container_type m_archiveArray ;
};

/** 智能指针定义
*/
typedef RCComPtr<RCArchiveManager> RCArchiveManagerPtr ;

END_NAMESPACE_RCZIP

#endif //__RCArchiveManager_h_
