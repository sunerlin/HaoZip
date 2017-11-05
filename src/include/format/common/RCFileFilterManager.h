/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCFileFilterManager_h_
#define __RCFileFilterManager_h_ 1

#include "base/RCDefs.h"
#include "base/RCString.h"
#include "base/RCSmartPtr.h"
#include "common/RCVector.h"

BEGIN_NAMESPACE_RCZIP

class RCFileNameParts
{
public:

    /** 文件名
    */
    RCString m_name;

    /** 匹配部分
    */
    RCVector<RCString> m_parts;
};

/** RCFileNameParts智能指针
*/
typedef RCSharedPtr<RCFileNameParts> RCFileNamePartsPtr ;

class RCFileFilterManager
{
public:

    /** 默认构造函数
    */
    RCFileFilterManager(const RCString& fileList) ;

    /** 默认析构函数
    */
    ~RCFileFilterManager() ;

public:

    /** 文件是否过滤
    @param [in] absolutePath 绝对路径
    @param [in] relativePath 相对路径
    @return 是返回ture，否则返回false
    */
    bool IsFilter(const RCString& absolutePath, const RCString& relativePath) ;

private:

    /** 解析过滤文件列表
    @return 解析成功返回true，否则返回false
    */
    bool ParseList(const RCString& fileList) ;

    /** 比较字符串，可以处理含通配符
    @param [in] sPattern 通配符
    @param [in] sFileName 文件名
    @param [in] bNoCase 是否区分大小写
    @return 相等返回true，否则返回false
    */
    bool CompareStrings(const char_t* sPattern, 
                        const char_t* sFileName, 
                        bool bNoCase);

private:

    /** 文件名
    */
    RCVector<RCFileNamePartsPtr> m_fileNames;
};

END_NAMESPACE_RCZIP

#endif //__RCFileFilterManager_h_
