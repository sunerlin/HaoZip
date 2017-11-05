/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RC7zAESKeyInfoCache_h_
#define __RC7zAESKeyInfoCache_h_ 1

#include "crypto/7zAES/RC7zAESKeyInfo.h"
#include <vector>

BEGIN_NAMESPACE_RCZIP

/** 7z AES Key Info Cache
*/
class RC7zAESKeyInfoCache
{
public:

    /** 构造函数
    */
    RC7zAESKeyInfoCache(size_t size) ;
    
    /** 默认析构函数
    */
    ~RC7zAESKeyInfoCache() ;

public:

    /** 查找 
    @param [in] key 查找的key
    @return 成功返回true,否则返回false
    */
    bool Find(RC7zAESKeyInfo& key) ;

    /** 增加
    @param [in] key 增加的key
    */
    void Add(RC7zAESKeyInfo& key) ;
    
private:


    /** 大小
    */
    size_t m_size ;

    /** Key 列表
    */
    std::vector<RC7zAESKeyInfo> m_keys ;
};

END_NAMESPACE_RCZIP

#endif //__RC7zAESKeyInfoCache_h_
