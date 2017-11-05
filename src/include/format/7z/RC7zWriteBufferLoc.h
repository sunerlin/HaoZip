/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RC7zWriteBufferLoc_h_
#define __RC7zWriteBufferLoc_h_ 1

#include "base/RCNonCopyable.h"

BEGIN_NAMESPACE_RCZIP

class RC7zWriteBufferLoc:
    private RCNonCopyable
{
public:

    /** 默认构造函数
    */
    RC7zWriteBufferLoc() ;
    
    /** 默认析构函数
    */
    ~RC7zWriteBufferLoc() ;
    
public:

    /** 初始化
    @param [in] data 数据
    @param [in] size 大小
    */
    void Init(byte_t* data, size_t size) ;

    /** 写字节
    @param [in] data 数据
    @param [in] size 大小
    */
    void WriteBytes(const void* data, size_t size) ;

    /** 写一个字节
    */
    void WriteByte(byte_t b) ;

    /** 获取位置
    */
    size_t GetPos() const ;
    
private:

    /** 数据
    */
    byte_t* m_data ;

    /** 大小
    */
    size_t m_size ;

    /** 位置
    */
    size_t m_pos ;
};

END_NAMESPACE_RCZIP

#endif //__RC7zWriteBufferLoc_h_
