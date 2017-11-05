/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "compress/bzip2/RCBZip2CRCTableInit.h"
#include "compress/bzip2/RCBZip2CRC.h"

/////////////////////////////////////////////////////////////////
//RCBZip2CRCTableInit class implementation

BEGIN_NAMESPACE_RCZIP

RCBZip2CRCTableInit::RCBZip2CRCTableInit()
{
    RCBZip2CRC::InitTable() ;
}

RCBZip2CRCTableInit::~RCBZip2CRCTableInit()
{
    
}

END_NAMESPACE_RCZIP
