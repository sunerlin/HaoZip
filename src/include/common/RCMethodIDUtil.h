/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCMethodIDUtil_h_
#define __RCMethodIDUtil_h_ 1

#include "base/RCNonCopyable.h"
#include "base/RCString.h"

BEGIN_NAMESPACE_RCZIP

class RCMethodIDUtil:
    private RCNonCopyable
{
public:

    /** 默认构造函数
    */
    RCMethodIDUtil() ;
    
    /** 默认析构函数
    */
    ~RCMethodIDUtil() ;
    
    /** 将MethodID转换成字符串
    @param [in] id MethodID 值
    @return 转换结果字符串
    */
    static RCStringW ConvertMethodIdToString(RCMethodID id) ;
    
private:
    
    /** 将字节值转换成十六字符
    @param [in] value 字节值
    @return 返回十六进制字符值
    */
    static wchar_t GetHex(byte_t value) ;
};

END_NAMESPACE_RCZIP

#endif //__RCMethodIDUtil_h_
