/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCPropertyIDUtils_h_
#define __RCPropertyIDUtils_h_ 1

#include "base/RCString.h"
#include "interface/RCPropertyID.h"
#include "common/RCVariant.h"
#include "filesystem/RCFileDefs.h"
#include "interface/RCArchiveType.h"

BEGIN_NAMESPACE_RCZIP

class RCPropertyIDUtils
{
public:

    /** 将属性转换成可以显示的字符串
    @param [in] propID 属性ID
    @param [in] propValue 需要转换的属性值
    @return 返回显示用的属性字符串，如果失败返回空串
    */
    static RCString ConvertPropertyToString(RCPropIDEnumType propID, 
                                            const RCVariant& propValue) ;

    
    /** 将属性转换成可以显示的字符串
    @param [in] propID 属性ID
    @param [in] propValue 需要转换的属性值
    @param [out] value 返回显示用的属性字符串
    @return 如果失败返回false， 如果成功返回true
    */
    static bool ConvertPropertyToString(RCPropIDEnumType propID, 
                                        const RCVariant& propValue,
                                        RCString& value) ;

    /** 将文件时间转换成字符串
    @param [in] fileTime 文件时间
    @param [in] includeTime 是否包含时间（指时分）
    @param [in] includeSeconds 是否包含秒
    @return 返回显示的字符串，如果失败返回空串
    */
    static RCString ConvertFileTimeToString(const RC_FILE_TIME& fileTime, 
                                            bool includeTime, 
                                            bool includeSeconds) ;
    
    /** 将文件大小转换成字符串显示， 每三位用逗号分隔
    @param [in] fileSize 文件大小值
    @return 返回转换后的字符串
    */
    static RCString ConvertFileSizeToString(uint64_t fileSize) ;
    
    /** 将文件大小转换成字符串显示， 用KB/MB等显示， 忽略KB以下值
    @param [in] fileSize 文件大小值
    @return 返回转换后的字符串
    */
    static RCString ConvertFileSize(uint64_t fileSize) ;
    
    /** 将文件格式类型转换为字符串
    @param [in] archiveType 文件格式值
    @return 返回转换后的字符串
    */
    static RCString ConvertArchiveType(RCArchiveType archiveType) ;
};

END_NAMESPACE_RCZIP

#endif //__RCPropertyIDUtils_h_
