/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __IEnumDirItemFilter_h_
#define __IEnumDirItemFilter_h_ 1

#include "base/RCString.h"
#include "base/RCSmartPtr.h"

BEGIN_NAMESPACE_RCZIP

/** 文件信息类
*/
class RCFileInfo ;

/** 被压缩文件的排除过滤器接口
*/
class IEnumDirItemFilter
{
public:
    
    /** 析构函数
    */
    virtual ~IEnumDirItemFilter() {} ;
    
public:
    
    /** 对文件进行过滤
    @param [in] filePathPrefix 被压缩的文件路径前缀
    @param [in] fileInfo 被压缩的文件属性
    @return 返回值：RC_S_OK - 表示该文件已被过滤，应被忽略
                     RC_S_FALSE - 表示该文件可进行压缩
                     其他值，发生错误
                    
    */
    virtual HResult Filter(const RCString& filePathPrefix, const RCFileInfo& fileInfo) = 0 ;
};

/** 智能指针定义
*/
typedef RCSharedPtr<IEnumDirItemFilter> IEnumDirItemFilterPtr ;

END_NAMESPACE_RCZIP

#endif //__IEnumDirItemFilter_h_
