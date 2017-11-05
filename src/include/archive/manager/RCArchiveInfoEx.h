/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCArchiveInfoEx_h_
#define __RCArchiveInfoEx_h_ 1

#include "archive/manager/RCArchiveExtInfo.h"
#include "interface/IArchiveInfo.h"
#include "common/RCVector.h"
#include "base/RCString.h"

BEGIN_NAMESPACE_RCZIP

/** 文档管信息数据
*/
class RCArchiveInfoEx
{
public:

    /** 默认构造函数
    */
    RCArchiveInfoEx() ;
    
    /** 默认析构函数
    */
    ~RCArchiveInfoEx() ;
    
    /** 拷贝构造函数
    */
    RCArchiveInfoEx(const RCArchiveInfoEx& rhs) ;
    
    /** 赋值操作符
    */
    RCArchiveInfoEx& operator= (const RCArchiveInfoEx& rhs) ;
    
public:
    
    /** 初始化
    @param [in] spArcInfo 文档格式信息
    */
    void initialize(const IArchiveInfoPtr& spArcInfo) ;
    
    /** 文档类型唯一ID
    @return 文档ID
    */
    RCArchiveID GetArchiveID() const ;

    /** 获取文档名称
    @return 文档名称
    */
    RCString GetName() const ;
    
    /** 获取文档签名
    @param [out] signature 返回文件签名
    */
    void GetSignature(std::vector<byte_t>& signature) const ;
        
    /** 获取KeepName属性
    @return 如果KeepName则返回true，否则返回false
    */
    bool IsKeepName() const ;
    
    /** 是否允许更新文档
    @return 可以更新返回true,否则返回false
    */
    bool IsEnableUpdate(void) const ;
    
    /** 获取主扩展名
    @return 返回格式扩展名
    */
    RCString GetMainExt(void) const ;
    
    /** 查找扩展名
    @param [in] ext 扩展名
    @return 如果匹配到扩展名，返回值 >=0 ，如果没有匹配，则返回-1
    */
    int32_t FindExtension(const RCString& ext) const ;
    
    /** 获取所有扩展名，以空格作为分隔符
    @return 返回所有扩展名
    */
    RCString GetAllExtensions(void) const ;
    
    /** 获取扩展名总数
    @return 返回扩展名总数
    */
    uint32_t GetExtCount(void) const ;
    
    /** 获取指定扩展名
    @param [in] index 扩展名对应的下标值
    @return 返回扩展名，如果下标不存在，则返回空
    */
    RCString GetExt(uint32_t index) const ; 
    
    /** 获取指定附加扩展名
    @param [in] index 扩展名对应的下标值
    @return 返回扩展名，如果下标不存在，则返回空
    */
    RCString GetAddExt(uint32_t index) const ; 
    
    /** 获取IArchiveInfo接口
    @return 返回文档信息接口指针
    */
    const IArchiveInfoPtr& GetArchiveInfo(void) const ;
    
private:
    
    /** 解析扩展名为数组
    @param [in] src 扩展名
    @param [out] dest 解析后数组
    */
    void SplitExt(const RCString& src, RCVector<RCString>& dest) ;
    
private:
    
    /** 文档格式信息
    */
    IArchiveInfoPtr m_spArcInfo ;
    
    /** 扩展名容器
    */
    RCVector<RCArchiveExtInfo> m_arcExts ;
};

END_NAMESPACE_RCZIP

#endif //__RCArchiveInfoEx_h_
