/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCSha1Hmac_h_
#define __RCSha1Hmac_h_ 1

#include "crypto/hash/RCSha1Context.h"
#include "crypto/hash/RCSha1Defs.h"

BEGIN_NAMESPACE_RCZIP

/** Sha1 Hmac
*/
class RCSha1Hmac
{
public:

    /** 默认构造函数
    */
    RCSha1Hmac() ;
    
    /** 默认析构函数
    */
    ~RCSha1Hmac() ;

public:
    
    /** 设置key
    @param [in] key key值
    @param [in] keySize 大小
    */
    void SetKey(const byte_t *key, size_t keySize);

    /** 更新
    @param [in] data 数据
    @param [in] dataSize 数据大小
    */
    void Update(const byte_t *data, size_t dataSize);

    /** 结束计算
    @param [in] mac mac地址
    @param [in] macSize mac大小
    */
    void Final(byte_t *mac, size_t macSize = RCSha1Defs::kDigestSize);
        
private:

    /** sha
    */
    RCSha1Context m_sha ;

    /** sha2
    */
    RCSha1Context m_sha2 ;
};

END_NAMESPACE_RCZIP

#endif //__RCSha1Hmac_h_
