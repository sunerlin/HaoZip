/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCSha1Hmac32_h_
#define __RCSha1Hmac32_h_ 1

#include "crypto/hash/RCSha1Context32.h"

BEGIN_NAMESPACE_RCZIP

/** Sha1 Hmac
*/
class RCSha1Hmac32
{
public:

    /** 默认构造函数
    */
    RCSha1Hmac32() ;
    
    /** 默认析构函数
    */
    ~RCSha1Hmac32() ;

public:

    /** 设置key
    @param [in] key 设置的key
    @param [in] keySize 大小
    */
    void SetKey(const byte_t *key, size_t keySize);

    /** 更新
    @param [in] data 数据
    @param [in] dataSize 数据大小
    */
    void Update(const uint32_t *data, size_t dataSize) ;

    /** 结束计算
    @param [in] mac mac地址
    @param [in] macSize mac大小
    */
    void Final(uint32_t *mac, size_t macSize = RCSha1Defs::kDigestSizeInWords);  

    /** 计算摘要
    @param [in] mac mac地址
    @param [in] numIteration 迭代次数
    */
    void GetLoopXorDigest(uint32_t *mac, uint32_t numIteration);
    
private:

    /** sha
    */
    RCSha1Context32 m_sha ;

    /**
    */
    RCSha1Context32 m_sha2 ;
};

END_NAMESPACE_RCZIP

#endif //__RCSha1Hmac32_h_
