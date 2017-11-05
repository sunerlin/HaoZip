/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCArchiveUpdateFilter_h_
#define __RCArchiveUpdateFilter_h_ 1

#include "base/RCString.h"
#include "base/RCSmartPtr.h"

BEGIN_NAMESPACE_RCZIP

/** 压缩文件过滤器
*/
class RCArchiveUpdateFilter
{
public:

    /** 默认构造函数
    */
    RCArchiveUpdateFilter() {} ;
    
public:
    
    /** 压缩源文件名过滤函数
    @param [in] index 当前文件在包内的索引值
    @param [in] filePath 当前文件的完整路径
    @param [in] newFilePath 过滤后的文件名
    @return 如果返回RC_S_OK, 则已经进行过滤， 否则未进行过滤
    */
    virtual HResult OnGetFilePath(uint32_t index, 
                                  const RCString& filePath,
                                  RCString& newFilePath) = 0 ;

    /** 获取数据流
    @param [in] index 当前文件在包内的索引值
    @param [in] newFilePath 过滤后的文件名
    @return 如果返回RC_S_OK, 则已经进行过滤， 否则未进行过滤
    */
    virtual HResult OnGetStream(uint32_t index, RCString& newFilePath) = 0;
    
    /** 判断数据流是否已经被转换
    @param [in] index 文件编号
    @return 已经转换返回true,否则返回false
    */
    virtual bool IsFileConverted(uint32_t index) = 0 ;
    
    /** 判断是否需要转换
    @param [in] index 文件编号
    @return 需要转换返回true,否则返回false
    */
    virtual bool NeedConvert(uint32_t index) = 0 ;
    
protected:    
    
    /** 默认析构函数
    */
    ~RCArchiveUpdateFilter() {} ;
};

/** 智能指针定义
*/
typedef RCSharedPtr<RCArchiveUpdateFilter> RCArchiveUpdateFilterPtr ;

END_NAMESPACE_RCZIP

#endif //__RCArchiveUpdateFilter_h_
