/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCWimImageInfo_h_
#define __RCWimImageInfo_h_ 1

#include "filesystem/RCFileDefs.h"
#include "base/RCString.h"

BEGIN_NAMESPACE_RCZIP

class RCXmlItem ;

/** Wim Image Info
*/
class RCWimImageInfo
{
public:

    /** 默认构造函数
    */
    RCWimImageInfo() ;
    
    /** 默认析构函数
    */
    ~RCWimImageInfo() ;
    
public:  

    /** 解析
    @param [in] item xml项
    */
    void Parse(const RCXmlItem& item) ;
      
public:

    /** 创建时间是否定义
    */
    bool m_cTimeDefined;

    /** 修改时间是否定义
    */
    bool m_mTimeDefined;

    /** 名称是否定义
    */
    bool m_nameDefined;
    
    /** 创建时间
    */
    RC_FILE_TIME m_cTime;

    /** 修改时间
    */
    RC_FILE_TIME m_mTime;

    /** 名称
    */
    RCString m_name ;
    
private:

    /** 解析时间
    @param [in] item xml项
    @param [out] defined 时间是否定义
    @param [out] ft 时间
    @param [in] s
    */
    static void ParseTime(const RCXmlItem& item, bool& defined, RC_FILE_TIME& ft, const RCStringA& s) ;

    /** 解析数字
    @param [in] s 字符串
    @param [out] res 返回数字
    @return 成功返回true,否则返回false
    */
    static bool ParseNumber32(const RCStringA& s, uint32_t& res) ;

    /** 解析64位数字
    @param [in] s 字符串
    @param [out] res 返回的数字
    @return 成功返回true,否则返回false
    */
    static bool ParseNumber64(const RCStringA& s, uint64_t& res) ;
};

END_NAMESPACE_RCZIP

#endif //__RCWimImageInfo_h_
