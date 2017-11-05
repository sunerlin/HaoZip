/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCExtractOptions_h_
#define __RCExtractOptions_h_  1

#include "base/RCTypes.h"
#include "archive/extract/RCExtractDefs.h"
#include "archive/common/RCArchiveProperty.h"
#include "common/RCVector.h"
#include "filesystem/RCFileInfo.h"

BEGIN_NAMESPACE_RCZIP

/** 解压选线参数
*/
class RCExtractOptions
{
public:
    
    /** 构造函数
    */
    RCExtractOptions() ;

public:
    
    /** 是否为标准控制台输出模式
    */
    bool m_isStdOutMode;
    
    /** 测试模式
    */
    bool m_isTestMode ;
    
    /** 对所有提示已确认回答
    */
    bool m_isYesToAll ;
    
    /** 更新方式
    */
    RCExractUpdateMode m_updateMode ;
    
    /** 路径处理模式
    */
    RCExractPathMode m_pathMode ;
    
    /** 覆盖文件处理模式
    */
    RCExractOverwriteMode m_overwriteMode ;
    
    /** 是否保留损坏文件
    */
    bool m_keepDamageFiles ;

    /** 文档文件信息
    */
    RCFileInfo m_archiveFileInfo ;

    /** 输出路径
    */
    RCString m_outputDir ;
    
    /** 默认压缩文件名
    */
    RCString m_defaultItemName ;
    
    /** 解压属性
    */
    RCVector<RCArchiveProperty> m_properties ;
    
    /** 解压到子目录
    */
    bool m_extractToSubDir;

    /** 重复多余目录移除功能
    */
    bool m_removeSameRoot;
    
    /** 解压文件时间属性标记
    */
    RCExractFileTimeFlag m_fileTimeFlag ;

    /** 需要移除的目录
    */
    RCVector<RCString> m_setRemovePathList;
};

END_NAMESPACE_RCZIP

#endif //__RCExtractOptions_h_
