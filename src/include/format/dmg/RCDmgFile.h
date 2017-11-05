/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCDmgFile_h_
#define __RCDmgFile_h_ 1

#include "base/RCNonCopyable.h"
#include "common/RCVector.h"
#include "common/RCBuffer.h"
#include "format/dmg/RCDmgBlock.h"
#include "base/RCString.h"

BEGIN_NAMESPACE_RCZIP

class RCDmgFile:
    private RCNonCopyable
{
public:

    /** 默认构造函数
    */
    RCDmgFile() ;
    
public:

    /** 获取解压后大小
    @return 返回解压后大小
    */
    uint64_t GetUnpackSize() const ;

    /** 获取包大小
    @return 返回包大小
    */
    uint64_t GetPackSize() const ;
    
public:

    /** 原始数据
    */
    RCByteBuffer m_raw ;

    /** 起始偏移
    */
    uint64_t m_startPos ;

    /** dmg块
    */
    RCVector<RCDmgBlockPtr> m_blocks ;

    /** 名字
    */
    RCStringA m_name ;
};

/** RCDmgFile智能指针
*/
typedef RCSharedPtr<RCDmgFile> RCDmgFilePtr ;

END_NAMESPACE_RCZIP

#endif //__RCDmgFile_h_
