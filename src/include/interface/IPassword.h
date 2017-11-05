/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __IPassword_h
#define __IPassword_h 1

#include "interface/IUnknown.h"
#include "base/RCString.h"

BEGIN_NAMESPACE_RCZIP

/** 接口ID定义
*/
enum
{
    IID_ICryptoGetTextPassword     = IID_IPASSWORD_BASE,
    IID_ICryptoGetTextPassword2
};

/** 获取密码接口
*/
class ICryptoGetTextPassword:
    public IUnknown
{
public:
    
    /** 接口ID
    */
    enum { IID = IID_ICryptoGetTextPassword } ;
    
public:
    
    /** 获取密码
    @param [out] password 获取的密码
    @return 成功返回RC_S_OK, 否则返回错误码
    */
    virtual HResult CryptoGetTextPassword(RCString& password) = 0 ;
    
protected:
    
    /** 默认析构函数
    */
    ~ICryptoGetTextPassword(){} ;
};

/** 获取密码接口
*/
class ICryptoGetTextPassword2:
    public IUnknown
{
public:
    
    /** 接口ID
    */
    enum { IID = IID_ICryptoGetTextPassword2 } ;
    
public:
    
    /** 获取密码
    @param [out] passwordIsDefined 密码是否定义 返回0表示没有定义密码，返回1表示有密码 返回0表示没有定义密码，返回1表示有密码
    @param [out] password 获取的密码
    @return 成功返回RC_S_OK, 否则返回错误码
    */
    virtual HResult CryptoGetTextPassword2(int32_t* passwordIsDefined, RCString& password) = 0 ;
    
protected:
    
    /** 默认析构函数
    */
    ~ICryptoGetTextPassword2(){} ;
};

/** 智能指针定义
*/
typedef RCComPtr<ICryptoGetTextPassword>    ICryptoGetTextPasswordPtr ;
typedef RCComPtr<ICryptoGetTextPassword2>   ICryptoGetTextPassword2Ptr ;

END_NAMESPACE_RCZIP

#endif //__IPassword_h
