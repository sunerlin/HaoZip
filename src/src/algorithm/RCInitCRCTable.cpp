/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "algorithm/RCInitCRCTable.h"
#include "algorithm/7zCrc.h"

/////////////////////////////////////////////////////////////////
//RCInitCRCTable class implementation

BEGIN_NAMESPACE_RCZIP

RCInitCRCTableImpl::RCInitCRCTableImpl()
{
    CrcGenerateTable() ;
}


END_NAMESPACE_RCZIP
