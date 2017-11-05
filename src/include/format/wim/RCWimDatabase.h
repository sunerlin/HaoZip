/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCWimDatabase_h_
#define __RCWimDatabase_h_ 1

#include "format/wim/RCWimItem.h"
#include "common/RCVector.h"

BEGIN_NAMESPACE_RCZIP

/** Wim Database
*/
class RCWimDatabase
{
public:

    /** 默认构造函数
    */
    RCWimDatabase() ;
    
    /** 默认析构函数
    */
    ~RCWimDatabase() ;
    
public:

    /** 取得解压后大小
    @return 返回解压后大小
    */
    uint64_t GetUnpackSize() const ;

    /** 取得压缩大小
    @return 返回压缩大小
    */
    uint64_t GetPackSize() const ;

    /** 清除
    */
    void Clear() ;

public:
 
    /** 文件流
    */
    RCVector<RCWimStreamInfo> m_streams;

    /** 文件项列表
    */
    RCVector<RCWimItem> m_items;
};

END_NAMESPACE_RCZIP

#endif //__RCWimDatabase_h_
