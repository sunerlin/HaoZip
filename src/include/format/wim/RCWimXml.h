/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCWimXml_h_
#define __RCWimXml_h_ 1

#include "base/RCDefs.h"
#include "common/RCVector.h"
#include "common/RCBuffer.h"
#include "format/wim/RCWimImageInfo.h"

BEGIN_NAMESPACE_RCZIP

/** Win xml
*/
class RCWimXml
{
public:

    /** 默认构造函数
    */
    RCWimXml() ;
    
    /** 默认析构函数
    */
    ~RCWimXml() ;
    
public:

    /** 解析
    */
    void Parse() ;
    
public:

    /** 数据
    */
    RCByteBuffer m_data;

    /** 编号
    */
    uint16_t m_volIndex;

    /** Image 列表
    */
    RCVector<RCWimImageInfo> m_images;
};

END_NAMESPACE_RCZIP

#endif //__RCWimXml_h_
