/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RC7zInArchiveInfo_h_
#define __RC7zInArchiveInfo_h_ 1

#include "format/7z/RC7zHeader.h"
#include "common/RCVector.h"

BEGIN_NAMESPACE_RCZIP

class RC7zInArchiveInfo
{
public:

    /** 默认构造函数
    */
    RC7zInArchiveInfo() ;
    
    /** 默认析构函数
    */
    ~RC7zInArchiveInfo() ;

public:

    /** 清除
    */
    void Clear() ;

public:

    /** 版本
    */
    RC7zArchiveVersion m_version;

    /** 起始位置
    */
    uint64_t m_startPosition;

    /** 头文件后的起始位置
    */
    uint64_t m_startPositionAfterHeader;

    /** 数据起始位置
    */
    uint64_t m_dataStartPosition;

    /** 数据起始位置2
    */
    uint64_t m_dataStartPosition2;

    /** 文件属性信息
    */
    RCVector<uint64_t> m_fileInfoPopIDs;
};

END_NAMESPACE_RCZIP

#endif //__RC7zInArchiveInfo_h_
