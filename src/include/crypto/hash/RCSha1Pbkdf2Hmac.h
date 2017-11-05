/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCSha1Pbkdf2Hmac_h_
#define __RCSha1Pbkdf2Hmac_h_ 1

#include "base/RCDefs.h"

BEGIN_NAMESPACE_RCZIP

/** Sha1 Pbkdf2 Hmac
*/
class RCSha1Pbkdf2Hmac
{
public:

    /**
    @param [in] pwd 密码
    @param [in] pwdSize 密码大小
    @param [in] salt 
    @param [in] saltSize
    @param [in] numIterations 迭代次数
    @param [in] key 设置key
    @param [in] keySize key大小
    */
    static void Pbkdf2Hmac( const byte_t* pwd, 
                            size_t pwdSize, 
                            const byte_t* salt, 
                            size_t saltSize,
                            uint32_t numIterations, 
                            byte_t* key, 
                            size_t keySize
                           );
    
    /**
    @param [in] pwd 密码
    @param [in] pwdSize 密码大小
    @param [in] salt 
    @param [in] saltSize
    @param [in] numIterations 迭代次数
    @param [in] key 设置key
    @param [in] keySize key大小
    */
    static void Pbkdf2Hmac32( const byte_t* pwd, 
                              size_t pwdSize, 
                              const uint32_t* salt,
                              size_t saltSize,
                              uint32_t numIterations, 
                              uint32_t* key,
                              size_t keySize
                            );
};

END_NAMESPACE_RCZIP

#endif //__RCSha1Pbkdf2Hmac_h_
