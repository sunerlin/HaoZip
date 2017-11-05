/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCVariant_h_
#define __RCVariant_h_ 1

#include "base/RCDefs.h"
#include "base/RCString.h"

BEGIN_NAMESPACE_RCZIP

/** Variant 类型定义
*/
class RCVariantType
{
public:
    
    /** 类型枚举值
    */
    typedef enum
    {
        /** 空类型
        */
        RC_VT_EMPTY  = 0 ,
        
        /** 字符串类型
        */
        RC_VT_STRING = 1 ,
        
        /** 有符号整型
        */
        RC_VT_INT64  = 2 ,
        
        /** 无符号整型
        */
        RC_VT_UINT64 = 3 ,
        
        /** Boolean 类型
        */
        RC_VT_BOOL   = 4
        
    }RCVariantID ;
};

/** Variant 类型定义
*/
class RCVariant
{
public:

    /** 默认构造函数
    */
    RCVariant() ;
    
    /** 构造函数
    @param [in] rhs 复制对象
    */
    RCVariant(const RCString& rhs) ;
    
    /** 构造函数
    */
    RCVariant(uint64_t rhs) ;
    
    /** 构造函数
    @param [in] rhs 初始化值
    */
    RCVariant(int64_t rhs) ;
    
    /** 构造函数
    @param [in] rhs 初始化值
    */
    RCVariant(bool rhs) ;
    
    /** 析构函数
    */
    ~RCVariant() ;
    
public:
    
    /** 清除类型
    */
    void Clear() ;
    
    /** 取是何种类型
    @return 返回当前的类型值
    */
    int32_t which() const ;
    
    /** 获取variant的字符串值
    @return 返回字符串类型值
    */
    const RCString& GetStringValue() const ;
    
    /** 获取variant的64 位有符号整型值
    @return 返回整型值
    */
    int64_t GetInt64Value() const ;
    
    /** 获取variant的64 位无符号整型值
    @return 返回整型值
    */
    uint64_t GetUInt64Value() const ;
    
    /** 获取variant的布尔值
    @return 返回布尔值
    */
    bool GetBooleanValue() const ;
    
public:
    
    /** 赋值为字符串类型
    @param [in] rhs 字符串值
    @return 当前对象引用
    */
    RCVariant& operator=(const RCString& rhs) ;
    
    /** 赋值为无符号64位整型
    @param [in] rhs 无符号64位整型值
    @return 当前对象引用
    */
    RCVariant& operator=(uint64_t rhs) ;
    
    /** 赋值为有符号64位整型
    @param [in] rhs 有符号64位整型值
    @return 当前对象引用
    */
    RCVariant& operator=(int64_t rhs) ;
    
    /** 赋值为布尔类型
    @param [in] rhs 布尔类型值
    @return 当前对象引用
    */
    RCVariant& operator=(bool rhs) ;

private:
    
    /** 数据类型
    */
    RCVariantType::RCVariantID m_type ;
    
    /** 字符串类型
    */
    RCString m_strValue ;
    
    /** 数值类型
    */
    uint64_t m_intValue ;
};

/** 清空RCVariant值
@param [out] variant 需要操作的对象
*/
inline void ClearVariantValue(RCVariant& variant)
{
    variant.Clear() ;
}

/** 判断 RCVariant 是否为空
@param [in] variant 需要操作的对象
@return 如果为空返回true, 否则返回false
*/
inline bool IsEmptyType(const RCVariant& variant)
{
    return (variant.which() == RCVariantType::RC_VT_EMPTY) ;
}

/** 判断 RCVariant 是否为 字符串类型
@param [in] variant 需要操作的对象
@return 如果为字符串类型返回true, 否则返回false
*/
inline bool IsStringType(const RCVariant& variant)
{
    return (variant.which() == RCVariantType::RC_VT_STRING) ;
}

/** 获取variant的字符串值
@param [in] variant 需要操作的对象
@return 返回字符串值
*/
inline const RCString& GetStringValue(const RCVariant& variant)
{
    return variant.GetStringValue() ;
}

/** 判断 RCVariant 是否为 64 位有符号整型
@param [in] variant 需要操作的对象
@return 如果为64位有符号整型类型返回true, 否则返回false
*/
inline bool IsInt64Type(const RCVariant& variant)
{
    return (variant.which() == RCVariantType::RC_VT_INT64) ;
}

/** 获取variant的64 位有符号整型值
@param [in] variant 需要操作的对象
@return 返回64位有符号整型值
*/
inline int64_t GetInt64Value(const RCVariant& variant)
{
    return variant.GetInt64Value() ;
}

/** 判断 RCVariant 是否为 64 位无符号整型
@param [in] variant 需要操作的对象
@return 如果为64位无符号整型类型返回true, 否则返回false
*/
inline bool IsUInt64Type(const RCVariant& variant)
{
    return (variant.which() == RCVariantType::RC_VT_UINT64) ;
}

/** 获取variant的64 位无符号整型值
@param [in] variant 需要操作的对象
@return 返回64位无符号整型值
*/
inline uint64_t GetUInt64Value(const RCVariant& variant)
{
    return variant.GetUInt64Value() ;
}

/** 判断 RCVariant 是否为 64 位有/无符号整型
@param [in] variant 需要操作的对象
@return 如果是64 位有/无符号整型返回true, 否则返回false
*/
inline bool IsInteger64Type(const RCVariant& variant)
{
    return (variant.which() == RCVariantType::RC_VT_INT64) ||
           (variant.which() == RCVariantType::RC_VT_UINT64 ) ;
}

/** 获取variant的64 位有符号整型值
@param [in] variant 需要操作的对象
@return 返回64位有符号值
*/
inline int64_t GetInteger64Value(const RCVariant& variant)
{
    return variant.GetInt64Value() ;
}

/** 判断 RCVariant 是否为布尔类型
@param [in] variant 需要操作的对象
@return 如果为布尔类型返回true, 否则返回false
*/
inline bool IsBooleanType(const RCVariant& variant)
{
    return (variant.which() == RCVariantType::RC_VT_BOOL) ;
}

/** 获取variant的布尔值
@param [in] variant 需要操作的对象
@return 返回对象对应的布尔值
*/
inline bool GetBooleanValue(const RCVariant& variant)
{
   return variant.GetBooleanValue() ;
}

END_NAMESPACE_RCZIP

#endif //__RCVariant_h_
