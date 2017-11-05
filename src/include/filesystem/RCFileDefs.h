/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCFileDefs_h_
#define __RCFileDefs_h_ 1

#include "base/RCDefs.h"

BEGIN_NAMESPACE_RCZIP

/** 文件时间
*/
class RC_FILE_TIME
{
public:
    
    /** 文件时间低位值
    */
    uint32_t u32LowDateTime ;
    
    /** 文件时间高位值
    */
    uint32_t u32HighDateTime ;

public:
    
    /** 默认构造函数
    */
    RC_FILE_TIME():
        u32LowDateTime(0),
        u32HighDateTime(0)
    {
    }
};

/** 判断文件时间是否为有效值
@param [in] fileTime 文件时间值
@return 如为有效值返回true, 否则返回false
*/
inline bool IsValidFileTime(const RC_FILE_TIME& fileTime)
{
    return (fileTime.u32HighDateTime != 0) || (fileTime.u32LowDateTime != 0) ;
}

/** 文件时间封装
*/
class RCFileTimeValue
{
public: 
    
    /** 文件时间
    */
    RC_FILE_TIME m_fileTime ;
    
    /** 当前是否含有有效时间值
    */
    bool m_hasFileTime ;

public:
    
    /** 默认构造函数
    */
    RCFileTimeValue():
        m_hasFileTime(false)
    {
    }
    
    /** 设置文件时间
    @param [in] fileTime 文件时间值
    */
    void SetFileTime(RC_FILE_TIME fileTime)
    {
        m_fileTime = fileTime ;
        m_hasFileTime = true ;
    }
};

END_NAMESPACE_RCZIP

#endif //__RCFileDefs_h_
