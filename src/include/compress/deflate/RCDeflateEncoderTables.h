/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCDeflateEncoderTables_h_
#define __RCDeflateEncoderTables_h_ 1

#include "compress/deflate/RCDeflateLevels.h"

BEGIN_NAMESPACE_RCZIP

/** Deflate 编码表
*/
class RCDeflateEncoderTables:
    public RCDeflateLevels
{
public:

    /** 默认构造函数
    */
    RCDeflateEncoderTables() ;
    
    /** 默认析构函数
    */
    ~RCDeflateEncoderTables() ;
    
public:
    
    /** 初始化结构
    */
    void InitStructures() ;
    
public:
    
    /** 是否使用子块
    */
    bool m_useSubBlocks;
    
    /** 是否存储模式
    */
    bool m_storeMode;
    
    /** 是否统计模式
    */
    bool m_staticMode;
    
    /** 块大小
    */
    uint32_t m_blockSizeRes;
    
    /** 位置
    */
    uint32_t m_pos;
};

END_NAMESPACE_RCZIP

#endif //__RCDeflateEncoderTables_h_
