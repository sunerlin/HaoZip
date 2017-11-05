/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCArchiveCodecsLoader_h_
#define __RCArchiveCodecsLoader_h_ 1

#include "archive/manager/RCArchiveManager.h"
#include "archive/manager/RCArchiveCodecs.h"

BEGIN_NAMESPACE_RCZIP

/** 编码管理器加载
*/
class RCArchiveCodecsLoader:
    public RCArchiveCodecs
{
public:

    /** 默认构造函数
    */
    RCArchiveCodecsLoader() ;
    
    /** 默认析构函数
    */
    ~RCArchiveCodecsLoader() ;
    
public:
    
    /** 从动态链接库中加载压缩算法加密解密器
    @param [in] codecFilePath 需要加载的动态链接库文件列表
    @param [out] errMsg 如果加载错误，返回错误信息，按照下标与文件对应，如果信息为空表示加载成功
    @return 返回成功加载的文件数
    */
    uint32_t LoadCompressCodecs(const RCVector<RCString>& codecFilePath,
                                RCVector<RCString>* errMsg) ;
    
    /** 从动态链接库中加载文档格式信息
    @param [in] formatFilePath 需要加载的动态链接库文件列表
    @param [out] errMsg 如果加载错误，返回错误信息，按照下标与文件对应，如果信息为空表示加载成功
    @return 返回成功加载的文件数
    */  
    uint32_t LoadArchiveFormats(const RCVector<RCString>& formatFilePath,
                                RCVector<RCString>* errMsg) ;
   
private:
       
    /** 文档格式管理器
    */
    RCArchiveManagerPtr m_spArchive ;
    
    /** 动态链接库管理数据结构
    */
    class RCShareLibData ;
    RCShareLibData* m_shareLibData ;
};

/** 智能指针定义
*/
typedef RCIntrusivePtr<RCArchiveCodecsLoader> RCArchiveCodecsLoaderPtr ;

END_NAMESPACE_RCZIP

#endif //__RCArchiveCodecsLoader_h_
