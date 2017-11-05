/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RC7zInByte_h_
#define __RC7zInByte_h_ 1

#include "base/RCNonCopyable.h"
#include "base/RCString.h"
#include "format/7z/RC7zDefs.h"
#include "base/RCSmartPtr.h"

BEGIN_NAMESPACE_RCZIP

class RC7zInByte:
    private RCNonCopyable
{
public:

    /** 默认构造函数
    */
    RC7zInByte() ;
    
    /** 默认析构函数
    */
    ~RC7zInByte() ;
    
public:

    /** 初始化
    @param [in] buffer 内存缓冲
    @param [in] size 内存大小
    */
    void Init(const byte_t* buffer, size_t size) ;

    /** 读一个字节
    @return 返回一个字节
    */
    byte_t ReadByte() ;

    /** 读字节
    @param [out] data 返回读取的内容
    @param [in] size 读取的大小
    */
    void ReadBytes(byte_t* data, size_t size) ;

    /** 跳过
    @param [in] size 跳过的大小
    */
    void SkipData(uint64_t size) ;

    /** 跳过
    */
    void SkipData() ;

    /** 读数
    @return 返回64位无符号整型
    */
    uint64_t ReadNumber() ;

    /** 读数
    @return 返回RC7zNum
    */
    RC7zNum ReadNum() ;

    /** 读四个字节转成32位无符号整型
    @return 返回32位无符号整型
    */
    uint32_t ReadUInt32() ;

    /** 读八个字节转成64位无符号整型
    @return 返回64位无符号整型
    */
    uint64_t ReadUInt64() ;

    /** 读字符串
    @param [in] s 返回字符串
    */
    void ReadString(RCString& s) ;
    
public:

    /** 获取当前位置偏移
    @return 返回当前位置偏移
    */
    size_t GetPos() const ;
    
private:

    /** 当前位置
    */
    size_t m_pos ;

    /** 内存
    */
    const byte_t* m_buffer;

    /** 内存大小
    */
    size_t m_size ;
};

/** RC7zInByte智能指针
*/
typedef RCSharedPtr<RC7zInByte> RC7zInBytePtr ;

END_NAMESPACE_RCZIP

#endif //__RC7zInByte_h_
