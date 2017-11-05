/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#include "base/RCString.h"
#include "common/RCVector.h"
#include "interface/IArchive.h"
#include "base/RCSmartPtr.h"

#ifndef __RCArchiveLink_h_
#define __RCArchiveLink_h_ 1

BEGIN_NAMESPACE_RCZIP

/** 文档链接
*/
class RCArchiveLink
{
public:

    /** 默认构造函数
    */
    RCArchiveLink() ;
    
    /** 默认析构函数
    */
    ~RCArchiveLink() ;

public:

    /** 返回压缩包嵌套级
    @return 如果是非嵌套包, 返回1,
            嵌套包,如tar.gz等, 返回2
    */
    int32_t GetNumLevels() const ;

    /** 获取默认的Archive
    @return 返回默认输入Archive指针
    */
    IInArchive* GetArchive() const ;
    
    /** 获取第1个的Archive
    @return 返回第1个输入Archive指针
    */
    IInArchivePtr GetArchive0() const ;
    
    /** 获取第2个的Archive
    @return 返回第2个输入Archive指针
    */
    IInArchivePtr GetArchive1() const ;

    /** 获取默认的压缩文件名
    @return 返回默认压缩文件名
    */
    RCString GetDefaultItemName() const ;

    /** 获取文件格式Format Index
    @return 返回压缩文件格式序号
    */
    int32_t GetArchiverIndex() const ;
    
    /** 获取当前压缩文件的格式信息
    @return 返回当前压缩文件格式信息
    */
    RCString GetArchiveTypeString() const ;

    /** 关闭打开的文档
    @return 成功返回RC_S_OK,否则返回错误号
    */
    HResult Close() ;

    /** 释放资源
    */
    void Release() ;

public:

    /** 第1个压缩文件输入
    */
    IInArchivePtr m_archive0;
    
    /** 嵌套的第2个压缩文件输入
    */
    IInArchivePtr m_archive1;

    /** 第1个压缩文件默认名称
    */
    RCString m_defaultItemName0;
    
    /** 第2个压缩文件默认名称
    */
    RCString m_defaultItemName1;

    /** 第1个压缩文件格式Format Index
    */
    int32_t m_formatIndex0;
    
    /** 第2个压缩文件格式Format Index
    */
    int32_t m_formatIndex1;

    /** 压缩文件的完整路径
    */
    RCVector<RCString> m_volumePaths;

    /** 是否已经打开
    */
    bool m_isOpen ;
    
    /** 压缩文件卷大小
    */
    uint64_t m_volumesSize;
};

/** 智能指针定义
*/
typedef RCSharedPtr<RCArchiveLink> RCArchiveLinkPtr ;

END_NAMESPACE_RCZIP

#endif //__RCArchiveLink_h_
