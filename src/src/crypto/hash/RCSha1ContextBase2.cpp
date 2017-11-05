/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "crypto/hash/RCSha1ContextBase2.h"
#include "algorithm/RotateDefs.h"

/////////////////////////////////////////////////////////////////
//RCSha1ContextBase2 class implementation

BEGIN_NAMESPACE_RCZIP

RCSha1ContextBase2::RCSha1ContextBase2():
    m_count2(0)
{
}

RCSha1ContextBase2::~RCSha1ContextBase2()
{
}

void RCSha1ContextBase2::UpdateBlock()
{
    RCSha1ContextBase::UpdateBlock(m_buffer);
}

void RCSha1ContextBase2::Init()
{
    RCSha1ContextBase::Init() ;
    m_count2 = 0 ;
}


END_NAMESPACE_RCZIP
