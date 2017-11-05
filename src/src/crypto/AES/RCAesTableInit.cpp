/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "crypto/AES/RCAesTableInit.h"
#include "algorithm/Aes.h"

/////////////////////////////////////////////////////////////////
//RCAesTableInitImpl class implementation

BEGIN_NAMESPACE_RCZIP

RCAesTableInitImpl::RCAesTableInitImpl()
{
    AesGenTables();
}

RCAesTableInitImpl::~RCAesTableInitImpl()
{
}

END_NAMESPACE_RCZIP
