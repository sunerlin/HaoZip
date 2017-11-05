/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "format/dmg/RCDmgBlock.h"

/////////////////////////////////////////////////////////////////
//RCDmgBlock class implementation

BEGIN_NAMESPACE_RCZIP

RCDmgBlock::RCDmgBlock():
    m_type(0),
    m_unpPos(0),
    m_unpSize(0),
    m_packPos(0),
    m_packSize(0)
{
}

END_NAMESPACE_RCZIP
