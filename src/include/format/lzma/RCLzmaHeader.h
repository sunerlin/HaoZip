/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCLzmaHeader_h_
#define __RCLzmaHeader_h_ 1

#include "base/RCDefs.h"

BEGIN_NAMESPACE_RCZIP

class RCLzmaHeader
{
public:

    /** 默认构造函数
    */
    RCLzmaHeader() ;

public:

    /** 获取字典大小
    @return 返回字典大小
    */
    uint32_t GetDicSize() const ;

    /** 是否有大小
    @return 是返回true，否则返回false
    */
    bool HasSize() const ;

    /** 解析
    @param [in] buf 内存数据
    @param [in] isThereFilter 是否是过滤
    @return 解析成功返回true，否则返回false
    */
    bool Parse(const byte_t* buf, bool isThereFilter);

    /** 检查字典大小
    @param [in] p 内存数据
    @return 通过检查返回true，否则返回false
    */
    static bool CheckDicSize(const byte_t* p) ;

public:

    /** 大小
    */
    uint64_t m_size;

    /** 过滤id
    */
    byte_t m_filterID;

    /** lzma属性
    */
    byte_t m_lzmaProps[5];
};

END_NAMESPACE_RCZIP

#endif //__RCLzmaHeader_h_
